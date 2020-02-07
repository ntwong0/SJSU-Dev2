#include <cstdio>

#include "L1_Peripheral/stm32f4xx/gpio.hpp"
#include "utility/time.hpp"
#include "utility/log.hpp"

int main()
{
  LOG_INFO("Starting FK407M1 Gpio Application...");
  ///////////// Setup LED GPIO /////////////
  sjsu::stm32f4xx::Gpio led('C', 13);
  led.SetAsOutput();

  sjsu::stm32f4xx::Gpio button('A', 15);
  button.SetAsInput();
  button.GetPin().PullUp();

  while (true)
  {
    // If button is not pressed (true), repeat blinking led
    // If the button is held down (false), stop blinking.
    if (button.Read())
    {
      led.SetLow();
      sjsu::Delay(100ms);

      led.SetHigh();
      sjsu::Delay(100ms);
    }
    else
    {
      led.SetHigh();
    }
  }
  return 0;
}
