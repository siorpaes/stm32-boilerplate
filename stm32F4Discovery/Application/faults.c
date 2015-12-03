/** Generates HardFaults
 */

#include <stdint.h>
#include "faults.h"

/* Invalid RAM address, just 1KB out of SRAM boundaries */
#define OUTER_SRAM (0x20000000 + 193 * 1024)
#define PERIPHERALS_ADDR (0x40020000)

/* Vector containing invalid instructions */
uint16_t undefinedInstructions[] = {0xf3fd, 0x4aff, 0x4455};

/* Executes code from a given address
 * We need to add one to the address to set Thumb mode
 */
__asm void executeAt(uint16_t* x);
__asm void executeAt(uint16_t* x)
{
	ADD R0, R0, #1
	BX	R0
}


/** Triggers a Bus Data Fault
 * Ends up with CFSR = PRECISERR + BFARVALID (0x8200)
 * or
 *              CFSR = IMPRECISERR (0x0400)
 */
void busDataFault(void)
{
	/* Disable Write Buffer so to avoid IMPRECISERR */
	*((uint32_t*)(0xE000E008)) |= 0x2;
	
	/* Try to access data at end of RAM */
	*((uint32_t*)(OUTER_SRAM)) = 0;
}


/** Triggers an undefined instruction
 * Ends up with CFSR = UNDEFINSTR (0x10000)
 */
void undefinedInstruction(void)
{
	executeAt((uint16_t*)undefinedInstructions);
}


/** Triggers an Instruction Access Violation
 * Ends up with CFSR = IACCVIOL (0x1)
 */
void instructionAccessViolation(void)
{
	executeAt((uint16_t*)PERIPHERALS_ADDR);
}


/** Triggers an Instruction Bus Error
 * Ends up with CFSR = IBUSERR (0x100)
 */
void instructionBusError(void)
{
	executeAt((uint16_t*)OUTER_SRAM);
}

