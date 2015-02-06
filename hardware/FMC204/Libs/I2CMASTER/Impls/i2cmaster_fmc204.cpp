///////////////////////////////////////////////////////////////////////////////////
///@file i2cmaster_FMC204.cpp
///@author Arnaud Maye (4DSP) 
///\brief i2cmaster module to interface with the i2cmaster star (implementation)
///
/// FMC204 related algorithm
///////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include "i2cmaster_fmc204.h"
#include "sipif.h"

#if defined WIN32

 // Include declarations for _aligned_malloc and _aligned_free 
 #include <malloc.h>
 #include <windows.h>
#else
 #include <unistd.h>
 #ifndef __int64
   #define __int64 long long
 #endif
 #ifndef ULONG
   #define ULONG unsigned long
 #endif
 #define INVALID_HANDLE_VALUE -1

static void *_aligned_malloc(size_t size, size_t alignment)  
{
	void *p;
	if (posix_memalign(&p, alignment, size))
		return NULL;
	return p; 
}

static void _aligned_free(void *p)
{
	free(p);
}

static void DeleteFile(const char *filename)
{
	unlink(filename);
}

#define Sleep(x)	(usleep((unsigned long long)(x*1000)))

#ifndef API_ENUM_DISPLAY
 #define API_ENUM_DISPLAY 1
#endif

#endif

int i2cmaster_getdiagnosticsFMC204(unsigned long bar, float *siliconrevision, float *temperature, float *voltage3V3_rail, float *voltage3V3_clk, 
							 float *voltage1V8_dig, float *voltage2V5_adj, float *voltage2V5_clk, float *voltage3V3_dig, float *voltage3V3_adc, 
							 float *voltage3V3_cp, float *voltage12V, int outputconsole)
{
	ULONG lsb, msb, dword, rc;
	float result, Vref;

	if(outputconsole)
		printf("\n-------- Onboard Monitoring ---------\n");
	
	rc = sipif_readsipreg(bar+0x4D, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	if(dword==0x02) {
		if(outputconsole)
			printf("IIC Mon Dev ID  : OK (0x%2.2X)\n", dword);
	}
	else {
		if(outputconsole)
			printf("IIC Mon Dev ID  : ERROR (0x%2.2X)\n", dword);
	}

	rc = sipif_readsipreg(bar+0x4E, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	if(dword==0x41) {
		if(outputconsole)
			printf("IIC Man Dev ID  : OK (0x%2.2X)\n", dword);
	}
	else {
		if(outputconsole)
			printf("IIC Man Dev ID  : ERROR (0x%2.2X)\n", dword);
	}

	rc = sipif_readsipreg(bar+0x4F, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	if(outputconsole)
		printf("IIC Mon Sil Rev : OK (0x%2.2X)\n", dword);
	if(siliconrevision!=NULL)
		*siliconrevision = (float)dword;

	//Control configuration 1
	rc = sipif_writesipreg(bar+0x18, 0x29);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	Sleep(100);
	//Control configuration 3
	rc = sipif_writesipreg(bar+0x1A, 0x10);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	Sleep(10);

	Sleep(100); //wait untill al least all channels have been measured in the ADT7411

	//Onchip TEMP
	rc = sipif_readsipreg(bar+0x03, &dword); //LSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	lsb = (dword >> 0) && 0x3;
	rc = sipif_readsipreg(bar+0x07, &dword); //MSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	msb = dword << 2;
	result = (msb + lsb) / 4.0f;
	if (result > -40.0 && result < 85.0) {
		if(outputconsole)
			printf("TEMP (  ADT   ) : OK (%5.2fC)\n", result);
	}
	else {
		if(outputconsole)
			printf("TEMP (  ADT   ) : ERROR (%5.2fC)\n", result);
	}
	if(temperature!=NULL)
		*temperature = (float)result;

	//Onchip VDD
	rc = sipif_readsipreg(bar+0x03, &dword); //LSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	lsb = (dword >> 2) && 0x3;
	rc = sipif_readsipreg(bar+0x06, &dword); //MSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	msb = dword << 2;
	result = (msb + lsb) * 3.11f * 2.197f / 1000.0f;
	if (result > (3.3*0.95) && result < (3.3*1.05)) {
		if(outputconsole)
			printf("VDD  ( 3.3V   ) : OK (%5.2fV)\n", result);
	}
	else {
		if(outputconsole)
			printf("VDD  ( 3.3V   ) : ERROR (%5.2fV)\n", result);
	}
	Vref = result;
	if(voltage3V3_rail!=NULL)
		*voltage3V3_rail =  Vref;

	//AIN1
	rc = sipif_readsipreg(bar+0x04, &dword); //LSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	lsb = (dword >> 0) && 0x3;
	rc = sipif_readsipreg(bar+0x08, &dword); //MSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	msb = dword << 2;
	result = 2 * (msb + lsb) * Vref / 1024.0f;
	if (result > (3.3*0.95) && result < (3.3*1.05)) {
		if(outputconsole)
			printf("AIN1 ( 3.3Vclk) : OK (%5.2fV)\n", result);
	}
	else {
		if(outputconsole)
			printf("AIN1 ( 3.3Vclk) : ERROR (%5.2fV)\n", result);
	}
	if(voltage3V3_clk!=NULL)
		*voltage3V3_clk = result;

	//AIN2
	rc = sipif_readsipreg(bar+0x04, &dword); //LSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	lsb = (dword >> 2) && 0x3;
	rc = sipif_readsipreg(bar+0x09, &dword); //MSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	msb = dword << 2;
	result = 1 * (msb + lsb) * Vref / 1024.0f;
	if (result > (1.8*0.95) && result < (1.8*1.05)) {
		if(outputconsole)
			printf("AIN2 ( 1.8Vdig) : OK (%5.2fV)\n", result);
	}
	else {
		if(outputconsole)
			printf("AIN2 ( 1.8Vdig) : ERROR (%5.2fV)\n", result);
	}
	if(voltage1V8_dig!=NULL)
		*voltage1V8_dig = result;

	//AIN3
	rc = sipif_readsipreg(bar+0x04, &dword); //LSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	lsb = (dword >> 4) && 0x3;
	rc = sipif_readsipreg(bar+0x0A, &dword); //MSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	msb = dword << 2;
	result = 1 * (msb + lsb) * Vref / 1024.0f;
	if (result > (1.65*0.95) && result < (3.3*1.05)) {
		if(outputconsole)
			printf("AIN3 ( Vadj)    : OK (%5.2fV)\n", result);
	}
	else {
		if(outputconsole)
			printf("AIN3 ( Vadj)    : ERROR (%5.2fV)\n", result);
	}
	if(voltage2V5_adj!=NULL)
		*voltage2V5_adj = result;

	//AIN4
	rc = sipif_readsipreg(bar+0x04, &dword); //LSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	lsb = (dword >> 6) && 0x3;
	rc = sipif_readsipreg(bar+0x0B, &dword); //MSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	msb = dword << 2;
	result = 1 * (msb + lsb) * Vref / 1024.0f;
	if (result > (2.5*0.95) && result < (2.5*1.05)) {
		if(outputconsole)
			printf("AIN4 ( 2.5Vclk) : OK (%5.2fV)\n", result);
	}
	else {
		if(outputconsole)
			printf("AIN4 ( 2.5Vclk) : ERROR (%5.2fV)\n", result);
	}
	if(voltage2V5_clk!=NULL)
		*voltage2V5_clk = result;

	//AIN5
	rc = sipif_readsipreg(bar+0x05, &dword); //LSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	lsb = (dword >> 0) && 0x3;
	rc = sipif_readsipreg(bar+0x0C, &dword); //MSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	msb = dword << 2;
	result = 2 * (msb + lsb) * Vref / 1024.0f;
	if (result > (3.3*0.95) && result < (3.3*1.05)) {
		if(outputconsole)
			printf("AIN5 ( 3.3Vdig) : OK (%5.2fV)\n", result);
	}
	else {
		if(outputconsole)
			printf("AIN5 ( 3.3Vdig) : ERROR (%5.2fV)\n", result);
	}
	if(voltage3V3_dig!=NULL)
		*voltage3V3_dig =  result;

	//AIN6
	rc = sipif_readsipreg(bar+0x05, &dword); //LSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	lsb = (dword >> 2) && 0x3;
	rc = sipif_readsipreg(bar+0x0D, &dword); //MSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	msb = dword << 2;
	result = 2 * (msb + lsb) * Vref / 1024.0f;
	if (result > (3.3*0.95) && result < (3.3*1.05)) {
		if(outputconsole)
			printf("AIN6 ( 3.3Vadc) : OK (%5.2fV)\n", result);
	}
	else {
		if(outputconsole)
			printf("AIN6 ( 3.3Vadc) : ERROR (%5.2fV)\n", result);
	}
	if(voltage3V3_adc!=NULL)
		*voltage3V3_adc = result;

	//AIN7
	rc = sipif_readsipreg(bar+0x05, &dword); //LSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	lsb = (dword >> 4) && 0x3;
	rc = sipif_readsipreg(bar+0x0E, &dword); //MSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	msb = dword << 2;
	result = 2 * (msb + lsb) * Vref / 1024.0f;
	//VCXO power on (internal clock)
	if (result > (3.3*0.95) && result < (3.3*1.05)) {
		if(outputconsole)
			printf("AIN7 ( 3.3Vcp ) : OK (%5.2fV)\n", result);
	}
	//VCXO power off (external clock)
	else if (result < 0.05) {
		if(outputconsole)
			printf("AIN7 ( 3.3Vcp ) : OK (%5.2fV)\n", result);
	}
	else {
		if(outputconsole)
			printf("AIN7 ( 3.3Vcp ) : ERROR (%5.2fV)\n", result);
	}
	if(voltage3V3_cp!=NULL)
		*voltage3V3_cp = result;

	//AIN8
	rc = sipif_readsipreg(bar+0x05, &dword); //LSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	lsb = (dword >> 6) && 0x3;
	rc = sipif_readsipreg(bar+0x0F, &dword); //MSB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	msb = dword << 2;
	result = 7.04f * (msb + lsb) * Vref / 1024.0f;
	if (result > (12.0*0.90) && result < (12.0*1.10)) {
		if(outputconsole)
			printf("AIN8 (  12V   ) : OK (%5.2fV)\n", result);
	}
	else {
		if(outputconsole)
			printf("AIN8 (  12V   ) : ERROR (%5.2fV)\n", result);
	}
	if(voltage12V!=NULL)
		*voltage12V = result;
	
	if(outputconsole)
		printf("-------------------------------------\n");

	return I2CMASTER_FMC204_ERR_OK;
}
