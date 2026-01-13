1. IDE Setup
      a. Board "Arduino NANO"
      b. CPU "ATmega328p (old bootloader)"
2. To program the Arduino - the RESET control signal (black) need to be disconnected!!.


The desire is that each character Aduino sends to the display, it will also send to the serial lie.
To do this, the usage of Tx pin (and Rx pin need to be moved to other pins.
Only Tx pin is expected to use by display, the Rx will go to the shell/keyboard input.

Currently the Tx is used by another signal. Need to free Tx and transmit the display data on it
LCD Interface _data_pins uses pins 0 and 1
Proposal 1
Step A
[Enable]     Move D11 (PB3) --> A6 PC6  (en = 11) 
[Reg select] Move D12 (PB4) --> A7 PC7  (rs = 12)
Step B 
Move Display bit 0 --> D11 (PB3) 
Move Display bit 1 --> D12 (PB4)
Step C 
Turn on Serial line
Check if printout works
Step D
Code print char of every new char written tot he display.

Alternative (Preferred)
1. Move bit 6 from data bus to PC6
2. Move bit Do of the display to D8
3. Configure the Serial to use TX only.