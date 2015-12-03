/** Generates HardFaults
 */

#include <stdint.h>

__asm void executeAt(uint16_t* x);
void busDataFault(void);
void undefinedInstruction(void);
void instructionAccessViolation(void);
void instructionBusError(void);
