///////////////////////////////////////////////////////////////////////////////////
///@file FMC116_adc.cpp
///@author Arnaud Maye (4DSP) 
///\brief FMC116_adc module to interface with the FMC116 star (implementation)
///
/// This module is in charge of configuring the adc chips mounted on the 
/// FMC116 hardware.
///
///////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "FMC116_adc.h"
#include "sipif.h"


int FMC116_adc_init(unsigned long bar_adc0, unsigned long bar_adc1, unsigned long bar_adc2, unsigned long bar_adc3, unsigned long bar_adc_phy, unsigned int nbrch) 
{

	unsigned long dword;
	int rc;
	
	//ADC0
	sipif_writesipreg(bar_adc0+0x00, 0x80);	Sleep(2); //reset
	sipif_writesipreg(bar_adc0+0x01, 0x20);	Sleep(2); //two's complement
	sipif_writesipreg(bar_adc0+0x02, 0x00);	Sleep(2);
	sipif_writesipreg(bar_adc0+0x03, 0xBF);	Sleep(2); //Pattern on, bit 13..8
	sipif_writesipreg(bar_adc0+0x04, 0xC0);	Sleep(2); //bit 7..0

	//ADC1
	sipif_writesipreg(bar_adc1+0x00, 0x80);	Sleep(2); //reset
	sipif_writesipreg(bar_adc1+0x01, 0x20);	Sleep(2); //two's complement
	sipif_writesipreg(bar_adc1+0x02, 0x00);	Sleep(2);
	sipif_writesipreg(bar_adc1+0x03, 0xBF);	Sleep(2); //Pattern on, bit 13..8
	sipif_writesipreg(bar_adc1+0x04, 0xC0);	Sleep(2); //bit 7..0

	//ADC2
	sipif_writesipreg(bar_adc2+0x00, 0x80);	Sleep(2); //reset
	sipif_writesipreg(bar_adc2+0x01, 0x20);	Sleep(2); //two's complement
	sipif_writesipreg(bar_adc2+0x02, 0x00);	Sleep(2);
	sipif_writesipreg(bar_adc2+0x03, 0xBF);	Sleep(2); //Pattern on, bit 13..8
	sipif_writesipreg(bar_adc2+0x04, 0xC0);	Sleep(2); //bit 7..0

	//ADC3
	sipif_writesipreg(bar_adc3+0x00, 0x80);	Sleep(2); //reset
	sipif_writesipreg(bar_adc3+0x01, 0x20);	Sleep(2); //two's complement
	sipif_writesipreg(bar_adc3+0x02, 0x00);	Sleep(2);
	sipif_writesipreg(bar_adc3+0x03, 0xBF);	Sleep(2); //Pattern on, bit 13..8
	sipif_writesipreg(bar_adc3+0x04, 0xC0);	Sleep(2); //bit 7..0

//while (1)
//{

	//Reset clock buffer and iDelays first
	rc = sipif_writesipreg(bar_adc_phy+0, 0x03); Sleep(10);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	//then reset iSerdes, when the clocks are stable
	rc = sipif_writesipreg(bar_adc_phy+0, 0x04); Sleep(10);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	//Start training
	printf("Training status : ");
	rc = sipif_writesipreg(bar_adc_phy+0, 0x08); Sleep(10);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_readsipreg(bar_adc_phy+0, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	if((dword&0x1)==0x1)
		printf("Ready\n");
	else 
		printf("Busy\n");
	
/*	while ((dword&0x1)!=0x1)
	{
		rc = sipif_writesipreg(bar_adc_phy+0, 0x02); Sleep(10);
		rc = sipif_readsipreg(bar_adc_phy+0, &dword);
		if((dword&0x1)==0x1)
			printf("_Ready\n");
		else
			printf("_Busy\n");
	} */
		
	Sleep (1000);
//}

	sipif_writesipreg(bar_adc0+0x03, 0x00);	Sleep(2); //Pattern off
	sipif_writesipreg(bar_adc1+0x03, 0x00);	Sleep(2); //Pattern off
	sipif_writesipreg(bar_adc2+0x03, 0x00);	Sleep(2); //Pattern off
	sipif_writesipreg(bar_adc3+0x03, 0x00);	Sleep(2); //Pattern off

	// Print IDELAY state
	printf("--------------------------------------\n");
	rc = fmc116_idelay_state(bar_adc_phy, nbrch);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	return FMC116_ADC_ERR_OK;
}

int FMC116_dac_init(unsigned long bar_dac0, unsigned long bar_dac1) 
{
	int rc;

#if 0
	for (int i=0; i<65535; i=i+10)
	{
		sipif_writesipreg(bar_dac0+0x3F, i);	Sleep(2); //reset
		sipif_writesipreg(bar_dac1+0x3F, i);	Sleep(2); //reset
	}
#endif

	//DAC0 - All Channels MID-SCALE (Write&Update)
	rc = sipif_writesipreg(bar_dac0+0x3F, 0x7FFF+0x600);	Sleep(2); //reset
	//rc = sipif_writesipreg(bar_dac0+0x3F, 0x0000);	Sleep(2); //reset
	//rc = sipif_writesipreg(bar_dac0+0x3F, 0xFFFF);	Sleep(2); //reset
	if(rc!=SIPIF_ERR_OK)
		return rc;

	//DAC1 - All Channels MID-SCALE (Write&Update)
	rc = sipif_writesipreg(bar_dac1+0x3F, 0x7FFF+0x600);	Sleep(2); //reset
	//rc = sipif_writesipreg(bar_dac1+0x3F, 0x0000);	Sleep(2); //reset
	//rc = sipif_writesipreg(bar_dac1+0x3F, 0xFFFF);	Sleep(2); //reset
	if(rc!=SIPIF_ERR_OK)
		return rc;

	return FMC116_DAC_ERR_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function for IDELAY state printing
int fmc116_idelay_state(unsigned long bar_adc_phy, unsigned int nbrch)
{
	unsigned long dword;
	int rc;
	int nbregs = 8;
	
	if (nbrch == 12)
      nbregs = 6;

	for (int i=0; i<nbregs; i++)
	{
		//printf("IDELAY Ch %d: ", i);
		rc = sipif_readsipreg(bar_adc_phy+8+i, &dword);
		if(rc!=SIPIF_ERR_OK)
			return rc;
		printf("%2d | %2d | %2d | %2d | ", (dword>>0)&0xFF, (dword>>8)&0xFF, (dword>>16)&0xFF, (dword>>24)&0xFF );

	}

	return FMC116_ADC_ERR_OK;
}
