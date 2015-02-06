///////////////////////////////////////////////////////////////////////////////////
///@file FMC204_ctrl.cpp
///@author Arnaud Maye (4DSP) 
///\brief FMC204_ctrl module to interface with the FMC204 star (implementation)
///
/// This module is in charge of configuring the FMC204 control module. This module
/// allows users to configure the grabbing and channel enables among various other 
/// things.
///
///////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include "fmc204_ctrl.h"
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

int FMC204_ctrl_init(unsigned long bar) 
{
	return FMC204_CTRL_ERR_OK;
}


int FMC204_ctrl_probefmc(unsigned long bar)
{
	unsigned long dword;
	int rc;

	// Read info register
	rc = sipif_readsipreg(bar+4, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	if ((dword&0x01)!=0x01)
		return FMC204_CTRL_ERR_PRESENT;
	if ((dword&0x02)!=0x02)
		return FMC204_CTRL_ERR_POWER_GOOD;

	return FMC204_CTRL_ERR_OK;
}

int FMC204_ctrl_configure_burst(unsigned long bar, unsigned int burstnumber, unsigned int burstlength)
{
	int rc;

	rc = sipif_writesipreg(bar+0x02, burstnumber);         // Nr of Bursts
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar+0x03, burstlength);		 // Burst Size
	if(rc!=SIPIF_ERR_OK)
		return rc;
	
	return FMC204_CTRL_ERR_OK;
}

int FMC204_ctrl_prepare_wfm_load(unsigned long bar, unsigned int dacchannel)
{
	int rc; 
	if(dacchannel==DAC0) {
		rc = sipif_writesipreg(bar+0x01, 0x01); //enable DAC0
		if(rc!=SIPIF_ERR_OK)
			return rc;
	}
	else if(dacchannel==DAC1) {
		rc = sipif_writesipreg(bar+0x01, 0x02); //enable DAC1
		if(rc!=SIPIF_ERR_OK)
			return rc;
	}
	else if(dacchannel==DAC2) {
		rc = sipif_writesipreg(bar+0x01, 0x04); //enable DAC1
		if(rc!=SIPIF_ERR_OK)
			return rc;
	}
	else if(dacchannel==DAC3) {
		rc = sipif_writesipreg(bar+0x01, 0x08); //enable DAC1
		if(rc!=SIPIF_ERR_OK)
			return rc;
	}

	rc = sipif_writesipreg(bar+0x00, 0x08); //load WFM command
	if(rc!=SIPIF_ERR_OK)
		return rc;

	return FMC204_CTRL_ERR_OK;
}

int FMC204_ctrl_arm_dac(unsigned long bar)
{
	int rc;

	rc = sipif_writesipreg(bar+0x00, 0x01); 
	if(rc!=SIPIF_ERR_OK)
		return rc;

	return FMC204_CTRL_ERR_OK;
}

int FMC204_ctrl_disarm_dac(unsigned long bar)
{
	int rc;

	rc = sipif_writesipreg(bar+0x00, 0x02); 
	if(rc!=SIPIF_ERR_OK)
		return rc;

	return FMC204_CTRL_ERR_OK;
}

int FMC204_ctrl_sw_trigger(unsigned long bar)
{
	int rc;

	rc = sipif_writesipreg(bar+0x00, 0x04); 
	if(rc!=SIPIF_ERR_OK)
		return rc;

	return FMC204_CTRL_ERR_OK;
}

int FMC204_ctrl_enable_channel(unsigned long bar, unsigned char dac0en, unsigned char dac1en, unsigned char dac2en, unsigned char dac3en)
{
	unsigned long dword;
	int rc;

	// read what we have there already
	rc = sipif_readsipreg(bar+0x01, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	// mask out the enable bits
	dword &= ~0xF;

	// append our values
	dword |= (dac0en<<0);
	dword |= (dac1en<<1);
	dword |= (dac2en<<2);
	dword |= (dac3en<<3);

	// write back
	rc = sipif_writesipreg(bar+0x01, dword); 
	if(rc!=SIPIF_ERR_OK)
		return rc;

	return FMC204_CTRL_ERR_OK;
}



