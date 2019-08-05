#pragma once

#include "L1_Peripheral/lpc17xx/gpio.hpp"
#include "L1_Peripheral/lpc17xx/i2c.hpp"
#include "L1_Peripheral/lpc17xx/spi.hpp"

#include "L2_HAL/displays/led/onboard_led.hpp"
// #include "L2_HAL/displays/oled/ssd1306.hpp"
#include "L2_HAL/displays/seven_seg/pca9535.hpp"
#include "L2_HAL/memory/sd.hpp"
#include "L2_HAL/sensors/environment/temperature/si7060.hpp"
#include "L2_HAL/sensors/movement/accelerometer/mma8452q.hpp"
#include "L2_HAL/sensors/optical/apds9960.hpp"

#include "L3_Application/graphics.hpp"

struct sjone // NOLINT
{
  inline static sjsu::lpc17xx::Spi spi0 =
      sjsu::lpc17xx::Spi(sjsu::lpc17xx::Spi::Bus::kSpi0);
  inline static sjsu::lpc17xx::Spi spi1 =
      sjsu::lpc17xx::Spi(sjsu::lpc17xx::Spi::Bus::kSpi1);
  inline static sjsu::lpc17xx::Spi spi2 =
      sjsu::lpc17xx::Spi(sjsu::lpc17xx::Spi::Bus::kSpi2);

  inline static sjsu::lpc17xx::I2c i2c0 =
      sjsu::lpc17xx::I2c(sjsu::lpc17xx::I2c::Bus::kI2c0);
  inline static sjsu::lpc17xx::I2c i2c1 =
      sjsu::lpc17xx::I2c(sjsu::lpc17xx::I2c::Bus::kI2c1);
//   inline static sjsu::lpc17xx::I2c i2c2 =
//       sjsu::lpc17xx::I2c(sjsu::lpc17xx::I2c::Bus::kI2c2);

  // [[gnu::always_inline]] inline static sjsu::Graphics & Oled()
  // {
  //   static sjsu::lpc17xx::Gpio oled_cs = sjsu::lpc17xx::Gpio(1, 22);
  //   static sjsu::lpc17xx::Gpio oled_dc = sjsu::lpc17xx::Gpio(1, 25);
  //   static sjsu::Ssd1306 oled_display(spi1, oled_cs, oled_dc);
  //   static sjsu::Graphics oled(&oled_display);
  //   return oled;
  // }

  [[gnu::always_inline]] inline static sjsu::OnBoardLed & Leds()
  {
    static sjsu::OnBoardLed leds;
    return leds;
  }

//   [[gnu::always_inline]] inline static sjsu::Pca9535 & SevenSeg()
//   {
//       static sjsu::Pca9535 seven_seg(i2c2);
//       return seven_seg;
//   }

//   [[gnu::always_inline]] inline static sjsu::Mma8452q & Accelerometer()
//   {
//     static sjsu::Mma8452q accelerometer(i2c2);
//     return accelerometer;
//   }

  // [[gnu::always_inline]] inline static sjsu::Sd & SdCard()
  // {
  //   static sjsu::lpc17xx::Gpio sd_cs = sjsu::lpc17xx::Gpio(1, 8);
  //   static sjsu::Sd sd(spi2, sd_cs);
  //   return sd;
  // }

  // [[gnu::always_inline]] inline static sjsu::Apds9960 & Gesture()
  // {
  //   static sjsu::Apds9960 apds9960(i2c2);
  //   return apds9960;
  // }

  // [[gnu::always_inline]] inline static sjsu::Si7060 & Temperature()
  // {
  //   static sjsu::Si7060 si7060(i2c2);
  //   return si7060;
  // }
};
