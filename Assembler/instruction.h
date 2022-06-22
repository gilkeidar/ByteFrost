#include <stdint.h>
#define MAX_LINE_LENGTH	    300
#define MAX_TOKENS          4
#define MAX_TOKEN_LENGTH    20
#define NUM_INSTRUCTIONS    (sizeof(assembly)/sizeof(Instruction))

typedef enum param {
    none, 
    instruction, 
    reg, 
    immediate,
    out_ai,
    out_id,
    branch_amt
} param;

typedef struct Instruction
{
    char * name;                    // Instruction name
    int num_params;                 // Number of parameters accepted by instruction
    uint8_t skeleton[2];            // Instruction skeleton
    param param_seq[MAX_TOKENS];    // Expected parameter sequence
    int param_position[MAX_TOKENS]; //
    int (*handler) (struct Instruction *, uint8_t *);     // Assembly instruction handler 
} Instruction;

/*  Handlers    */
int basic_handler(Instruction *, uint8_t *);
int abs_branch_handler(Instruction *, uint8_t *);
int rel_branch_handler(Instruction *, uint8_t *);


Instruction assembly[] = {
    // 0000: NOP
    {
        "NOP",
        0,
        {0x00, 0x00},
        {none},
        {0},
        basic_handler
    },
    // 0001: BRK
    {
        "BRK",
        0,
        {0x01, 0x00},
        {none},
        {0},
        basic_handler
    },
    // 0002: ALU (registers)
    {
        "OR",
        3,
        {0x02, 0x00},
        {reg, reg, reg},
        {6, 12, 14},
        basic_handler
    },
    {
        "AND",
        3,
        {0x02, 0x02},
        {reg, reg, reg},
        {6, 12, 14},
        basic_handler
    },
    {
        "XOR",
        3,
        {0x02, 0x04},
        {reg, reg, reg},
        {6, 12, 14},
        basic_handler
    },
    {
        "NOT",
        2,
        {0x02, 0x05},
        {reg, reg},
        {6, 14},    // second reg goes to Rs2
        basic_handler
    },
    {
        "ADD",
        3,
        {0x02, 0x06},
        {reg, reg, reg},
        {6, 12, 14},
        basic_handler
    },
    {
        "SUB",
        3,
        {0x02, 0x07},
        {reg, reg, reg},
        {6, 12, 14},
        basic_handler
    },
    {
        "ASL",
        2,
        {0x02, 0x08},
        {reg, reg},
        {6, 12},
        basic_handler
    },
    {
        "ROL",
        2,
        {0x02, 0x09},
        {reg, reg},
        {6, 12},
        basic_handler
    },
    {
        "LSR",
        2,
        {0x02, 0x0a},
        {reg, reg},
        {6, 12},
        basic_handler
    },
    {
        "ASR",
        2,
        {0x02, 0x0b},
        {reg, reg},
        {6, 12},
        basic_handler
    },
    {
        "ROR",
        2,
        {0x02, 0x0c},
        {reg, reg},
        {6, 12},    // Rotates A
        basic_handler
    },
    {
        "ADC",
        3,
        {0x02, 0x0e},
        {reg, reg, reg},
        {6, 12, 14},
        basic_handler
    },
    {
        "SBC",
        3,
        {0x02, 0x0f},
        {reg, reg, reg},
        {6, 12, 14},
        basic_handler
    },
    

    // Register

    // LDR Opcode 3
    {
        "LDR",
        2,
        {0x03, 0x00},
        {reg, immediate},
        {6, 8},
        basic_handler
    },
    // MOV Opcode 4
    {
        "MOV",
        2,
        {0x04, 0x00},
        {reg, reg},
        {6, 12},
        basic_handler
    },

    // Absolute branch  BNE #addr Opcode 5
    {
        "JMP", 
        1,
        {0x05, 0x00},
        {immediate},
        {8},
        abs_branch_handler
    },
   {
        "BMI", 
        1,
        {0x25, 0x00},
        {immediate},
        {8},
        abs_branch_handler
    },
    {
        "BCS", 
        1,
        {0x45, 0x00},
        {immediate},
        {8},
        abs_branch_handler
    },
   {
        "BEQ", 
        1,
        {0x65, 0x00},
        {immediate},
        {8},
        abs_branch_handler
    },
    {
        "BPL", 
        1,
        {0xA5, 0x00},
        {immediate},
        {8},
        abs_branch_handler
    },
   {
        "BCC", 
        1,
        {0xC5, 0x00},
        {immediate},
        {8},
        abs_branch_handler
    },
    {
        "BNE", 
        1,
        {0xe5, 0x00},
        {immediate},
        {8},
        abs_branch_handler
    },

    // ALU Short OpCode 6
    {
        "OR",
        2,
        {0x06, 0x00},
        {reg, immediate},
        {6, 12},
        basic_handler
    },
    {
        "AND",
        2,
        {0x06, 0x02},
        {reg, immediate},
        {6, 12},
        basic_handler
    },
    {
        "XOR",
        2,
        {0x06, 0x04},
        {reg, immediate},
        {6, 12},
        basic_handler
    },
    
    {
        "ADD",
        2,
        {0x06, 0x06},
        {reg, immediate},
        {6, 12},
        basic_handler
    },
    {
        "INC",
        1,
        {0x06, 0x16},
        {reg},
        {6},
        basic_handler
    },
    {
        "SUB",
        2,
        {0x06, 0x07},
        {reg, immediate},
        {6, 12},
        basic_handler
    },
    {
        "DEC",
        1,
        {0x06, 0x17},
        {reg},
        {6},
        basic_handler
    },
    {
        "ADC",
        2,
        {0x06, 0x0e},
        {reg, immediate},
        {6, 12},
        basic_handler
    },
    {
        "SBC",
        2,
        {0x06, 0x0f},
        {reg, immediate},
        {6, 12},
        basic_handler
    },

    // Branch Relative Opcode 7
    {
        "JMP",
        1,
        {0x07, 0x00},
        {branch_amt},
        {8},
        rel_branch_handler
    },
    {
        "BMI",
        1,
        {0x27, 0x00},
        {branch_amt},
        {8},
        rel_branch_handler
    },
    {
        "BCS",
        1,
        {0x47, 0x00},
        {branch_amt},
        {8},
        rel_branch_handler
    },
    {
        "BEQ",
        1,
        {0x67, 0x00},
        {branch_amt},
        {8},
        rel_branch_handler
    },
    {
        "BPL",
        1,
        {0xA7, 0x00},
        {branch_amt},
        {8},
        rel_branch_handler
    },
    {
        "BCC",
        1,
        {0xC7, 0x00},
        {branch_amt},
        {8},
        rel_branch_handler
    },
    {
        "BNE",
        1,
        {0xe7, 0x00},
        {branch_amt},
        {8},
        rel_branch_handler
    },

    // SRAM
    {           // Load from address to regioster: LMA Rx, #Addr
        "LMA",
        2,
        {0x09, 0x00},
        {reg, immediate},
        {6, 8},
        basic_handler
  
    },
    {           // Store from Register to address: SMA Rx, #Addr
        "SMA",
        2,
        {0x0A, 0x00},
        {reg, immediate},
        {6, 8},
        basic_handler
  
    },
    {           // Load from Address in Rs to Rd: LMR Rd, Rs
        "LMR",
        2,
        {0x0B, 0x00},
        {reg, reg},
        {6, 12},
        basic_handler
    },
    {           // Store from Rd to address in Rs: SMR Rd, Rs
        "SMR",
        2,
        {0x0C, 0x00},
        {reg, reg},
        {6, 12},
        basic_handler
    },

    {
        "OUT",               // "OUT Rx, a/i"
        2,
        {0x08, 0x00},
        {reg, out_ai},
        {12, 5},
        basic_handler
    },
    {
        "OUT",               // "OUT #N, a/i"
        2,
        {0x0D, 0x00},
        {immediate, out_ai},
        {8, 5},
        basic_handler
    },
    
  

};
