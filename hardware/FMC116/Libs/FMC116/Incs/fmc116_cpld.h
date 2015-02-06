///////////////////////////////////////////////////////////////////////////////////
///@file FMC116_cpld.h
///@author Arnaud Maye (4DSP) 
///\brief FMC116_cpld module to interface with the FMC116 star (header)
///
/// This module is in charge of configuring the CPLD mounted on the FMC116 hardware
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef _FMC116_CPLD_H_
#define _FMC116_CPLD_H_


enum 
{	// clock sources
	CLKSRC_EXTERNAL_CLK = 0,						/*!< FMC116_cpld_init() external clock. */
	CLKSRC_INTERNAL_CLK_EXTERNAL_REF = 3,			/*!< FMC116_cpld_init() internal clock / external reference. */
	CLKSRC_INTERNAL_CLK_INTERNAL_REF = 6,			/*!< FMC116_cpld_init() internal clock / internal reference. */
};

/* error codes */
#define FMC116_CPLD_ERR_OK						0			/*!< No error encountered during execution. */
#define FMC116_CPLD_ERR_CONFIG_VERIFICATION		-1			/*!< FMC116_cpld_init() could not read back the updated registers. */

// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 


/**
 * Send a hard reset to the clock tree chip mounted on the FMC116 hardware.
 *
 * @note This function communicates with the hardware.
 *
 * @param   bar     offset where FMC116.CPLD is located in the constellation memory space.
 * @return  - FMC116_CPLD_ERR_OK
 *			- Any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int FMC116_cpld_resetSPIclocktree(unsigned long bar);

/**
 * Send a hard reset to the analog to digital converter chips mounted on the FMC116 hardware.
 *
 * @note This function communicates with the hardware.
 *
 * @param   bar     offset where FMC116.DAC is located in the constellation memory space.
 * @return  - FMC116_CPLD_ERR_OK 
 *			- Any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int FMC116_cpld_resetSPIdac(unsigned long bar);

/**
 * Configure the CPLD mounted on the FMC116 in order to have desired operation modes. This function allows configuring 
 * direction of the signals provided on the HDMI connector as well as clocking mode and various other settings
 *
 * @note This function communicates with the hardware.
 *
 * @param   bar			offset where FMC116.CPLD is located in the constellation memory space.
 * @param   clksrc     clocking options on the FMC116:
 *						- CLKSRC_EXTERNAL_CLK
 *						- CLKSRC_INTERNAL_CLK_EXTERNAL_REF
 *						- CLKSRC_INTERNAL_CLK_INTERNAL_REF
 * @return  - FMC116_CPLD_ERR_OK
 *			- FMC116_CPLD_ERR_CONFIG_VERIFICATION
 *			- Any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int FMC116_cpld_init(unsigned long bar, unsigned int clksrc);


// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_FMC116_CPLD_H_