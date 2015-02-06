///////////////////////////////////////////////////////////////////////////////////
///@file FMC204_cpld.h
///@author Arnaud Maye (4DSP) 
///\brief FMC204_cpld module to interface with the FMC204 star (header)
///
/// This module is in charge of configuring the CPLD mounted on the FMC204 hardware
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef _FMC204_CPLD_H_
#define _FMC204_CPLD_H_


enum 
{	// clock sources
	CLKSRC_EXTERNAL_CLK = 0,						/*!< FMC204_cpld_init() external clock. */
	CLKSRC_INTERNAL_CLK_EXTERNAL_REF = 3,			/*!< FMC204_cpld_init() internal clock / external reference. */
	CLKSRC_INTERNAL_CLK_INTERNAL_REF = 6,			/*!< FMC204_cpld_init() internal clock / internal reference. */

	// sync sources
	SYNCSRC_EXTERNAL_TRIGGER = 0,					/*!< FMC204_cpld_init() external trigger. */
	SYNCSRC_HOST = 1,								/*!< FMC204_cpld_init() software trigger. */
	SYNCSRC_CLOCK_TREE = 2,							/*!< FMC204_cpld_init() signal from the clock tree. */
	SYNCSRC_NO_SYNC = 3,							/*!< FMC204_cpld_init() no synchronization. */

	// FAN enable bits
	FAN0_ENABLED = (0<<4),							/*!< FMC204_cpld_init() FAN 0 is enabled */
	FAN1_ENABLED = (0<<5),							/*!< FMC204_cpld_init() FAN 1 is enabled */
	FAN2_ENABLED = (0<<6),							/*!< FMC204_cpld_init() FAN 2 is enabled */
	FAN3_ENABLED = (0<<7),							/*!< FMC204_cpld_init() FAN 3 is enabled */
	FAN0_DISABLED = (1<<4),							/*!< FMC204_cpld_init() FAN 0 is disabled */
	FAN1_DISABLED = (1<<5),							/*!< FMC204_cpld_init() FAN 1 is disabled */
	FAN2_DISABLED = (1<<6),							/*!< FMC204_cpld_init() FAN 2 is disabled */
	FAN3_DISABLED = (1<<7),							/*!< FMC204_cpld_init() FAN 3 is disabled */

	// LVTTL bus direction (HDMI connector)
	DIR0_INPUT	= (0<<0),							/*!< FMC204_cpld_init() DIR 0 is input */
	DIR1_INPUT	= (0<<1),							/*!< FMC204_cpld_init() DIR 1 is input */
	DIR2_INPUT	= (0<<2),							/*!< FMC204_cpld_init() DIR 2 is input */
	DIR3_INPUT	= (0<<3),							/*!< FMC204_cpld_init() DIR 3 is input */
	DIR0_OUTPUT	= (1<<0),							/*!< FMC204_cpld_init() DIR 0 is output */
	DIR1_OUTPUT	= (1<<1),							/*!< FMC204_cpld_init() DIR 1 is output */
	DIR2_OUTPUT	= (1<<2),							/*!< FMC204_cpld_init() DIR 2 is output */
	DIR3_OUTPUT	= (1<<3),							/*!< FMC204_cpld_init() DIR 3 is output */
};

/* error codes */
#define FMC204_CPLD_ERR_OK						0			/*!< No error encountered during execution. */
#define FMC204_CPLD_ERR_CONFIG_VERIFICATION		-1			/*!< FMC204_cpld_init() could not read back the updated registers. */

// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 


/**
 * Send a hard reset to the clock tree chip mounted on the FMC204 hardware.
 *
 * @note This function communicates with the hardware.
 *
 * @param   bar     offset where FMC204.CPLD is located in the constellation memory space.
 * @return  - FMC204_CPLD_ERR_OK
 *			- Any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int FMC204_cpld_resetSPIclocktree(unsigned long bar);

/**
 * Send a hard reset to the digital to analog converter chips mounted on the FMC204 hardware.
 *
 * @note This function communicates with the hardware.
 *
 * @param   bar     offset where FMC204.CPLD is located in the constellation memory space.
 * @return  - FMC204_CPLD_ERR_OK 
 *			- Any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int FMC204_cpld_resetSPIDACs(unsigned long bar);

/**
 * Configure the CPLD mounted on the FMC204 in order to have desired operation modes. This function allows configuring 
 * direction of the signals provided on the HDMI connector as well as clocking mode and various other settings
 *
 * @note This function communicates with the hardware.
 *
 * @param   bar     offset where FMC204.CPLD is located in the constellation memory space.
 * @param   clksrc     clocking options on the FMC204:
 *						- CLKSRC_EXTERNAL_CLK
 *						- CLKSRC_INTERNAL_CLK_EXTERNAL_REF
 *						- CLKSRC_INTERNAL_CLK_INTERNAL_REF
 * @param   syncsrc     synchronization options on the FMC204 :
 *						- SYNCSRC_EXTERNAL_TRIGGER
 *						- SYNCSRC_HOST
 *						- SYNCSRC_CLOCK_TREE
 *						- SYNCSRC_NO_SYNC
 * @param   fans     This is the fan enabled field. One bit per FAN. Boolean operations can be done on this options.
 *						- FAN0_ENABLED / FAN0_DISABLED
 *						- FAN1_ENABLED / FAN1_DISABLED
 *						- FAN2_ENABLED / FAN2_DISABLED
 *						- FAN3_ENABLED / FAN3_DISABLED
 * @param   dirs     This is the bus direction field. One bit per direction. Boolean operations can be done on this options.
 *						- DIR0_INPUT / DIR0_OUTPUT
 *						- DIR1_INPUT / DIR1_OUTPUT
 *						- DIR2_INPUT / DIR2_OUTPUT
 *						- DIR3_INPUT / DIR3_OUTPUT
 * @return  - FMC204_CPLD_ERR_OK
 *			- FMC204_CPLD_ERR_CONFIG_VERIFICATION
 *			- Any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int FMC204_cpld_init(unsigned long bar, unsigned int clksrc, unsigned int syncsrc, unsigned char fans, unsigned char dirs);


// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_FMC204_CPLD_H_