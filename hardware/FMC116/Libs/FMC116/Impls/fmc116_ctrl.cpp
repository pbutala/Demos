///////////////////////////////////////////////////////////////////////////////////
///@file FMC116_ctrl.cpp
///@author Arnaud Maye (4DSP) 
///\brief FMC116_ctrl module to interface with the FMC116 star (implementation)
///
/// This module is in charge of configuring the FMC116 control module. This module
/// allows users to configure the grabbing and channel enables among various other 
/// things.
///
///////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "FMC116_ctrl.h"
#include "sipif.h"


int FMC116_ctrl_init(unsigned long bar) 
{ 
	return FMC116_CTRL_ERR_OK;
}

int FMC116_ctrl_probefmc(unsigned long bar)
{
	unsigned long dword;
	int rc;

	// Read info register 
	rc = sipif_readsipreg(bar+4, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	if ((dword&0x01)!=0x01)
		return FMC116_CTRL_ERR_PRESENT;
	if ((dword&0x02)!=0x02)
		return FMC116_CTRL_ERR_POWER_GOOD;

	return FMC116_CTRL_ERR_OK;
}

int FMC116_ctrl_configure_burst(unsigned long bar, unsigned int burstnumber, unsigned int burstlength)
{
	int rc;

	rc = sipif_writesipreg(bar+0x02, burstnumber); Sleep(10);	// Nr of Bursts
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar+0x03, burstlength); Sleep(10);	// Burst Size
	if(rc!=SIPIF_ERR_OK)
		return rc;
	
	return FMC116_CTRL_ERR_OK;
}

int FMC116_ctrl_prepare_wfm_load(unsigned long bar, unsigned int dacchannel)
{
	int rc; 
	if(dacchannel==DAC0) {
		rc = sipif_writesipreg(bar+0x01, 0x04); Sleep(10); //enable DAC0
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x00, 0x08); Sleep(10); //load WFM command
		if(rc!=SIPIF_ERR_OK)
			return rc;
	}
	else {
		rc = sipif_writesipreg(bar+0x01, 0x08); Sleep(10); //enable DAC1
		if(rc!=SIPIF_ERR_OK)
			return rc;
		rc = sipif_writesipreg(bar+0x00, 0x08); Sleep(10); //load WFM command
		if(rc!=SIPIF_ERR_OK)
			return rc;
	}

	return FMC116_CTRL_ERR_OK;
}

int FMC116_ctrl_arm(unsigned long bar)
{
	int rc;

	rc = sipif_writesipreg(bar+0x00, 0x01); Sleep(10); 
	if(rc!=SIPIF_ERR_OK)
		return rc;

	return FMC116_CTRL_ERR_OK;
}

int FMC116_ctrl_disarm_dac(unsigned long bar)
{
	int rc;

	rc = sipif_writesipreg(bar+0x00, 0x02);  Sleep(10);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	return FMC116_CTRL_ERR_OK;
}

int FMC116_ctrl_sw_trigger(unsigned long bar)
{
	int rc;

	rc = sipif_writesipreg(bar+0x00, 0x04);  Sleep(10);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	return FMC116_CTRL_ERR_OK;
}

int FMC116_ctrl_enable_channel(unsigned long bar, unsigned int ChannelEnable, unsigned char testen)
{
	unsigned long dword;
	int rc;

	// read what we have there already
	rc = sipif_readsipreg(bar+0x01, &dword);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	// mask out the enable bits
	dword &= ~0xFFFF;

	// append our values
	dword |= ChannelEnable;

	// write back
	rc = sipif_writesipreg(bar+0x01, dword); Sleep(2); 
	if(rc!=SIPIF_ERR_OK)
		return rc;

	return FMC116_CTRL_ERR_OK;
}

