//////////////////////////////////////////////////////////////////////////////
///@file cid.cpp
///@author Arnaud Maye (4DSP) 
///\brief cid module to interface with the sipcid star (implementation)
//////////////////////////////////////////////////////////////////////////////
#include <Windows.h>
#include <stdlib.h>
#include "cid.h"
#include "sipif.h"

unsigned long cid_getswbuildcode(void)
{
	return g_sipcidtbl.sw_build_code;
}

unsigned long cid_getfwbuildcode(void)
{
	return g_sipcidtbl.fw_build_code;
}

unsigned long cid_getnbrstar(void)
{
	return g_sipcidtbl.number_stars;
}

unsigned short cid_getconstellationid(void)
{
	return g_sipcidtbl.constellation_id;
}

unsigned long cid_getfirmwareversion(void)
{
	return (g_sipcidtbl.fw_version_high<<16)|g_sipcidtbl.fw_version_low;
}


int cid_getstaroffset(unsigned int starid, unsigned long *offset, unsigned long *size)
{
	int mainiter;
	int typeiter;
	int typereq;

	// offset cannot be NULL
	if(offset == NULL)
		return SIP_CID_ERR_NULL_ARG;

	// size cannot be NULL
	if(size == NULL)
		return SIP_CID_ERR_NULL_ARG;

	// star id cannot be 0
	if(starid == 0)
		return SIP_CID_ERR_NOTHING_TO_DO;


	// compute how many times that given star id is part of the constellation
	for(mainiter = 0, typereq = 0; mainiter < g_sipcidtbl.number_stars; mainiter++) {
		if(g_sipcidtbl.rows[mainiter].star_id == starid)
			typereq++;
	}

	// if the buffer is not big enough, let the caller aware about how many word should be present in his buffer
	if((unsigned long)typereq>*size) {
		*size = typereq;
		return SIP_CID_ERR_OUTBUF_TOO_SMALL;
	}

	// scan for a given star ID populating the array passed as argument
	for(mainiter = 0, typeiter = 0; mainiter < g_sipcidtbl.number_stars; mainiter++) {
		if(g_sipcidtbl.rows[mainiter].star_id == starid) {
			if((unsigned long)typeiter <= *size) {
				offset[typeiter] = g_sipcidtbl.rows[mainiter].base_address;
				typeiter++;
			}
		}
	}

	return SIP_CID_ERR_OK;	
}


psipcid_table cid_get_sipcidtbl()
{
	return &g_sipcidtbl;	
}

int cid_init(unsigned int constellationid)
{
	unsigned long dword;
	unsigned long rc;
	unsigned int iter;

	// zero the complete sipcid array
	memset(&g_sipcidtbl, 0, sizeof(g_sipcidtbl));

	// first of all read the first register in the sip_cid star implemented in the firmware
	// this register contains constellationID as well as number of stars present in the sip_cid
	// table
	rc = sipif_readsipreg(SIP_CID_BAR, &dword);
	if(rc!=SIPIF_ERR_OK) {
		return SIP_CID_ERR_LOW_LEVEL_IO;
	}
	g_sipcidtbl.constellation_id = dword>>16;
	g_sipcidtbl.number_stars = dword&0xFFFF;

	// we want to make sure that the constellation ID is what we excpect. A wrong constellation ID mean wrong firmware
	// installed on the hardware or that we are communicating to the wrong device
	if(g_sipcidtbl.constellation_id!=constellationid) {
		if(constellationid!=0)
			return SIP_CID_ERR_WRONG_CONSTELLATION_ID;
	}

	// read the software build. The software build is written by stellarIP at the time a design is generated
	rc = sipif_readsipreg(SIP_CID_SW_BUILD, &g_sipcidtbl.sw_build_code);
	if(rc!=SIPIF_ERR_OK) {
		return SIP_CID_ERR_LOW_LEVEL_IO;
	}
	// read the firmware build. The firmware build is undefined
	rc = sipif_readsipreg(SIP_CID_FW_BUILD, &g_sipcidtbl.fw_build_code);
	if(rc!=SIPIF_ERR_OK) {
		return SIP_CID_ERR_LOW_LEVEL_IO;
	}
	// read the firmware version
	rc = sipif_readsipreg(SIP_CID_FW_VERSION, &dword);
	if(rc!=SIPIF_ERR_OK) {
		return SIP_CID_ERR_LOW_LEVEL_IO;
	}
	g_sipcidtbl.fw_version_low = dword&0xFFFF;
	g_sipcidtbl.fw_version_high = dword>>16;

	// iterate here as many times we have stars present in the sipcid
	// one row is composed of three registers 
	for(iter = 0; iter <  g_sipcidtbl.number_stars; iter++) {
		rc = sipif_readsipreg(SIP_CID_ROW_START+3*iter, &dword);
		if(rc!=SIPIF_ERR_OK) {
			return SIP_CID_ERR_LOW_LEVEL_IO;
		}
		g_sipcidtbl.rows[iter+0].base_address = dword;

		rc = sipif_readsipreg(SIP_CID_ROW_START+3*iter+1, &dword);
		if(rc!=SIPIF_ERR_OK) {
			return SIP_CID_ERR_LOW_LEVEL_IO;
		}
		g_sipcidtbl.rows[iter].end_address = dword;

		rc = sipif_readsipreg(SIP_CID_ROW_START+3*iter+2, &dword);
		if(rc!=SIPIF_ERR_OK) {
			return SIP_CID_ERR_LOW_LEVEL_IO;
		}
		g_sipcidtbl.rows[iter].star_id = dword>>16;
		g_sipcidtbl.rows[iter].star_version = dword&0xFFFF;
	}
	
	return g_sipcidtbl.number_stars;
}