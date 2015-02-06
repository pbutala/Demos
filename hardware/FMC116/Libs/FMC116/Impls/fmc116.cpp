///////////////////////////////////////////////////////////////////////////////////
///@file FMC116.cpp
///@author Arnaud Maye (4DSP) 
///\brief FMC116 module to interface with the FMC116 star (implementation)
///
/// This module is in charge of configuring all the chips mounted on the FMC116 
/// hardware using FMC116_x sub modules.
///
///////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdlib.h>
#include "FMC116.h"
#include "sipif.h"
#include "cid.h"

int FMC116_init(unsigned long bar_cpld, unsigned long bar_clk, unsigned long bar_adc0, unsigned long bar_adc1, unsigned long bar_adc2, unsigned long bar_adc3,
				unsigned long bar_adc_phy, unsigned long bar_dac0, unsigned long bar_dac1, unsigned long bar_mon, unsigned int clockmode, unsigned int nbrch)
{

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Initialize CPLD
	if(FMC116_cpld_init(bar_cpld, clockmode)!=FMC116_CPLD_ERR_OK ) {
		printf("Could not initialize FMC116.CPLD\n");
		sipif_free();
		return FMC116_ERR_CPLD_INIT;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Configure the clock tree 
	if(FMC116_clocktree_init(bar_clk, clockmode)!=FMC116_CLOCKTREE_ERR_OK){
		printf("Could not initialize FMC116.CLOCKTREE\n");
		sipif_free();
		return FMC116_ERR_CLOCKTREE_INIT;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Configure ADC
	if(FMC116_adc_init(bar_adc0, bar_adc1, bar_adc2, bar_adc3, bar_adc_phy, nbrch)!=FMC116_ADC_ERR_OK) {
		printf("Could not initialize FMC116.ADC\n");
		sipif_free();
		return FMC116_ERR_ADC_INIT;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Configure DAC
	if(FMC116_dac_init(bar_dac0, bar_dac1)!=FMC116_ADC_ERR_OK) {
		printf("Could not initialize FMC116.DAC\n");
		sipif_free();
		return FMC116_ERR_ADC_INIT;
	}

	// Little pause after init
	Sleep(10);

	return FMC116_ERR_OK;
}