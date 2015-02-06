///////////////////////////////////////////////////////////////////////////////////
///@file FMC204_freqcnt.cpp
///@author Arnaud Maye (4DSP) 
///\brief FMC204_freqcnt module to interface with the FMC204 star (implementation)
///
/// This module is in charge of obtaining various frequencies from the clock tree and
/// by definition communicates with the firmware over ethernet
///
///////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include "fmc204_freqcnt.h"
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

int FMC204_freqcnt_getfrequency(unsigned long bar, unsigned int clksel, float *freq, int outputconsole) 
{
	unsigned long dword;
	int rc;

	// tell the firmware to start a measure on a given clock index
	rc = sipif_writesipreg(bar+0, clksel); Sleep(2);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	// read back the just measured value
	rc = sipif_readsipreg(bar+1, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	// compute the frequency
	float testClkPeriod = 1.0f/125.0f;
	float tmp = 8192 * testClkPeriod;
	tmp = (tmp / (dword + 1));
	tmp = 1.00f/tmp;
	
	// if we were asked to display to console then we do that
	if(outputconsole) {
		switch (clksel)
		{
		case 0	: printf("Stellar IP Clock : %6.2f MHz\n", tmp); break; 
		case 1	: printf("DAC PHY Clock    : %6.2f MHz (Fs = %6.2f)\n", tmp, 8*tmp); break; 
		case 2	: printf("DAC REF Clock    : %6.2f MHz (Fs = %6.2f)\n", tmp, 2*tmp); break; 
		case 3	: printf("External Trigger : %6.2f MHz\n", tmp); break; 
		default	: printf("Frequency(%d)    : %6.2f MHz\n", clksel, tmp); break; 
		}
	}

	// if the pointer passed as argument is not NULL we then update the memory pointed
	if(freq!=NULL)
		*freq = tmp;

	return FMC204_FREQCNT_ERR_OK;
}