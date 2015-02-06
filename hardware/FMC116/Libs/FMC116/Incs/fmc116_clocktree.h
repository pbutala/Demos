///////////////////////////////////////////////////////////////////////////////////
///@file FMC116_clocktree.h
///@author Arnaud Maye (4DSP) 
///\brief FMC116_clocktree module to interface with the FMC116 star (header)
///
/// This module is in charge of configuring the clocktree chip mounted on the 
/// FMC116 hardware.
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef _FMC116_CLOCKTREE_H_
#define _FMC116_CLOCKTREE_H_


#define FMC116_CLOCKTREE_PART_ID_3 0x53						/*!< Expected part ID for the AD9517-3 clock tree chip */

enum 
{
	CLOCKTREE_INTCLK_INTREF = 0,					/*!< FMC116_clocktree_init() configure the clock tree for internal clock operations with internal reference */
	CLOCKTREE_EXTCLK = 1,							/*!< FMC116_clocktree_init() configure the clock tree for external clock operations */
	CLOCKTREE_INTCLK_EXTREF = 2,					/*!< FMC116_clocktree_init() configure the clock tree for internal clock operations with external reference */
};


/* error codes */
#define FMC116_CLOCKTREE_ERR_OK						0		/*!< No error encountered during execution. */
#define FMC116_CLOCKTREE_ERR_WRONG_PART_ID			-1		/*!< The part ID obtained from the clock tree chip does not match what we expect. */
#define FMC116_CLOCKTREE_ERR_CLK0_PLL_NOT_LOCKED	-2		/*!< The PLL in the clock tree chip did not lock after a reset */
#define FMC116_CLOCKTREE_ERR_SPI_FAULT				-3		/*!< Error during SPI communication. */
#define FMC116_CLOCKTREE_ERR_NO_LOCK				-4		/*!< Internal clock is selected, but the PLL has no lock. */

// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 

/**
 * Initialize the clock tree chip on the FMC116. This function uses settings that should suit a general application but these settings can be changedt to suit your needs.
 * @note This function communicates with the hardware.
 * @warning the clocking setting argument should match FMC116.CPLD clock mode to provide correct operations.
 *
 * @param   bar     offset where FMC116.CLOCKTREE is located in the constellation memory space.
 * @param   clockmode     clocking mode for the clock tree chip :
 *							- CLOCKTREE_INTCLK_INTREF
 *							- CLOCKTREE_EXTCLK
 *							- CLOCKTREE_INTCLK_EXTREF
 * @return  - FMC116_CLOCKTREE_ERR_OK
 *			- FMC116_CLOCKTREE_ERR_WRONG_PART_REV
 *			- FMC116_CLOCKTREE_ERR_CLK0_PLL_NOT_LOCKED
 *			- FMC116_CLOCKTREE_ERR_SPI_FAULT
 *			- FMC116_CLOCKTREE_ERR_NO_LOCK
 *			- Any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int FMC116_clocktree_init(unsigned long bar,unsigned int clockmode);


// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_FMC116_CLOCKTREE_H_