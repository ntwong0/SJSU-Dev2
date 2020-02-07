#include "L2_HAL/sensors/environment/temperature/si7060.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
// Uncomment this when can class has been created
EMIT_ALL_METHODS(Si7060);

TEST_CASE("Si7060", "[si7060]")
{
  constexpr uint8_t kDeviceAddress = Si7060::kDefaultAddress;
  Mock<I2c> mock_i2c;
  Si7060 temperature_sensor(mock_i2c.get(), kDeviceAddress);

  SECTION("Initialize")
  {
    SECTION("I2c initialization failure")
    {
      mock_i2c.Reset();
      // The failure status should be returned immediately without performing
      // any further communication with the device.
      constexpr Status kExpectedStatus = Status::kBusError;
      bool i2c_transaction_not_invoked = true;

      When(Method(mock_i2c, Initialize)).AlwaysReturn(kExpectedStatus);
      When(Method(mock_i2c, Transaction))
          .AlwaysDo(
              [&i2c_transaction_not_invoked](I2c::Transaction_t) -> Status {
                i2c_transaction_not_invoked = false;
                return Status::kSuccess;
              });

      CHECK(temperature_sensor.Initialize() == kExpectedStatus);
      CHECK(i2c_transaction_not_invoked);
      Verify(Method(mock_i2c, Initialize)).Once();
      Verify(Method(mock_i2c, Transaction)).Never();
    }
    SECTION("Incorrect device information")
    {
      mock_i2c.Reset();
      constexpr Status kExpectedStatus     = Status::kDeviceNotFound;
      constexpr uint8_t kIncorrectDeviceId = 0xFF;

      When(Method(mock_i2c, Initialize)).AlwaysReturn(Status::kSuccess);
      When(Method(mock_i2c, Transaction))
          .AlwaysDo([](I2c::Transaction_t transaction) -> Status {
            transaction.data_in[0] = kIncorrectDeviceId;
            return Status::kSuccess;
          });

      CHECK(temperature_sensor.Initialize() == kExpectedStatus);
      Verify(Method(mock_i2c, Initialize), Method(mock_i2c, Transaction))
          .Once();
    }
    SECTION("I2c initialization success w/ correct device information")
    {
      mock_i2c.Reset();
      constexpr Status kExpectedStatus = Status::kSuccess;

      When(Method(mock_i2c, Initialize)).AlwaysReturn(kExpectedStatus);
      When(Method(mock_i2c, Transaction))
          .AlwaysDo([](I2c::Transaction_t transaction) -> Status {
            transaction.data_in[0] = Si7060::kExpectedSensorId;
            return Status::kSuccess;
          });

      CHECK(temperature_sensor.Initialize() == kExpectedStatus);
      Verify(Method(mock_i2c, Initialize), Method(mock_i2c, Transaction))
          .Once();
    }
  }

  SECTION("GetTemperature")
  {
    constexpr uint8_t kEnableOneBurstMode         = 0x04;
    constexpr uint8_t kEnableAutoIncrement        = 0x01;
    constexpr std::array kExpectedTemperatureData = { 0x66, 0xC1 };
    constexpr float kExpectedTemperature          = 117.006f;
    constexpr float kMarginOfError                = 0.001f;

    // Retreiving temperature data should produce the following expected
    // transaction record:
    //   1. Write transaction to enable one-shot mode.
    //   2. Write transaction to enable auto increment.
    //   3. Write transaction to register pointer to most significant byte
    //      register and retrieve the MSB of the temperature data.
    //   4. Read transaction to retrieve the LSB of the temperature data.
    constexpr std::array kExpectedDataOutLengths = { 2, 2, 1 };
    constexpr uint8_t kMaxDataLength = kExpectedDataOutLengths.size();
    // constexpr
    const uint8_t kExpectedDataOut[][kMaxDataLength] = {
      { Si7060::kOneBurstRegister, kEnableOneBurstMode },
      { Si7060::kAutomaticBitRegister, kEnableAutoIncrement },
      { Si7060::kMostSignificantRegister },
    };
    const std::array kExpectedTransactions = {
      // transaction to set one-shot mode
      I2c::Transaction_t({
          .operation  = I2c::Operation::kWrite,
          .address    = kDeviceAddress,
          .data_out   = kExpectedDataOut[0],
          .out_length = kExpectedDataOutLengths[0],
          .data_in    = nullptr,
          .in_length  = 0,
          .position   = 0,
          .repeated   = false,
          .busy       = true,
          .timeout    = I2c::kI2cTimeout,
          .status     = Status::kSuccess,
      }),
      // transaction to enable auto increment
      I2c::Transaction_t({
          .operation  = I2c::Operation::kWrite,
          .address    = kDeviceAddress,
          .data_out   = kExpectedDataOut[1],
          .out_length = kExpectedDataOutLengths[1],
          .data_in    = nullptr,
          .in_length  = 0,
          .position   = 0,
          .repeated   = false,
          .busy       = true,
          .timeout    = I2c::kI2cTimeout,
          .status     = Status::kSuccess,
      }),
      // transaction to read the MSB of the temperature data
      I2c::Transaction_t({
          .operation  = I2c::Operation::kWrite,
          .address    = kDeviceAddress,
          .data_out   = kExpectedDataOut[2],
          .out_length = kExpectedDataOutLengths[2],
          .data_in    = nullptr,
          .in_length  = 1,
          .position   = 0,
          .repeated   = true,
          .busy       = true,
          .timeout    = I2c::kI2cTimeout,
          .status     = Status::kSuccess,
      }),
      // transaction to read the LSB of the temperature data
      I2c::Transaction_t({
          .operation  = I2c::Operation::kRead,
          .address    = kDeviceAddress,
          .data_out   = nullptr,
          .out_length = 0,
          .data_in    = nullptr,
          .in_length  = 1,
          .position   = 0,
          .repeated   = false,
          .busy       = true,
          .timeout    = I2c::kI2cTimeout,
          .status     = Status::kSuccess,
      }),
    };

    // Array to store the two transactions.
    I2c::Transaction_t transactions[kExpectedTransactions.size()];
    // Storing Transaction's data_out in separate array since it's read-only.
    uint8_t data_out[kExpectedTransactions.size()][kMaxDataLength];
    // Stub Transaction to extract each Transaction record and to inject data to
    // receive buffer when temperature data is expected to be received.
    When(Method(mock_i2c, Transaction))
        .AlwaysDo([&transactions, &data_out, &kExpectedTemperatureData](
                      I2c::Transaction_t transaction) -> Status {
          static uint8_t transaction_id = 0;
          transactions[transaction_id]  = transaction;
          for (size_t i = 0; i < transaction.out_length; i++)
          {
            data_out[transaction_id][i] = transaction.data_out[i];
          }
          // Inject the temperature data for the 3rd and 4th transactions.
          constexpr uint8_t kTransactionGetMostSignificantByte  = 2;
          constexpr uint8_t kTransactionGetLeastSignificantByte = 3;
          switch (transaction_id)
          {
            case kTransactionGetMostSignificantByte:
              transaction.data_in[0] = kExpectedTemperatureData[0];
              break;
            case kTransactionGetLeastSignificantByte:
              transaction.data_in[0] = kExpectedTemperatureData[1];
              break;
            default: break;
          }
          transaction_id++;
          return Status::kSuccess;
        });

    units::temperature::celsius_t temperature;
    temperature_sensor.GetTemperature(&temperature);
    // Verify the configuration and data in the four transactions.
    Verify(Method(mock_i2c, Transaction)).Exactly(kExpectedTransactions.size());
    for (size_t i = 0; i < kExpectedTransactions.size(); i++)
    {
      INFO("Checking transaction: #" << (i + 1));
      CHECK(transactions[i].operation == kExpectedTransactions[i].operation);
      CHECK(transactions[i].address == kExpectedTransactions[i].address);
      CHECK(transactions[i].out_length == kExpectedTransactions[i].out_length);
      CHECK(transactions[i].in_length == kExpectedTransactions[i].in_length);
      CHECK(transactions[i].position == kExpectedTransactions[i].position);
      CHECK(transactions[i].repeated == kExpectedTransactions[i].repeated);
      CHECK(transactions[i].timeout == kExpectedTransactions[i].timeout);
      CHECK(transactions[i].status == kExpectedTransactions[i].status);
      for (size_t j = 0; j < transactions[i].out_length; j++)
      {
        CHECK(data_out[i][j] == kExpectedTransactions[i].data_out[j]);
      }
    }
    CHECK(temperature.to<float>() ==
          Approx(kExpectedTemperature).epsilon(kMarginOfError));
  }
}
}  // namespace sjsu
