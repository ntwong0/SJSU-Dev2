// Test for Pin class.
// Using a test by side effect on the Cortex M4 SysTick register
#include "config.hpp"
#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/cortex/system_timer.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::cortex
{
EMIT_ALL_METHODS(SystemTimer);

TEST_CASE("Testing ARM Cortex SystemTimer", "[cortex-system-timer]")
{
  cortex::DWT_Type local_dwt = {
    .PCSR = 0,
  };
  cortex::CoreDebug_Type local_core;

  memset(&local_dwt, 0, sizeof(local_dwt));
  memset(&local_core, 0, sizeof(local_core));

  DwtCounter::dwt  = &local_dwt;
  DwtCounter::core = &local_core;

  // Simulated local version of SysTick register to verify register
  // manipulation by side effect of Pin method calls
  // Default figure 552 page 703
  SysTick_Type local_systick = { 0x4, 0, 0, 0x000F'423F };
  // Substitute the memory mapped SysTick with the local_systick test struture
  // Redirects manipulation to the 'local_systick'
  SystemTimer::sys_tick = &local_systick;

  // Set mock for sjsu::SystemController
  constexpr units::frequency::hertz_t kClockFrequency = 10_MHz;

  Mock<SystemController> mock_system_controller;
  When(Method(mock_system_controller, GetSystemFrequency))
      .AlwaysReturn(kClockFrequency);
  sjsu::SystemController::SetPlatformController(&mock_system_controller.get());

  Mock<sjsu::InterruptController> mock_interrupt_controller;
  Fake(Method(mock_interrupt_controller, Enable));
  Fake(Method(mock_interrupt_controller, Disable));
  sjsu::InterruptController::SetPlatformController(
      &mock_interrupt_controller.get());

  constexpr uint8_t kExpectedPriority = 3;
  SystemTimer test_subject(kExpectedPriority);

  SECTION("Initialize()")
  {
    // Setup
    constexpr uint32_t kClockFrequencyInt = kClockFrequency.to<uint32_t>();
    constexpr auto kExpectedTicksPerMillisecond =
        kClockFrequencyInt / 1000 /* ms/s */;
    constexpr auto kExpectedNanosecondsPerTick =
        (SystemTimer::kFixedPointScaling * 1'000'000'000ns) /
        kClockFrequencyInt;

    // Exercise
    test_subject.Initialize();

    // Verify
    CHECK(CoreDebug_DEMCR_TRCENA_Msk == local_core.DEMCR);
    CHECK(0 == local_dwt.CYCCNT);
    CHECK(DWT_CTRL_CYCCNTENA_Msk == local_dwt.CTRL);

    CHECK(kExpectedTicksPerMillisecond == SystemTimer::ticks_per_millisecond);
    CHECK(kExpectedNanosecondsPerTick == SystemTimer::nanoseconds_per_tick);
  }

  SECTION("SetTickFrequency generate desired frequency")
  {
    constexpr auto kDivisibleFrequency = 1_kHz;
    local_systick.LOAD                 = 0;

    CHECK(0 == test_subject.SetTickFrequency(kDivisibleFrequency));
    constexpr uint32_t kExpectedLoadValue =
        (kClockFrequency / kDivisibleFrequency) - 1;
    CHECK(kExpectedLoadValue == local_systick.LOAD);
  }

  SECTION("SetTickFrequency should return remainder of ticks mismatch")
  {
    constexpr auto kOddFrequency = 7_Hz;
    local_systick.LOAD           = 0;

    CHECK(kClockFrequency.to<uint32_t>() % kOddFrequency.to<uint32_t>() ==
          test_subject.SetTickFrequency(kOddFrequency));
    CHECK(((kClockFrequency / kOddFrequency) - 1).to<uint32_t>() ==
          local_systick.LOAD);
  }

  SECTION("Start Timer should set necessary SysTick Ctrl bits and set VAL to 0")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 553 page 703
    constexpr uint8_t kEnableMask    = 0b0001;
    constexpr uint8_t kTickIntMask   = 0b0010;
    constexpr uint8_t kClkSourceMask = 0b0100;
    constexpr uint32_t kMask = kEnableMask | kTickIntMask | kClkSourceMask;
    local_systick.VAL        = 0xBEEF;
    local_systick.LOAD       = 1000;

    // Exercise
    CHECK(Status::kSuccess == test_subject.StartTimer());

    // Verify
    CHECK(kMask == local_systick.CTRL);
    CHECK(0 == local_systick.VAL);
    Verify(
        Method(mock_interrupt_controller, Enable)
            .Matching([](sjsu::InterruptController::RegistrationInfo_t info) {
              return (info.interrupt_request_number == cortex::SysTick_IRQn) &&
                     (info.priority == kExpectedPriority);
            }));
  }

  SECTION(
      "StartTimer should return false and set no bits if LOAD is set to zero")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 553 page 703
    constexpr uint8_t kClkSourceMask = 0b0100;
    // Default value see above.
    local_systick.CTRL = kClkSourceMask;
    // Setting load value to 0, should return false
    local_systick.LOAD = 0;
    local_systick.VAL  = 0xBEEF;

    // Exercise
    CHECK(Status::kInvalidSettings == test_subject.StartTimer());

    // Verify
    CHECK(kClkSourceMask == local_systick.CTRL);
    CHECK(0xBEEF == local_systick.VAL);
    Verify(Method(mock_interrupt_controller, Enable)).Never();
  }

  SECTION("DisableTimer should clear all bits")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 553 page 703
    constexpr uint8_t kEnableMask    = 0b0001;
    constexpr uint8_t kTickIntMask   = 0b0010;
    constexpr uint8_t kClkSourceMask = 0b0100;
    constexpr uint32_t kMask = kEnableMask | kTickIntMask | kClkSourceMask;
    local_systick.CTRL       = kMask;
    local_systick.LOAD       = 1000;

    // Exercise
    test_subject.DisableTimer();

    // Verify
    CHECK(0 == local_systick.CTRL);
  }

  SECTION(
      "SetInterrupt set SystemTimer::system_timer_callback to dummy_function")
  {
    // Setup
    bool was_called     = false;
    auto dummy_function = [&was_called](void) { was_called = true; };

    // Exercise
    test_subject.SetCallback(dummy_function);
    test_subject.SystemTimerHandler();

    // Verify
    CHECK(was_called);
  }

  SECTION("GetCount()")
  {
    // Setup
    constexpr uint32_t kDebugCountTicks = 128;
    constexpr auto kMilliseconds        = 1ms;
    SystemTimer::millisecond_count      = kMilliseconds;
    constexpr auto kExpectedUptime = kMilliseconds + (kDebugCountTicks * 100ns);

    // Exercise
    test_subject.Initialize();
    local_dwt.CYCCNT = kDebugCountTicks;
    auto uptime      = test_subject.GetCount();

    // Verify
    CHECK(kExpectedUptime == uptime);
  }

  // Cleanup
  SystemTimer::sys_tick = SysTick;
  DwtCounter::dwt       = DWT;
  DwtCounter::core      = CoreDebug;
}
}  // namespace sjsu::cortex
