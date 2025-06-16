/*  LUT definitions */

#include <stdint.h>
#include <stddef.h>

//  A LUT (look-up table) is a memory-based implementation of a combinational
//  logic function f: {0, 1}^m -> {0, 1}^n.
//  
//  Given a particular combinational logic function f, the corresponding LUT
//  would be of size |{0, 1}|^m * n = (2^m) * n, containing f(x) for each
//  x in {0, 1}^m, where f(x_1) appears before f(x_2) in the LUT if x_1 appears
//  before x_2 in the lexicographic order of the set of input strings {0, 1}^m.
//
//  This EEPROM LUT generator generates a binary file that contains a particular
//  LUT for storage in an EEPROM. The EEPROM receives the input binary strings
//  of length m via its address inputs, and produces the corresponding n-bit
//  output strings via its data output port.
//  The EEPROMs we use all have 8-bit data output ports; hence, our LUTs are
//  restricted such that n <= 8.
//  The EEPROMs we use have a maximum of 16 address bits; hence, our LUTs are
//  restricted such that m <= 16.
//
//  The following assumes that an EEPROM with 16 address bits and 8 data bits is
//  used:
//  Given a LUT with m < 16 or n < 8, the unused bits shall be the MSBs; in
//  other words, the m address bits and n output bits will be the LSBs of the
//  EEPROM's address and data ports, respectively.
//  Hence, the (16 - m) unused address bits must be the MSBs which are connected
//  to ground (0) inputs.
//  The (8 - n) unused data bits will be the MSBs and may contain either 0s or
//  1s.

/**
 * @brief State of input bits of a LUT.
 * Each LUT must explicitly define which of the InputState bits are in use and
 * what input signals they are mapped to.
 * If any bits of the InputState are unused, they must be the MSBs; in other 
 * words, all InputState bits that are in use must be the LSBs of the 
 * InputState
 * @note The size of the InputState is limited to 16 bits; this restriction
 * exists as the EEPROMs we use have a maximum of 16 address bits.
 */
typedef uint16_t InputState;

/**
 * @brief State of output bits of a LUT.
 * Each LUT must explicitly define which of the OutputState bits are in use and
 * what output signals they are mapped to.
 * If any bits of the OutputState are unused, the must be the MSBs; in other
 * words, all OutputState bits that are in use must be the LSBs of the
 * OutputState.
 * @note The size of the OutputState is limited to 8 bits; this restriction
 * exists as the EEPROMs we use always have 8 data bits.
 */
typedef uint8_t OutputState;

#define MAX_OUTPUT_STATE    0xff

/**
 * @brief LUT struct type.
 * Specifies the number of inputs of the LUT (m), number of outputs (n), and
 * holds a function pointer to the LUT's combinational logic function (f).
 */
typedef struct LUT_t {
    /**
     * @brief Number of 1-bit inputs of this LUT (m).
     * @note Must be stored in the LSBs of an InputState.
     */
    int numInputs;

    /**
     * @brief Number of 1-bit outputs of this LUT (n).
     * @note Must be stored as the LSBs of an OutputState.
     */
    int numOutputs;

    /**
     * @brief Function pointer to this LUT's combinational logic function (f).
     * Given an InputState x, this function generates the corresponding
     * OutputState y.
     */
    OutputState (*generateOutputState) (InputState input);
} LUT;

/*  LUT definitions */

OutputState ARDataBusLoadEnableGenerator(InputState state);
OutputState ARSelectGenerator(InputState state);

static LUT luts[] = {
    {   //  AR Data Bus Load Enable LUT
        8, 8, ARDataBusLoadEnableGenerator
    },
    {   //  ARSelect LUT
        8, 5, ARSelectGenerator
    }
};