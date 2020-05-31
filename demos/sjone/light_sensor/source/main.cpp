#include <cstdint>
#include <iterator>

#include "utility/log.hpp"
#include "utility/time.hpp"
#include "L0_Platform/lpc17xx/LPC17xx.h"
#include "L2_HAL/boards/sjone.hpp"
#include "L2_HAL/displays/seven_seg/pca9535.hpp"
#include "L2_HAL/sensors/environment/airflow/hafuht0020l4axt.hpp"

int main()
{
  LOG_INFO("Starting hafuht0020l4axt example...");
  sjone board;
  static sjsu::lpc17xx::LPC_SC_TypeDef * system_controller 
    = (( sjsu::lpc17xx::LPC_SC_TypeDef *) sjsu::lpc17xx::LPC_SC_BASE );
  LOG_INFO("PCONP[0]: 0x%lX", system_controller->PCONP >> 26 & 1); // check bit 26, should be 1

  static sjsu::lpc17xx::LPC_PINCON_TypeDef * pin_configurator
    = (( sjsu::lpc17xx::LPC_PINCON_TypeDef *) sjsu::lpc17xx::LPC_PINCON_BASE );
  LOG_INFO("PINSEL0[23:20] before: 0x%lX", pin_configurator->PINSEL0 >> 20 & 0b1010); // check bit 23:20, should be 4'b1010
  pin_configurator->PINSEL0 &= ~(0b1111 << 20);
  pin_configurator->PINSEL0 |= 0b1010 << 20;
  LOG_INFO("PINSEL0[23:20] after:  0x%lX", pin_configurator->PINSEL0 >> 20 & 0b1010); // check bit 23:20, should be 4'b1010
  
  // LOG_INFO("seven_seg init: %s", sjsu::Stringify(board.SevenSeg().Initialize()));
  LOG_INFO("flow init: %s", sjsu::Stringify(board.Flow().Initialize()));
  
  // board.SevenSeg().ConfigAllToOutput();

  // sjsu::Pca9535::GpioParallelBus seg_bus = {
  //   board.SevenSeg().GetGpio(0,0),
  //   board.SevenSeg().GetGpio(0,1),
  //   board.SevenSeg().GetGpio(0,2),
  //   board.SevenSeg().GetGpio(0,3),
  //   board.SevenSeg().GetGpio(0,4),
  //   board.SevenSeg().GetGpio(0,5),
  //   board.SevenSeg().GetGpio(0,6),
  //   board.SevenSeg().GetGpio(0,7)
  // };

  while (true)
  {
    for (uint8_t i = 0; i < 16; i++)
    {
      // LOG_INFO("Hello World 0x%X", i);
      
      // board.SevenSeg().SetAll(sjsu::Gpio::State::kLow);
      // sjsu::Delay(500ms);

      // TODO(#___):ensure GpioParallelBus has member Set(uint8_t val)
      // for(uint8_t j = 0; j < 8; j++)
      // {
      //   kSevenSegHexadeciMap[i] & (1 << j) ? seg_bus[j].SetHigh() : seg_bus[j].SetLow();
      // }
      // sjsu::Delay(500ms);
      LOG_INFO("flowrate: %f", board.Flow().GetFlowrate());
      // board.SevenSeg().SetAll(sjsu::Gpio::State::kHigh);
      sjsu::Delay(100ms);
    }
  }

  return -1;
}




// #include "L1_Peripheral/lpc17xx/adc.hpp"
// #include "L1_Peripheral/lpc17xx/system_controller.hpp"
// #include "L2_HAL/sensors/environment/light/temt6000x01.hpp"
// #include "utility/log.hpp"

// int main()
// {
//   LOG_INFO("Starting LPC176x/5x Light Sensor Example...");

//   sjsu::lpc17xx::SystemController system_controller;
//   system_controller.SetPeripheralClockDivider(
//       sjsu::lpc17xx::SystemController::Peripherals::kAdc, 8);
//   sjsu::SystemController::SetPlatformController(&system_controller);
//   // The LPC176x/5x ADC has a reference voltage of 3.3V.
//   constexpr units::voltage::volt_t kAdcReferenceVoltage = 3.3_V;
//   // A 10kOhm pull-down resistor is used on the SJOne board.
//   constexpr units::impedance::ohm_t kPullDownResistance = 10'000_Ohm;
//   // The SJOne board uses ADC Channel 0.2, P0.25 for the light sensor
//   sjsu::lpc17xx::Adc adc2(sjsu::lpc17xx::AdcChannel::kChannel2);

//   sjsu::Temt6000x01 light_sensor(
//       adc2, kAdcReferenceVoltage, kPullDownResistance);
//   SJ2_ASSERT_FATAL(light_sensor.Initialize() == sjsu::Status::kSuccess,
//                    "Failed to initialized light sensor!");

//   while (true)
//   {
//     LOG_INFO("Lux: %.4f, Brightness Percentage: %.2f%%",
//              light_sensor.GetIlluminance().to<double>(),
//              static_cast<double>(light_sensor.GetPercentageBrightness() * 100));
//     sjsu::Delay(1s);
//   }

//   return 0;
// }
