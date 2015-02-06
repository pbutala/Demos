///////////////////////////////////////////////////////////////////////////////////
///@file FMC116_clocktree.cpp
///@author Arnaud Maye (4DSP) 
///\brief FMC116_clocktree module to interface with the FMC116 star (implementation)
///
/// This module is in charge of configuring the clocktree chip mounted on the 
/// FMC116 hardware.
///
///////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include "FMC116_clocktree.h"
#include "sipif.h"
#include "cid.h"

#define CONSTELLATION_ID_ML605_FMC116		0xFF			/*!< firmware(constellation) ID for FMC116 is 131 */

int FMC116_clocktree_init(unsigned long bar, unsigned int clockmode) 
{

	unsigned long dword;
	int rc;

	//Check part ID
	rc = sipif_readsipreg(bar+0x03, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	if ((dword&0xFF)!=FMC116_CLOCKTREE_PART_ID_3)
		return FMC116_CLOCKTREE_ERR_WRONG_PART_ID;
		
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Internal clock with External Reference
	// Onboard reference is 100MHz
	if(clockmode==CLOCKTREE_INTCLK_INTREF) {
		
		printf("Clock tree uses internal clock with internal reference.\n");
		rc = sipif_writesipreg(bar+0x010, 0x7C); Sleep(2); //CP 4.8mA, normal op.
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x011,   10); Sleep(2); //R lo
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x012,    0); Sleep(2); //R hi
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x013,    8); Sleep(2); //A
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x014,   12); Sleep(2); //B lo
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x015,    0); Sleep(2); //B hi
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x016, 0x05); Sleep(2); //presc. DM16
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x017, 0xB4); Sleep(2); //STATUS = DLD
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x018, 0x01); Sleep(2); //VCO Cal.
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x019, 0x00); Sleep(2);
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x01A, 0x00); Sleep(2); //LD = DLD
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x01B, 0x00); Sleep(2); //REFMON = GND
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x01C, 0x87); Sleep(2); //Diff ref input
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x01D, 0x00); Sleep(2);
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x0F0, 0x0C); Sleep(2); //out0, adc1, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x0F1, 0x0C); Sleep(2); //out1, adc2, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x0F4, 0x0C); Sleep(2); //out2, adc3, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x0F5, 0x0C); Sleep(2); //out3, adc0, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x140, 0x00); Sleep(2); //out4, external clock output
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x141, 0x01); Sleep(2); //out5, unused, pd
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x142, 0x00); Sleep(2); //out6, clock to FPGA
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x143, 0x01); Sleep(2); //out7, unused, pd
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x190, 0x33); Sleep(2); //div0, clock to ADCs, /8
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x191, 0x00); Sleep(2); //div0, clock to ADCs, divider used
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x192, 0x00); Sleep(2); //div0, clock to ADCs, divider to output
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x196, 0x33); Sleep(2); //div1, clock to ADCs, /8
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x197, 0x00); Sleep(2); //div1, clock to ADCs, divider used
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x198, 0x00); Sleep(2); //div1, clock to ADCs, divider to output
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x199, 0x33); Sleep(2); //div2.1, /8
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19A, 0x00); Sleep(2); //phase
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19B, 0x00); Sleep(2); //div2.2, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19C, 0x20); Sleep(2); //div2.1 on, div2.2 bypassed
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19D, 0x00); Sleep(2); //div2 dcc on
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x19E, 0x33); Sleep(2); //div3.1, /8
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19F, 0x00); Sleep(2); //phase
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1A0, 0x00); Sleep(2); //div3.2, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1A1, 0x20); Sleep(2); //div3.1 on, div3.2 bypassed
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1A2, 0x00); Sleep(2); //div3 dcc on
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x1E0, 0x00); Sleep(2); //vco div /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1E1, 0x02); Sleep(2); //use internal vco with vco divider
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x230, 0x00); Sleep(2); //no pwd, no sync
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x232, 0x01); Sleep(2); //update 
		if(rc!=SIPIF_ERR_OK)
			return rc;

		Sleep(20);

		// verify PLL status
		rc = sipif_readsipreg(bar+0x1F, &dword);
		if(rc!=SIPIF_ERR_OK)
			return rc;
		if((dword&0x01)!=0x01){
			printf("PLL not locked!!!\n");
			return FMC116_CLOCKTREE_ERR_CLK0_PLL_NOT_LOCKED;
		} else {
			printf("PLL locked!!!\n");
		}

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Internal clock with External Reference
	// Expecting 10MHz external reference
	else if(clockmode==CLOCKTREE_INTCLK_EXTREF) {

		printf("Clock tree uses internal clock with external reference.\n");

		rc = sipif_writesipreg(bar+0x010, 0x7C); Sleep(2); //CP 4.8mA, normal op.
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x011,    1); Sleep(2); //R lo,
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x012,    0); Sleep(2); //R hi
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x013,    8); Sleep(2); //A
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x014,   12); Sleep(2); //B lo
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x015,    0); Sleep(2); //B hi
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x016, 0x05); Sleep(2); //presc. DM16
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x017, 0xB4); Sleep(2); //STATUS = DLD
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x018, 0x01); Sleep(2); //VCO Cal.
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x019, 0x00); Sleep(2);
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x01A, 0x00); Sleep(2); //LD = DLD
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x01B, 0x00); Sleep(2); //REFMON = GND
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x01C, 0x87); Sleep(2); //Diff ref input
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x01D, 0x00); Sleep(2);
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x0F0, 0x0C); Sleep(2); //out0, adc1, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x0F1, 0x0C); Sleep(2); //out1, adc2, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x0F4, 0x0C); Sleep(2); //out2, adc3, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x0F5, 0x0C); Sleep(2); //out3, adc0, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x140, 0x00); Sleep(2); //out4, external clock output
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x141, 0x01); Sleep(2); //out5, unused, pd
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x142, 0x00); Sleep(2); //out6, clock to FPGA
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x143, 0x01); Sleep(2); //out7, unused, pd
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x190, 0x33); Sleep(2); //div0, clock to ADCs, /8
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x191, 0x00); Sleep(2); //div0, clock to ADCs, divider used
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x192, 0x00); Sleep(2); //div0, clock to ADCs, divider to output
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x196, 0x33); Sleep(2); //div1, clock to ADCs, /8
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x197, 0x00); Sleep(2); //div1, clock to ADCs, divider used
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x198, 0x00); Sleep(2); //div1, clock to ADCs, divider to output
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x199, 0x33); Sleep(2); //div2.1, /8
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19A, 0x00); Sleep(2); //phase
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19B, 0x00); Sleep(2); //div2.2, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19C, 0x20); Sleep(2); //div2.1 on, div2.2 bypassed
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19D, 0x00); Sleep(2); //div2 dcc on
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x19E, 0x33); Sleep(2); //div3.1, /8
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19F, 0x00); Sleep(2); //phase
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1A0, 0x00); Sleep(2); //div3.2, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1A1, 0x20); Sleep(2); //div3.1 on, div3.2 bypassed
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1A2, 0x00); Sleep(2); //div3 dcc on
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x1E0, 0x00); Sleep(2); //vco div /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1E1, 0x02); Sleep(2); //use internal vco with vco divider
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x230, 0x00); Sleep(2); //no pwd, no sync
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x232, 0x01); Sleep(2); //update 
		if(rc!=SIPIF_ERR_OK)
			return rc;

		Sleep(20);

		// verify PLL status
		rc = sipif_readsipreg(bar+0x1F, &dword);
		if(rc!=SIPIF_ERR_OK)
			return rc;
		if((dword&0x01)!=0x01){
			printf("PLL not locked!!!\n");
			return FMC116_CLOCKTREE_ERR_CLK0_PLL_NOT_LOCKED;
		} else {
			printf("PLL locked!!!\n");
		}

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// External clock
	else {

		printf("Clock tree uses external clock.\n");

		rc = sipif_writesipreg(bar+0x010, 0x7D); Sleep(2); //CP 4.8mA, PLL power down
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x017, 0xB4); Sleep(2); //STATUS = DLD
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x019, 0x00); Sleep(2);
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x01A, 0x00); Sleep(2); //LD = DLD
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x01B, 0x00); Sleep(2); //REFMON = GND
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x01C, 0x87); Sleep(2); //Diff ref input
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x01D, 0x00); Sleep(2);
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x0F0, 0x0C); Sleep(2); //out0, adc1, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x0F1, 0x0C); Sleep(2); //out1, adc2, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x0F4, 0x0C); Sleep(2); //out2, adc3, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x0F5, 0x0C); Sleep(2); //out3, adc0, lvpecl 960mW
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x140, 0x00); Sleep(2); //out4, external clock output
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x141, 0x01); Sleep(2); //out5, unused, pd
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x142, 0x00); Sleep(2); //out6, clock to FPGA
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x143, 0x01); Sleep(2); //out7, unused, pd
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x190, 0x00); Sleep(2); //div0, clock to ADCs, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x191, 0x80); Sleep(2); //div0, clock to ADCs, divider bypased
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x192, 0x02); Sleep(2); //div0, clock to ADCs, direct to output
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x196, 0x00); Sleep(2); //div1, clock to ADCs, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x197, 0x80); Sleep(2); //div1, clock to ADCs, divider bypassed
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x198, 0x02); Sleep(2); //div1, clock to ADCs, direct to output
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x199, 0x00); Sleep(2); //div2.1, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19A, 0x00); Sleep(2); //phase
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19B, 0x00); Sleep(2); //div2.2, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19C, 0x30); Sleep(2); //div2.1 bypassed, div2.2 bypassed
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19D, 0x00); Sleep(2); //div2 dcc on
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x19E, 0x00); Sleep(2); //div3.1, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x19F, 0x00); Sleep(2); //phase
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1A0, 0x00); Sleep(2); //div3.2, /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1A1, 0x30); Sleep(2); //div3.1 bypassed, div3.2 bypassed
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1A2, 0x00); Sleep(2); //div3 dcc on
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x1E0, 0x00); Sleep(2); //vco div /2
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x1E1, 0x01); Sleep(2); //use external clock with divider bypassed
		if(rc!=SIPIF_ERR_OK)
			return rc;

		rc = sipif_writesipreg(bar+0x230, 0x00); Sleep(2); //no pwd, no sync
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x232, 0x01); Sleep(2); //update 
		if(rc!=SIPIF_ERR_OK)
			return rc;

	}

	return FMC116_CLOCKTREE_ERR_OK;
}