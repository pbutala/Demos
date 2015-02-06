///////////////////////////////////////////////////////////////////////////////////
///@file FMC116_cpld.cpp
///@author Arnaud Maye (4DSP) 
///\brief FMC116_cpld module to interface with the FMC116 star (implementation)
///
/// This module is in charge of configuring the CPLD mounted on the FMC116 hardware
///
///////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include "FMC116_cpld.h"
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

#endif


int FMC116_cpld_init(unsigned long bar, unsigned int clockmode) 
{
	unsigned long reg0, reg1, reg2, dword;
	int rc;

	// reset the clock tree
	rc = FMC116_cpld_resetSPIclocktree(bar);
	if(rc!=FMC116_CPLD_ERR_OK)
		return rc;
	
	// reset the DAC
	rc = FMC116_cpld_resetSPIdac(bar);
	if(rc!=FMC116_CPLD_ERR_OK)
		return rc;

	// configure bar+0 word for a set of given parameters
	// Bit 1=Reference enable, Bit 0=Input select (0=clock/1=reference)
	// clockmode = 0 --> "10"
	// clockmode = 1 --> "00"
	// clockmode = 2 --> "01"
	int clksrc = 0;
	switch (clockmode)
	{
		case 0: clksrc = 2; break; //Onboard reference enabled, external clock input switched to clock mode.
		case 1: clksrc = 0; break; //Onboard reference disabled, external clock input switched to clock mode.
		case 2: clksrc = 1; break; //Onboard reference disabled, external clock input switched to reference mode
		default: clksrc = 0; break;
	}
	reg0 = 0;
	reg0 |= clksrc&0x03;
	rc = sipif_writesipreg(bar+0, reg0); Sleep(2);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	
	// configure bar+1 word for a set of given parameters
	// bar+1 does not exist

	// configure bar+2
	reg2 = 0x0; //LED Off
	rc = sipif_writesipreg(bar+2, reg2); Sleep(2);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	// read back and verify
	rc = sipif_readsipreg(bar+0, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	if ((dword&0xFF)!=reg0)
		return FMC116_CPLD_ERR_CONFIG_VERIFICATION;

	return FMC116_CPLD_ERR_OK;
}

int FMC116_cpld_resetSPIclocktree(unsigned long bar)
{
	unsigned long dword;
	int rc;

	rc = sipif_readsipreg(bar+0, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	// set reset
	dword |= 0x20;
	rc = sipif_writesipreg(bar+0, dword); Sleep(2);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	
	// remove reset
	dword &= ~0x20;
	rc = sipif_writesipreg(bar+0, dword); Sleep(2);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	return FMC116_CPLD_ERR_OK;

}

int FMC116_cpld_resetSPIdac(unsigned long bar)
{
	unsigned long dword;
	int rc;

	rc = sipif_readsipreg(bar+0, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	// set reset
	dword |= 0x10;
	rc = sipif_writesipreg(bar+0, dword); Sleep(2);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	// remove reset
	dword &= ~0x10;
	rc = sipif_writesipreg(bar+0, dword); Sleep(2);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	return FMC116_CPLD_ERR_OK;

}
