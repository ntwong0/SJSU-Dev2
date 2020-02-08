
# SJ One PCA9535/Seven-Segment demonstration
## Summary
This application demonstrates the functionality of the PCA9535 and the seven-segment display.
Every 1.5 seconds, the display:
* Sets every segment for 500 ms
* Displays a hexademical value for 500 ms
* Clears every segment for 500 ms
The value increments once per 1.5 seconds, from `0x0` to `0xF`.

## Development notes
```
PCONP
0x42887DE
0000_0100_0010_1000_1000_0111_1101_1110
______1

PCLKSEL1
0x0
____________xx

PINSEL0
0xA00050
0000_0000_0000_0000_0000_0000_0101_0000
0000_0000_1010_0000_0000_0000_0101_0000
__________1010

PINMODE0
0x0
```