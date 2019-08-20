#include <cstdint>
#include <iterator>

#include "utility/log.hpp"
#include "utility/time.hpp"
#include "L0_Platform/lpc17xx/LPC17xx.h"
#include "L2_HAL/boards/sjone.hpp"

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

  auto retval = sjsu::Stringify(board.SevenSeg().Initialize());
  LOG_INFO("seven_seg init: %s", retval);

  while (true)
  {
    for (uint8_t i = 0; i < 16; i++)
    {
      LOG_INFO("Hello World 0x%X", i);
      board.SevenSeg().SetOutputPorts(board.SevenSeg().kSevenSegCharMap[i], board.SevenSeg().kSevenSegCharMap[i]);
      sjsu::Delay(500);
    }
  }

  return -1;
}
