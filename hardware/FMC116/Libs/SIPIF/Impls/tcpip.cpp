//////////////////////////////////////////////////////////////////////////////////////////////////
///@file tcpip.cpp
///@author Arnaud Maye (4DSP) 
///\brief wrapper around tcpip layer (implementation)
///
/// This module use operating's system network API in order to communicate with TCP/IP
//////////////////////////////////////////////////////////////////////////////////////////////////
#include <winsock2.h>
#include <stdio.h>
#include "tcpip.h"

int g_iLastSocketError = TCPIP_OK;		/*!< Holds the last socket error (ie WSA error codes) */
SOCKET g_pSocket = NULL;				/*!< Holds a pointer to the socket */

TCPIP_ERROR OpenConnection(char *pIPaddress, int iPort)
{
  // Initialize Winsock
  WSADATA wsaData;
  int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if(iResult!=NO_ERROR)
	  return TCPIP_CONNECT_WSA_STARTUP;
 
  // Create a SOCKET for connecting to server
  g_pSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(g_pSocket==INVALID_SOCKET) {
	  CleanConnection();
	  return TCPIP_CONNECT_SOCKET;
  }

  // The sockaddr_in structure specifies the address family,
  // IP address, and port of the server to be connected to.
  sockaddr_in clientService;
  clientService.sin_family = AF_INET;
  clientService.sin_addr.s_addr = inet_addr(pIPaddress);
  clientService.sin_port = htons(iPort);
 
  // Connect to server
  if(connect(g_pSocket, (SOCKADDR*)&clientService, sizeof(clientService))==SOCKET_ERROR) {
	  CleanConnection();
	  return TCPIP_CONNECT_UNREACHABLE;
  }

  // We are done
  return TCPIP_OK;
}


void CleanConnection()
{
	g_iLastSocketError = WSAGetLastError();
	if(g_pSocket!=NULL)
		closesocket(g_pSocket);
	WSACleanup();
}


TCPIP_ERROR SendData(void *pData, int iSizeData)
{
	int iResult;

	// Send the data
	iResult = send(g_pSocket, (const char *)pData, iSizeData, 0 );
	if(iResult!=iSizeData) {
		printf("Oups\r\n");
	}
	if(iResult==0) {
		printf("Oups2\r\n");
	}
	if(iResult==SOCKET_ERROR) {
		CleanConnection();
		printf("SEND:%x\r\n", WSAGetLastError());
		return TCPIP_SEND;
	}

	// We are done
	return TCPIP_OK;
}

TCPIP_ERROR ReceiveData(void *pData, int iSizeData)
{
	int iResult = 0;

	// Send the data
	iResult = recv(g_pSocket, (char *)pData, iSizeData, MSG_WAITALL );
	if(iResult!=iSizeData) {
		printf("Oups\r\n");
	}
	if(iResult==0) {
		return TCPIP_RECEIVE;
	}
	if(iResult==SOCKET_ERROR) {
		CleanConnection();
		printf("RECV:%x\r\n", WSAGetLastError());
		return TCPIP_RECEIVE;
	}

	// We are done
	return TCPIP_OK;
}