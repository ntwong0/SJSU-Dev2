#include <FreeRTOS.h>
#include <task.h>

#include <cstdint>
#include <cstring>
#include <iterator>

#include "L0_Platform/ram.hpp"
#include "L0_Platform/startup.hpp"
#include "L0_Platform/stm32f10x/stm32f10x.h"
#include "L1_Peripheral/cortex/dwt_counter.hpp"
#include "L1_Peripheral/cortex/system_timer.hpp"
#include "L1_Peripheral/interrupt.hpp"
#include "L1_Peripheral/stm32f10x/system_controller.hpp"
#include "L1_Peripheral/inactive.hpp"
#include "utility/log.hpp"
#include "utility/macros.hpp"
#include "utility/time.hpp"

// Private namespace to make sure that these do not conflict with other globals
namespace
{
using sjsu::cortex::InterruptController;
// Create stm32f10x system controller to be used by low level initialization.
sjsu::stm32f10x::SystemController system_controller;
// Create timer0 to be used by lower level initialization for uptime calculation
sjsu::cortex::DwtCounter arm_dwt_counter;
// System timer is used to count milliseconds of time and to run the RTOS
// scheduler.
sjsu::cortex::SystemTimer system_timer;
// Cortex NVIC interrupt controller used to setup interrupt service routines
sjsu::cortex::InterruptController<sjsu::lpc40xx::kNumberOfIrqs,
                                  __NVIC_PRIO_BITS>
    interrupt_controller;

int Stm32f10xStdOut([[maybe_unused]] const char * data,
                    [[maybe_unused]] size_t length)
{
  return length;
}

int Stm32f10xStdIn([[maybe_unused]] char * data, [[maybe_unused]] size_t length)
{
  return length;
}
}  // namespace

extern "C" uint32_t ThreadRuntimeCounter()
{
  return arm_dwt_counter.GetCount();
}

// The entry point for the C++ library startup
extern "C"
{
  // These are defined after the compilation of the FreeRTOS port for Cortex M4F
  // These will link to those definitions.
  extern void xPortPendSVHandler(void);   // NOLINT
  extern void vPortSVCHandler(void);      // NOLINT
  extern void xPortSysTickHandler(void);  // NOLINT
  // Forward declaration of the default handlers. These are aliased.
  // When the application defines a handler (with the same name), the
  // application's handler will automatically take precedence over these weak
  // definitions.
  extern void ArmHardFaultHandler(void);
  extern void ArmResetHandler(void);

  void vPortSetupTimerInterrupt(void)  // NOLINT
  {
    interrupt_controller.Enable({
        .interrupt_request_number = sjsu::cortex::SVCall_IRQn,
        .interrupt_handler        = vPortSVCHandler,
    });
    interrupt_controller.Enable({
        .interrupt_request_number = sjsu::cortex::PendSV_IRQn,
        .interrupt_handler        = xPortPendSVHandler,
    });
    // Set the SystemTick frequency to the RTOS tick frequency
    // It is critical that this happens before you set the system_clock,
    // since The system_timer keeps the time that the system_clock uses to
    // delay itself.
    system_timer.SetCallback(xPortSysTickHandler);
  }
}

SJ2_SECTION(".crp") const uint32_t kCrpWord = 0xFFFFFFFF;

// The Interrupt vector table.
// This relies on the linker script to place at correct location in memory.
SJ2_SECTION(".isr_vector")
// NOLINTNEXTLINE(readability-identifier-naming)
const sjsu::InterruptVectorAddress kInterruptVectorTable[] = {
  // Core Level - CM4
  &StackTop,                           // 0, The initial stack pointer
  ArmResetHandler,                     // 1, The reset handler
  interrupt_controller.LookupHandler,  // 1, The NMI  handler
  ArmHardFaultHandler,                 // 3, The hard fault handler
  interrupt_controller.LookupHandler,  // 2, The MPU fault  handler
  interrupt_controller.LookupHandler,  // 3, The bus fault  handler
  interrupt_controller.LookupHandler,  // 4, The usage fault  handler
  nullptr,                             // 7, Reserved
  nullptr,                             // 8, Reserved
  nullptr,                             // 9, Reserved
  nullptr,                             // 10, Reserved
  interrupt_controller.LookupHandler,  // 5, SVCall  handler
  interrupt_controller.LookupHandler,  // 6, Debug monitor  handler
  nullptr,                             // 13, Reserved
  interrupt_controller.LookupHandler,  // 7, FreeRTOS PendSV  Handler
  interrupt_controller.LookupHandler,  // 8, The SysTick  handler
  // Chip Level - stm32f10x
  interrupt_controller.LookupHandler,  // 0, Window WatchDog
  interrupt_controller.LookupHandler,  // 1, PVD through EXTI Line detection
  interrupt_controller.LookupHandler,  // 2, Tamper
  interrupt_controller.LookupHandler,  // 3, RTC
  interrupt_controller.LookupHandler,  // 4, FLASH
  interrupt_controller.LookupHandler,  // 5, RCC
  interrupt_controller.LookupHandler,  // 6, EXTI Line0
  interrupt_controller.LookupHandler,  // 7, EXTI Line1
  interrupt_controller.LookupHandler,  // 8, EXTI Line2
  interrupt_controller.LookupHandler,  // 9, EXTI Line3
  interrupt_controller.LookupHandler,  // 10, EXTI Line4
  interrupt_controller.LookupHandler,  // 11, DMA1 Channel 1
  interrupt_controller.LookupHandler,  // 12, DMA1 Channel 2
  interrupt_controller.LookupHandler,  // 13, DMA1 Channel 3
  interrupt_controller.LookupHandler,  // 14, DMA1 Channel 4
  interrupt_controller.LookupHandler,  // 15, DMA1 Channel 5
  interrupt_controller.LookupHandler,  // 16, DMA1 Channel 6
  interrupt_controller.LookupHandler,  // 17, DMA1 Channel 7
  // All of the exceptions beyond this point are different depending on the
  // variant of stm32f10x you are using
  interrupt_controller.LookupHandler,  // 18
  interrupt_controller.LookupHandler,  // 19
  interrupt_controller.LookupHandler,  // 20
  interrupt_controller.LookupHandler,  // 21
  interrupt_controller.LookupHandler,  // 22
  interrupt_controller.LookupHandler,  // 23
  interrupt_controller.LookupHandler,  // 24
  interrupt_controller.LookupHandler,  // 25
  interrupt_controller.LookupHandler,  // 26
  interrupt_controller.LookupHandler,  // 27
  interrupt_controller.LookupHandler,  // 28
  interrupt_controller.LookupHandler,  // 29
  interrupt_controller.LookupHandler,  // 30
  interrupt_controller.LookupHandler,  // 31
  interrupt_controller.LookupHandler,  // 32
  interrupt_controller.LookupHandler,  // 33
  interrupt_controller.LookupHandler,  // 34
  interrupt_controller.LookupHandler,  // 35
  interrupt_controller.LookupHandler,  // 36
  interrupt_controller.LookupHandler,  // 37
  interrupt_controller.LookupHandler,  // 38
  interrupt_controller.LookupHandler,  // 39
  interrupt_controller.LookupHandler,  // 40
  interrupt_controller.LookupHandler,  // 41
  interrupt_controller.LookupHandler,  // 42
  interrupt_controller.LookupHandler,  // 43
  interrupt_controller.LookupHandler,  // 44
  interrupt_controller.LookupHandler,  // 45
  interrupt_controller.LookupHandler,  // 46
  interrupt_controller.LookupHandler,  // 47
  interrupt_controller.LookupHandler,  // 48
  interrupt_controller.LookupHandler,  // 49
  interrupt_controller.LookupHandler,  // 50
  interrupt_controller.LookupHandler,  // 51
  interrupt_controller.LookupHandler,  // 52
  interrupt_controller.LookupHandler,  // 53
  interrupt_controller.LookupHandler,  // 54
  interrupt_controller.LookupHandler,  // 55
  interrupt_controller.LookupHandler,  // 56
  interrupt_controller.LookupHandler,  // 57
  interrupt_controller.LookupHandler,  // 58
  interrupt_controller.LookupHandler,  // 59
  interrupt_controller.LookupHandler,  // 60
  interrupt_controller.LookupHandler,  // 61
  interrupt_controller.LookupHandler,  // 62
  interrupt_controller.LookupHandler,  // 63
  interrupt_controller.LookupHandler,  // 64
  interrupt_controller.LookupHandler,  // 65
  interrupt_controller.LookupHandler,  // 66
  interrupt_controller.LookupHandler,  // 67
};

namespace sjsu
{
SJ2_WEAK(void InitializePlatform());
void InitializePlatform()
{
  sjsu::newlib::SetStdout(Stm32f10xStdOut);
  sjsu::newlib::SetStdin(Stm32f10xStdIn);

  // Set the platform's interrupt controller.
  // This will be used by other libraries to enable and disable interrupts.
  sjsu::InterruptController::SetPlatformController(&interrupt_controller);
  sjsu::SystemController::SetPlatformController(&system_controller);

  system_timer.SetTickFrequency(config::kRtosFrequency);
  sjsu::Status timer_start_status = system_timer.StartTimer();

  SJ2_ASSERT_FATAL(timer_start_status == sjsu::Status::kSuccess,
                   "System Timer (used by FreeRTOS) has FAILED to start!");

  arm_dwt_counter.Initialize();
  sjsu::SetUptimeFunction(sjsu::cortex::SystemTimer::GetCount);
}
}  // namespace sjsu
