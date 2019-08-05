#include <cstdint>
#include <iterator>

#include "utility/log.hpp"
#include "utility/time.hpp"
#include "L2_HAL/boards/sjone.hpp"
#include "L0_Platform/lpc17xx/LPC17xx.h"
#include "L2_HAL/sensors/movement/accelerometer/mma8452q.hpp"

uint8_t tmp_dev_addr = 0x90;
uint8_t tmp_tmp_reg  = 0x00;
uint8_t tmp_conf_reg = 0x01;
uint8_t tmp_low_reg  = 0x02;
uint8_t tmp_high_reg = 0x03;

uint8_t acc_dev_addr      = 0x38; // 7'b0011_100
uint8_t acc_status_reg    = 0x00;
uint8_t acc_out_x_msb_reg = 0x01;
uint8_t acc_out_x_lsb_reg = 0x02;
uint8_t acc_out_y_msb_reg = 0x03;
uint8_t acc_out_y_lsb_reg = 0x04;
uint8_t acc_out_z_msb_reg = 0x05;
uint8_t acc_out_z_lsb_reg = 0x06;
uint8_t acc_control_reg   = 0x2a;
uint8_t acc_who_am_i_reg  = 0x0d;

int main()
{
  static sjsu::lpc17xx::LPC_SC_TypeDef * system_controller 
    = (( sjsu::lpc17xx::LPC_SC_TypeDef *) LPC_SC_BASE );
  uint32_t value = system_controller->PCONP;
  LOG_INFO("PCONP: 0x%X", value); // check bit 26

  static sjsu::lpc17xx::LPC_PINCON_TypeDef * pin_configurator
    = (( sjsu::lpc17xx::LPC_PINCON_TypeDef *) LPC_PINCON_BASE );
  auto pinsel_val = pin_configurator->PINSEL0;
  LOG_INFO("PINSEL0: 0x%X", pinsel_val); // check bit 23:20, should be 1010
  pin_configurator->PINSEL0 |= 0xA << 20;
  
  static constexpr sjsu::lpc17xx::SystemController kLpc17xxSystemController =
      sjsu::lpc17xx::SystemController();
  sjsu::lpc17xx::I2c i2c2 = sjsu::lpc17xx::I2c(sjsu::lpc17xx::I2c::Bus::kI2c2, kLpc17xxSystemController);
  LOG_INFO("i2c2 init: %s", sjsu::Stringify(i2c2.Initialize()));

  sjsu::Mma8452q accel(i2c2, acc_dev_addr);
  if(accel.Initialize())
    LOG_INFO("accel init success");
  else
    LOG_INFO("accel init failure");

  sjsu::Pca9535 seven_segment(i2c2);
  auto retval = sjsu::Stringify(seven_segment.Initialize());
  LOG_INFO("seven_segment: %s", retval);

  while (true)
  {
    for (uint8_t i = 0; i < 16; i++)
    {
      LOG_INFO("Hello World 0x%X", i);
      sjsu::Delay(500);
    }
  }


  // // I2C2 is P0[10] and P0[11]
  // // 1. Power: PCONP PCI2C0/1/2
  // static sjsu::lpc17xx::LPC_SC_TypeDef * system_controller 
  //   = (( sjsu::lpc17xx::LPC_SC_TypeDef *) LPC_SC_BASE );
  // uint32_t value = system_controller->PCONP;
  // LOG_INFO("PCONP: 0x%X", value); // check bit 26
  // // 2. Clock: in PCLKSEL1 select PCLK_I2C2
  // value = system_controller->PCLKSEL1;
  // LOG_INFO("PCLKSEL1: 0x%X", value); // check bit 21:20
  // // 3. Pins: Select I2C2 pins through the PINSEL registers. 
  // static sjsu::lpc17xx::LPC_PINCON_TypeDef * pin_configurator
  //   = (( sjsu::lpc17xx::LPC_PINCON_TypeDef *) LPC_PINCON_BASE );
  // pin_configurator->PINSEL0 |= 0xA << 20;
  // value = pin_configurator->PINSEL0;
  // LOG_INFO("PINSEL0: 0x%X", value); // check bit 23:20, should be 1010
  // //    Select the pin modes for the port pins with I2C2 functions through the PINMODE 
  // //    registers
  // value = pin_configurator->PINMODE0;
  // LOG_INFO("PINMODE0: 0x%X", value); // check bit 23:20, should be 0000 or 1010

  // // (no pull-up, no pull-down resistors) and the PINMODE_OD registers (open drain)
  // // (See Section 8.5).
  // // Remark: I 2 C0 pins SDA0 and SCL0 are open-drain outputs and fully I 2 C-bus
  // // compliant (see Table 73). I 2 C0 can be further configured through the I2CPADCFG
  // // register to support Fast Mode Plus (See Table 99).
  // // Remark: I 2 C0 is not available in the 80-pin package.
  // // Remark: I 2 C pins that do not use specialized I 2 C pads (as identified in Table 73) can
  // // be configured to an open-drain mode via the relevant IOCON registers, and can be
  // // used with fast mode (400 kHz) and standard mode (100 kHz) I 2 C. These pins do not
  // // include an analog filter to suppress line glitches, but a similar function is performed by
  // // the digital filter in the I 2 C block itself. These pins should be configured as: no pull-up,
  // // no pull-down, open drain mode.
  // // 4. Interrupts are enabled in the NVIC using the appropriate Interrupt Set Enable regist
    

  // LOG_INFO("Starting Hello World Application");
  // auto retval = sjsu::Stringify(i2c2.Write(tmp_dev_addr, {tmp_conf_reg, 0x60, 0xa0}, 3));
  // LOG_INFO("tmp_dev: %s", retval);
  // // i2c2.Write(0x40, {0x06, 0xf0, 0x0f}, 3);
  // // i2c2.Write(0x40, {0x02, 0xff, 0xff}, 3);
  // // i2c2.Write(acc_dev_addr, {acc_control_reg, 0x01}, 2);
  // uint8_t acc_dev_id;
  // LOG_INFO("seven_segment: %s", retval);
  // while (true)
  // {
  //   for (uint8_t i = 0; i < 16; i++)
  //   {
  //     LOG_INFO("Hello World 0x%X", i);

  //     uint8_t tmp_status = 0;
  //     uint8_t tmp_bytes[2] = {0};
  //     i2c2.Write(tmp_dev_addr, {tmp_conf_reg}, 1);
  //     i2c2.Read(tmp_dev_addr, &tmp_status, 1);
  //     i2c2.Write(tmp_dev_addr, {tmp_conf_reg}, 1);
  //     i2c2.Read(tmp_dev_addr, tmp_bytes, 2);
  //     LOG_INFO("Temp byte 0x%X 0x%X 0x%X", tmp_status, tmp_bytes[0], tmp_bytes[1]);

  //     // uint8_t acc_status, acc_x_msb, acc_x_lsb;
  //     // i2c2.WriteThenRead(acc_dev_addr, {acc_status_reg}, &acc_status, 1);
  //     // i2c2.WriteThenRead(acc_dev_addr, {acc_out_x_msb_reg}, &acc_x_msb, 1);
  //     // i2c2.WriteThenRead(acc_dev_addr, {acc_out_x_lsb_reg}, &acc_x_lsb, 1);
  //     // LOG_INFO("Acc byte 0x%X 0x%X 0x%X", acc_status, acc_x_msb, acc_x_lsb);
  //     i2c2.WriteThenRead(acc_dev_addr, {acc_who_am_i_reg}, &acc_dev_id, 1);
  //     LOG_INFO("Acc byte 0x%X", acc_dev_id);

  //     seven_segment.SetOutputs(0xff, 0xff);
  //     sjsu::Delay(500);
  //   }
  // }

  return -1;
}
