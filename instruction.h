#include <stdint.h>
#define MAX_LINE_LENGTH	    300
#define MAX_TOKENS          4
#define MAX_TOKEN_LENGTH    20
#define NUM_INSTRUCTIONS    5

typedef enum param {
    none, 
    instruction, 
    reg, 
    immediate,
    out_hl,
    out_id
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


Instruction assembly[] = {
        {
        "LDR",
        2,
        {0x03, 0x00},
        {reg, immediate},
        {6, 8},
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
        "OUT",  // OUT Rx, h/l
        2,
        {0x08, 0x00},
        {reg, out_hl},
        {12, 4},
        basic_handler
    },
    {
        "BNE", 
        1,
        {0xe5, 0x00},
        {immediate},
        {8},
        abs_branch_handler
    },
    {
        "BRK",
        0,
        {0x01, 0x00},
        {none},
        {0},
        basic_handler
    }
};
