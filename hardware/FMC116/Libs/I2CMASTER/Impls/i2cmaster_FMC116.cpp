///////////////////////////////////////////////////////////////////////////////////
///@file i2cmaster_FMC116.cpp
///@author Arnaud Maye (4DSP) 
///\brief i2cmaster module to interface with the i2cmaster star (implementation)
///
/// FMC116 related algorithm
///////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "i2cmaster_FMC116.h"
#include "sipif.h"

int i2cmaster_WriteEepromFMC116(unsigned long bar, unsigned long offset, unsigned long data)
{
	unsigned long addr, dword;
	int rc;

	addr = bar + (FMC116_EEPROM_SS<<8) + offset;

	//write
	rc = sipif_writesipreg(addr, data); Sleep(50);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	//check
	rc = sipif_readsipreg(addr, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	if(dword!=data) 
		return I2CMASTER_FMC116_ERR_WRITE_FAIL;
	
	return I2CMASTER_FMC116_ERR_OK;
}

int i2cmaster_ReadEepromFMC116(unsigned long bar, unsigned long offset, unsigned long *data)
{
	unsigned long addr;
	int rc;

	if(data==NULL) 
		return I2CMASTER_FMC116_ERR_NULL_ARGUMENT;

	addr = bar + (FMC116_EEPROM_SS<<8) + offset;

	//check
	rc = sipif_readsipreg(addr, data);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	
	return I2CMASTER_FMC116_ERR_OK;
}

int i2cmaster_TestEepromFMC116(unsigned long bar)
{
	unsigned long offset = 0xFF;
	unsigned long str, tst;
	int rc;

	//store register
	rc = i2cmaster_ReadEepromFMC116(bar, offset, &str);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	printf("EERPOM Read       : OK\n");

	//make sure the test value is not the same as the value already stored in 
	//the EEPROM location under test.
	tst = str+0x55;

	//write register (includes check)
	rc = i2cmaster_WriteEepromFMC116(bar, offset, tst);
	if(rc==I2CMASTER_FMC116_ERR_WRITE_FAIL)
	{
		printf("EERPOM Protection : YES\n");
		return I2CMASTER_FMC116_ERR_OK;
	}
	else if(rc!=SIPIF_ERR_OK)
		return rc;

	//restore register (includes check)
	rc = i2cmaster_WriteEepromFMC116(bar, offset, str);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	printf("EEPROM Write      : OK\n");
	printf("EEPROM Protection : NO\n");

	return I2CMASTER_FMC116_ERR_OK;

}

