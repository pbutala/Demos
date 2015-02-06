///////////////////////////////////////////////////////////////////////////////////
///@file FMC204.h
///@author Arnaud Maye (4DSP) 
///\brief FMC204 module to interface with the FMC204 star (header)
///
/// This module is in charge of configuring all the chips mounted on the FMC204 
/// hardware using FMC204_x sub modules.
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef _FMC204_H_
#define _FMC204_H_

#include <stdio.h>

#include "fmc204_cpld.h"
#include "fmc204_clocktree.h"
#include "fmc204_dac.h"
#include "fmc204_freqcnt.h"
#include "fmc204_ctrl.h"

enum 
{
	FMC204_INTERNAL_CLK = 0,						/*!< FMC204_init() configure the FMC204 for external clock operations */
	FMC204_EXTERNAL_CLK = 1,						/*!< FMC204_init() configure the FMC204 for internal clock operations */
};

/* error codes */
#define FMC204_ERR_OK					0			/*!< No error encountered during execution. */
#define FMC204_ERR_CPLD_INIT			-1			/*!< Could not initialize the cpld peripheral on the FMC204 hardware . */
#define FMC204_ERR_CLOCKTREE_INIT		-2			/*!< Could not initialize the clock tree peripheral on the FMC204 hardware . */
#define FMC204_ERR_DAC_INIT				-3			/*!< Could not initialize the digital to analog converter peripherals on the FMC204 hardware . */


// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 


/**
 * \brief Initialize both ADC chips on the FMC204. This function uses settings that should suit a general application but these settings can be changedt to suit your needs.
 * @note This function communicates with the hardware.
 *
 * @param   bar_cpld     offset where CPLD peripheral is located in the constellation memory space.
 * @param   bar_clk     offset where clock tree peripheral is located in the constellation memory space.
 * @param   bar_dac0     offset where the digital to analog converter 0 is located in the constellation memory space.
 * @param   bar_dac0_phy     offset where the digital to analog converter 0's phy is located in the constellation memory space.
 * @param   bar_dac1     offset where the digital to analog converter 1 is located in the constellation memory space.
 * @param   bar_dac1_phy     offset where the digital to analog converter 1's phy is located in the constellation memory space.
 * @param   clockmode     decide which clock operation modes is used:
 *							- FMC204_INTERNAL_CLK
 *							- FMC204_EXTERNAL_CLK
 * @return  - FMC204_ERR_OK
 *			- FMC204_ERR_CPLD_INIT
 *			- FMC204_ERR_CLOCKTREE_INIT
 *			- FMC204_ERR_DAC_INIT
 *			- FMC204_ERR_PRESENT
 *			- FMC204_ERR_POWER_GOOD
 */
int FMC204_init(unsigned long bar_cpld, unsigned long bar_clk, unsigned long bar_dac0, unsigned long bar_dac0_phy, 
		unsigned long bar_dac1, unsigned long bar_dac1_phy, unsigned int clockmode);

// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_FMC204_H_