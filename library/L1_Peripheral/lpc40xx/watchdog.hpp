#pragma once

#include <cstdint>
#include <iterator>

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/watchdog.hpp"
#include "third_party/units/units.h"
#include "utility/time.hpp"
#include "utility/bit.hpp"

namespace sjsu
{
namespace lpc40xx
{
class Watchdog final : public sjsu::Watchdog
{
 public:
  const uint8_t kWatchdogResetAndEnable = 0x3;
  const uint32_t kTimerWarningMax       = 0b11'1111'1111;
  static constexpr units::frequency::hertz_t kWatchdogClockFrequency = 500_kHz;
  static constexpr units::frequency::hertz_t kWatchdogClockDivider   = 4_Hz;
  inline static LPC_WDT_TypeDef * wdt_base                           = LPC_WDT;

  explicit Watchdog(uint8_t priority = -1) : priority_(priority) {}

  // Initializes the watchdog timer
  Status Initialize(std::chrono::seconds duration) const override
  {
    uint32_t watchdog_clock_scalar =
        (kWatchdogClockFrequency / kWatchdogClockDivider).to<uint32_t>();
    uint32_t timer_constant =
        (watchdog_clock_scalar * static_cast<uint32_t>(duration.count()));

    // Insert timer_constant value into TC register
    wdt_base->TC = bit::Extract(timer_constant, { .position = 0, .width = 24 });

    // Enables the watchdog and enables the watchdog reset
    wdt_base->MOD = kWatchdogResetAndEnable;

    // Insert timer_warning value into WARNINT register
    wdt_base->WARNINT = kTimerWarningMax;

    return Status::kSuccess;
  }

  void Enable() const override
  {
    // Register WDT_IRQ defined by the structure
    sjsu::InterruptController::GetPlatformController().Enable({
        .interrupt_request_number = WDT_IRQn,
        .interrupt_handler        = []() {},
        .priority                 = priority_,
    });
  }

  // Feeds the watchdog timer
  void FeedSequence() const override
  {
    // Insert valid feed sequence
    wdt_base->FEED = 0xAA;
    wdt_base->FEED = 0x55;
  }
  // Reads the current counter value of the watchdog timer
  uint32_t CheckTimeout() const override
  {
    return wdt_base->TV;
  }
  uint8_t priority_;
};
}  // namespace lpc40xx
}  // namespace sjsu
