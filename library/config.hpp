/// \mainpage SJSU-Dev2 Reference Page
///
/// \section welcome_sec Welcome
/// This is the reference API documentation for the SJSU-Dev2 Project. The
/// reference material should be used as such. This page is not a good place to
/// learn how to use SJSU-Dev2, but instead should be used to get details about
/// specific interfaces/classes/implementations/data structures/functions/
/// macros used in SJSU-Dev2.
///
/// <br />
///
/// \section class_api_sec Finding Class APIs
/// The easiest way to get started with the reference material is to go to the
/// "classes list" page to see all of the classes and data structures used in
/// SJSU-Dev2.
///
/// <br />
///
/// \section namespace_sec Finding everything within the SJSU Namespace
/// See the the "namespace" page and open the sjsu drop down to see everything
/// contained within the sjsu namespace. This will include data structures,
/// templated functions, class interfaces (not their descendants), and
/// everything contained within the subnamespaces of sjsu.
///
/// <br />
///
/// \section support_sec Contact & Support
/// If you want to contribute to SJSU-Dev2, check out the documentation
/// <a
///  href="https://sjsu-dev2.readthedocs.io/en/latest/contributing/philosophy/">
///  here</a>.
///
/// If you spotted a bug, need a feature or have an awesome idea, post an issue
/// on the SJSU-Dev2 github
/// <a href="https://github.com/kammce/SJSU-Dev2/issues">issues</a> page.
///
/// <br />
///
/// \section license_sec License
///
/// <a href="https://github.com/kammce/SJSU-Dev2/blob/master/LICENSE">
/// Apache License Version 2.0
/// </a>
///

#pragma once
#include <cstddef>
#include <cstdint>

// Include using <> instead of "" for  to make sure we only grab the project
// version of project_config.hpp
#include <project_config.hpp>

#include "log_levels.hpp"
#include "utility/units.hpp"


namespace config
{
/// @defgroup config_group Configuration
/// @brief Lists all of the configuration options for a SJSU-Dev2 project.
///
/// How to properly use global configuration option:
/// ================================================
/// If you are using the preprocessor "if", then use the macro name directly.
/// Otherwise always use the kConstant type. Since this section is namespaced,
/// don't forget to include the config:: scope for the kConstant. Example:
/// config::kConstant.
///
/// How to create a new global configuration option:
/// ================================================
/// 1) Check if the macro is already defined. The macros should only be changed
///    in the <project_config.hpp> file
///
/// 2) If the macro is not defined, give it a default value here.
///
/// 3) Generate a typed constexpr version of the macro using
///    SJ2_DECLARE_CONSTANT This will check that the desired typed variable,
///    and the macro equal each other. They wouldn't in cases where the type is
///    bool and the user uses the value 2 for the macro.
///
/// 4) It is recommend, that if there exists an acceptable range for a constant,
///    then use a static_assert to check that the constant generated from
///    SJ2_DECLARE_CONSTANT, is within range. For example if kSystemClockRate
///    can only be between 1Hz and 100Mhz, then kSystemClockRate should be
///    checked if it is within range.
///
/// @{

/// Creates a typed constexpr version of the macro defintion which should be
/// used rather than using the macro directly.
#define SJ2_DECLARE_CONSTANT(macro_name, type, constant_name) \
  constexpr type constant_name = SJ2_##macro_name;            \
  static_assert(constant_name == SJ2_##macro_name,            \
                "SJ2_" #macro_name " must be of type '" #type "'")

/// Used to enable and disable the sending of ANSI color escape codes via the
/// ansi_terminal_codes.hpp
#if !defined(SJ2_ENABLE_ANSI_CODES)
#define SJ2_ENABLE_ANSI_CODES true
#endif  // !defined(SJ2_ENABLE_ANSI_CODES)
/// Delcare Constant ENABLE_ANSI_CODES
SJ2_DECLARE_CONSTANT(ENABLE_ANSI_CODES, bool, kEnableAnsiCodes);

/// Used to set the system clock speed for the LPC4078 in MHz
#if !defined(SJ2_SYSTEM_CLOCK_RATE_MHZ)
#define SJ2_SYSTEM_CLOCK_RATE_MHZ 48_MHz
#endif  // !defined(SJ2_SYSTEM_CLOCK_RATE)
/// Delcare Constant SYSTEM_CLOCK_RATE_MHZ
SJ2_DECLARE_CONSTANT(SYSTEM_CLOCK_RATE_MHZ,
                     units::frequency::megahertz_t,
                     kSystemClockRateMhz);
static_assert(1_MHz <= kSystemClockRateMhz && kSystemClockRateMhz <= 120_MHz,
              "SJ2_SYSTEM_CLOCK can only be between 1 Hz and 4000 Mhz");

/// Used to set the FreeRTOS tick frequency defined in Hz
#if !defined(SJ2_RTOS_FREQUENCY)
#define SJ2_RTOS_FREQUENCY 1000_Hz
#endif  // !defined(SJ2_RTOS_FREQUENCY)
/// Delcare Constant RTOS_FREQUENCY
SJ2_DECLARE_CONSTANT(RTOS_FREQUENCY, units::frequency::hertz_t, kRtosFrequency);
static_assert(1_Hz <= kRtosFrequency && kRtosFrequency <= 10'000_Hz,
              "SJ2_RTOS_FREQUENCY can only be between 10,000 Hz and 1 Hz");

/// Default baud rate of 38400 divides perfectly with the LPC17xx and LPC40xx
/// UART clock dividers perfectly, where as all other standard baud rates
/// do not.
#if !defined(SJ2_BAUD_RATE)
#define SJ2_BAUD_RATE 38400
#endif  // !defined(SJ2_BAUD_RATE)
/// Delcare Constant BAUD_RATE
SJ2_DECLARE_CONSTANT(BAUD_RATE, uint32_t, kBaudRate);
static_assert(4'800 <= kBaudRate && kBaudRate <= 4'000'000,
              "SJ2_BAUD_RATE must be between 4800 bits/s and 4 MBits/s");

/// Used to dump all the call stack when "PrintBacktrace" is called or an assert
/// using PrintBacktrace is occurs.
/// Disable this to omit getting these logs and reduce the binary size by ~5kB
/// to ~10kB
#if !defined(SJ2_INCLUDE_BACKTRACE)
#define SJ2_INCLUDE_BACKTRACE true
#endif  // !defined(SJ2_INCLUDE_BACKTRACE)
/// Delcare Constant INCLUDE_BACKTRACE
SJ2_DECLARE_CONSTANT(INCLUDE_BACKTRACE, bool, kIncludeBacktrace);

/// Enables vsnprintf which enables full control over printf. Without this,
/// printf cannot print out variables.
#if !defined(SJ2_INCLUDE_VSNPRINTF)
#define SJ2_INCLUDE_VSNPRINTF true
#endif  // !defined(SJ2_INCLUDE_VSNPRINTF)
/// Delcare Constant INCLUDE_VSNPRINTF
SJ2_DECLARE_CONSTANT(INCLUDE_VSNPRINTF, bool, kIncludeVsnprintf);

/// Used to offset the returned addresses from the libunwind GetIP function
/// (get instruction pointer), in order to properly identify the line of code
/// that caused the sjsu::debug::PrintStackTrace function to be called. GetIP
/// will retrieve the next instruction after a function has returned, which
/// usually results in the file lookup showing the line right after the
/// function was called. To fix this on ARM platforms, subtract 4 from the
/// address pointer to move 1 line up to the exact call site.
#if !defined(SJ2_BACKTRACE_ADDRESS_OFFSET)
#define SJ2_BACKTRACE_ADDRESS_OFFSET 4
#endif  // !defined(SJ2_BACKTRACE_ADDRESS_OFFSET)
/// Delcare Constant BACKTRACE_ADDRESS_OFFSET
SJ2_DECLARE_CONSTANT(BACKTRACE_ADDRESS_OFFSET, size_t, kBacktraceAddressOffset);

/// Used to set the default scheduler size for the TaskScheduler.
#if !defined(SJ2_TASK_SCHEDULER_SIZE)
#define SJ2_TASK_SCHEDULER_SIZE 16
#endif  // !defined(SJ2_TASK_SCHEDULER_SIZE)
/// Delcare Constant TASK_SCHEDULER_SIZE
SJ2_DECLARE_CONSTANT(TASK_SCHEDULER_SIZE, uint8_t, kTaskSchedulerSize);

/// Used to set the receiver buffer size of the ESP8266 driver
#if !defined(SJ2_ESP8266_BUFFER_SIZE)
#define SJ2_ESP8266_BUFFER_SIZE 512
#endif  // !defined(SJ2_ESP8266_BUFFER_SIZE)
/// Delcare Constant ESP8266_BUFFER_SIZE
SJ2_DECLARE_CONSTANT(ESP8266_BUFFER_SIZE, size_t, kEsp8266BufferSize);

/// Used to define the log level of the build
#if !defined(SJ2_LOG_LEVEL)
#define SJ2_LOG_LEVEL SJ2_LOG_LEVEL_INFO
#endif  // !defined(SJ2_LOG_LEVEL)
/// Delcare Constant LOG_LEVEL
SJ2_DECLARE_CONSTANT(LOG_LEVEL, uint8_t, kLogLevel);

static_assert(kLogLevel == SJ2_LOG_LEVEL_NONESET ||
              kLogLevel == SJ2_LOG_LEVEL_DEBUG ||
              kLogLevel == SJ2_LOG_LEVEL_INFO ||
              kLogLevel == SJ2_LOG_LEVEL_WARNING ||
              kLogLevel == SJ2_LOG_LEVEL_ERROR,
              "SJ2_LOG_LEVEL must equal to one of the predefined log levels "
              "such as SJ2_LOG_LEVEL_INFO.");

/// If set to true, will display function name in LOG_* function calls.
/// Otherwise omit writing function names.
#if !defined(SJ2_DESCRIPTIVE_FUNCTION_NAME)
#define SJ2_DESCRIPTIVE_FUNCTION_NAME true
#endif  // !defined(SJ2_DESCRIPTIVE_FUNCTION_NAME)
/// Delcare Constant DESCRIPTIVE_FUNCTION_NAME
SJ2_DECLARE_CONSTANT(DESCRIPTIVE_FUNCTION_NAME, bool, kDescriptiveFunctionName);

/// Enable or disable float support in printf statements. Setting to false will
/// reduce binary size.
#if !defined(SJ2_PRINTF_SUPPORT_FLOAT)
#define SJ2_PRINTF_SUPPORT_FLOAT true
#endif  // !defined(PRINTF_SUPPORT_FLOAT)
/// Enables FLOAT support for the 3rd party printf library.
#define PRINTF_SUPPORT_FLOAT SJ2_PRINTF_SUPPORT_FLOAT

/// Enable printing of 64 bit numbers. Setting to false will reduce binary size.
#if !defined(SJ2_PRINTF_SUPPORT_LONG_LONG)
#define SJ2_PRINTF_SUPPORT_LONG_LONG true
#endif  // !defined(PRINTF_SUPPORT_LONG_LONG)
/// Enables LONG LONG support for the 3rd party printf library.
#define PRINTF_SUPPORT_LONG_LONG SJ2_PRINTF_SUPPORT_LONG_LONG

/// Enable support for the ptrdiff_t type. Setting to false will reduce binary
/// size.
#if !defined(SJ2_PRINTF_SUPPORT_PTRDIFF_T)
#define SJ2_PRINTF_SUPPORT_PTRDIFF_T true
#endif  // !defined(PRINTF_SUPPORT_PTRDIFF_T)
/// Enables PTRDIFF support for the 3rd party printf library.
#define PRINTF_SUPPORT_PTRDIFF_T SJ2_PRINTF_SUPPORT_PTRDIFF_T
}  // namespace config
/// @}
