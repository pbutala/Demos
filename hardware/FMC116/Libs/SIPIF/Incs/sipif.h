//////////////////////////////////////////////////////////////////////////////////////////////////
///@file sipif.h
///@author Arnaud Maye (4DSP) 
///\brief sipif module interfaces with various 4DSP communication layers (header)
///
/// This module using generic function to communicate with several layer. This module allows the
/// software not being tied to a layer on another. The available supports are :
/// - 4FM API ( Communicating with PCI/PMC/PMX 4DSP devices as FM489, FM680 and so on ).
/// - ETH API ( Communication with Ethernet devices as FMC116, FMC110, ... ).
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _SIPIF_H_
#define _SIPIF_H_

#ifdef WIN32
 #include "ethapi.h"
#endif

	// type of interface
#define	SIPIF_ETHAPI 0								/*!< sipif_init() communication uses ETHAPI layer. */
#define	SIPIF_4FM 1									/*!< sipif_init() communication uses 4FM API layer. */
#define	SIPIF_TCPIP_V4 2								/*!< sipif_init() communication uses TCP/IP v4. */

// Packet
typedef struct {
	unsigned int data;										//!< Data, used for both read and write opprations
	unsigned int address;											//!< Start address, this can be used to prepare data transfer or simply give address of a register to be written/read
	unsigned int cmd;												//!< The StellarIP command (the commands are defined by sip_cmds)
	unsigned int size;												//!< For data transfers we tell how many bytes are following after this command
} SIP_PKT;


/**
 * A read command, we write a read command and receive a packet pack with the value.
 */
#define STELLAR_OPCODE_READ				2			// Host -> Fw   ( Read a register )
#define STELLAR_OPCODE_WRITE			3			// Host -> Fw   ( Write a register )
#define STELLAR_OPCODE_WRITE_ACK		4			// Fw   -> Host ( Ack a register write )
#define STELLAR_OPCODE_WRITE_DATA		5			// Host -> Fw   ( Host is going to send data )
#define STELLAR_OPCODE_WRITE_DATA_ACK	6			// Fw   -> Host ( Ack a data receiving )	
#define STELLAR_OPCODE_READ_ACK			7			// Fw   -> Host ( Ack a register read )
#define STELLAR_OPCODE_READ_DATA		8			// Host -> Fw   ( Host is asking for data)
#define STELLAR_OPCODE_READ_DATA_ACK	9			// Fw   -> Host ( Ack a data receiving )
#define STELLAR_OPCODE_READ_TO_ACK		10			// Fw   -> Host ( Ack a register read in case of timeout on the sip address range )



/* error codes */
#define SIPIF_ERR_OK					0		/*!< No error encountered during execution. */
#define SIPIF_ERR_UNEXPECTED_LAYER_ID	-1		/*!< sipif_init() does not know the type of interface passed as argument. */
#define SIPIF_ERR_NO_4FM_DEVICE_FOUND	-2		/*!< sipif_init() could not find any 4FM devices. */
#define SIPIF_ERR_NO_ETH_DEVICE_FOUND	-3		/*!< sipif_init() could not find any Ethernet based devices. */
#define SIPIF_ERR_NO_ETH_DRIVER_FOUND	-4		/*!< sipif_init() could not find any Ethernet driver. */
#define SIPIF_ERR_UNABLE_SET_TIMEOUT	-5		/*!< sipif_init() could not setup 4FM timeout into the driver. */
#define SIPIF_ERR_NULL_ARGUMENT			-6		/*!< Unexpected NULL argument received in a function. */
#define SIPIF_ERR_TIMEOUT				-7		/*!< A communication function has timed out. */
#define SIPIF_ERR_NO_INTERFACE_COMPS	-8		/*!< sipif_init() could not dynamically load components(dlls) for the specified interface */
#define SIPIF_ERR_NO_ENUM				-9		/*!< getdeviceenumeration could not obtain enumeration */
#define SIPIF_ERR_NO_TCPIP_DEVICE_FOUND	-10		/*!< sipif_init() could not find any TCPIP based devices. */
#define SIPIF_ERR_WRONG_TCPIP_ACK		-11		/*!< Did not get the expected ack */
#define SIPIF_ERR_WRONG_TCPIP_PKT		-12		/*!< Wrong address in the packet */

// C++ "helper"
#ifdef __cplusplus
extern "C" {
#endif 

/**
 * Initialize a given interface. It also to init 4FM (PCI) and ETH (Ethernet) interfaces for a set of modes.
 *
 * @param	typeif	the type of interface to be used:
 *					- SIPIF_ETHAPI	( interface to the Ethernet API )
 *					- SIPIF_4FM		( interface to the PCI/PMC/XMC API )
 * @param	devtype4FM	unused argument.
 * @param	devidx	the device index in the NDIS system table.
 * @param	timeout the timeout for PCI operations. This argument is ignored by the Ethernet API.
 * @param	M VCXO loop M parameters ( the multiplier ). This parameters is ignored by the Ethernet API. 
 * @param	N VCXO loop N parameters ( the divider ). This parameters is ignored by the Ethernet API.
 * @return  - SIPIF_ERR_OK
 *			- SIPIF_ERR_NO_ETH_DEVICE_FOUND
 *			- SIPIF_ERR_NO_ETH_DRIVER_FOUND
 *			- SIPIF_ERR_NO_4FM_DEVICE_FOUND
 *			- SIPIF_ERR_UNABLE_SET_TIMEOUT
 *			- SIPIF_ERR_UNEXPECTED_LAYER_ID
 */
int sipif_init(unsigned char typeif, const char *devtype4FM, unsigned int devidx, unsigned int timeout, unsigned int M, unsigned int N);

/**
 * Read a system register from the constellation memory address space.
 *
 * @param	addr	address we want to read from
 * @param	value	pointer to a 32 bit variable about to receive value pointed by addr.
 * @return  - SIPIF_ERR_OK
 *			- SIPIF_ERR_TIMEOUT
 *			- SIPIF_ERR_UNEXPECTED_LAYER_ID
 */
int sipif_readsipreg(unsigned int addr, unsigned long *value);

/**
 * Write a system register to the constellation memory address space.
 *
 * @param	addr	address we want to write to
 * @param	value	32 bit value about to be sent in the variable pointed by addr.
 * @return  - SIPIF_ERR_OK
 *			- SIPIF_ERR_TIMEOUT
 *			- SIPIF_ERR_UNEXPECTED_LAYER_ID
 */
int sipif_writesipreg(unsigned int addr, unsigned long value);

/**
 * Read data using DMA transactions in the case of 4FM interface. In the case of an Ethernet device this function
 * read as many EthernetII packets as required to obtain the data.
 *
 * @param	buf	pointer to the buffer receiving data from the firmware
 * @param	size	size of the buffer in bytes.
 * @return  - SIPIF_ERR_OK
 *			- SIPIF_ERR_TIMEOUT
 *			- SIPIF_ERR_UNEXPECTED_LAYER_ID
 */
int sipif_readdata(void *buf, unsigned int size);

/**
 * Obtain the NDIS ethernet device enumeration
 *
 * @param   mode		display the enumeration to the console ( ::API_ENUM_DISPLAY ) or do not display anything ( ::API_ENUM_NODISPLAY ). The second case is meant to only populate the API with the enumeration.
 * @return  - SIPIF_ERR_OK
 *			- SIPIF_ERR_NO_INTERFACE_COMPS
 *			- SIPIF_ERR_NO_ENUM
 */
int sipif_getdeviceenumeration(unsigned long mode);

/**
 * Write data using DMA transactions in the case of 4FM interface. In the case of an Ethernet device this function
 * write as many EthernetII packets as required to send the data.
 *
 * @param	buf	pointer to the buffer containing the data sent to the firmware
 * @param	size	size of the buffer in bytes.
 * @return  - SIPIF_ERR_OK
 *			- SIPIF_ERR_TIMEOUT
 *			- SIPIF_ERR_UNEXPECTED_LAYER_ID
 */
int sipif_writedata(void *buf, unsigned int size);

/**
 * Free the current interface being used.
 *
 * @return  - SIPIF_ERR_OK
 *			- SIPIF_ERR_UNEXPECTED_LAYER_ID
 */
int sipif_free(void);

// C++ "helper"
#ifdef __cplusplus
}
#endif 


#endif //_SIPIF_H_