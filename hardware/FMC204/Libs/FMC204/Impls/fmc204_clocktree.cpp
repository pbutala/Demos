///////////////////////////////////////////////////////////////////////////////////
///@file FMC204_clocktree.cpp
///@author Arnaud Maye (4DSP)
///\brief FMC204_clocktree module to interface with the FMC204 star (implementation)
///
/// This module is in charge of configuring the clocktree chip mounted on the
/// FMC204 hardware.
///
///////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include "fmc204_clocktree.h"
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

int FMC204_clocktree_init(unsigned long bar, unsigned int clocksource)
{
	unsigned long dword;
	int rc;

	rc = sipif_readsipreg(bar+0x03, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	if (dword!=FMC204_CLOCKTREE_PART_ID_0 && dword!=FMC204_CLOCKTREE_PART_ID_1 &&
		dword!=FMC204_CLOCKTREE_PART_ID_2 && dword!=FMC204_CLOCKTREE_PART_ID_3 &&
		dword!=FMC204_CLOCKTREE_PART_ID_4)
		return FMC204_CLOCKTREE_ERR_WRONG_PART_ID;

	if(clocksource==CLOCKTREE_CLKSRC_INTERNAL) {
		rc = sipif_writesipreg(bar+0x010, 0x7C); Sleep(1); //CP 4.8mA, normal op.
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x011,   10); Sleep(1); //R lo
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x012,    0); Sleep(1); //R hi
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x013,    4); Sleep(1); //A
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x014,   12); Sleep(1); //B lo
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x015,    0); Sleep(1); //B hi
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x016, 0x04); Sleep(1); //presc. DM8
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x017, 0xB4); Sleep(1); //STATUS = DLD
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x019, 0x00); Sleep(1);
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x01A, 0x00); Sleep(1); //LD = DLD
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x01B, 0x00); Sleep(1); //REFMON = GND
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x01C, 0x87); Sleep(1); //Diff ref input
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x01D, 0x00); Sleep(1);
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x0F0, 0x0C); Sleep(1); //out0, adc1, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x0F1, 0x0C); Sleep(1); //out1, adc0, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x0F4, 0x0C); Sleep(1); //out2, dac1, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x0F5, 0x0C); Sleep(1); //out3, dac0, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x140, 0x01); Sleep(1); //out4, sync, pd
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x141, 0x01); Sleep(1); //out5, pd
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x142, 0x00); Sleep(1); //out6, lvds 1.75mA
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x143, 0x01); Sleep(1); //out7, pd
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x190, 0x00); Sleep(1); //div0, adc, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x191, 0x80); Sleep(1); //div0, adc, divider bypassed
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x192, 0x00); Sleep(1); //div0, adc, divider to output
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x196, 0x00); Sleep(1); //div1, dac, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x197, 0x80); Sleep(1); //div1, dac, divider bypassed
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x198, 0x00); Sleep(1); //div1, dac, divider to output
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x199, 0x00); Sleep(1); //div2.1, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19A, 0x00); Sleep(1); //phase
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19B, 0x00); Sleep(1); //div2.2, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19C, 0x20); Sleep(1); //div2.1 on, div2.2 bypass
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19D, 0x00); Sleep(1); //div2 dcc on
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x19E, 0x00); Sleep(1); //div3.1, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19F, 0x00); Sleep(1); //phase
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1A0, 0x00); Sleep(1); //div3.2, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1A1, 0x20); Sleep(1); //div3.1 on, div3.2 bypass
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1A2, 0x00); Sleep(1); //div3 dcc on
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x1E0, 0x00); Sleep(1); //vco div /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1E1, 0x01); Sleep(1); //bypass vco divider
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x230, 0x00); Sleep(1); //no pwd, no sync
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x232, 0x01); Sleep(1); //update
		if(rc!=SIPIF_ERR_OK)
			return rc;

		Sleep(10);

		// verify CLK0 PLL status
		rc = sipif_readsipreg(bar+0x1F, &dword);
		if(rc!=SIPIF_ERR_OK)
			return rc;
		if((dword&0x01)!=0x01)
			return FMC204_CLOCKTREE_ERR_CLK0_PLL_NOT_LOCKED;

	}
	else {
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		// External clock

#if 0
		rc = sipif_writesipreg(0x13402, 0x01); //Enable 1GHz generation on ML605
		if(rc!=SIPIF_ERR_OK)
			return rc;
#endif;

		rc = sipif_writesipreg(bar+0x0F0, 0x0C); Sleep(1); //out0, adc0, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x0F1, 0x0C); Sleep(1); //out1, adc1, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x0F4, 0x0C); Sleep(1); //out0, dac0, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x0F5, 0x0C); Sleep(1); //out1, dac1, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x140, 0x01); Sleep(1); //out4, pd
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x141, 0x01); Sleep(1); //out5, pd
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x142, 0x00); Sleep(1); //out6, lvds 1.75mA
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x143, 0x01); Sleep(1); //out7, pd
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x190, 0x00); Sleep(1); //div0, adc, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x191, 0x00); Sleep(1); //div0, adc
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x192, 0x02); Sleep(1); //div0, adc, direct to output
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x196, 0x00); Sleep(1); //div1, dac, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x197, 0x00); Sleep(1); //div1, dac
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x198, 0x02); Sleep(1); //div1, dac, direct to output
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x199, 0x00); Sleep(1); //div2.1
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19A, 0x00); Sleep(1); //phase div2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19B, 0x00); Sleep(1); //div2.2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19C, 0x30); Sleep(1); //div2 bypass
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19D, 0x00); Sleep(1); //div2 dcc on
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x19E, 0x00); Sleep(1); //div3.1
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19F, 0x00); Sleep(1); //phase div3
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1A0, 0x00); Sleep(1); //div3.2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1A1, 0x30); Sleep(1); //div3 bypass
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1A2, 0x00); Sleep(1); //div3 dcc on
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x1E0, 0x00); Sleep(1); //vco dic /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1E1, 0x00); Sleep(1); //ena vco divider
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x230, 0x00); Sleep(1);
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x232, 0x01); Sleep(10); //update
		if(rc!=SIPIF_ERR_OK)
			return rc;
	}
	return FMC204_CLOCKTREE_ERR_OK;
}
