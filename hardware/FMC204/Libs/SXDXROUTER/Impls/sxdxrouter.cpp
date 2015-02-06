///////////////////////////////////////////////////////////////////////////////////
///@file sxdxrouter.cpp
///@author Arnaud Maye (4DSP) 
///\brief router module to interface with the sxdxrouter star (implementation)
///
///////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include "sxdxrouter.h"
#include "sipif.h"

/**
 * Configure a router star.
 * @note This function communicates with the hardware.
 *
 * @param   bar     offset where SXDX is located in the constellation memory space.
 * @param   value    64 bit configuration value ( 32 bit MSB and 32 bit LSB ). Please consult SD018 star documentation.
						to understand what kind of configuration words are expected to be sent.
 * @return  SXDXROUTER_ERR_OK in case of success or any ethapi ( please see ethapi documentation ) error codes.
 */
int sxdx_configurerouter(unsigned long bar, unsigned __int64 value)
{
	unsigned long router_lsb, router_msb, rc;

	router_lsb = value&0xFFFFFFFF;
	router_msb = (value>>32)&0xFFFFFFFF;

	rc = sipif_writesipreg(bar+0, router_lsb);
	if(rc!=SIPIF_ERR_OK)
		return rc;
	rc = sipif_writesipreg(bar+1, router_msb);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	return SXDXROUTER_ERR_OK;
}
