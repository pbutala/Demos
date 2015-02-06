///////////////////////////////////////////////////////////////////////////////////
///@file ctgen.h
///@author Arnaud Maye (4DSP) 
///\brief module to interface with the mfc_ct_gen star (header)
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef _CTGEN_H_
#define _CTGEN_H_

/* defines */
#define OUT_XGBPS_DISABLED		0							/*!< Nothing goes out on ML605's J26, the clock generation is disabled. */

#define OUT_2GBPS_1000MHZ			1							/*!< Get the star to output 1000MHz on ML605's J26 (If the star uses 2Gbps configuration). */
#define OUT_2GBPS_500MHZ			2							/*!< Get the star to output 500MHz on ML605's J26 (If the star uses 2Gbps configuration). */
#define OUT_2GBPS_250MHZ			3							/*!< Get the star to output 250MHz on ML605's J26 (If the star uses 2Gbps configuration). */
#define OUT_2GBPS_125MHZ			4							/*!< Get the star to output 125MHz on ML605's J26 (If the star uses 2Gbps configuration). */
#define OUT_2GBPS_62MHZ5			5							/*!< Get the star to output 62.5MHz on ML605's J26 (If the star uses 2Gbps configuration). */

#define OUT_5GBPS_2500MHZ			1							/*!< Get the star to output 2500MHz on ML605's J26 (If the star uses 5Gbps configuration). */
#define OUT_5GBPS_1250MHZ			2							/*!< Get the star to output 1250MHz on ML605's J26 (If the star uses 5Gbps configuration). */
#define OUT_5GBPS_625MHZ			3							/*!< Get the star to output 625MHz on ML605's J26 (If the star uses 5Gbps configuration). */
#define OUT_5GBPS_312MHZ5			4							/*!< Get the star to output 312.5MHz on ML605's J26 (If the star uses 5Gbps configuration). */
#define OUT_5GBPS_156MHZ25		5							/*!< Get the star to output 156.25MHz on ML605's J26 (If the star uses 5Gbps configuration). */

/* error codes */
#define CTGEN_ERR_OK					0							/*!< No error encountered during execution. */

// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 

int ctgen_configure(unsigned long bar, unsigned char freq_sel);

// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_CTGEN_H_