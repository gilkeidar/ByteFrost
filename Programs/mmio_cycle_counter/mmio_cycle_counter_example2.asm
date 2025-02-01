//	MMIO Cycle Counter Example 2
//	This program tests the 32-bit cycle counter, and is written in a more
//	modular assembly style.
//	The interface of the 32-bit cycle counter is as follows:
//	Address		R/W Access?		Effect
//	0xE200		W				Reset 32-bit counter (to 0)
//	0xE201		W				Latch 32-bit counter (snapshot current value)
//	0xE202		R				Read counter byte 0 (LSB)
//	0xE203		R				Read counter byte 1
//	0xE204		R				Read counter byte 2
//	0xE205		R				Read counter byte 3 (MSB)
//	(Note that the counter value is accessed as a little-endian integer)
//	===============================================
//	TODO:
//		Add the following functions:
//			void print_ln():
//				Prints a '\n' and return.
//			void print_uint32(uint32_t * val):
//				Prints an unsigned hex representation of val and return.
//			void print_str(char * s):
//				Print characters starting at s until a '\0' null byte is
//				reached.
//			void reset_ctr():
//				Reset the cycle counter.
//			uint32_t * snapshot_ctr():
//				Latch the cycle counter (snapshot current value), and return the
//				address of the counter value in memory (at 0xE202)
//			uint32_t diff_uint32(uint32_t * x, uint32_t * y):
//				Compute the 32-bit unsigned int difference of x - y, and return
//				the result.
//		Then the test code is:
//		Setup.
//			1.	Store the string "RESET COUNTER." in RAM. (addr: a)
//			2.	Store the string "COUNTER: " in RAM. (addr: b)
//			3.	print_str(a);
//			4.	print_ln();
//			5.	reset_ctr();
//			6.	R1 = 3
//			7.	while (R1 != 0):
//			8.		uint32_t * ctr_val = snapshot_ctr();
//			9.		print_str(b);
//			10.		print_uint32(ctr_val);
//			11.		print_ln();
//			12.		R1--;