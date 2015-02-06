///////////////////////////////////////////////////////////////////////////////////
///@file FMC204.cpp
///@author Arnaud Maye (4DSP) 
///\brief FMC204 module to interface with the FMC204 star (implementation)
///
/// This module is in charge of configuring all the chips mounted on the FMC204 
/// hardware using FMC204_x sub modules.
///
///////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include "fmc204.h"
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


int FMC204_init(unsigned long bar_cpld, unsigned long bar_clk, unsigned long bar_dac0, unsigned long bar_dac0_phy, 
				unsigned long bar_dac1, unsigned long bar_dac1_phy, unsigned int clockmode)
{

	unsigned int clksrc_cpld;
	unsigned int clksrc_clktree;
	if(clockmode==FMC204_INTERNAL_CLK) {
		clksrc_cpld = CLKSRC_INTERNAL_CLK_INTERNAL_REF;
		clksrc_clktree = CLOCKTREE_CLKSRC_INTERNAL;
	}
	else {
		clksrc_cpld = CLKSRC_EXTERNAL_CLK;
		clksrc_clktree = CLOCKTREE_CLKSRC_EXTERNAL;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// initialize CPLD portion of FMC204 star
	if(FMC204_cpld_init(bar_cpld, clksrc_cpld, SYNCSRC_NO_SYNC, 
		FAN0_ENABLED|FAN1_ENABLED|FAN2_ENABLED|FAN3_ENABLED, DIR0_INPUT|DIR1_INPUT|DIR2_INPUT|DIR3_INPUT)!=FMC204_CPLD_ERR_OK ) {
			printf("Could not initialize FMC204.CPLD\n");
			return FMC204_ERR_CPLD_INIT;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Configure the clock tree 
	if(FMC204_clocktree_init(bar_clk, clksrc_clktree)!=FMC204_CLOCKTREE_ERR_OK){
		printf("Could not initialize FMC204.CLOCKTREE\n");
		return FMC204_ERR_CLOCKTREE_INIT;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Configure DAC0 and DAC1
	if(FMC204_dac_init(bar_dac0, bar_dac0_phy, bar_dac1, bar_dac1_phy)!=FMC204_DAC_ERR_OK) {
		printf("Could not initialize FMC204.DAC\n");
		return FMC204_ERR_DAC_INIT;
	}

	// little pause after init
	Sleep(100);

	return FMC204_ERR_OK;
}