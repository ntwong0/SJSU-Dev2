#pragma once

#include "L1_Peripheral/i2c.hpp"

namespace sjsu
{
class Pca9535
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

  Status Initialize()
  {
    i2c_.Initialize();
    return i2c_.Write(address_, { kCtrlRegAddrConfigPort0, 0x00, 0x00 });
  }

  // Set pins as inputs or outputs
  Status SetConfigPorts(uint8_t config0, uint8_t config1)
  {
    return i2c_.Write(address_, { kCtrlRegAddrConfigPort0, config0, config1 });
  }

  Status SetConfigPort0(uint8_t config0)
  {
    return i2c_.Write(address_, { kCtrlRegAddrConfigPort0, config0 });
  }

  Status SetConfigPort1(uint8_t config1)
  {
    return i2c_.Write(address_, { kCtrlRegAddrConfigPort1, config1 });
  }

  Status SetPolarityPorts(uint8_t pol0, uint8_t pol1)
  {
    return i2c_.Write(address_, { kCtrlRegAddrPolInvPort0, pol0, pol1 });
  }

  Status SetPolarityPort0(uint8_t pol0)
  {
    return i2c_.Write(address_, { kCtrlRegAddrPolInvPort0, pol0 });
  }

  Status SetPolarityPort1(uint8_t pol1)
  {
    return i2c_.Write(address_, { kCtrlRegAddrPolInvPort1, pol1 });
  }

  Status SetOutputPorts(uint8_t outval0, uint8_t outval1)
  {
    return i2c_.Write(address_, { kCtrlRegAddrOutputPort0, outval0, outval1 });
  }

  Status SetOutputPort0(uint8_t outval0)
  {
    return i2c_.Write(address_, { kCtrlRegAddrOutputPort0, outval0 });
  }

  Status SetOutputPort1(uint8_t outval1)
  {
    return i2c_.Write(address_, { kCtrlRegAddrOutputPort1, outval1 });
  }

  Status GetInputPorts(uint16_t *inval)
  {
    uint8_t inputs[2];
    i2c_.Write(address_, { kCtrlRegAddrInputPort0 });
    Status retval = i2c_.Read(address_, inputs, 2);
    *inval = static_cast<uint16_t>(inputs[0]) | static_cast<uint16_t>(inputs[1]) << 8;
    return retval;
  }

  Status GetInputPorts(uint8_t *inval0, uint8_t *inval1)
  {
    uint8_t inputs[2];
    i2c_.Write(address_, { kCtrlRegAddrInputPort0 });
    Status retval = i2c_.Read(address_, inputs, 2);
    *inval0 = inputs[0];
    *inval1 = inputs[1];
    return retval;
  }

  Status GetInputPort0(uint8_t *inval0)
  {
    i2c_.Write(address_, { kCtrlRegAddrInputPort0 });
    return i2c_.Read(address_, inval0, 1);
  }

  Status GetInputPort1(uint8_t *inval1)
  {
    i2c_.Write(address_, { kCtrlRegAddrInputPort1 });
    return i2c_.Read(address_, inval1, 1);
  }

 private:
  const I2c & i2c_;
  uint8_t address_;
};
}  // namespace sjsu
