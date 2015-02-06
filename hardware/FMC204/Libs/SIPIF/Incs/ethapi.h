/**
 * @file
 * 4DSP's EthernetII API (ethapi) Header.
 */


#ifndef ETHAPI_H
#define ETHAPI_H

// includes
#include <windows.h>
#include <ctype.h>

// API options
#define API_OPTION_POLL_WRITE_BUSY_FLAG				1			/*!< With this option enabled, the API is polling a ready flag before and after writing a register to the firmware. */
#define API_OPTION_AUTOOFFLOAD_SANITY_CHECK			2			/*!< With this option enabled, the API is checking the integrity of the frame. This is the only way to make sure no
																	  frame have been missed. Pay a particular care to the fact this option on itself might makes you miss more frames 
																	  as checking the frame integrity takes a pretty important CPU time. */
#define API_OPTION_LOG_FUNCTIONS_IOUT				4			/*!< No documentation available for this option. This option is not yet implemented. */
#define API_OPTION_LOG_SYS_WRITE					8			/*!< No documentation available for this option. Create a writeregister file */


// A few function behavioral arguments defines
#define API_ENUM_DISPLAY							1			/*!< Used as GetDevicesEnumeration() argument telling the function to display enumeration to the console  */						
#define API_ENUM_NODISPLAY							0			/*!< Used as GetDevicesEnumeration() argument telling the function to NOT display enumeration to the console  */

// Format of the file output, ASCII or binary.
#define ASCII										0			/*!< Used as Save16BitArrayToFile() argument telling the function to write ASCII data to the file.  */
#define BINARY										1			/*!< Used as Save16BitArrayToFile() argument telling the function to write binary data to the file.  */

// calling convention
#define ETH_API										__cdecl	/*!< We define a general calling convention for all our function calls  */

/*! \typedef ETHAPI_HANDLE
 * The device handle.
 */
/*! \typedef PETHAPI_HANDLE
 * Pointer to the device handle.
 */
/*!
 * \brief The device handle.
 *
 * This is a wrapper wrapping around the Windows's HANDLE type.
 */
typedef struct tagETHAPI_HANDLE
{
	HANDLE hDev;	/*!< Handle to the device, actually a Windows's HANDLE */
}  ETHAPI_HANDLE, *PETHAPI_HANDLE;

// the DEVENUM handle and various defines
#define _MAX_NDISDENOM								1024		/*!< Maximum device name/description string for each device in our ::tagDEVENUM structure  */				
#define _MAX_NDISPERIPHERALS						10			/*!< Maximum number of device(peripherals) in our ::tagDEVENUM structure  */

/*! \typedef DEVENUM
 * The device enumeration structure.
 */
/*! \typedef PDEVNUM
 * Pointer to the device enumeration structure.
 */
/*!
 * \brief The device enumeration structure.
 *
 * This structure contains detailed information for every Ethernet device found in the system.
 */
typedef struct tagDEVENUM
{
	int nbrDevices;											/*!< number of devices in our device enumeration structure. */
	int IdArray[_MAX_NDISPERIPHERALS];						/*!< array for every devices. Actually the NDIS binding index. */
	int OpenArray[_MAX_NDISPERIPHERALS];					/*!< 0 ( not opened ) or 1 ( opened ) for every devices. */
	char *NDISArray[_MAX_NDISPERIPHERALS][_MAX_NDISDENOM];	/*!< NDIS name for every devices. Actually NDIS's GUID for every devices. */
	char *DescrArray[_MAX_NDISPERIPHERALS][_MAX_NDISDENOM];	/*!< NDIS description for every devices. */
} DEVENUM, *PDEVNUM;

/**
 * \brief Configure various API behavior.
 *
 * This function configures the API and provide the user with a way to fine tune the API operation.
 * Following is a list with the various options available:
 * ::API_OPTION_POLL_WRITE_BUSY_FLAG (polling to ensure firmware is ready to receive new commands/writes).
 * ::API_OPTION_AUTOOFFLOAD_SANITY_CHECK (check frames integrity over the Ethernet packets, missing frames, etc).
 * Note that ::API_OPTION_AUTOOFFLOAD_SANITY_CHECK consumes CPU power and should only enabled for specific verifications.
 * All the options can be cumulated using a boolean or operation , ie (API_OPTION_POLL_WRITE_BUSY_FLAG|API_OPTION_AUTOOFFLOAD_SANITY_CHECK)
 *
 * @param   nOptions     nOptions is an enable bit field. A bit set to '1' enable a particular option. Only a few set of these 32 bits are 
 *						 actually used by the API.
 *
 * @return  none.
 */
VOID	ETH_API SetApiOptions(ULONG nOptions);

/**
 * \brief Obtain the current API option word.
 *
 * This function retrieves the API options/configuration word.
 * Following is a list with the various options available:
 * ::API_OPTION_POLL_WRITE_BUSY_FLAG (polling to ensure firmware is ready to receive new commands/writes).
 * ::API_OPTION_AUTOOFFLOAD_SANITY_CHECK (check frames integrity over the Ethernet packets, missing frames, etc).
 * Note that ::API_OPTION_AUTOOFFLOAD_SANITY_CHECK consumes CPU power and should only enabled for specific verifications.
 * All the options can be cumulated using a boolean or operation , ie (API_OPTION_POLL_WRITE_BUSY_FLAG|API_OPTION_AUTOOFFLOAD_SANITY_CHECK)
 *
 * @param   pOptions     pOptions is a pointer to a unsigned long memory. The memory pointed by pOption will receive the option word.
 *
 * @return  ::ERR_ETH_API_OK or ::ERR_ETH_API_UNABLE_GET_API_OPTIONS. Calling GetLastApiError() in case of error retrieves a more detailed error code.
 */
int	ETH_API GetApiOptions(PULONG pOptions);

/**
 * \brief Free API resources.
 *
 * This function first close any opened device and then free (unlock) the protocol driver.
 *
 */
VOID	ETH_API ClosePort(void);

/**
 * \brief Retrieve the last encountered error. 
 *
 * This function is useful in case a function call fails. Calling this function provide the user with a more detailed error code.
 *
 *
 * @return  The error code associated with the last encountered API error.
 */
ULONG	ETH_API GetLastApiError();

/**
 * \brief Open an Ethernet device present in the system.
 *
 * This function connects to the 4DSP's protocol driver and then open a device corresponding to a device index passed as argument. The device index
 * can be found using GetDevicesEnumeration().
 *
 * @param   devIdx	peripheral index in out device enumeration table.
 *
 * @return  ::ERR_ETH_API_OK or ::ERR_ETH_API_COULD_NOT_OPEN_DEVICE. Calling GetLastApiError() in case of error retrieves a more detailed error code.
 */
ULONG	ETH_API OpenDevice(ULONG devIdx);


/**
 * \brief Write a value to a firmware register.
 *
 * This function writes a given 32 bits value to a given system address.
 *
 * @param   address		the place (address) where data should be written to.
 * @param   data		the value to be written at address.
 *
 * @return  ::ERR_ETH_API_OK, ::ERR_ETH_API_UNABLE_TO_READ or ::ERR_ETH_API_UNABLE_TO_WRITE. Calling GetLastApiError() in case of error retrieves a more detailed error code.
 */
ULONG	ETH_API WriteSystemRegister(ULONG address, ULONG data);

/**
 * \brief Read a value from a  firmware register.
 *
 * This function reads a 32 bits system registers and place this value at the memory pointed by address.
 *
 * @param   address		the place (address) where data should be read from.
 * @param   data		pointer to a memory receiving the system register value.
 *
 * @return  ::ERR_ETH_API_OK, ::ERR_ETH_API_UNABLE_TO_READ or ::ERR_ETH_API_UNABLE_TO_WRITE. Calling GetLastApiError() in case of error retrieves a more detailed error code.
 */
ULONG	ETH_API ReadSystemRegister(ULONG address, PULONG data);

/**
 * \brief Obtain data sent automatically by the firmware.
 *
 * This function actually try to read some data received by the protocol driver. This function does not read from the firmware itself but read from a 
 * software queue part of the protocol driver.
 *
 * @note No specific data integrity verification is enabled by default. This option is enabled using SetApiOptions() and is disabled by default. This option is called ::API_OPTION_AUTOOFFLOAD_SANITY_CHECK
 *
 * @param   burstSize		the size of a burst per channel in bytes.
 * @param   outbuf		pointer to a memory receiving the actual burst from the hardware. This buffer should be previously allocated by the caller
 *
 * @return  ::ERR_ETH_API_OK or ::ERR_ETH_API_UNABLE_TO_READ. Calling GetLastApiError() in case of error retrieves a more detailed error code.
 */
ULONG	ETH_API GetAutoOffloadData(ULONG burstSize, PUCHAR outbuf);

/**
 * \brief Obtain the various Ethernet device present in the system.
 *
 * This function actually obtains the Ethernet device list from the operating system and display ( or not ) the enumeration to the console.
 *
 * @param   bDisplayToConsole		display the enumeration to the console ( ::API_ENUM_DISPLAY ) or do not display anything ( ::API_ENUM_NODISPLAY ). The second case is meant to only populate the API with the enumeration.
 *
 * @return  NULL or a pointer to an initialized DEVENUM structure. Calling GetLastApiError() in case of error retrieves a more detailed error code.
 */
PDEVNUM ETH_API GetDevicesEnumeration(ULONG bDisplayToConsole);


/**
 * \brief Send a block of data to the firmware.
 *
 * This function sends a block of data to the firmware via Ethernet. The function sends the data in multiple EthernetII packets before to send as many packet as required.
 *
 * @param   pOutBuffer		pointer to the memory containing the data to be sent to the firmware.
 * @param   nSizeBuffer		size of the data present at this memory location.
 * @param   nChanNumber		channel to write data to. This could be a digital to analog channel.
 *
 * @note	this function is not yet supported by all the firmwares. Please consult 4DSP support for more information.
 *
 * @return  ::ERR_ETH_API_OK or ::ERR_ETH_API_UNABLE_TO_WRITE_BLOCK in case of error. Calling GetLastApiError() in case of error retrieves a more detailed error code.
 */
ULONG	ETH_API WriteBlock(PUCHAR pOutBuffer, ULONG nSizeBuffer, UCHAR nChanNumber);

/**
 * \brief Read a block of data from the firmware.
 *
 * This function reads a block of data to the firmware via Ethernet. The function reads the data in multiple EthernetII packets before to make it available to the user space.
 *
 * @param   pInBuffer		pointer to the memory receiving the data firmware.
 * @param   nSizeBuffer		size of the data present at this memory location.
 * @param   nChanNumber		channel to write data to. This could be a digital to analog channel.
 *
 * @note	this function is not yet supported by all the firmwares. Please consult 4DSP support for more information.
 *
 * @return  ::ERR_ETH_API_OK or ::ERR_ETH_API_UNABLE_TO_READ_BLOCK in case of error. Calling GetLastApiError() in case of error retrieves a more detailed error code.
 */
ULONG	ETH_API ReadBlock(PUCHAR pInBuffer, ULONG nSizeBuffer, UCHAR nChanNumber);


/**
 * \brief Write a 16 bit samples array to file.
 *
 * This function write the data contained by a buffer into a file. This function receives the file name as an asrgument.
 * The function can write to ASCII file or binary file.
 * The function expects the buffer to contain 16 bits wide samples. 
 *
 * @param   buf		pointer to the memory memory area containing samples.
 * @param   bufsize		number of samples at this memory location. Note that this is not a number of byte but a number of samples. A sample is two bytes wide as samples are 16 bits wide.
 * @param   filename	pointer to a string holding file name. 
 * @param   mode	writing mode. Either binary (::BINARY) or ASCII (::ASCII). 
 *
 *
 * @return  ::ERR_ETH_API_OK or ::ERR_ETH_API_UNABLE_WRITE_TO_FILE in case of error. Calling GetLastApiError() in case of error retrieves a more detailed error code.
 */
ULONG ETH_API Save16BitArrayToFile(void *buf, int bufsize, const char *filename, int mode);

/**
 * \brief Obtain a frequency from the firmware.
 *
 * This function query the firmware to obtain a given frequency from the frequency tree. The IDs of various clocks in the tree is subject to change and the user should always refer 
 * to the reference firmware design documentation on the FREQ_SEL description. 
 *
 * @param   freqID		frequency ID in the frequency tree.
 * @param   freqMHz		pointer to a float variable about to receive clock information from the firmware.
 * @return  ::ERR_ETH_API_OK or ::ERR_ETH_API_UNABLE_GET_FREQUENCY in case of error. Calling GetLastApiError() in case of error retrieves a more detailed error code.
 */
ULONG	ETH_API GetFrequency(ULONG freqID, float *freqMHz);

/**
 * \brief Obtain the number of channels supported by the firmware.
 *
 * Read the number of channels available through the firmware. Arguments are optionals and NULL can be passed as argument if the user is not interested by
 * one of the information
 *
 * @param   pChanEn		pointer to a 32 bit memory location about to receive the bit enable field from the firmware.
 * @param   pNbrChannels	number of channels available. In other words, number of bit actually '1' in the bit enable field.
 * @return  ::ERR_ETH_API_OK or ::ERR_ETH_API_UNABLE_GET_CHAN_EN_INFO in case of error. Calling GetLastApiError() in case of error retrieves a more detailed error code.
 */
ULONG	ETH_API GetChannelEnableInfo(PULONG pChanEn, PUCHAR pNbrChannels);

/**
 * \brief Set the number of channels to be enabled.
 *
 * Update the channel enable bit field word in the firmware.
 *
 * @param   nChanEn		value to place in the firmware. This is the new channel enable bit field to be used by the firmware.
 * @return  ::ERR_ETH_API_OK or ::ERR_ETH_API_UNABLE_SET_CHAN_EN in case of error. Calling GetLastApiError() in case of error retrieves a more detailed error code.
 */
ULONG ETH_API SetChannelEnableWord( ULONG nChanEn );

/**
 * \brief Retrieve the board information.
 *
 * Obtain a pointer to the board information string.
 *
 * @note If the board type could not be read from the firmware then the pointer returned points to an empty string. If no FMC card is connected the
 * pointer points to the "No FMC card present" string otherwise points to a string like "FMC108" if FMC108 is being used
 *
 * @return  Pointer to a string otherwise.
 */
PUCHAR ETH_API GetBoardType();

/**
 * \brief Retrieve the carrier board type.
 *
 * Obtain a pointer to the carrier board type string.
 *
 * @return  NULL in case of error or a pointer to a string otherwise. Calling GetLastApiError() in case of error retrieves a more detailed error code.
 */
PUCHAR ETH_API GetCarrierType();

/**
 * \brief Make sure the driver's queue (software) is empty.
 *
 * Empty the software queues in the protocol driver. This function returns as soon the queue is empty.
 * @return  ::ERR_ETH_API_OK or ::ERR_ETH_API_UNABLE_GET_CHAN_EN_INFO in case of error. Calling GetLastApiError() in case of error retrieves a more detailed error code.
 */
int ETH_API EmptyDriverQueue();

/**
 * \brief Retrieve the firmware version type.
 *
 * Obtain a pointer to the firmware version string.
 *
 * @return  NULL in case of error or a pointer to a string otherwise. Calling GetLastApiError() in case of error retrieves a more detailed error code.
 */
PUCHAR ETH_API GetFirmwareVersion();

// error codes
#define ERR_ETH_API_OK						0						/*!< API success. The called function has returned successfully, no error encountered. */
#define ERR_ETH_API_COULD_NOT_OPEN_DEVICE	-98						/*!< Could not open a given device index. */
#define ERR_ETH_API_COULD_NOT_OPEN			-99						/*!< Could not open a connexion to the 4DSP protocol driver. */
#define ERR_ETH_API_COULD_NOT_ENUMERATE		NULL					/*!< Could not enumerate Ethernet devices part of the host system. */
#define ERR_ETH_API_NO_MORE_MEMORY			-100					/*!< Could not allocate memory. */
#define ERR_ETH_API_INVALID_HANDLE_VALUE	INVALID_HANDLE_VALUE	/*!< The function encountered a invalid handle, make sure to call OpenDevice() prior calling this function. */
#define ERR_ETH_API_INVALID_HANDLE_INT			-101				/*!< The function encountered a invalid handle, make sure to call OpenDevice() prior calling this function. */
#define ERR_ETH_API_INVALID_ADDRESS				-102				/*!< The function encountered a invalid address, make sure the address you are trying to read/write is valid(existing). */
#define ERR_ETH_API_NULL_ARG					-103				/*!< The function has encountered a NULL argument where a NULL value is not allowed. */
#define ERR_ETH_API_IO_ERROR					-104				/*!< The function encountered a low level IO (Input/Output) error. */
#define ERR_ETH_API_WRONG_ADDRESS				-105				/*!< The function encountered a invalid address, make sure the address you are trying to read/write is valid(existing). */
#define ERR_ETH_API_NULL_MEMBER					-106				/*!< The function encountered a NULL member. This is an unexpected internal API error. */
#define ERR_ETH_API_NO_SUCH_DEVICE				-107				/*!< The device supposed to be opened is not existing in the enumeration list. */
#define ERR_ETH_API_UNABLE_TO_WRITE				-108				/*!< Unable to write to the firmware register. */
#define ERR_ETH_API_UNABLE_TO_WRITEFILE			-109				/*!< No documentation available for this error. */
#define ERR_ETH_API_UNABLE_TO_READ				-110				/*!< Unable to read from a firmware register. */
#define ERR_ETH_API_UNABLE_TO_READFILE			-111				/*!< No documentation available for this error. */
#define ERR_ETH_API_TIMEOUT_RD_WR				-112				/*!< The function encountered a read (or write) timeout. The data could not be transfered during the timeout interval. */
#define ERR_ETH_API_UNABLE_TO_GET_SRC_MAC		-113				/*!< The function could not retrieve host's MAC address. */
#define ERR_ETH_API_UNABLE_TO_SEND_CMD			-114				/*!< Unable to send a firmware command to the hardware. */
#define ERR_ETH_API_AO_SANITY_SO_NOT_ZERO		-115				/*!< We have missed frame, the first frame(s) are missing in the packet queue */
#define ERR_ETH_API_AO_SANITY_MISSED_FRAME		-116				/*!< We have missed frame, some frame(s) are missing in the packet queue */
#define ERR_ETH_API_UNABLE_TO_WRITE_BLOCK		-117				/*!< Unable to write a block to the firmware memory. */
#define ERR_ETH_API_WRONG_ACK_VALUE				-118				/*!< The ACK value received from the hardware does not match what is expected. Missed packets or incomplete packet received */
#define ERR_ETH_API_UNABLE_TO_RECEIVE_ACK		-119				/*!< The ACK value cannot be received, most likely a transfer error, timeout. */
#define ERR_ETH_API_UNABLE_TO_READ_BLOCK		-120				/*!< Unable to read a block from the firmware memory. */
#define ERR_ETH_API_UNABLE_WRITE_TO_FILE		-121				/*!< No documentation available for this error. */
#define ERR_ETH_API_UNABLE_GET_FREQUENCY		-122				/*!< Could not retrieve a frequency from the frequency tree. */
#define ERR_ETH_API_UNABLE_GET_CHAN_EN_INFO		-123				/*!< Could not retrieve the channel enable word from the firmware. */
#define ERR_ETH_API_UNABLE_SET_CHAN_EN			-124				/*!< Could not write the channel enable word to the firmware. */
#define ERR_ETH_API_UNABLE_GET_BOARD_TYPE		NULL				/*!< Could not obtain board type information from the firmware. */
#define ERR_ETH_API_UNABLE_GET_CARRIER_TYPE		NULL				/*!< Could not obtain carrier type information from the firmware. */
#define ERR_ETH_API_UNABLE_GET_FIRMWARE_VERSION NULL				/*!< Could not obtain firmware version from the firmware. */
#define ERR_ETH_API_NO_FMC						-125				/*!< Did not find any FMC devices */
#define ERR_ETH_API_UNABLE_GET_API_OPTIONS		-126				/*!< Could not retrieve the API options word from the API */
#define ERR_ETH_API_NO_SUCH_FILE				-127				/*!< The file passed as argument is not existing */
#define ERR_ETH_API_UNABLE_EMPTY_DRIVER_QUEUE	-128				/*!< Could not empty the driver queue */

// a few system register offset for the FMC107 firmware and hopefully others
#define BOARDTYPE			0x00									/*!< Firmware register offset containing the board type. */
#define BOARDRESET			0x00									/*!< Firmware register offset containing reset (FMC110). */
#define FWINFOS				0x01									/*!< Firmware register offset containing the firmware informations. Carrier type and such things. */
#define STATUSREGOFFSET		0x02									/*!< Firmware status register offset. */
#define MACLSBOFFSET		0x03									/*!< Firmware MAC address register offset (LSB). */
#define MACMSBOFFSET		0x04									/*!< Firmware MAC address register offset (MSB). */
#define LEDREGOFFSET		0x05									/*!< Firmware LED register offset.  */
#define CMDREGOFFSET		0x10									/*!< Firmware commands register offset.  */
#define CHANENOFFSET		0x11									/*!< Firmware channel enable register offset.  */
#define BURSTSIZEOFFSET		0x12									/*!< Firmware burst size register offset.  */
#define TRIGINTERVALOFFSET	0x13									/*!< Firmware trigger interval register offset.  */
#define FREQSELREGOFFSET	0x60									/*!< Firmware frequency selection register offset.  */
#define FREQREGOFFSET		0x61									/*!< Firmware raw frequency register offset.  */

#endif /* ndef ETHAPI */
