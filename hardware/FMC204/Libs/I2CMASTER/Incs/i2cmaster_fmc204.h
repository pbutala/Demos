///////////////////////////////////////////////////////////////////////////////////
///@file i2cmaster_FMC204.h
///@author Arnaud Maye (4DSP) 
///\brief i2cmaster module to interface with the i2cmaster star (header)
///
/// FMC204 related algorithm
///////////////////////////////////////////////////////////////////////////////////
#ifndef _I2CMASTER_FMC204_H_
#define _I2CMASTER_FMC204_H_

enum 
{
	I2CMASTER_DISPLAY_CONSOLE = 1,			/*!< i2cmaster_getdiagnosticsFMC204 function displays the frequency to the console */  
	I2CMASTER_NO_DISPLAY_CONSOLE = 0,		/*!< i2cmaster_getdiagnosticsFMC204 function does not display the frequency to the console */   
};


/* error codes */
#define I2CMASTER_FMC204_ERR_OK					0	/*!< No error encountered during execution. */

// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 

/**
 * Obtain all the FMC204 diagnostics.
 * @note This function communicates with the hardware.
 *
 * @param   bar     offset where FMC204.I2CMASTER is located in the constellation memory space.
 * @param   siliconrevision     pointer to a float variable about to receive the silicon revision.
 * @param   temperature			pointer to a float variable about to receive the silicon temperature.
 * @param   voltage3V3_rail		pointer to a float variable about to receive the 3.3V Voltage rail.
 * @param   voltage3V3_clk		pointer to a float variable about to receive the 3.3V Voltage for the clock tree.
 * @param   voltage1V8_dig		pointer to a float variable about to receive the 1.8V Voltage digital.
 * @param   voltage2V5_adj		pointer to a float variable about to receive the 2.5V Voltage adjustable.
 * @param   voltage2V5_clk		pointer to a float variable about to receive the 2.5V Voltage clock.
 * @param   voltage3V3_dig		pointer to a float variable about to receive the 3.3V Voltage digital.
 * @param   voltage3V3_adc		pointer to a float variable about to receive the 3.3V Voltage for the adc chips.
 * @param   voltage3V3_cp		pointer to a float variable about to receive the 3.3V Voltage for the charge pump.
 * @param   voltage12V		pointer to a float variable about to receive the 12V Voltage.
 * @param	outputconsole	decide if the function displays the frequency to the console or not. Either I2CMASTER_DISPLAY_CONSOLE or I2CMASTER_NO_DISPLAY_CONSOLE
 *							can be passed as argument.
 * @return  I2CMASTER_FMC204_ERR_OK in case of success or any ethapi ( please see ethapi documentation ) error codes.
 */
int i2cmaster_getdiagnosticsFMC204(unsigned long bar, float *siliconrevision, float *temperature, float *voltage3V3_rail, float *voltage3V3_clk, 
	float *voltage1V8_dig, float *voltage2V5_adj, float *voltage2V5_clk, float *voltage3V3_dig, float *voltage3V3_adc, 
	float *voltage3V3_cp, float *voltage12V, int outputconsole);

// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_I2CMASTER_FMC204_H_