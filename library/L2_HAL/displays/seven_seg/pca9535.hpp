#pragma once

#include "L1_Peripheral/i2c.hpp"
#include "L1_Peripheral/gpio.hpp"
// #include "L1_Peripheral/pin.hpp"
#include "L1_Peripheral/inactive.hpp"
#include "utility/bit.hpp"

#include "utility/log.hpp"

namespace sjsu
{
class Pca9535
{
  public:
    // =====
    // PCA9535-specific constants
    // =====

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
    static constexpr size_t kNumberOfPorts = 2;

    // =====
    // PCA9535-specific I2C methods
    // =====

    static Status Initialize(const I2c &i2c, uint8_t address) 
    {
      i2c.Initialize();
      uint8_t vals_from_config_ports[kNumberOfPorts];
      Status retval =
        i2c.WriteThenRead(address, { kCtrlRegAddrConfigPort0 }, &vals_from_config_ports[0], kNumberOfPorts);
      // confirm pins are defaulted as inputs - return error if this is not the case
      if(vals_from_config_ports[0] != 0xFF || vals_from_config_ports[1] != 0xFF)
        retval = Status::kBusError;
      return retval;
    }

    // Set pins as inputs or outputs
    static Status SetConfigPorts(const I2c &i2c, uint8_t address, uint8_t config0, uint8_t config1) 
    {
      return i2c.Write(address, { kCtrlRegAddrConfigPort0, config0, config1 });
    }

    static Status SetConfigPort0(const I2c &i2c, uint8_t address, uint8_t config0) 
    {
      return i2c.Write(address, { kCtrlRegAddrConfigPort0, config0 });
    }

    static Status SetConfigPort1(const I2c &i2c, uint8_t address, uint8_t config1) 
    {
      return i2c.Write(address, { kCtrlRegAddrConfigPort1, config1 });
    }

    static Status GetConfigPorts(const I2c &i2c, uint8_t address, uint16_t *inval) 
    {
      uint8_t inputs[2];
      i2c.Write(address, { kCtrlRegAddrConfigPort0 });
      Status retval = i2c.Read(address, inputs, 2);
      *inval = static_cast<uint16_t>(inputs[0]) | static_cast<uint16_t>(inputs[1]) << 8;
      return retval;
    }

    static Status GetConfigPorts(const I2c &i2c, uint8_t address, uint8_t *inval0, uint8_t *inval1) 
    {
      uint8_t inputs[2];
      i2c.Write(address, { kCtrlRegAddrConfigPort0 });
      Status retval = i2c.Read(address, inputs, 2);
      *inval0 = inputs[0];
      *inval1 = inputs[1];
      return retval;
    }

    static Status GetConfigPort0(const I2c &i2c, uint8_t address, uint8_t *inval0) 
    {
      i2c.Write(address, { kCtrlRegAddrConfigPort0 });
      return i2c.Read(address, inval0, 1);
    }

    static Status GetConfigPort1(const I2c &i2c, uint8_t address, uint8_t *inval1) 
    {
      i2c.Write(address, { kCtrlRegAddrConfigPort1 });
      return i2c.Read(address, inval1, 1);
    }

    static Status SetPolarityPorts(const I2c &i2c, uint8_t address, uint8_t pol0, uint8_t pol1)
    {
      return i2c.Write(address, { kCtrlRegAddrPolInvPort0, pol0, pol1 });
    }

    static Status SetPolarityPort0(const I2c &i2c, uint8_t address, uint8_t pol0)
    {
      return i2c.Write(address, { kCtrlRegAddrPolInvPort0, pol0 });
    }

    static Status SetPolarityPort1(const I2c &i2c, uint8_t address, uint8_t pol1)
    {
      return i2c.Write(address, { kCtrlRegAddrPolInvPort1, pol1 });
    }

    static Status GetPolarityPorts(const I2c &i2c, uint8_t address, uint16_t *inval)
    {
      uint8_t inputs[2];
      i2c.Write(address, { kCtrlRegAddrPolInvPort0 });
      Status retval = i2c.Read(address, inputs, 2);
      *inval = static_cast<uint16_t>(inputs[0]) | static_cast<uint16_t>(inputs[1]) << 8;
      return retval;
    }

    static Status GetPolarityPorts(const I2c &i2c, uint8_t address, uint8_t *inval0, uint8_t *inval1)
    {
      uint8_t inputs[2];
      i2c.Write(address, { kCtrlRegAddrPolInvPort0 });
      Status retval = i2c.Read(address, inputs, 2);
      *inval0 = inputs[0];
      *inval1 = inputs[1];
      return retval;
    }

    static Status GetPolarityPort0(const I2c &i2c, uint8_t address, uint8_t *inval0)
    {
      i2c.Write(address, { kCtrlRegAddrPolInvPort0 });
      return i2c.Read(address, inval0, 1);
    }

    static Status GetPolarityPort1(const I2c &i2c, uint8_t address, uint8_t *inval1)
    {
      i2c.Write(address, { kCtrlRegAddrPolInvPort1 });
      return i2c.Read(address, inval1, 1);
    }

    static Status SetOutputPorts(const I2c &i2c, uint8_t address, uint8_t outval0, uint8_t outval1)
    {
      return i2c.Write(address, { kCtrlRegAddrOutputPort0, outval0, outval1 });
    }

    static Status SetOutputPort0(const I2c &i2c, uint8_t address, uint8_t outval0)
    {
      return i2c.Write(address, { kCtrlRegAddrOutputPort0, outval0 });
    }

    static Status SetOutputPort1(const I2c &i2c, uint8_t address, uint8_t outval1)
    {
      return i2c.Write(address, { kCtrlRegAddrOutputPort1, outval1 });
    }

    static Status GetOutputPorts(const I2c &i2c, uint8_t address, uint16_t *inval)
    {
      uint8_t inputs[2];
      i2c.Write(address, { kCtrlRegAddrOutputPort0 });
      Status retval = i2c.Read(address, inputs, 2);
      *inval = static_cast<uint16_t>(inputs[0]) | static_cast<uint16_t>(inputs[1]) << 8;
      return retval;
    }

    static Status GetOutputPorts(const I2c &i2c, uint8_t address, uint8_t *inval0, uint8_t *inval1)
    {
      uint8_t inputs[2];
      i2c.Write(address, { kCtrlRegAddrOutputPort0 });
      Status retval = i2c.Read(address, inputs, 2);
      *inval0 = inputs[0];
      *inval1 = inputs[1];
      return retval;
    }

    static Status GetOutputPort0(const I2c &i2c, uint8_t address, uint8_t *inval0)
    {
      // LOG_INFO("In GetOutputPort0");
      i2c.Write(address, { kCtrlRegAddrOutputPort0 });
      return i2c.Read(address, inval0, 1);
    }

    static Status GetOutputPort1(const I2c &i2c, uint8_t address, uint8_t *inval1)
    {
      i2c.Write(address, { kCtrlRegAddrOutputPort1 });
      return i2c.Read(address, inval1, 1);
    }

    static Status GetInputPorts(const I2c &i2c, uint8_t address, uint16_t *inval)
    {
      uint8_t inputs[2];
      i2c.Write(address, { kCtrlRegAddrInputPort0 });
      Status retval = i2c.Read(address, inputs, 2);
      *inval = static_cast<uint16_t>(inputs[0]) | static_cast<uint16_t>(inputs[1]) << 8;
      return retval;
    }

    static Status GetInputPorts(const I2c &i2c, uint8_t address, uint8_t *inval0, uint8_t *inval1)
    {
      uint8_t inputs[2];
      i2c.Write(address, { kCtrlRegAddrInputPort0 });
      Status retval = i2c.Read(address, inputs, 2);
      *inval0 = inputs[0];
      *inval1 = inputs[1];
      return retval;
    }

    static Status GetInputPort0(const I2c &i2c, uint8_t address, uint8_t *inval0)
    {
      i2c.Write(address, { kCtrlRegAddrInputPort0 });
      return i2c.Read(address, inval0, 1);
    }

    static Status GetInputPort1(const I2c &i2c, uint8_t address, uint8_t *inval1)
    {
      i2c.Write(address, { kCtrlRegAddrInputPort1 });
      return i2c.Read(address, inval1, 1);
    }

    // GPIO implementation specific to PCA9535
    class Gpio final : public sjsu::Gpio
    {
    public:
      Gpio(uint8_t port_number,
          uint8_t pin_number,
          const I2c & i2c, uint8_t address = kDevI2CBaseAddr)
          : port_(port_number), pin_(pin_number), i2c_(i2c), address_(address)
      {
          // TODO(#___):add assertions on port, pin, and address
      }

      // =====
      // GPIO interface methods
      // =====

      void SetDirection(Direction direction) const override
      {
        if(port_) // Port1
        {
          uint8_t val = 0;
          GetConfigPort1(i2c_, address_, &val);
          val = direction == kInput ? bit::Set(val, pin_) : bit::Clear(val, pin_);
          SetConfigPort1(i2c_, address_, val);
        }
        else // Port0
        {
          uint8_t val = 0;
          GetConfigPort0(i2c_, address_, &val);
          val = direction == kInput ? bit::Set(val, pin_) : bit::Clear(val, pin_);
          SetConfigPort0(i2c_, address_, val);
        }
      }

      void Set(State output) const override
      {
        // LOG_INFO("In Set(): port value is %x", port_);
        if(port_) // Port1
        {
          uint8_t val = 0;
          GetOutputPort1(i2c_, address_, &val);
          val = output == kHigh ? bit::Set(val, pin_) : bit::Clear(val, pin_);
          SetOutputPort1(i2c_, address_, val);
        }  
        else // Port0
        {
          // LOG_INFO("In Set() else");
          uint8_t val = 0;
          GetOutputPort0(i2c_, address_, &val);
          val = output == kHigh ? bit::Set(val, pin_) : bit::Clear(val, pin_);
          SetOutputPort0(i2c_, address_, val);
        }
      }

      void Toggle() const override
      {
        // LOG_INFO("In Toggle(): port value is %x, pin values is %x", port_, pin_);
        if(port_) // Port1
        {
          uint8_t val = 0;
          GetOutputPort1(i2c_, address_, &val);
          val = bit::Read(val, pin_) ? bit::Clear(val, pin_) : bit::Set(val, pin_);
          SetOutputPort1(i2c_, address_, val);
        }  
        else // Port0
        {
          uint8_t val = 0;
          GetOutputPort0(i2c_, address_, &val);
          // LOG_INFO("  got value %x", val);
          val = bit::Read(val, pin_) ? bit::Clear(val, pin_) : bit::Set(val, pin_);
          // LOG_INFO("  sending value %x", val);
          SetOutputPort0(i2c_, address_, val);
        }
      }

      bool Read() const override
      {
        bool retval = false;
        if(port_) // Port1
        {
          uint8_t val = 0;
          GetInputPort1(i2c_, address_, &val);
          retval = bit::Read(val, pin_);
        }  
        else // Port0
        {
          uint8_t val = 0;
          GetInputPort1(i2c_, address_, &val);
          retval = bit::Read(val, pin_);
        }
        return retval;
      }

      const sjsu::Pin & GetPin() const override 
      { 
        return GetInactive<Pin>();
      };

      // can't do hardware interrupts with pca9535
      // TODO(#___):consider software interrupt via polling
      void AttachInterrupt(IsrPointer function, Edge edge) const override { return; }
      void DetachInterrupt() const override                               { return; }

      // ===
      // Override inherited utility methods
      // ===

      void SetHigh() const
      {
        // LOG_INFO("In SetHigh()");
        Set(State::kHigh);
      }
      void SetLow() const
      {
        Set(State::kLow);
      }
      void SetAsInput() const
      {
        SetDirection(Direction::kInput);
      }
      void SetAsOutput() const
      {
        SetDirection(Direction::kOutput);
      }

    private:
      const I2c & i2c_;
      uint8_t address_;
      uint8_t port_;
      uint8_t pin_;
    }; // class Gpio

    // =====
    // Pca9535 methods
    // =====

    Pca9535(const I2c & i2c, uint8_t address = kDevI2CBaseAddr)
    : i2c_(i2c), address_(address)
    {}

    sjsu::Pca9535::Gpio GetGpio(uint8_t port, uint8_t pin)
    {
      // TODO(#___):assertion check port and pin values
      return sjsu::Pca9535::Gpio(port, pin, i2c_, address_); 
    }  

    Status Initialize()
    {
        return Initialize(i2c_, address_);
    }

    Status ConfigAllToOutput()
    {
        return SetConfigPorts(i2c_, address_, 0x00, 0x00);
    }
    
    Status ConfigAllToInput()
    {
        return SetConfigPorts(i2c_, address_, 0xFF, 0xFF);
    }

    Status SetAll(Gpio::State level)
    {
        return (level == Gpio::State::kHigh) ?
                SetOutputPorts(i2c_, address_, 0xFF, 0xFF) :
                SetOutputPorts(i2c_, address_, 0x00, 0x00);
    }

  private:
    const I2c & i2c_;
    uint8_t address_;
  
  public:
    // TODO(#___):replace with GpioParallelBus
    typedef sjsu::Pca9535::Gpio GpioParallelBus[];
}; // class Pca9535
}  // namespace sjsu
