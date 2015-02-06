///////////////////////////////////////////////////////////////////////////////////
///@file FMC116.h
///@author Arnaud Maye (4DSP) 
///\brief FMC116 module to interface with the FMC116 star (header)
///
/// This module is in charge of configuring all the chips mounted on the FMC116 
/// hardware using FMC116_x sub modules.
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef _FMC116_H_
#define _FMC116_H_

#include <stdio.h>

#include "FMC116_ctrl.h"
#include "FMC116_cpld.h"
#include "FMC116_clocktree.h"
#include "FMC116_adc.h"
#include "FMC116_freqcnt.h"
#include "FMC116_monitor.h"


enum 
{
	FMC116_INTCLK_INTREF = 0,				/*!< FMC116_init() configure the FMC116 for internal clock operations with internal reference */
	FMC116_EXTCLK = 1,						/*!< FMC116_init() configure the FMC116 for external clock operations */
	FMC116_INTCLK_EXTREF = 2,				/*!< FMC116_init() configure the FMC116 for internal clock operations with external reference */

};

/* error codes */
#define FMC116_ERR_OK					0			/*!< No error encountered during execution. */
#define FMC116_ERR_CLOCKTREE_INIT		-1			/*!< Could not initialize the clock tree peripheral on the FMC116 hardware . */
#define FMC116_ERR_ADC_INIT				-2			/*!< Could not initialize the analog to digital converter peripherals on the FMC116 hardware . */
#define FMC116_ERR_CPLD_INIT			-3			/*!< Could not initialize the cpld peripherals on the FMC116 hardware . */

// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 


/**
 * \brief Initialize both ADC chips on the FMC116. This function uses settings that should suit a general application but these settings can be changedt to suit your needs.
 * @note This function communicates with the hardware.
 *
 * @param   bar_cpld    offset where cpld peripheral is located in the constellation memory space.
 * @param   bar_clk     offset where clock tree peripheral is located in the constellation memory space.
 * @param   bar_adc0    offset where the analog to digital converter 0 is located in the constellation memory space.
 * @param   bar_adc1    offset where the analog to digital converter 1 is located in the constellation memory space.
 * @param   bar_adc2    offset where the analog to digital converter 2 is located in the constellation memory space.
 * @param   bar_adc3    offset where the analog to digital converter 3 is located in the constellation memory space.
 * @param   bar_adc_phy     offset where the analog to digital converter's phy is located in the constellation memory space.
 * @param   bar_dac0    offset where the digital to analog converter 0 is located in the constellation memory space.
 * @param   bar_dac1    offset where the digital to analog converter 1 is located in the constellation memory space.
 * @param   bar_mon     offset where the monitoring peripheral is located in the constellation memory space.
 * @param   clockmode     decide which clock operation modes is used:
 *							- FMC116_INTERNAL_CLK
 *							- FMC116_EXTERNAL_CLK
 * @param   nbrch		number of channels on the board
 * @return  - FMC116_ERR_OK
 *			- FMC116_ERR_CLOCKTREE_INIT
 *			- FMC116_ERR_ADC_INIT
 */
int FMC116_init(unsigned long bar_cpld, unsigned long bar_clk, unsigned long bar_adc0, unsigned long bar_adc1, unsigned long bar_adc2, unsigned long bar_adc3,
				unsigned long bar_adc_phy, unsigned long bar_dac0, unsigned long bar_dac1, unsigned long bar_mon, unsigned int clockmode, unsigned int nbrch);

// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_FMC116_H_