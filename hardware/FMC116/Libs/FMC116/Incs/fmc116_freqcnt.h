///////////////////////////////////////////////////////////////////////////////////
///@file FMC116_freqcnt.h
///@author Arnaud Maye (4DSP) 
///\brief FMC116_freqcnt module to interface with the FMC116 star (header)
///
/// This module is in charge of obtaining various frequencies from the clock tree and
/// by definition communicates with the firmware over ethernet
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef _FMC116_FREQCNT_H_
#define _FMC116_FREQCNT_H_


enum 
{
	FMC116_FREQCNT_DISPLAY_CONSOLE = 1,			/*!< FMC116_freqcnt_getfrequency function displays the frequency to the console */  
	FMC116_FREQCNT_NO_DISPLAY_CONSOLE = 0,		/*!< FMC116_freqcnt_getfrequency function does not display the frequency to the console */   
};


/* error codes */
#define FMC116_FREQCNT_ERR_OK					0	/*!< No error encountered during execution. */


// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 


/**
 * Obtain a given frequency from the clock tree. This function can either display the frequency on the console or not.
 * @note This function communicates with the hardware.
 *
 * @param   bar     offset where FMC116.FREQCNT is located in the constellation memory space.
 * @param   clksel     ID of the clock we want to obtain. Each clock in the clock tree has an ID, the following value can be passed as argument.
 *						- StellarIP (0)
 *						- ADC0 PHY (1)
 *						- DAC0 PHY (2)
 *						- External Trigger (3)
 *						- ADC1 PHY (4)
 *						- DAC1 PHY (5)
 *						- DAC0/DAC1 Reference (6)
 * @param   freq     pointer to a memory location where the frequency will be written. This area should be big enough to hold a float ( 4 bytes ).
 *					 This argument is optional and NULL can be passed instead a pointer if you are not interested to obtain the value in a variable.
 * @param	outputconsole	decide if the function displays the frequency to the console or not. Either FMC116_FREQCNT_DISPLAY_CONSOLE or FMC116_FREQCNT_NO_DISPLAY_CONSOLE
 *							can be passed as argument.
 * @return  FMC116_FREQCNT_ERR_OK in case of success or any ethapi ( please see ethapi documentation ) error codes.
 */
int FMC116_freqcnt_getfrequency(unsigned long bar, unsigned int clksel, float *freq, int outputconsole);

// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_FMC116_FREQCNT_H_
