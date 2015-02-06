///////////////////////////////////////////////////////////////////////////////////
///@file FMC116_adc.h
///@author Arnaud Maye (4DSP) 
///\brief FMC116_adc module to interface with the FMC116 star (header)
///
/// This module is in charge of configuring the adc chips mounted on the 
/// FMC116 hardware.
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef _FMC116_ADC_H_
#define _FMC116_ADC_H_

/* defines */
#define FMC116_ADC_PART_ID 0x61								/*!< Expected part ID for this particular adc chip */
enum 
{	
};


/* error codes */
#define FMC116_ADC_ERR_OK						0			/*!< No error encountered during execution. */
#define FMC116_ADC_ERR_SPI_FAULT				-1			/*!< The SPI test fails. */
#define FMC116_ADC_ERR_NULL_ARGUMENT			-2			/*!< An unexpected NULL argument has been passed to a function. */
#define FMC116_DAC_ERR_OK						0			/*!< No error encountered during execution. */
#define FMC116_DAC_ERR_SPI_FAULT				-1			/*!< The SPI test fails. */
#define FMC116_DAC_ERR_NULL_ARGUMENT			-2			/*!< An unexpected NULL argument has been passed to a function. */

// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 


/**
 * \brief Initialize all four ADC chips on the FMC116. This function uses settings that should suit a general application but these settings can be changedt to suit your needs.
 * @note This function communicates with the hardware.
 *
 * @param   bar_adc0	offset where FMC116.ADC0SPI is located in the constellation memory space.
 * @param   bar_adc1	offset where FMC116.ADC1SPI is located in the constellation memory space.
 * @param   bar_adc2	offset where FMC116.ADC2SPI is located in the constellation memory space.
 * @param   bar_adc3	offset where FMC116.ADC3SPI is located in the constellation memory space.
 * @param   bar_adc_phy	offset where FMC116.ADCPHY is located in the constellation memory space.
 * @param   nbrch		number of channels on the board
 * @return  - FMC116_ADC_ERR_OK
 *			- FMC116_ADC_ERR_SPI_FAULT
 *			- FMC116_ADC_ERR_NULL_ARGUMENT
 *			- Any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int FMC116_adc_init(unsigned long bar_adc0, unsigned long bar_adc1, unsigned long bar_adc2, unsigned long bar_adc3, unsigned long bar_adc_phy, unsigned int nbrch);

/**
 * \brief Initialize both DAC chips on the FMC116. This function uses settings that should suit a general application but these settings can be changedt to suit your needs.
 * @note This function communicates with the hardware.
 *
 * @param   bar_dac0     offset where FMC116.DAC0SPI is located in the constellation memory space.
 * @param   bar_dac1     offset where FMC116.DAC1SPI is located in the constellation memory space.
 * @return  - FMC116_DAC_ERR_OK
 *			- FMC116_DAC_ERR_SPI_FAULT
 *			- FMC116_DAC_ERR_NULL_ARGUMENT
 *			- Any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int FMC116_dac_init(unsigned long bar_dac0, unsigned long bar_dac1);

/**
 * \brief Print status of the IDELAYs.
 *
 * @param	bar_adc_phy	offset where FMC116.ADCPHY is located in the constellation memory space.
 * @param   nbrch		number of channels on the board
 * @return  - FMC116_ADC_ERR_OK
 *			- Any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int fmc116_idelay_state(unsigned long bar_adc_phy, unsigned int nbrch);

// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_FMC116_ADC_H_