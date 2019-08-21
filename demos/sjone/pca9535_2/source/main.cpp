#include <cstdint>
#include <iterator>

#include "utility/log.hpp"
#include "utility/time.hpp"
#include "L0_Platform/lpc17xx/LPC17xx.h"
#include "L2_HAL/boards/sjone.hpp"
#include "L2_HAL/displays/seven_seg/pca9535_2.hpp"

int main()
{
  sjone board;
  static sjsu::lpc17xx::LPC_SC_TypeDef * system_controller 
    = (( sjsu::lpc17xx::LPC_SC_TypeDef *) LPC_SC_BASE );
  LOG_INFO("PCONP: 0x%X", system_controller->PCONP); // check bit 26, should be 1

  static sjsu::lpc17xx::LPC_PINCON_TypeDef * pin_configurator
    = (( sjsu::lpc17xx::LPC_PINCON_TypeDef *) LPC_PINCON_BASE );
  LOG_INFO("PINSEL0 before: 0x%X", pin_configurator->PINSEL0); // check bit 23:20, should be 1010
  pin_configurator->PINSEL0 |= 0xA << 20;
  LOG_INFO("PINSEL0 after: 0x%X", pin_configurator->PINSEL0); // check bit 23:20, should be 1010
  
  LOG_INFO("i2c2 init: %s", sjsu::Stringify(board.i2c2.Initialize()));

  // auto retval = sjsu::Stringify(board.SevenSeg().Initialize());
  // LOG_INFO("seven_seg init: %s", retval);
  
  sjsu::pca9535::Pca9535 seven_seg(board.i2c2);
  LOG_INFO("seven_seg init: %s", sjsu::Stringify(seven_seg.Initialize()));
  seven_seg.ConfigToOutput();

  static constexpr uint8_t kSevenSegCharMap[16] =
  {
             //      PGFE_DCBA
      0x3f,  // 0: 8'b0011_1111
      0x06,  // 1: 8'b0000_0110
      0x5b,  // 2: 8'b0101_1011
      0x4f,  // 3: 8'b0100_1111
      0x66,  // 4: 8'b0110_0110
      0x6d,  // 5: 8'b0110_1101
      0x7d,  // 6: 8'b0111_1101
      0x07,  // 7: 8'b0000_0111
      0x7f,  // 8: 8'b0111_1111
      0x67,  // 9: 8'b0110_0111
      0x77,  // a: 8'b0111_0111
      0x7c,  // b: 8'b0111_1100
      0x58,  // c: 8'b0101_1000
      0x5e,  // d: 8'b0101_1110
      0x79,  // e: 8'b0111_1001
      0x71   // f: 8'b0111_0001
  };

  sjsu::pca9535::GpioParallelBus seg_bus = {
    seven_seg.GetGpio(0,1),
    seven_seg.GetGpio(0,0),
    seven_seg.GetGpio(0,2),
    seven_seg.GetGpio(0,3),
    seven_seg.GetGpio(0,4),
    seven_seg.GetGpio(0,5),
    seven_seg.GetGpio(0,6),
    seven_seg.GetGpio(0,7)
  };

  // ptr = &seven_seg.GetGpio(0,0);
  sjsu::pca9535::Gpio urkle(0,3,board.i2c2);
  // sjsu::pca9535::Gpio *ptr = &urkle; // success
  // sjsu::pca9535::Gpio *ptr = new sjsu::pca9535::Gpio(0,3,board.i2c2); // failure at time delay (why is this related?)
  const sjsu::pca9535::Gpio *ptr = &(seven_seg.GetGpio(0,1)); // failure at SetHigh(), port value was reportedly 0x2a???

  while (true)
  {
    for (uint8_t i = 0; i < 16; i++)
    {
      LOG_INFO("Hello World 0x%X", i);
      // board.SevenSeg().SetOutputPorts(board.SevenSeg().kSevenSegCharMap[i], board.SevenSeg().kSevenSegCharMap[i]);
      // sjsu::Delay(500);
      seven_seg.SetAll(sjsu::Gpio::State::kLow);
      sjsu::Delay(500);
      // for(uint8_t j = 0; j < 8; j++)
      // {
      //   kSevenSegCharMap[i] & (1 << j) ? seg_bus[j].SetHigh() : seg_bus[j].SetLow();
      //   // kSevenSegCharMap[i] & (1 << j) ? seven_seg.GetGpio(0,j).Set(sjsu::Gpio::State::kHigh) : seven_seg.GetGpio(0,j).SetLow();
      // }
      ptr->SetHigh();
      sjsu::Delay(500);
      seven_seg.SetAll(sjsu::Gpio::State::kHigh);
      sjsu::Delay(500);
    }
  }

  return -1;
}
