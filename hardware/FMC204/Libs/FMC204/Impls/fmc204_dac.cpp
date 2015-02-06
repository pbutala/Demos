///////////////////////////////////////////////////////////////////////////////////
///@file FMC204_dac.cpp
///@author Arnaud Maye (4DSP)
///\brief FMC204_dac module to interface with the FMC204 star (implementation)
///
/// This module is in charge of configuring the DAC chips populated
/// on the FMC204 printed circuit board.
///
///////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include "fmc204_dac.h"
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

int FMC204_dac_init(unsigned long bar_dac0, unsigned long bar_dac0_phy, unsigned long bar_dac1, unsigned long bar_dac1_phy)
{
	unsigned long dword;
	unsigned long dll0[2] = {0x00,0x01}; //tuning words for DAC0 DLL
	unsigned long dll1[2] = {0x00,0x01}; //tuning words for DAC1 DLL
	int dac0_pattern = 0;
	int dac1_pattern = 0;
	int rc;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Reset DCM in the FPGA. This is required after the DAC reference clock from the
	// clock tree has become stable
	rc = sipif_writesipreg(bar_dac0_phy+1, 0x02); //force DCM reset
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac0_phy+1, 0x00); //release DCM reset
	if(rc!=SIPIF_ERR_OK)
		return rc;
	Sleep(10); //wait for dcm lock

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Setup DAC0
	rc = sipif_readsipreg(bar_dac0+0, &dword); //check Device ID
	if(rc!=SIPIF_ERR_OK)
		return rc;
	dword = (dword&0x1C)>>2;
	if(dword != FMC204_DAC_PART_ID)
		return FMC204_DAC_ERR_WRONG_PART_ID;

	rc = sipif_writesipreg(bar_dac0+0x01, 0x11); Sleep(2); // FIR on, FIFO offset 1
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac0+0x02, 0xC0); Sleep(2); // twos compl, dual DAC
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac0+0x03, 0x08); Sleep(2); // no masks, swap A and B
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac0+0x04, 0x00); Sleep(2); // clear errors
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac0+0x05, 0x42); Sleep(2); // DLL enable, PLL bypass
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac0+0x06, 0x0E); Sleep(2); // 472kHz, DLL awake, PLL sleep
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac0+0x07, 0xFF); Sleep(2); // DAC gain
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac0+0x08, 0x00); Sleep(2); // no DLL restart
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac0+0x09, 0x00); Sleep(2); // M=0, N=0
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac0+0x0A, dll0[0]); Sleep(2); // DLL tuning for 500MHz DDR
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac0+0x0B, 0x00); Sleep(2); // no PLL tuning
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac0+0x0C, 0x00); Sleep(2); // no manual offset DACA
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac0+0x0D, 0x00); Sleep(2); // no manual offset DACA
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac0+0x0E, 0x00); Sleep(2); // normal SDO function, no manual offset DACB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac0+0x0F, 0x00); Sleep(2); // no manual offset DACB
	if(rc!=SIPIF_ERR_OK)
		return rc;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Setup DAC1
	rc = sipif_readsipreg(bar_dac1+0, &dword); //check Device ID
	if(rc!=SIPIF_ERR_OK)
		return rc;
	dword = (dword&0x1C)>>2;
	if(dword != FMC204_DAC_PART_ID)
		return FMC204_DAC_ERR_WRONG_PART_ID;

	rc = sipif_writesipreg(bar_dac1+0x01, 0x51); Sleep(2); // FIR on, FIFO offset 1, delay 1 sample
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac1+0x02, 0xC0); Sleep(2); // twos compl, dual DAC
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac1+0x03, 0x00); Sleep(2); // no masks, no swap
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac1+0x04, 0x00); Sleep(2); // clear errors
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac1+0x05, 0x42); Sleep(2); // DLL enable, PLL bypass
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac1+0x06, 0x0E); Sleep(2); // 472kHz, DLL awake, PLL sleep
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac1+0x07, 0xFF); Sleep(2); // DAC gain
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac1+0x08, 0x00); Sleep(2); // no DLL restart
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac1+0x09, 0x00); Sleep(2); // M=0, N=0
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac1+0x0A, dll1[0]); Sleep(2); // DLL tuning for 500MHz DDR
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac1+0x0B, 0x00); Sleep(2); // no PLL tuning
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac1+0x0C, 0x00); Sleep(2); // no manual offset DACA
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac1+0x0D, 0x00); Sleep(2); // no manual offset DACA
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac1+0x0E, 0x00); Sleep(2); // normal SDO function, no manual offset DACB
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar_dac1+0x0F, 0x00); Sleep(2); // no manual offset DACB
	if(rc!=SIPIF_ERR_OK)
		return rc;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check DAC0: is DLL locked? Is internal FIFO OK? Is Pattern checking OK?
	// Loop until DAC0 Pattern check is OK
	for (int i = 0; i < 2; i++)
	{
		rc = sipif_writesipreg(bar_dac0_phy+1, 0x01); Sleep(2); //enable test pattern
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar_dac0+0x0A, dll0[i]); Sleep(2); // DLL tuning for 500MHz DDR
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar_dac0+0x08, 0x04); Sleep(2); // set DLL restart flag
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar_dac0+0x08, 0x00); Sleep(2); // clear DLL restart flag
		if(rc!=SIPIF_ERR_OK)
			return rc;
		Sleep(10); //wait for DLL lock
		rc = sipif_readsipreg(bar_dac0+0, &dword); //check for DLL lock
		if(rc!=SIPIF_ERR_OK)
			return rc;
		if((dword&FMC204_DAC_DLL_LOCKED)!=FMC204_DAC_DLL_LOCKED)
			return FMC204_DAC_ERR_DLL_NOT_LOCKED;
		rc = sipif_writesipreg(bar_dac0+0x04, 0x00); Sleep(2); //clear error flags
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar_dac0_phy+1, 0x01|0x04); Sleep(2); //enable test pattern, drive TXENABLE high
		if(rc!=SIPIF_ERR_OK)
			return rc;
		Sleep(100); //checking time
		rc = sipif_readsipreg(bar_dac0+0x04, &dword); //read error flags
		if(rc!=SIPIF_ERR_OK)
			return rc;
		if((dword&FMC204_DAC_FIFO_ERROR)==FMC204_DAC_FIFO_ERROR)
			return FMC204_DAC_ERR_CHAN0_FIFO;
		if((dword&FMC204_DAC_PATTERN_ERROR)!=FMC204_DAC_PATTERN_ERROR) {
			printf("DLL0 Tuning : 0x%02X\n", dll0[i]);
			dac0_pattern = 1;
			break;
		}
	}
	if (dac0_pattern == 0)
		return FMC204_DAC_ERR_CHAN0_PATTERN;


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check DAC1: is DLL locked? Is internal FIFO OK? Is Pattern checking OK?
	// Loop until DAC1 Pattern check is OK
	for (int i = 0; i < 2; i++)
	{
		rc = sipif_writesipreg(bar_dac0_phy+1, 0x01); Sleep(2); //enable test pattern
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar_dac1+0x0A, dll1[i]); Sleep(2); // DLL tuning for 500MHz DDR
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar_dac1+0x08, 0x04); Sleep(2); // set DLL restart flag
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar_dac1+0x08, 0x00); Sleep(2); // clear DLL restart flag
		if(rc!=SIPIF_ERR_OK)
			return rc;
		Sleep(10); //wait for DLL lock
		rc = sipif_readsipreg(bar_dac1+0, &dword); //check for DLL lock
		if(rc!=SIPIF_ERR_OK)
			return rc;
		if((dword&FMC204_DAC_DLL_LOCKED)!=FMC204_DAC_DLL_LOCKED)
			return FMC204_DAC_ERR_DLL_NOT_LOCKED;
		rc = sipif_writesipreg(bar_dac1+0x04, 0x00); Sleep(2); //clear error flags
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar_dac0_phy+1, 0x01|0x04); Sleep(2); //enable test pattern, drive TXENABLE high
		if(rc!=SIPIF_ERR_OK)
			return rc;
		Sleep(100); //checking time
		rc = sipif_readsipreg(bar_dac1+0x04, &dword); //read error flags
		if(rc!=SIPIF_ERR_OK)
			return rc;
		if((dword&FMC204_DAC_FIFO_ERROR)==FMC204_DAC_FIFO_ERROR)
			return FMC204_DAC_ERR_CHAN1_FIFO;
		if((dword&FMC204_DAC_PATTERN_ERROR)!=FMC204_DAC_PATTERN_ERROR) {
			printf("DLL1 Tuning : 0x%02X\n", dll1[i]);
			dac1_pattern = 1;
			break;
		}
	}

	if (dac1_pattern == 0)
		return FMC204_DAC_ERR_CHAN1_PATTERN;


	rc = sipif_writesipreg(bar_dac0_phy+1, 0x00); //disable test pattern, drive TXENABLE low
	if(rc!=SIPIF_ERR_OK)
		return rc;

	rc = sipif_writesipreg(bar_dac0_phy+1, 0x04); //disable test pattern, drive TXENABLE high
	if(rc!=SIPIF_ERR_OK)
		return rc;

	return FMC204_DAC_ERR_OK;
}
