///////////////////////////////////////////////////////////////////////////////////
///@file FMC204_cpld.cpp
///@author Arnaud Maye (4DSP) 
///\brief FMC204_cpld module to interface with the FMC204 star (implementation)
///
/// This module is in charge of configuring the CPLD mounted on the FMC204 hardware
///
///////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include "fmc204_cpld.h"
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

int FMC204_cpld_init(unsigned long bar, unsigned int clksrc, unsigned int syncsrc, unsigned char fans, unsigned char dirs) 
{
	unsigned long reg0, reg1, reg2, dword;
	int rc;

	// reset the clock tree
	rc = FMC204_cpld_resetSPIclocktree(bar);
	if(rc!=FMC204_CPLD_ERR_OK)
		return rc;
	
	// reset the DACs
	rc = FMC204_cpld_resetSPIDACs(bar);
	if(rc!=FMC204_CPLD_ERR_OK)
		return rc;

	// configure bar+0 word for a set of given parameters
	reg0 = 0;
	reg0 |= clksrc&0x07;
	reg0 |= (syncsrc&0x3)<<3;
	rc = sipif_writesipreg(bar+0, reg0);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	Sleep(10);

	// configure bar+1 word for a set of given parameters
	reg1 = 0;
	reg1 |= dirs&0x0F;
	reg1 |= ((fans&0x0F)<<4);
	rc = sipif_writesipreg(bar+1, reg1);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	Sleep(10);

	// configure bar+2
	reg2 = 0;
	rc = sipif_writesipreg(bar+2, reg2);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	Sleep(10);

	// read back and verify register 0
	rc = sipif_readsipreg(bar+0, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	if (dword!=reg0)
		return FMC204_CPLD_ERR_CONFIG_VERIFICATION;

	// read back and verify register 1
	rc = sipif_readsipreg(bar+1, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	if (dword!=reg1)
		return FMC204_CPLD_ERR_CONFIG_VERIFICATION;

	return FMC204_CPLD_ERR_OK;
}

int FMC204_cpld_resetSPIclocktree(unsigned long bar)
{
	unsigned long dword;
	int rc;

	rc = sipif_readsipreg(bar+0, &dword); //MSB
	if(rc!=SIPIF_ERR_OK)
		return rc;

	// set reset
	dword |= 0x20;
	rc = sipif_writesipreg(bar+0, dword); //MSB
	if(rc!=SIPIF_ERR_OK)
		return rc;

	Sleep(10);

	// remove reset
	dword &= ~0x20;
	rc = sipif_writesipreg(bar+0, dword); //MSB
	if(rc!=SIPIF_ERR_OK)
		return rc;

	Sleep(10);

	return FMC204_CPLD_ERR_OK;

}

int FMC204_cpld_resetSPIDACs(unsigned long bar)
{
	unsigned long dword;
	int rc;

	rc = sipif_readsipreg(bar+0, &dword); //MSB
	if(rc!=SIPIF_ERR_OK)
		return rc;

	// set reset
	dword |= 0x40;
	rc = sipif_writesipreg(bar+0, dword); //MSB
	if(rc!=SIPIF_ERR_OK)
		return rc;

	Sleep(10);

	// remove reset
	dword &= ~0x40;
	rc = sipif_writesipreg(bar+0, dword); //MSB
	if(rc!=SIPIF_ERR_OK)
		return rc;

	Sleep(10);

	return FMC204_CPLD_ERR_OK;

}
