///////////////////////////////////////////////////////////////////////////////////
///@file FMC116_monitor.h
///@author Arnaud Maye (4DSP) 
///\brief FMC116_monitor module to interface with the FMC116 star (header)
///
/// This module is in charge of controlling the monitoring device populated
/// on the FMC116 printed circuit board.
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef _FMC116_MON_H_
#define _FMC116_MON_H_

/* defines */
#define FMC116_MON_PART_REVISION 0xE000						/*!< Expected part revision for this particular monitoring device */

enum 
{
	FMC116_DIAG_DISPLAY_CONSOLE = 1,						/*!< FMC116_getdiagnostics function displays the results to the console */  
	FMC116_DIAG_NO_DISPLAY_CONSOLE = 0,						/*!< FMC116_getdiagnostics function does not display the results to the console */   
};

#define FMC116_ADT_DEV	1

/* error codes */
#define FMC116_MON_ERR_OK						0			/*!< No error encountered during execution. */
#define FMC116_MON_ERR_WRONG_PART_REVISION		-1			/*!< The part revision obtained from the monitoring device does not match what we expect. */
#define FMC116_MON_ERR_SPI_FAULT				-2			/*!< The SPI bus test has failed */
#define FMC116_MON_ERR_VOLTAGE_OUT_OF_RANGE		-3			/*!< One of the voltages on the board is below/above the threshold */


// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 


/**
 * \brief Configure and readout monitoring device on the FMC116. This function uses settings that should suit a general application but these settings can be changed to suit your needs.
 * @note This function communicates with the hardware.
 *
 * @param   bar_mon     offset where FMC116.MONSPI is located in the constellation memory space.
 * @return  - FMC116_MON_ERR_OK
 *			- FMC116_MON_ERR_WRONG_PART_REVISION
 *			- FMC116_MON_ERR_SPI_FAULT
 *			- FMC116_MON_ERR_VOLTAGE_OUT_OF_RANGE
 */
int FMC116_monitor_getdiags(unsigned long bar_mon);

/**
 * \brief Configure and readout monitoring device on the FMC116. This function uses settings that should suit a general application but these settings can be changed to suit your needs.
 * @note This function communicates with the hardware and returns the values it finds.
 *
 * @param   bar_mon     offset where FMC116.MONSPI is located in the constellation memory space.
 * @param	deviceNum	the ADT device (1 or 2)
 * @param	dev_id		the device ID
 * @param	man_id		the manufacturer ID
 * @param	silicon_rev	the silicon revision
 * @param	chipTemp	the on-chip temperature
 * @param	vdd			expected voltage on the devices's voltage input.
 * @param	ain1		expected voltage on the devices's analog 1 input.
 * @param	ain2		expected voltage on the devices's analog 2 input.
 * @param	ain3		expected voltage on the devices's analog 3 input.
 * @param	ain4		expected voltage on the devices's analog 4 input.
 * @param	ain5		expected voltage on the devices's analog 5 input.
 * @param	ain6		expected voltage on the devices's analog 6 input.
 * @param	ain7		expected voltage on the devices's analog 7 input.
 * @param	ain8		expected voltage on the devices's analog 8 input.
 * @return  - FMC116_MON_ERR_OK
 *			- FMC116_MON_ERR_WRONG_PART_REVISION
 *			- FMC116_MON_ERR_SPI_FAULT
 *			- FMC116_MON_ERR_VOLTAGE_OUT_OF_RANGE
 */
int FMC116_monitor_getDeviceDiags(unsigned long bar_mon, int deviceNum, ULONG* dev_id, ULONG* man_id, ULONG* silicon_rev,
								  float* chipTemp, float* vdd, float* ain1, float* ain2, 
								  float* ain3, float* ain4, float* ain5, float* ain6, float* ain7, float* ain8);

// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_FMC116_MON_H_