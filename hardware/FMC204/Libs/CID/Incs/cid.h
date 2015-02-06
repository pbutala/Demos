//////////////////////////////////////////////////////////////////////////////
///@file cid.h
///@author Arnaud Maye (4DSP)
///\brief cid module to interface with the sipcid star (header)
//////////////////////////////////////////////////////////////////////////////
#ifndef _SIPCID_H_
#define _SIPCID_H_

#define MAX_STR_SIZE						1024					/*!< Length of a string in bytes. This value is used for statically allocated buffers. */
#define MAX_NBR_STAR						512						/*!< Maximum number of stars (rows) in the sipcid table. This value is used for statically allocated buffers */
#define SIP_CID_BAR							0x00002000				/*!< Address where the sipcid area is located on the firmware's memory address space. */
#define SIP_CID_SW_BUILD					(SIP_CID_BAR+0x01)		/*!< Address where the software build register is located on the firmware's memory address space. */
#define SIP_CID_FW_BUILD					(SIP_CID_BAR+0x02)		/*!< Address where the firmware build register is located on the firmware's memory address space. */
#define SIP_CID_FW_VERSION					(SIP_CID_BAR+0x03)		/*!< Address where the firmware version register is located on the firmware's memory address space. */
#define SIP_CID_ROW_START					(SIP_CID_BAR+0x04)		/*!< Address where the first ROW register is located on the firmware's memory address space. */

/* error codes */
#define SIP_CID_ERR_OK						0						/*!< No error encountered during execution. */
#define SIP_CID_ERR_LOW_LEVEL_IO			-1						/*!< Unable to communicate with the hardware/firmware over Ethernet. */
#define SIP_CID_ERR_NULL_ARG				-2						/*!< An unexpected NULL argument has been passed to the function. */
#define SIP_CID_ERR_NOTHING_TO_DO			-3						/*!< cid_getstaroffset() has been invoked with starid argument equal to 0 and this is not accepted */
#define SIP_CID_ERR_OUTBUF_TOO_SMALL		-4						/*!< cid_getstaroffset() has more base addresses than the buffer passed as argument can hold without overflow */
#define SIP_CID_ERR_WRONG_CONSTELLATION_ID	-5						/*!< cid_init() the constellation ID received from the firmware does not match the constellation ID passed as argument */

/* this is one ROW in our sip_cid table */
/*! \typedef sip_cid_row
 * 
 */
/*! \typedef psip_cid_row
 * Pointer to a row structure.
 */
/*!
 * \brief format of a sipcid row. The sip_cid table has as many rows as there are stars in the constellation.
 *
 */
typedef struct {
	unsigned long base_address;					/*!< Address corresponding to the beginning of a star memory in the firmware's memory space. */
	unsigned long end_address;					/*!< Address corresponding to the end of a star memory in the firmware's memory space. */
	unsigned short star_id;						/*!< Identification code of the star. */
	unsigned short star_version;				/*!< Version of the star. */
	unsigned char starname[MAX_STR_SIZE];		/*!< All the sip_cid_row fields except this one are directly populated from the firmware. This particular
													field is optional and should be written by the software translating IDs to strings. */
} sip_cid_row, *psip_cid_row;

/* this is the complete sip_cid table as it would be in memory. This structure receive values retrieved from the firmware. */
/*! \typedef sipcid_table
 */
/*! \typedef psipcid_table
 * Pointer to a sipcid_table structure.
 */
/*!
 * \brief definition of the main sip_cid table in memory.
 *
 */
typedef struct  {
	unsigned short constellation_id;			/*!< ID of the firmware currently loaded in the FPGA. */
	unsigned short number_stars;				/*!< Number of IPCores blocks in the firmware currently loaded in the FPGA */
	unsigned long sw_build_code;				/*!< Incremental/Random code serving as software build code. */
	unsigned long fw_build_code;				/*!< The firmware build code. */
	unsigned long fw_version_low;				/*!< Firmware version ( minor ). */
	unsigned long fw_version_high;				/*!< Firmware version ( major ). */
	sip_cid_row rows[MAX_NBR_STAR];				/*!< One row per number_stars above. */
} sipcid_table, *psipcid_table;

/* this is the global sip_cid_table "object" */
static sipcid_table g_sipcidtbl;				/*!< The "global" sip_cid table in memory */

// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 

/**
 * \brief Initialize the sip_cid module.
 *
 * This function populates the global sip_cid table in memory. 
 *
 * @note Communication with the hardware/firmware over Ethernet is happening in this function.
 * @param   constellationid		the constellation (firmware) identification code we expect to read back from the firmware.
 * @return  SIP_CID_ERR_LOW_LEVEL_IO, SIP_CID_ERR_WRONG_CONSTELLATION_ID or the number of ipcores blocks found in the firmware. Note that a return value of 0 indicates
 * that no ipcores blocks have been found in the firmware.
 */
int cid_init(unsigned int constellationid);

/**
 * \brief Retrieve the software build code from the sip_cid table in memory.
 *
 * @note This function does not communicate with the hardware/firmware.
 * @warning Calling cid_init() prior calling this function is mandatory. This function returns undefined data if cid_init() has not yet been called.
 * @return  The software build code as provided by the firmware during cid_init().
 */
unsigned long cid_getswbuildcode(void);

/**
 * \brief Retrieve the firmware build code from the sip_cid table in memory.
 *
 * @note This function does not communicate with the hardware/firmware.
 * @warning Calling cid_init() prior calling this function is mandatory. This function returns undefined data if cid_init() has not yet been called.
 * @return  The firmware build code as provided by the firmware during cid_init().
 */
unsigned long cid_getfwbuildcode(void);

/**
 * \brief Retrieve the firmware identification number (ID) from the sip_cid table in memory.
 *
 * @note This function does not communicate with the hardware.
 * @warning Calling cid_init() prior calling this function is mandatory. This function returns undefined data if cid_init() has not yet been called.
 * @return  The firmware identification number (ID) as provided by the firmware during cid_init().
 */
unsigned short cid_getconstellationid(void);

/**
 * \brief Retrieve the firmware revision from the sip_cid table in memory. 
 *
 * Retrieve the firmware revision from the sip_cid table in memory. The returned value is both firmware major and minor version 
 * joined together. Two 16 bits wide integer are provided, one for the minor version and the other one for the major version.
 *
 * Bits 31 down to 16 are the major version and bits 15 down to 0 are the minor revision.
 *
 * @note This function does not communicate with the hardware.
 * @warning Calling cid_init() prior calling this function is mandatory. This function returns undefined data if cid_init() has not yet been called.
 * @return  The firmware revision as provided by the firmware during cid_init(). (MAJOR<<16)|MINOR is the output format.
 */
unsigned long cid_getfirmwareversion(void);

/**
 * \brief Retrieve a pointer to the sip_cid table in memory. 
 *
 * Retrieve a pointer to the sip_cid table in memory.
 *
 * @note This function does not communicate with the hardware.
 * @return  A pointer to the sip_cid table in memory or NULL in case of error.
 */
psipcid_table cid_get_sipcidtbl(void);

/**
 * \brief Retrieve the number of ipcores blocks present in the firmware. 
 *
 * Retrieve the number of ipcores in the firmware from the sip_cid table in memory. 
 *
 * @note This function does not communicate with the hardware.
 * @warning Calling cid_init() prior calling this function is mandatory. This function returns undefined data if cid_init() has not yet been called.
 * @return  the number of ipcores in the firmware.
 */
unsigned long cid_getnbrstar(void);

/**
 * \brief Get the offset(address) of an ipcore block in the firmware. 
 *
 * Get the offset(address) of an ipcore block in the firmware. This address can be directly passed to sipif_readsipreg() and sipif_writesipreg() part
 * of the ethapi API. If a particular starid is found several times in the firmware then the function is able to report back several offsets. 
 *
 * @note This function does not communicate with the hardware.
 * @warning Calling cid_init() prior calling this function is mandatory. This function returns undefined data if cid_init() has not yet been called.
 * @param   starid     the ipcore's identification code we want to retrieve offset for.
 * @param   offset     pointer to a memory location about to receive the 32 bit wide offset. This can be a 32 bit wide array location and in this case size should be bigger than 1.
 * @param   size     pointer to a memory location where size of the buffer pointed by offset can be found. This can be one or bigger depending how many 32 bit integer space is available at this location. 
 *                   This variable is updated to represent the number of 32 bit integer required to represent the information available.
 * @return  SIP_CID_ERR_NULL_ARG ( arguments cannot be NULL ), SIP_CID_ERR_NOTHING_TO_DO ( starid cannot be 0 ), SIP_CID_ERR_OUTBUF_TOO_SMALL ( the buffer provided by the caller is not big enough. In this case size is updated with the size required to hold all the offsets for a given starid ) and SIP_CID_ERR_OK ( no error ).
 */
int cid_getstaroffset(unsigned int starid, unsigned long *offset, unsigned long *size);



// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_SIPCID_H_