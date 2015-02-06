///////////////////////////////////////////////////////////////////////////////////
///@file FMC204_dac.h
///@author Arnaud Maye (4DSP) 
///\brief FMC204_dac module to interface with the FMC204 star (header)
///
/// This module is in charge of configuring the DAC chips populated
/// on the FMC204 printed circuit board.
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef _FMC204_DAC_H_
#define _FMC204_DAC_H_

/* defines */
#define FMC204_DAC_PART_ID			0x00					/*!< Expected part ID for this particular DAC chip */
#define FMC204_DAC_DLL_LOCKED		0x40					/*!< DAC chip's status register : DLL locked status */
#define FMC204_DAC_FIFO_ERROR		0x20					/*!< DAC chip's status register : FIFO check has failed */
#define FMC204_DAC_PATTERN_ERROR	0x10					/*!< DAC chip's status register : pattern check has failed */

enum 
{	
};


/* error codes */
#define FMC204_DAC_ERR_OK						0			/*!< No error encountered during execution. */
#define FMC204_DAC_ERR_WRONG_PART_ID			-1			/*!< The part ID obtained from the DAC chip does not match what we expect. */
#define FMC204_DAC_ERR_DLL_NOT_LOCKED			-2			/*!< The DLL peripheral in the DAC chip did not lock after reset. */
#define FMC204_DAC_ERR_CHAN0_FIFO				-3			/*!< The FIFO check has failed for DAC0 */
#define FMC204_DAC_ERR_CHAN1_FIFO				-4			/*!< The FIFO check has failed for DAC1 */
#define FMC204_DAC_ERR_CHAN0_PATTERN			-5			/*!< The pattern check has failed for DAC0 */
#define FMC204_DAC_ERR_CHAN1_PATTERN			-6			/*!< The pattern check has failed for DAC1 */


// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 


/**
 * \brief Initialize both DAC chips on the FMC204. This function uses settings that should suit a general application but these settings can be changedt to suit your needs.
 * @note This function communicates with the hardware.
 *
 * @param   bar_dac0     offset where FMC204.DAC0SPI is located in the constellation memory space.
 * @param   bar_dac0_phy     offset where FMC204.DAC0PHY is located in the constellation memory space.
 * @param   bar_dac1     offset where FMC204.DAC1SPI is located in the constellation memory space.
 * @param	bar_dac1_phy	offset where FMC204.DAC1PHY is located in the constellation memory space.
 * @return  - FMC204_DAC_ERR_OK
 *			- FMC204_DAC_ERR_WRONG_PART_ID
 *			- FMC204_DAC_ERR_DLL_NOT_LOCKED
 *			- FMC204_DAC_ERR_CHAN0_FIFO
 *			- FMC204_DAC_ERR_CHAN1_FIFO
 *			- FMC204_DAC_ERR_CHAN0_PATTERN
 *			- FMC204_DAC_ERR_CHAN1_PATTERN
 */
int FMC204_dac_init(unsigned long bar_dac0, unsigned long bar_dac0_phy, unsigned long bar_dac1, unsigned long bar_dac1_phy) ;

// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_FMC204_DAC_H_