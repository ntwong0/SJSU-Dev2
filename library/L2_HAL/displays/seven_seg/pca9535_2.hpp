#pragma once

#include "L1_Peripheral/i2c.hpp"
#include "L1_Peripheral/gpio.hpp"
// #include "L1_Peripheral/pin.hpp"
#include "L1_Peripheral/inactive.hpp"
#include "utility/bit.hpp"

namespace sjsu
{
namespace pca9535
{
/* Actually, just use GetInactive<Pin>();
// for pca9535, Pin is purely for storing port/pin values, and provides an interface to access these values
class Pin final : public sjsu::Pin
{
  public:
    // obligatory implementation of pure virtuals, but pca9535 lacks these, so return void
    void SetPinFunction(uint8_t function) const override              { return; }
    void SetPull(Resistor mode) const override                        { return; }
    void SetAsOpenDrain(bool set_as_open_drain = true) const override { return; }
    void SetAsAnalogMode(bool set_as_analog = true) const override    { return; }
    // inherit constructors
    constexpr Pin(uint8_t port, uint8_t pin) : sjsu::Pin(port, pin) {}
};
*/

class Gpio final : public sjsu::Gpio
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

  // Specifies number of ports and pins that can be used
  static constexpr uint8_t kNumberOfPins  = 8;
  static constexpr uint8_t kNumberOfPorts = 2;

  Gpio(uint8_t port_number,
       uint8_t pin_number,
       const I2c & i2c, uint8_t address = kDevI2CBaseAddr)
      : port_(port_number), pin_(pin_number), i2c_(i2c), address_(address)
  {
      // TODO add assertions on port, pin, and address
  }

  // I2C-specific methods

  Status Initialize() const
  {
    i2c_.Initialize();
    return i2c_.Write(address_, { kCtrlRegAddrConfigPort0, 0x00, 0x00 });
  }

  // Set pins as inputs or outputs
  Status SetConfigPorts(uint8_t config0, uint8_t config1) const
  {
    return i2c_.Write(address_, { kCtrlRegAddrConfigPort0, config0, config1 });
  }

  Status SetConfigPort0(uint8_t config0) const
  {
    return i2c_.Write(address_, { kCtrlRegAddrConfigPort0, config0 });
  }

  Status SetConfigPort1(uint8_t config1) const
  {
    return i2c_.Write(address_, { kCtrlRegAddrConfigPort1, config1 });
  }

  Status GetConfigPorts(uint16_t *inval) const
  {
    uint8_t inputs[2];
    i2c_.Write(address_, { kCtrlRegAddrConfigPort0 });
    Status retval = i2c_.Read(address_, inputs, 2);
    *inval = static_cast<uint16_t>(inputs[0]) | static_cast<uint16_t>(inputs[1]) << 8;
    return retval;
  }

  Status GetConfigPorts(uint8_t *inval0, uint8_t *inval1) const
  {
    uint8_t inputs[2];
    i2c_.Write(address_, { kCtrlRegAddrConfigPort0 });
    Status retval = i2c_.Read(address_, inputs, 2);
    *inval0 = inputs[0];
    *inval1 = inputs[1];
    return retval;
  }

  Status GetConfigPort0(uint8_t *inval0) const
  {
    i2c_.Write(address_, { kCtrlRegAddrConfigPort0 });
    return i2c_.Read(address_, inval0, 1);
  }

  Status GetConfigPort1(uint8_t *inval1) const
  {
    i2c_.Write(address_, { kCtrlRegAddrConfigPort1 });
    return i2c_.Read(address_, inval1, 1);
  }

  Status SetPolarityPorts(uint8_t pol0, uint8_t pol1) const
  {
    return i2c_.Write(address_, { kCtrlRegAddrPolInvPort0, pol0, pol1 });
  }

  Status SetPolarityPort0(uint8_t pol0) const
  {
    return i2c_.Write(address_, { kCtrlRegAddrPolInvPort0, pol0 });
  }

  Status SetPolarityPort1(uint8_t pol1) const
  {
    return i2c_.Write(address_, { kCtrlRegAddrPolInvPort1, pol1 });
  }

  Status GetPolarityPorts(uint16_t *inval) const
  {
    uint8_t inputs[2];
    i2c_.Write(address_, { kCtrlRegAddrPolInvPort0 });
    Status retval = i2c_.Read(address_, inputs, 2);
    *inval = static_cast<uint16_t>(inputs[0]) | static_cast<uint16_t>(inputs[1]) << 8;
    return retval;
  }

  Status GetPolarityPorts(uint8_t *inval0, uint8_t *inval1) const
  {
    uint8_t inputs[2];
    i2c_.Write(address_, { kCtrlRegAddrPolInvPort0 });
    Status retval = i2c_.Read(address_, inputs, 2);
    *inval0 = inputs[0];
    *inval1 = inputs[1];
    return retval;
  }

  Status GetPolarityPort0(uint8_t *inval0) const
  {
    i2c_.Write(address_, { kCtrlRegAddrPolInvPort0 });
    return i2c_.Read(address_, inval0, 1);
  }

  Status GetPolarityPort1(uint8_t *inval1) const
  {
    i2c_.Write(address_, { kCtrlRegAddrPolInvPort1 });
    return i2c_.Read(address_, inval1, 1);
  }

  Status SetOutputPorts(uint8_t outval0, uint8_t outval1) const
  {
    return i2c_.Write(address_, { kCtrlRegAddrOutputPort0, outval0, outval1 });
  }

  Status SetOutputPort0(uint8_t outval0) const
  {
    return i2c_.Write(address_, { kCtrlRegAddrOutputPort0, outval0 });
  }

  Status SetOutputPort1(uint8_t outval1) const
  {
    return i2c_.Write(address_, { kCtrlRegAddrOutputPort1, outval1 });
  }

  Status GetOutputPorts(uint16_t *inval) const
  {
    uint8_t inputs[2];
    i2c_.Write(address_, { kCtrlRegAddrOutputPort0 });
    Status retval = i2c_.Read(address_, inputs, 2);
    *inval = static_cast<uint16_t>(inputs[0]) | static_cast<uint16_t>(inputs[1]) << 8;
    return retval;
  }

  Status GetOutputPorts(uint8_t *inval0, uint8_t *inval1) const
  {
    uint8_t inputs[2];
    i2c_.Write(address_, { kCtrlRegAddrOutputPort0 });
    Status retval = i2c_.Read(address_, inputs, 2);
    *inval0 = inputs[0];
    *inval1 = inputs[1];
    return retval;
  }

  Status GetOutputPort0(uint8_t *inval0) const
  {
    i2c_.Write(address_, { kCtrlRegAddrOutputPort0 });
    return i2c_.Read(address_, inval0, 1);
  }

  Status GetOutputPort1(uint8_t *inval1) const
  {
    i2c_.Write(address_, { kCtrlRegAddrOutputPort1 });
    return i2c_.Read(address_, inval1, 1);
  }

  Status GetInputPorts(uint16_t *inval) const
  {
    uint8_t inputs[2];
    i2c_.Write(address_, { kCtrlRegAddrInputPort0 });
    Status retval = i2c_.Read(address_, inputs, 2);
    *inval = static_cast<uint16_t>(inputs[0]) | static_cast<uint16_t>(inputs[1]) << 8;
    return retval;
  }

  Status GetInputPorts(uint8_t *inval0, uint8_t *inval1) const
  {
    uint8_t inputs[2];
    i2c_.Write(address_, { kCtrlRegAddrInputPort0 });
    Status retval = i2c_.Read(address_, inputs, 2);
    *inval0 = inputs[0];
    *inval1 = inputs[1];
    return retval;
  }

  Status GetInputPort0(uint8_t *inval0) const
  {
    i2c_.Write(address_, { kCtrlRegAddrInputPort0 });
    return i2c_.Read(address_, inval0, 1);
  }

  Status GetInputPort1(uint8_t *inval1) const
  {
    i2c_.Write(address_, { kCtrlRegAddrInputPort1 });
    return i2c_.Read(address_, inval1, 1);
  }

  // GPIO interface methods

  void SetDirection(Direction direction) const override
  {
    if(port_) // Port1
    {
      uint8_t val = 0;
      GetConfigPort1(&val);
      direction == kInput ? bit::Set(val, pin_) : bit::Clear(val, pin_);
      SetConfigPort1(val);
    }
    else // Port0
    {
      uint8_t val = 0;
      GetConfigPort0(&val);
      direction == kInput ? bit::Set(val, pin_) : bit::Clear(val, pin_);
      SetConfigPort0(val);
    }
  }

  void Set(State output) const override
  {
    if(port_) // Port1
    {
      uint8_t val = 0;
      GetOutputPort1(&val);
      output == kHigh ? bit::Set(val, pin_) : bit::Clear(val, pin_);
      SetOutputPort1(val);
    }  
    else // Port0
    {
      uint8_t val = 0;
      GetOutputPort0(&val);
      output == kHigh ? bit::Set(val, pin_) : bit::Clear(val, pin_);
      SetOutputPort0(val);
    }
  }

  void Toggle() const override
  {
    if(port_) // Port1
    {
      uint8_t val = 0;
      GetOutputPort1(&val);
      bit::Read(val, pin_) ? bit::Clear(val, pin_) : bit::Set(val, pin_);
      SetOutputPort1(val);
    }  
    else // Port0
    {
      uint8_t val = 0;
      GetOutputPort0(&val);
      bit::Read(val, pin_) ? bit::Clear(val, pin_) : bit::Set(val, pin_);
      SetOutputPort0(val);
    }
  }

  bool Read() const override
  {
    bool retval = false;
    if(port_) // Port1
    {
      uint8_t val = 0;
      GetInputPort1(&val);
      retval = bit::Read(val, pin_);
    }  
    else // Port0
    {
      uint8_t val = 0;
      GetInputPort1(&val);
      retval = bit::Read(val, pin_);
    }
    return retval;
  }

  const sjsu::Pin & GetPin() const override 
  { 
    return GetInactive<Pin>();
  };

  // can't do hardware interrupts with pca9535
  // TODO consider software interrupt via polling
  void AttachInterrupt(IsrPointer function, Edge edge) const override { return; }
  void DetachInterrupt() const override                               { return; }

 private:
  const I2c & i2c_;
  uint8_t address_;
  uint8_t port_;
  uint8_t pin_;
};

class Pca9535_2
{

}; // class Pca9535_2

}  // namespace pca9535
}  // namespace sjsu
