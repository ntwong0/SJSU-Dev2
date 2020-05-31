#pragma once

#include "L1_Peripheral/i2c.hpp"
#include "L1_Peripheral/gpio.hpp"
// #include "L1_Peripheral/pin.hpp"
#include "L1_Peripheral/inactive.hpp"
#include "utility/bit.hpp"

#include "utility/log.hpp"

namespace sjsu
{
class Hafuht0020l4axt
{
  public:
    // =====
    // HAFUHT0020L4AXT-specific constants
    // =====

    // Note: The 8 bit slave addr following the start bit at the start of every I2C transaction
    //       has the following structure: 
    //               (uint8_t)(kDevI2CBaseAddr << 1 | i2c_read_or_nwrite_bit)
    static constexpr uint8_t kDevI2CBaseAddr         = 0x49; // 7 bit width (r/w excluded, ignore msb)
    
    // HAFUHT0020L4AXT command bytes
    static constexpr uint8_t kGetSerialNumber        = 0x01; // response time max: 10 ms
                                                             // Next two read requests will each 
                                                             //   return two bytes of the sensor's 
                                                             //   unique 4-byte serial number
    static constexpr uint8_t kPowerOnReset           = 0x02; // response time max: 20 ms
                                                             // Force power-on reset of sensor
                                                             //   microcontroller
    static constexpr uint8_t kChecksum               = 0x03; // response time max: 1 s
                                                             // Calculates EEPROM checksum and 
                                                             //   compares to production checksum
                                                             //   value. If the values match, the 
                                                             //   next read request will respond with
                                                             //   0xCCA5. Otherwise, the next read will
                                                             //   respond with 0xCC90.

    // HAFUHT0020L4AXT sensor responses
    static constexpr uint8_t kPOSACK        = 0xCCA5; // non-response command was executed successfully
    static constexpr uint8_t kBadCommand    = 0xCC99; // command byte was not recongized
    static constexpr uint8_t kBadParam      = 0xCC9A; // command sent with incorrect parameter bytes
    static constexpr uint8_t kFailure       = 0xCC9B; // command failed during execution
    static constexpr uint8_t kBadChecksum   = 0xCC90; // checksum did not match stored value
    static constexpr uint8_t kBusy          = 0xCCBB; // sensor is busy calculating the checksum value
    
    // HAFUHT0020L4AXT calculating constants
    static constexpr float   kFullScaleFlow = 20.0;   // max flow

    // HAFUHT0020L4AXT test sensor values
    static constexpr uint32_t kSerialNumber  = 0x4609A495;

    // =====
    // HAFUHT0020L4AXT-specific I2C methods
    // =====

    static Status Initialize(const I2c &i2c, uint8_t address, uint16_t serialNumber) 
    {
      i2c.Initialize();
      i2c.Write(address, { kGetSerialNumber } );
      Delay(10ms);
      uint8_t vals_from_get_serial[4] = {0, 0, 0, 0};
      Status retval0 = i2c.Read(address, &vals_from_get_serial[0], 2);
      Delay(10ms);
      Status retval1 = i2c.Read(address, &vals_from_get_serial[2], 2);
      LOG_INFO("received %x %x %x %x %x", 
               vals_from_get_serial[0],
               vals_from_get_serial[1],
               vals_from_get_serial[2],
               vals_from_get_serial[3]);
      uint32_t gotten_serial = (uint32_t)vals_from_get_serial[0] << 24 | 
                               (uint32_t)vals_from_get_serial[1] << 16 |
                               (uint32_t)vals_from_get_serial[2] <<  8 |
                               (uint32_t)vals_from_get_serial[3];
      if(retval0 == Status::kSuccess && retval1 == Status::kSuccess
                                     && gotten_serial == serialNumber)
      {
        retval0 = Status::kSuccess;
      }
      else
      {
        LOG_INFO("bad serial: %x", gotten_serial);
        retval0 = Status::kInvalidSettings;
      }
      return retval0;
    }

    // Note: Read requests taken faster than the response time (1ms) are not guaranteed to return fresh data
    uint16_t ReadFlow(const I2c &i2c, uint8_t address)
    {
      uint8_t vals_from_blank_read[2] = {0, 0};
      i2c.Read(address, &vals_from_blank_read[0], 2);
      uint16_t retval = (uint16_t) vals_from_blank_read[0] << 8 | vals_from_blank_read[1];
      return retval;
    }

    // =====
    // Hafuht0020l4axt methods
    // =====

    Hafuht0020l4axt(const I2c & i2c, uint8_t address = kDevI2CBaseAddr, uint16_t serialNumber = kSerialNumber)
    : i2c_(i2c), address_(address), serialNumber_(serialNumber)
    {}

    Status Initialize()
    {
      return Initialize(i2c_, address_, serialNumber_);
    }

    // convert "digital output code" to "standard liters per minute"
    float SLPMfromDOC(uint16_t code)
    {
      return kFullScaleFlow * (((float)code/16384.0) - 0.1)/0.8;
    }

    float GetFlowrate()
    {
      return SLPMfromDOC(ReadFlow(i2c_, address_));
    }

    void DelayForSensorResponseTime()
    {
      Delay(1ms);
      return;
    }

  private:
    const I2c & i2c_;
    uint8_t     address_;
    uint16_t    serialNumber_;
}; // class Hafuht0020l4axt
}  // namespace sjsu
