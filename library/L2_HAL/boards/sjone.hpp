#pragma once

#include "L1_Peripheral/lpc17xx/adc.hpp"
#include "L1_Peripheral/lpc17xx/gpio.hpp"
#include "L1_Peripheral/lpc17xx/i2c.hpp"
#include "L1_Peripheral/lpc17xx/spi.hpp"

// #include "L2_HAL/displays/led/onboard_led.hpp"
#include "L2_HAL/displays/seven_seg/pca9535.hpp"
#include "L2_HAL/sensors/environment/airflow/hafuht0020l4axt.hpp"
#include "L2_HAL/memory/sd.hpp"
#include "L2_HAL/sensors/environment/light/temt6000x01.hpp"
#include "L2_HAL/sensors/environment/temperature/tmp102.hpp"
#include "L2_HAL/sensors/movement/accelerometer/mma8452q.hpp"

#include "L3_Application/graphics.hpp"

struct sjone // NOLINT
{
  static constexpr sjsu::lpc17xx::SystemController kLpc17xxSystemController =
      sjsu::lpc17xx::SystemController();

  inline static sjsu::lpc17xx::Adc adc2 =
      sjsu::lpc17xx::Adc(sjsu::lpc17xx::AdcChannel::kChannel2);

  inline static sjsu::lpc40xx::Gpio led0 = sjsu::lpc40xx::Gpio(1, 0);
  inline static sjsu::lpc40xx::Gpio led1 = sjsu::lpc40xx::Gpio(1, 1);
  inline static sjsu::lpc40xx::Gpio led2 = sjsu::lpc40xx::Gpio(1, 4);
  inline static sjsu::lpc40xx::Gpio led3 = sjsu::lpc40xx::Gpio(1, 8);

  inline static sjsu::lpc17xx::I2c i2c0 =
      sjsu::lpc17xx::I2c(sjsu::lpc17xx::I2c::Bus::kI2c0);
  inline static sjsu::lpc17xx::I2c i2c1 =
      sjsu::lpc17xx::I2c(sjsu::lpc17xx::I2c::Bus::kI2c1);
  inline static sjsu::lpc17xx::I2c i2c2 =
      sjsu::lpc17xx::I2c(sjsu::lpc17xx::I2c::Bus::kI2c2);

  inline static sjsu::lpc17xx::Spi spi0 =
      sjsu::lpc17xx::Spi(sjsu::lpc17xx::Spi::Bus::kSpi0);
  inline static sjsu::lpc17xx::Spi spi1 =
      sjsu::lpc17xx::Spi(sjsu::lpc17xx::Spi::Bus::kSpi1);
  inline static sjsu::lpc17xx::Spi spi2 =
      sjsu::lpc17xx::Spi(sjsu::lpc17xx::Spi::Bus::kSpi2);

//   [[gnu::always_inline]] inline static sjsu::OnBoardLed & Leds()
//   {
//     static sjsu::OnBoardLed leds;
//     return leds;
//   }

  [[gnu::always_inline]] inline static sjsu::Pca9535 & SevenSeg()
  {
      static sjsu::Pca9535 seven_seg(i2c2);
      return seven_seg;
  }

  [[gnu::always_inline]] inline static sjsu::Hafuht0020l4axt & Flow()
  {
      static sjsu::Hafuht0020l4axt flow(i2c2);
      return flow;
  }

  // [[gnu::always_inline]] inline static sjsu::Sd & SdCard()
  // {
  //   static sjsu::lpc17xx::Gpio sd_cs = sjsu::lpc17xx::Gpio(1, 8);
  //   static sjsu::Sd sd(spi2, sd_cs);
  //   return sd;
  // }
  
  [[gnu::always_inline]] inline static sjsu::Temt6000x01 & LightSensor()
  {
    // The LPC176x/5x ADC has a reference voltage of 3.3V.
    constexpr units::voltage::volt_t kAdcReferenceVoltage = 3.3_V;
    // A 10kOhm pull-down resistor is used on the SJOne board.
    constexpr units::impedance::ohm_t kPullDownResistance = 10_kOhm;
    static sjsu::Temt6000x01 temt6000(
        adc2, kAdcReferenceVoltage, kPullDownResistance);
    return temt6000;
  }

  [[gnu::always_inline]] inline static sjsu::Tmp102 & Temperature()
  {
    static sjsu::Tmp102 tmp102(i2c2);
    return tmp102;
  }

  [[gnu::always_inline]] inline static sjsu::Mma8452q & Accelerometer()
  {
    static sjsu::Mma8452q accelerometer(i2c2);
    return accelerometer;
  }
};

// TODO(#___):move this pattern to a generic seven seg implementation
static constexpr uint8_t kSevenSegHexadeciMap[16] =
{          /*       PGFE_DCBA         _____         */
    0x3f,  /* 0: 8'b0011_1111        /  A  \        */
    0x06,  /* 1: 8'b0000_0110        \_____/        */
    0x5b,  /* 2: 8'b0101_1011     /\         /\     */
    0x4f,  /* 3: 8'b0100_1111    |  |       |  |    */
    0x66,  /* 4: 8'b0110_0110    | F|       | B|    */
    0x6d,  /* 5: 8'b0110_1101    |  |       |  |    */
    0x7d,  /* 6: 8'b0111_1101     \/  _____  \/     */
    0x07,  /* 7: 8'b0000_0111        /  G  \        */
    0x7f,  /* 8: 8'b0111_1111        \_____/        */
    0x67,  /* 9: 8'b0110_0111     /\         /\     */
    0x77,  /* a: 8'b0111_0111    |  |       |  |    */
    0x7c,  /* b: 8'b0111_1100    | E|       | C|    */
    0x58,  /* c: 8'b0101_1000    |  |       |  |    */
    0x5e,  /* d: 8'b0101_1110     \/  _____  \/  _  */
    0x79,  /* e: 8'b0111_1001        /  D  \    /P\ */
    0x71   /* f: 8'b0111_0001        \_____/    \_/ */
};
