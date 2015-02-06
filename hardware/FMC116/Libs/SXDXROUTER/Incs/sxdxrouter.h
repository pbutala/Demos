///////////////////////////////////////////////////////////////////////////////////
///@file sxdxrouter.h
///@author Arnaud Maye (4DSP) 
///\brief router module to interface with the sxdxrouter star (header)
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef _SXDXROUTER_H_
#define _SXDXROUTER_H_

/* defines */

/* error codes */
#define SXDXROUTER_ERR_OK					0							/*!< No error encountered during execution. */

// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 


int sxdx_configurerouter(unsigned long bar, unsigned __int64 value);

// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_SXDXROUTER_H_