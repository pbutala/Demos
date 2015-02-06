//////////////////////////////////////////////////////////////////////////////////////////////////
///@file tcpip.h
///@author Arnaud Maye (4DSP) 
///\brief wrapper around tcpip layer (header)
///
/// This module use operating's system network API in order to communicate with TCP/IP
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


/*! Holds various layer error codes */
typedef enum {
	TCPIP_OK					= 0,					/*!< No errors */
	TCPIP_NO_CONNECTION			= -1,					/*!< Could not open connection to remote tcpip server */
	TCPIP_CONNECT_WSA_STARTUP	= -2,					/*!< Problem when initializing winsock2 */
	TCPIP_CONNECT_SOCKET		= -3,					/*!< Problem when binding to the socket */
	TCPIP_CONNECT_UNREACHABLE	= -4,					/*!< Problem when connecting the remote server */
	TCPIP_SEND					= -5,					/*!< Problem when sending the data */
	TCPIP_RECEIVE				= -6,					/*!< Problem when receiving the data */	
} TCPIP_ERROR;	



/**
 * Try to open connection to the remote TCP/IP server (Zynq, ML605, VC707, FC6301, FC6603, ...)
 *
 * @param	pIPaddress	Zero terminated string containing the IP address to open a TCP/IP connection to
 * @param	nPort		Port to be used for TCP/IP connection unused argument.
 * @return  One of the error represented by the TCPIP_ERROR enumeration
 */
TCPIP_ERROR OpenConnection(char *pIPaddress, int nPort);


/**
 * Close a connection previously openned.
 */
void CleanConnection();


/**
 * Try to send data to the remote TCP/IP server (Zynq, ML605, VC707, FC6301, FC6603, ...)
 *
 * @param	pData	pointer to the data about to be sent. Note that the buffer should be equal or bigger
 *					then iSizeData.
 * @param	iSizeData		Size of the data to be sent, in bytes.
 * @return  One of the error represented by the TCPIP_ERROR enumeration
 */
TCPIP_ERROR SendData(void *pData, int iSizeData);

/**
 * Try to receive data from the remote TCP/IP server (Zynq, ML605, VC707, FC6301, FC6603, ...)
 *
 * @param	pData	pointer to a previously allocated buffer about to receive the data. Note that the buffer should be equal or bigger
 *					then iSizeData.
 * @param	iSizeData		Size of the data to be sent, in bytes.
 * @return  One of the error represented by the TCPIP_ERROR enumeration
 */
TCPIP_ERROR ReceiveData(void *pData, int iSizeData);