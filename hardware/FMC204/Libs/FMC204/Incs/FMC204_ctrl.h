///////////////////////////////////////////////////////////////////////////////////
///@file FMC204_ctrl.h
///@author Arnaud Maye (4DSP) 
///\brief FMC204_ctrl module to interface with the FMC204 star (header)
///
/// This module is in charge of configuring the FMC204 control module. This module
/// allows users to configure the grabbing and channel enables among various other 
/// things.
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef _FMC204_CTRL_H_
#define _FMC204_CTRL_H_

/* defines */

enum 
{

	DAC0 = 0,				/*!< Prepare to send waveform to DAC0, argument for FMC204_ctrl_prepare_wfm_load() */ 
	DAC1 = 1,				/*!< Prepare to send waveform to DAC1, argument for FMC204_ctrl_prepare_wfm_load() */ 
	DAC2 = 2,				/*!< Prepare to send waveform to DAC2, argument for FMC204_ctrl_prepare_wfm_load() */ 
	DAC3 = 3,				/*!< Prepare to send waveform to DAC3, argument for FMC204_ctrl_prepare_wfm_load() */ 

	ENABLED = 1,			/*!< A specific channel is enabled, argument for FMC204_ctrl_enable_channel() */ 
	DISABLED = 0,			/*!< A specific channel is disabled, argument for FMC204_ctrl_enable_channel() */ 
};


/* error codes */
#define FMC204_CTRL_ERR_OK					0	/*!< No error encountered during execution. */
#define FMC204_CTRL_ERR_PRESENT				-1	/*!< No FMC204 hardware is found. */
#define FMC204_CTRL_ERR_POWER_GOOD			-2	/*!< FMC204 hardware indicates a power failure. */


// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 


/**
 * \brief do nothing.
 * @note This function does not communicate with the hardware.
 *
 * @param   bar     offset where FMC204.CTRL is located in the constellation memory space.
 * @return  - FMC204_CTRL_ERR_OK
 */
int FMC204_ctrl_init(unsigned long bar);

/**
 * \brief check if an FMC daughter board is actually connected to the main board.
 * @note This function communicates with the hardware.
 *
 * @param   bar     offset where FMC204.CTRL is located in the constellation memory space.
 * @return  - FMC204_CTRL_ERR_OK
 *			- FMC204_ERR_PRESENT
 *			- FMC204_ERR_POWER_GOOD
 */
int FMC204_ctrl_probefmc(unsigned long bar);

/**
 * Configure FMC204 for a given burst number and burst size. The actual amount of data provided by the firmware is then
 *
 * size(byte) = burstnumber * burstlength * 2
 *
 * @note This function communicates with the hardware.
 *
 * @param   bar     offset where FMC204.CTRL is located in the constellation memory space.
 * @param   burstnumber     number of burst the FMC204 firmware should acquire.
 * @param   burstlength     lenght of a burst ( in samples where one sample equal 16 bit, 2 bytes ).
 * @return  - FMC204_CTRL_ERR_OK or any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int FMC204_ctrl_configure_burst(unsigned long bar, unsigned int burstnumber, unsigned int burstlength);

/**
 * Prepare FMC204 firmware to receive data on a given waveform memory.
 *
 * @note This function communicates with the hardware.
 *
 * @param   bar     offset where FMC204.CTRL is located in the constellation memory space.
 * @param   dacchannel     decide if the next data received should be place in DAC 0 waveform memory ( DAC0 ) or DAC 1
 *							waveform memory ( DAC1 ).
 * @return  - FMC204_CTRL_ERR_OK or any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int FMC204_ctrl_prepare_wfm_load(unsigned long bar, unsigned int dacchannel);

/**
 * Tell the firmware to enable or disable DAC channels. Four argument for four channels are available and can be
 * individually set.
 *
 * @note This function communicates with the hardware.
 *
 * @param   bar     offset where FMC204.CTRL is located in the constellation memory space.
 * @param   dac0en     decide if DAC0 is enabled ( ENABLED ) or disabled ( DISABLED ).
 * @param   dac1en     decide if DAC1 is enabled ( ENABLED ) or disabled ( DISABLED ).
 * @param   dac2en     decide if DAC0 is enabled ( ENABLED ) or disabled ( DISABLED ).
 * @param   dac3en     decide if DAC1 is enabled ( ENABLED ) or disabled ( DISABLED ).
 * @return  - FMC204_CTRL_ERR_OK or any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int FMC204_ctrl_enable_channel(unsigned long bar, unsigned char dac0en, unsigned char dac1en, 
							   unsigned char dac2en, unsigned char dac3en);


/**
 * Send a software trigger to the FMC204 firmware. As soon a trigger is received, the FMC204 firmware start to acquire samples.
 * the actual number of samples to acquire is configured using FMC204_ctrl_configure_burst(). Calling this function is equivalent
 * sending a trigger to the external trigger input connector on the FMC204.
 *
 * @note This function communicates with the hardware.
 *
 * @param   bar     offset where FMC204.CTRL is located in the constellation memory space.
 * @return  - FMC204_CTRL_ERR_OK or any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int FMC204_ctrl_sw_trigger(unsigned long bar);

/**
 * Disarm the digital to analog converters on the FMC204. In other words, calling this function prevents the digital to analog
 * converter to play the samples stored in the waveform memory.
 *
 * @note This function communicates with the hardware.
 *
 * @param   bar     offset where FMC204.CTRL is located in the constellation memory space.
 * @return  - FMC204_CTRL_ERR_OK or any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int FMC204_ctrl_disarm_dac(unsigned long bar);


/**
 * Arm the digital to analog converters on the FMC204. In other words, calling this function makes the digital to analog
 * converter to play the samples stored in the waveform memory. Implicitly, waveform data has to be present in the waveform
 * memory.
 *
 * @note This function communicates with the hardware.
 *
 * @param   bar     offset where FMC204.CTRL is located in the constellation memory space.
 * @return  - FMC204_CTRL_ERR_OK or any ethapi error codes ( please consult ethapi documentation for more informations ).
 */
int FMC204_ctrl_arm_dac(unsigned long bar);


// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_FMC204_CTRL_H_