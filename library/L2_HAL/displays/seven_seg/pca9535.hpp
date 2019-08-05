#pragma once

#include "L1_Peripheral/i2c.hpp"
// #include "utility/bit.hpp"

namespace sjsu
{
class Pca9535 final
{
 public:
  // static constexpr uint8_t kDevI2CBaseAddr         = 0x40; // 8 bit width, lsb is r/w
  static constexpr uint8_t kDevI2CBaseAddr         = 0x40 >> 1; // 8 bit width, lsb is r/w
  
  // bitwise association to pca9535's io port: msb is pin7 -> lsb is pin0
  // Input Port reg values reflect voltage levels
  // **These registers are READ ONLY**
  //   Get as: 0 for V_L, 1 for V_H
  static constexpr uint8_t kCtrlRegAddrInputPort0  = 0x00; // power-on init value: 8b'xxxx_xxxx
  static constexpr uint8_t kCtrlRegAddrInputPort1  = 0x01; // power-on init value: 8b'xxxx_xxxx
  
  // Output Port reg values reflect intended voltage levels
  // If a pin is config'd as output, then the associated bit value in the register will have no effect
  //   Set as: 0 for V_L, 1 for V_H
  static constexpr uint8_t kCtrlRegAddrOutputPort0 = 0x02; // power-on init value: 8b'1111_1111
  static constexpr uint8_t kCtrlRegAddrOutputPort1 = 0x03; // power-on init value: 8b'1111_1111
  
  // Polarity Inversion reg values reflect polarity inversion of input pins
  // Bits read from polarity inverted input pins will read 0 for V_H, 1 for V_L
  //   Set as: 0 for no inversion, 1 for inversion
  static constexpr uint8_t kCtrlRegAddrPolInvPort0 = 0x04; // power-on init value: 8b'0000_0000
  static constexpr uint8_t kCtrlRegAddrPolInvPort1 = 0x05; // power-on init value: 8b'0000_0000

  // Config Port reg values reflect I/O state
  //   Set to: 0 for output, 1 for input
  static constexpr uint8_t kCtrlRegAddrConfigPort0 = 0x06; // power-on init value: 8b'1111_1111
  static constexpr uint8_t kCtrlRegAddrConfigPort1 = 0x07; // power-on init value: 8b'1111_1111

  explicit Pca9535(I2c & i2c, uint8_t address = kDevI2CBaseAddr)
      : i2c_(i2c), address_(address)
  {
  }

  Status Initialize()
  {
    i2c_.Initialize();
    return i2c_.Write(address_, { kCtrlRegAddrConfigPort0, 0x0f, 0xf0 }, 1000U);
  }

  void SetOutputs(uint8_t value_port0, uint8_t value_port1)
  {
    i2c_.Write(address_, { kCtrlRegAddrOutputPort0, value_port0, value_port1 });
  }

  uint16_t GetInputs()
  {
    uint8_t value_port0 = 0;
    uint8_t value_port1 = 0;
    i2c_.WriteThenRead(address_, { kCtrlRegAddrInputPort0 }, &value_port0, 2);
    i2c_.Read(address_, &value_port1, 1);
    return (uint16_t)((value_port0 << 8) | value_port1);
  }

  // float GetCelsius()
  // {
  //   // Note that: 1 << 14 = 2^14 = 16384
  //   constexpr int32_t kSubtractTemperatureData = 1 << 14;

  //   uint8_t most_significant_register;
  //   uint8_t least_significant_register;
  //   // The register will enable the device to collect data once
  //   // and automatically sets the stop bit to 0 (2nd bit).
  //   i2c_.Write(address_, { kOneBurstRegister, 0x04 }, 2);
  //   // Auto increments I2c register address pointer.
  //   i2c_.Write(address_, { kAutomaticBitRegister, 0x01 }, 2);
  //   i2c_.WriteThenRead(
  //       address_, { kMostSignificantRegister }, &most_significant_register, 1);
  //   i2c_.Read(address_, &least_significant_register, 1);

  //   // The write and read operation sets the most significant bit to one,
  //   // telling the register that the data has been read.
  //   // Therefore must clear the most significant bit from above.
  //   most_significant_register = bit::Clear(most_significant_register, 7);
  //   // Combine temperature register data
  //   int32_t temperature_data =
  //       (most_significant_register << 8) | least_significant_register;
  //   // The required computation after bit shifting.
  //   // Formula can be found below, see page 4:
  //   // datasheets/Temperature-Sensor/si7060-datasheets.pdf
  //   float temperature =
  //       static_cast<float>(temperature_data - kSubtractTemperatureData);
  //   temperature = (temperature / 160.0f) + 55.0f;

  //   return temperature;
  // }

  // float GetFahrenheit() override
  // {
  //   // General formula used to convert Celsius to Fahrenheit
  //   return (((GetCelsius() * 9.0f) / 5.0f) - 32.0f);
  // }

 private:
  const I2c & i2c_;
  uint8_t address_;
};
}  // namespace sjsu
