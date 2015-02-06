///////////////////////////////////////////////////////////////////////////////////
///@file ctgen.cpp
///@author Arnaud Maye (4DSP) 
///\brief module to interface with the mfc_ct_gen star (implementation)
///
///////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include "ctgen.h"
#include "sipif.h"

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

/**
 * Configure the clock/trigger star.\n
 * 
 * Depending how the star is configured in the constellation ( which .lst file is use (2gsps or 5gsps) ), two distincts frequency group
 * is available.
 *
 * @note This function communicates with the hardware.
 *
 * @param   bar     offset where fmc_ct_gen is located in the constellation memory space.
 * @param   freq_sel    - OUT_XGBPS_DISABLED
 *						2Gbps (only):
 *						- OUT_2GBPS_1000MHZ
 *						- OUT_2GBPS_500MHZ
 *						- OUT_2GBPS_250MHZ
 *						- OUT_2GBPS_125MHZ
 *						- OUT_2GBPS_62MHZ5
 *						5Gbps (only):
 *						- OUT_5GBPS_2500MHZ
 *						- OUT_5GBPS_1250MHZ
 *						- OUT_5GBPS_625MHZ
 *						- OUT_5GBPS_312MHZ5
 *						- OUT_5GBPS_156MHZ25
 * @return  CTGEN_ERR_OK in case of success or any sipif ( please see sipif documentation ) error codes.
 */
int ctgen_configure(unsigned long bar, unsigned char freq_sel)
{
	int rc;
	
	rc = sipif_writesipreg(bar+0, freq_sel);
	if(rc!=SIPIF_ERR_OK)
		return rc;

	return CTGEN_ERR_OK;
}