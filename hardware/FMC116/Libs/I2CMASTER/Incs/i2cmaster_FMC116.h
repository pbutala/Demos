///////////////////////////////////////////////////////////////////////////////////
///@file i2cmaster_FMC116.h
///@author Arnaud Maye (4DSP) 
///\brief i2cmaster module to interface with the i2cmaster star (header)
///
/// FMC116 related algorithm
///////////////////////////////////////////////////////////////////////////////////
#ifndef _I2CMASTER_FMC116_H_
#define _I2CMASTER_FMC116_H_

/* defines */
#define FMC116_EEPROM_SS 0x50				/*!< Slave select byte for I2C EEPROM device */

enum 
{
	I2CMASTER_DISPLAY_CONSOLE = 1,			/*!< i2cmaster_getdiagnosticsFMC116 function displays the frequency to the console */  
	I2CMASTER_NO_DISPLAY_CONSOLE = 0,		/*!< i2cmaster_getdiagnosticsFMC116 function does not display the frequency to the console */   
};


/* error codes */
#define I2CMASTER_FMC116_ERR_OK					0	/*!< No error encountered during execution. */
#define I2CMASTER_FMC116_ERR_WRITE_FAIL			-1	/*!< Could not write to the EEPROM, write protection is possible enabled on the EEPROM device. */
#define I2CMASTER_FMC116_ERR_NULL_ARGUMENT		-2	/*!< An unexpected NULL argument has been passed to a function. */

// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 

/**
 * Write a 32 bit word to the EEPROM at a given address.
 *
 * @param   bar     offset where FMC116.I2CMASTER is located in the constellation memory space.
 * @param   offset     address where the 32 bit word should be written in the EEPROM.
 * @param   data			32 bit wide value to be written in the EEPROM.
 *
 * @return  I2CMASTER_FMC116_ERR_OK in case of success, I2CMASTER_FMC116_ERR_NULL_ARGUMENT, I2CMASTER_FMC116_ERR_WRITE_FAIL or any ethapi ( please see ethapi documentation ) error codes.
 */
int i2cmaster_WriteEepromFMC116(unsigned long bar, unsigned long offset, unsigned long data);

/**
 * Read a 32 bit word to the EEPROM at a given address.
 *
 * @param   bar     offset where FMC116.I2CMASTER is located in the constellation memory space.
 * @param   offset     address where the 32 bit word should be read in the EEPROM.
 * @param   data			pointer to a 32 bit memory about to  receive the data read from the EEPROM
 *
 * @return  I2CMASTER_FMC116_ERR_OK in case of success, I2CMASTER_FMC116_ERR_NULL_ARGUMENT, I2CMASTER_FMC116_ERR_WRITE_FAIL or any ethapi ( please see ethapi documentation ) error codes.
 */
int i2cmaster_ReadEepromFMC116(unsigned long bar, unsigned long offset, unsigned long *data);

/**
 * Test the EEPROM
 *
 * @param   bar     offset where FMC116.I2CMASTER is located in the constellation memory space.
 * @return  I2CMASTER_FMC116_ERR_OK in case of success or any ethapi ( please see ethapi documentation ) error codes.
 */
int i2cmaster_TestEepromFMC116(unsigned long bar);

// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_I2CMASTER_FMC116_H_