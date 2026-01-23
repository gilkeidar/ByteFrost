//	LDA Test. Testing on new HW changes (AR DB LE LUT and replacing LSP).
//	Runs on bare metal from ROM (starts at 0x0000).

LDA %PC, H, #0x02
LDA %DP, L, #0x25
LDA %DP, H, #0x26
