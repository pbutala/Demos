///////////////////////////////////////////////////////////////////////////////////
///@file FMC204_clocktree.h
///@author Arnaud Maye (4DSP) 
///\brief FMC204_clocktree module to interface with the FMC204 star (header)
///
/// This module is in charge of configuring the clocktree chip mounted on the 
/// FMC204 hardware.
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef _FMC204_CLOCKTREE_H_
#define _FMC204_CLOCKTREE_H_


#define FMC204_CLOCKTREE_PART_ID_0 0x11						/*!< Expected part ID for the AD9517-0 clock tree chip */
#define FMC204_CLOCKTREE_PART_ID_1 0x51						/*!< Expected part ID for the AD9517-1 clock tree chip */
#define FMC204_CLOCKTREE_PART_ID_2 0x91						/*!< Expected part ID for the AD9517-2 clock tree chip */
#define FMC204_CLOCKTREE_PART_ID_3 0x53						/*!< Expected part ID for the AD9517-3 clock tree chip */
#define FMC204_CLOCKTREE_PART_ID_4 0xD3						/*!< Expected part ID for the AD9517-4 clock tree chip */

enum 
{
	CLOCKTREE_CLKSRC_EXTERNAL = 0,							/*!< FMC204_clocktree_init() configure the clock tree for external clock operations */
	CLOCKTREE_CLKSRC_INTERNAL = 1,							/*!< FMC204_clocktree_init() configure the clock tree for internal clock operations */
};


/* error codes */
#define FMC204_CLOCKTREE_ERR_OK						0		/*!< No error encountered during execution. */
#define FMC204_CLOCKTREE_ERR_WRONG_PART_ID			-1		/*!< The part ID obtained from the clock tree chip does not match what we expect. */
#define FMC204_CLOCKTREE_ERR_CLK0_PLL_NOT_LOCKED	-2		/*!< The PLL in the clock tree chip did not lock after a reset */

// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 

/**
 * Initialize the clock tree chip on the FMC204. This function uses settings that should suit a general application but these settings can be changedt to suit your needs.
 * @note This function communicates with the hardware.
 * @warning the clocking setting argument should match FMC204.CPLD clock mode to provide correct operations.
 *
 * @param   bar     offset where FMC204.CLOCKTREE is located in the constellation memory space.
 * @param   clocksource     clocking mode for the clock tree chip :
 *							- CLOCKTREE_CLKSRC_EXTERNAL
 *							- CLOCKTREE_CLKSRC_INTERNAL
 * @return  - FMC204_CLOCKTREE_ERR_OK
 *			- FMC204_CLOCKTREE_ERR_WRONG_PART_ID
 *			- Any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int FMC204_clocktree_init(unsigned long bar, unsigned int clocksource);

// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_FMC204_CLOCKTREE_H_