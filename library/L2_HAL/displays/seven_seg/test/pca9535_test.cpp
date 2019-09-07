#include "L2_HAL/displays/seven_seg/pca9535.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
  TEST_CASE("Testing Pca9535", "[Pca9535]")
  {
    Mock<I2c> mock_i2c;
    Fake(
      Method(mock_i2c, Initialize),
      Method(mock_i2c, Write),
      Method(mock_i2c, Read)
    );

    Mock<Pca9535> test_pca9535(mock_i2c);

    REQUIRE(1 == 0);
  }
} // namespace sjsu
