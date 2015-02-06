///////////////////////////////////////////////////////////////////////////////////
///@file FMC116_freqcnt.cpp
///@author Arnaud Maye (4DSP) 
///\brief FMC116_freqcnt module to interface with the FMC116 star (implementation)
///
/// This module is in charge of obtaining various frequencies from the clock tree and
/// by definition communicates with the firmware over ethernet
///
///////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "FMC116_freqcnt.h"
#include "sipif.h"

int FMC116_freqcnt_getfrequency(unsigned long bar, unsigned int clksel, float *freq, int outputconsole) 
{
	unsigned long dword;
	int rc;

	// tell the firmware to start a measure on a given clock index
	rc = sipif_writesipreg(bar+0, clksel); Sleep(10);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	// read back the just measured value
	rc = sipif_readsipreg(bar+1, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	// compute the frequency
	float testClkPeriod = 1.0f/125.0f;
	float tmp = 8192 * testClkPeriod;
	tmp = (tmp / (dword + 1));
	tmp = 1.00f/tmp;
	
	// if we were asked to display to console then we do that
	if(outputconsole) {
		switch (clksel)
		{
		case 6	: printf("Trigger to FPGA : %6.2f MHz\n", tmp); break;
		case 5	: printf("Clock to FPGA   : %6.2f MHz\n", tmp); break;                    
		case 4	: printf("Clock ADC 3     : %6.2f MHz\n", tmp); break;                    
		case 3	: printf("Clock ADC 2     : %6.2f MHz\n", tmp); break;
		case 2	: printf("Clock ADC 1     : %6.2f MHz\n", tmp); break;
		case 1	: printf("Clock ADC 0     : %6.2f MHz\n", tmp); break;
		case 0	: printf("Command Clock   : %6.2f MHz\n", tmp); break;
		default	: printf("Frequency(%d)   : %6.2f MHz\n", clksel, tmp); break;       
		}
	}

	// if the pointer passed as argument is not NULL we then update the memory pointed
	if(freq!=NULL)
		*freq = tmp;

	return FMC116_FREQCNT_ERR_OK;
}