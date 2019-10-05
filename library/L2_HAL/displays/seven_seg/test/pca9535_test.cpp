#include "L2_HAL/displays/seven_seg/pca9535.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(Pca9535);
TEST_CASE("Testing Pca9535", "[Pca9535]")
{
  Mock<I2c> mock_i2c;
  Fake(
    Method(mock_i2c, Initialize)
    // Method(mock_i2c, Write),
    // Method(mock_i2c, Read)
  );

  I2c & i2c = mock_i2c.get();
  Pca9535 test_pca9535(i2c);

  SECTION("Check Initialize")
  {
    // for a successful initialization, i2c will retun kSuccess, and the 
    // contents of the config registers are both 0xff
    When(Method(mock_i2c, Initialize)).Return(Status::kSuccess);

    // WriteThenRead() is a wrapper for I2c::Transaction(), so  
    // let's mock I2c::Transaction instead
    // 1. specify transaction contents
    static constexpr uint8_t kExpectedValsFromConfigPorts[2] = {0xFF, 0xFF};
    // 2. create the mock
    When(
      Method(mock_i2c, Transaction)
    ).AlwaysDo(
      [=](I2c::Transaction_t transaction) -> Status {
        transaction.data_in[0] = kExpectedValsFromConfigPorts[0];
        transaction.data_in[1] = kExpectedValsFromConfigPorts[1];
        // Check transaction values
        CHECK(transaction.operation   == I2c::Operation::kWrite);
        CHECK(transaction.address     == Pca9535::kDevI2CBaseAddr);
        CHECK(transaction.data_out[0] == Pca9535::kCtrlRegAddrConfigPort0);
        CHECK(transaction.out_length  == 1);
        // CHECK(transaction.data_in     == nullptr); // redundant
        CHECK(transaction.in_length   == 2);
        CHECK(transaction.position    == 0);
        CHECK(transaction.repeated    == true);
        CHECK(transaction.busy        == true);
        CHECK(transaction.timeout     == I2c::kI2cTimeout);
        CHECK(transaction.status      == Status::kSuccess);
        return Status::kSuccess;
      }
    );

    CHECK(test_pca9535.Initialize() == Status::kSuccess);
    Verify(Method(mock_i2c, Initialize)).Exactly(1);
    Verify(Method(mock_i2c, Transaction)).Exactly(1);
  }

  // SECTION("Check SetConfigPorts")
  // {
  //   // Write() is a wrapper for I2c::Transaction(), so  
  //   // let's mock I2c::Transaction instead
  //   // 1. define input parameters to SetConfigPorts
  //   static constexpr uint8_t address = 0xde;
  //   static constexpr uint8_t config0 = 0xad;
  //   static constexpr uint8_t config1 = 0xbe;
  //   // 2. mock Transaction()
  //   When(
  //     Method(mock_i2c, Transaction)
  //   ).AlwaysDo(
  //     [=](I2c::Transaction_t transaction) -> Status {
  //       // Check transaction values
  //       CHECK(transaction.operation   == I2c::Operation::kWrite);
  //       CHECK(transaction.address     == Pca9535::kDevI2CBaseAddr);
  //       CHECK(transaction.data_out[0] == Pca9535::kCtrlRegAddrConfigPort0);
  //       CHECK(transaction.data_out[1] == config0);
  //       CHECK(transaction.data_out[2] == config1);
  //       CHECK(transaction.out_length  == 3);
  //       CHECK(transaction.data_in     == nullptr);
  //       CHECK(transaction.in_length   == 0);
  //       CHECK(transaction.position    == 0);
  //       CHECK(transaction.repeated    == false);
  //       CHECK(transaction.busy        == true);
  //       CHECK(transaction.timeout     == I2c::kI2cTimeout);
  //       CHECK(transaction.status      == Status::kSuccess);
  //       return Status::kSuccess;
  //     }
  //   );
  //   // 3. check success
  //   CHECK(test_pca9535.SetConfigPorts(mock_i2c, address, config0, config1) 
  //               == Status::kSuccess);
  // }
}

} // namespace sjsu
