#pragma once

#include "L1_Peripheral/i2c.hpp"
// #include "utility/bit.hpp"

namespace sjsu
{
class Pca9535 final
{
 public:
  // Note: The 8 bit slave addr following the start bit at the start of every I2C transaction
  //       has the following structure: 
  //               (uint8_t)(kDevI2CBaseAddr << 1 | i2c_read_or_nwrite_bit)
  static constexpr uint8_t kDevI2CBaseAddr         = 0x20; // 7 bit width (r/w excluded, ignore msb)
  
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

  // The following is specific to the sjone board
  // @todo migrate to sjone.hpp
  // @todo write a constexpr to parse "8'bxxxx_xxxx" into uint8_t
  static constexpr uint8_t kSevenSegCharMap[16] =
  {
           //       PGFE_DCBA
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

  Status Initialize()
  {
    i2c_.Initialize();
    return i2c_.Write(address_, { kCtrlRegAddrConfigPort0, 0x00, 0x00 });
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

 private:
  const I2c & i2c_;
  uint8_t address_;
};
}  // namespace sjsu
