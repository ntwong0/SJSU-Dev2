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
    When(Method(mock_i2c, Initialize)).Return(Status::kSuccess);
    auto initialize_WriteThenRead_callback = 
      [](uint8_t address,
         std::initializer_list<uint8_t> transmit,
         uint8_t * receive_buffer,
         size_t receive_buffer_length,
         uint32_t timeout = I2c::kI2cTimeout) -> Status 
    {
      CHECK(receive_buffer_length == 2);
      static constexpr uint8_t kExpectedValsFromConfigPorts[2] = {0xFF, 0xFF};
      for (size_t iter = 0; iter < 1; iter++)
      {
        receive_buffer[iter] = kExpectedValsFromConfigPorts[iter];
      }
      return Status::kSuccess;
    };

    When(
      ConstOverloadedMethod(
        mock_i2c,
        WriteThenRead,
        Status(
          uint8_t, std::initializer_list<uint8_t>, uint8_t *, size_t, uint32_t
        )
      )
    ).AlwaysDo(initialize_WriteThenRead_callback);
    Status retval = test_pca9535.Initialize();
    // CHECK(test_pca9535.Initialize() == Status::kSuccess);
    // Verify(Method(mock_i2c, Initialize));
    // Verify(Method(mock_i2c, WriteThenRead)).Exactly(1);
  }
}
} // namespace sjsu
