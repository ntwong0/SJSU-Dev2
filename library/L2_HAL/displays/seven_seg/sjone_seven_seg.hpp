#pragma once

#include "L2_HAL/displays/seven_seg/pca9535.hpp"

namespace sjsu
{
    class SJOneSevenSeg : public Pca9535
    {
    public:
        // inherit constructors
        using Pca9535::Pca9535;

        // The following is specific to the sjone board
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
    };
}