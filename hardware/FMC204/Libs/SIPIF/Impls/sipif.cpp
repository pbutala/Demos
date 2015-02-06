//////////////////////////////////////////////////////////////////////////////////////////////////
///@file sipif.cpp
///@author Arnaud Maye (4DSP) 
///\brief sipif module interfaces with various 4DSP communication layers (implementation)
///
/// This module using generic function to communicate with several layer. This module allows the
/// software not being tied to a layer on another. The available supports are :
/// - 4FM API ( Communicating with PCI/PMC/PMX 4DSP devices as FM489, FM680 and so on ).
/// - ETH API ( Communication with Ethernet devices as FMC10x, FMC110, ... ).
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __linux__
 #define _XOPEN_SOURCE 600
 #include <unistd.h>
 #include <sys/time.h>
#endif 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <4FM.h>
#include "sipif.h"

#ifdef WIN32
#else
 static void Sleep(unsigned long timems)
 {
   usleep(timems*1000);
 }
#endif

_4FM_DeviceContext g_hDev;			/*!< The 4FM API handle */
unsigned int g_timeout;				/*!< The timeout value */
unsigned int g_typeif;				/*!< The currently selected interface */


// Function pointers for 4FM.dll. They receive pointer of the various function required by this module. We actually want to load
// each interface DLL dynamically
_4FM_error_t (_4FM_CALL *g_p4FM_OpenDeviceEx)(_4FM_DeviceContext *ctx, const char *type, 
											int *devno, enum _4FM_OpenModes mode) = NULL;			/*!< Pointer to 4FM_OpenDeviceEx() */
_4FM_error_t (_4FM_CALL *g_p4FM_ResetDevice)(_4FM_DeviceContext *ctx) = NULL;						/*!< Pointer to 4FM_ResetDevice() */
_4FM_error_t (_4FM_CALL *g_p4FM_SetClockSynth)(_4FM_DeviceContext *ctx,
											   unsigned M, unsigned N) = NULL;						/*!< Pointer to _4FM_SetClockSynth() */
_4FM_error_t (_4FM_CALL *g_p4FM_SelectTarget)(_4FM_DeviceContext *ctx, 
											  unsigned long target) = NULL;							/*!< Pointer to _4FM_SelectTarget() */
_4FM_error_t (_4FM_CALL *g_p4FM_CloseDevice)(_4FM_DeviceContext *ctx) = NULL;						/*!< Pointer to _4FM_CloseDevice() */
_4FM_error_t (_4FM_CALL *g_p4FM_SetTransferTimeout)(_4FM_DeviceContext *ctx,	
													unsigned int time) = NULL;						/*!< Pointer to _4FM_SetTransferTimeout() */
_4FM_error_t (_4FM_CALL *g_p4FM_SetTimeoutOnce)(_4FM_DeviceContext *ctx, 
													unsigned int time) = NULL;						/*!< Pointer to _4FM_SetTimeoutOnce() */
_4FM_error_t (_4FM_CALL *g_p4FM_Read)(_4FM_DeviceContext *ctx, unsigned long addr, 
									  unsigned long *value, unsigned long timeout) = NULL;			/*!< Pointer to _4FM_Read() */
_4FM_error_t (_4FM_CALL *g_p4FM_Write)(_4FM_DeviceContext *ctx, unsigned long addr, 
									  unsigned long value, unsigned long ack) = NULL;				/*!< Pointer to _4FM_Write() */
_4FM_error_t (_4FM_CALL *g_p4FM_ReceiveData)(_4FM_DeviceContext *ctx, 
											 void *buffer, unsigned long count) = NULL;				/*!< Pointer to _4FM_ReceiveData() */
_4FM_error_t (_4FM_CALL *g_p4FM_SendData)(_4FM_DeviceContext *ctx, 
											 const void *buffer, unsigned long count) = NULL;				/*!< Pointer to _4FM_SendData() */


#ifdef WIN32
// Function pointers for ethapi.dll. They receive pointer of the various function required by this module. We actually want to load
// each interface DLL dynamically.
VOID (ETH_API  *g_pETH_ClosePort)(void) = NULL;														/*!< Pointer to ClosePort() */
ULONG (ETH_API *g_pETH_OpenDevice)(ULONG devIdx) = NULL;											/*!< Pointer to OpenDevice() */
PDEVNUM (ETH_API *g_pETH_GetDevicesEnumeration)(ULONG bDisplayToConsole) = NULL;					/*!< Pointer to GetDevicesEnumeration() */
ULONG (ETH_API *g_pETH_ReadSystemRegister)(ULONG address, PULONG data) = NULL;						/*!< Pointer to ReadSystemRegister() */
ULONG (ETH_API *g_pETH_WriteSystemRegister)(ULONG address, ULONG data) = NULL;						/*!< Pointer to WriteSystemRegister() */

ULONG (ETH_API *g_pETH_WriteBlock)(PUCHAR pOutBuffer, ULONG nSizeBuffer, 
								   UCHAR nChanNumber) = NULL;										/*!< Pointer to WriteBlock() */
ULONG (ETH_API *g_pETH_ReadBlock)(PUCHAR pInBuffer, ULONG nSizeBuffer, 
								   UCHAR nChanNumber) = NULL;										/*!< Pointer to ReadBlock() */
#endif

#ifdef WIN32
 HMODULE g_hDll = NULL;	/*!< Global handle to dll loaded using load_4fmptrs() or load_ethptrs(). */
#endif 
int load_4fmptrs(void)
{
#ifdef WIN32
	// first of all try to load the library. We do not provide a path here as the folder containing this DLL is supposed to be referenced
	// in the PATH windows environment variable.
	g_hDll = LoadLibrary("4FM.dll");
	if(g_hDll==NULL) {
		return -1;
	}

	// get 4FM_OpenDeviceEx() offset
	g_p4FM_OpenDeviceEx = (_4FM_error_t (__cdecl *)(_4FM_DeviceContext *,const char *,
		int *,_4FM_OpenModes))GetProcAddress(g_hDll, "_4FM_OpenDeviceEx");
	if(g_p4FM_OpenDeviceEx==NULL) {
		return -2;
	}

	// get 4FM_ResetDevice() offset
	g_p4FM_ResetDevice = (_4FM_error_t (__cdecl *)(_4FM_DeviceContext *))GetProcAddress(g_hDll, "_4FM_ResetDevice");
	if(g_p4FM_ResetDevice==NULL) {
		return -3;
	}

	// get 4FM_SetClockSynth() offset
	g_p4FM_SetClockSynth = (_4FM_error_t (__cdecl *)(_4FM_DeviceContext *,
		unsigned int,unsigned int))GetProcAddress(g_hDll, "_4FM_SetClockSynth");
	if(g_p4FM_SetClockSynth==NULL) {
		return -4;
	}


	// get 4FM_SelectTarget() offset
	g_p4FM_SelectTarget = (_4FM_error_t (__cdecl *)(_4FM_DeviceContext *,
		unsigned long))GetProcAddress(g_hDll, "_4FM_SelectTarget");
	if(g_p4FM_SelectTarget==NULL) {
		return -5;
	}


	// get 4FM_CloseDevice() offset
	g_p4FM_CloseDevice = (_4FM_error_t (__cdecl *)(_4FM_DeviceContext *))GetProcAddress(g_hDll, "_4FM_CloseDevice");
	if(g_p4FM_CloseDevice==NULL) {
		return -6;
	}


	// get 4FM_SetTransferTimeout() offset
	g_p4FM_SetTransferTimeout = (_4FM_error_t (__cdecl *)(_4FM_DeviceContext *,
		unsigned int))GetProcAddress(g_hDll, "_4FM_SetTransferTimeout");
	if(g_p4FM_SetTransferTimeout==NULL) {
		return -7;
	}

	// get _4FM_SetTimeoutOnce() offset
	g_p4FM_SetTimeoutOnce = (_4FM_error_t (__cdecl *)(_4FM_DeviceContext *,
		unsigned int))GetProcAddress(g_hDll, "_4FM_SetTimeoutOnce");
	if(g_p4FM_SetTimeoutOnce==NULL) {
		return -8;
	}

	// get _4FM_Read() offset
	g_p4FM_Read = (_4FM_error_t (__cdecl *)(_4FM_DeviceContext *,unsigned long,
		unsigned long *,unsigned long))GetProcAddress(g_hDll, "_4FM_Read");
	if(g_p4FM_Read==NULL) {
		return -9;
	}

	// get _4FM_Write() offset
	g_p4FM_Write = (_4FM_error_t (__cdecl *)(_4FM_DeviceContext *,unsigned long,
		unsigned long,unsigned long))GetProcAddress(g_hDll, "_4FM_Write");
	if(g_p4FM_Write==NULL) {
		return -10;
	}

	// get _4FM_ReceiveData() offset
	g_p4FM_ReceiveData = (_4FM_error_t (__cdecl *)(_4FM_DeviceContext *,
		void *,unsigned long))GetProcAddress(g_hDll, "_4FM_ReceiveData");
	if(g_p4FM_ReceiveData==NULL) {
		return -11;
	}

	// get _4FM_SendData() offset
	g_p4FM_SendData = (_4FM_error_t (__cdecl *)(_4FM_DeviceContext *, const void *,unsigned long))
		GetProcAddress(g_hDll, "_4FM_SendData");
	if(g_p4FM_SendData==NULL) {
		return -12;
	}
#else
	// get 4FM_OpenDeviceEx() offset
	g_p4FM_OpenDeviceEx = _4FM_OpenDeviceEx;
	if(g_p4FM_OpenDeviceEx==NULL) {
		return -2;
	}

	// get 4FM_ResetDevice() offset
	g_p4FM_ResetDevice = _4FM_ResetDevice;
	if(g_p4FM_ResetDevice==NULL) {
		return -3;
	}

	// get 4FM_SetClockSynth() offset
	g_p4FM_SetClockSynth = _4FM_SetClockSynth;
	if(g_p4FM_SetClockSynth==NULL) {
		return -4;
	}


	// get 4FM_SelectTarget() offset
	g_p4FM_SelectTarget = _4FM_SelectTarget;
	if(g_p4FM_SelectTarget==NULL) {
		return -5;
	}


	// get 4FM_CloseDevice() offset
	g_p4FM_CloseDevice = _4FM_CloseDevice;
	if(g_p4FM_CloseDevice==NULL) {
		return -6;
	}


	// get 4FM_SetTransferTimeout() offset
	g_p4FM_SetTransferTimeout = _4FM_SetTransferTimeout;
	if(g_p4FM_SetTransferTimeout==NULL) {
		return -7;
	}

	// get _4FM_SetTimeoutOnce() offset
	g_p4FM_SetTimeoutOnce = _4FM_SetTimeoutOnce;
	if(g_p4FM_SetTimeoutOnce==NULL) {
		return -8;
	}

	// get _4FM_Read() offset
	g_p4FM_Read = _4FM_Read;
	if(g_p4FM_Read==NULL) {
		return -9;
	}

	// get _4FM_Write() offset
	g_p4FM_Write = _4FM_Write;
	if(g_p4FM_Write==NULL) {
		return -10;
	}

	// get _4FM_ReceiveData() offset
	g_p4FM_ReceiveData = _4FM_ReceiveData;
	if(g_p4FM_ReceiveData==NULL) {
		return -11;
	}

	// get _4FM_SendData() offset
	g_p4FM_SendData = _4FM_SendData;
	if(g_p4FM_SendData==NULL) {
		return -12;
	}
#endif

	return 0;
}

int load_ethptrs(void)
{
#ifdef WIN32
	// first of all try to load the library. We do not provide a path here as the folder containing this DLL is supposed to be referenced
	// in the PATH windows environment variable.
	g_hDll = LoadLibrary("ethapi.dll");
	if(g_hDll==NULL) {
		return -1;
	}

	// get ClosePort() offset
	g_pETH_ClosePort = (void (__cdecl *)(void))GetProcAddress(g_hDll, "ClosePort");
	if(g_pETH_ClosePort==NULL) {
		return -2;
	}

	// get OpenDevice() offset
	g_pETH_OpenDevice = (ULONG (__cdecl *)(ULONG))GetProcAddress(g_hDll, "OpenDevice");
	if(g_pETH_OpenDevice==NULL) {
		return -3;
	}

	// get GetDevicesEnumeration() offset
	g_pETH_GetDevicesEnumeration = (PDEVNUM (__cdecl *)(ULONG))GetProcAddress(g_hDll, "GetDevicesEnumeration");
	if(g_pETH_GetDevicesEnumeration==NULL) {
		return -4;
	}


	// get ReadSystemRegister() offset
	g_pETH_ReadSystemRegister = (ULONG (__cdecl *)(ULONG,PULONG))GetProcAddress(g_hDll, "ReadSystemRegister");
	if(g_pETH_ReadSystemRegister==NULL) {
		return -5;
	}


	// get WriteSystemRegister() offset
	g_pETH_WriteSystemRegister = (ULONG (__cdecl *)(ULONG,ULONG))GetProcAddress(g_hDll, "WriteSystemRegister");
	if(g_pETH_WriteSystemRegister==NULL) {
		return -6;
	}


	// get WriteBlock() offset
	g_pETH_WriteBlock = (ULONG (__cdecl *)(PUCHAR,ULONG,UCHAR))GetProcAddress(g_hDll, "WriteBlock");
	if(g_pETH_WriteBlock==NULL) {
		return -7;
	}

	// get ReadBlock() offset
	g_pETH_ReadBlock = (ULONG (__cdecl *)(PUCHAR,ULONG,UCHAR))GetProcAddress(g_hDll, "ReadBlock");
	if(g_pETH_ReadBlock==NULL) {
		return -8;
	}

	return 0;
#endif
}

_4FM_error_t _4FM_CALL Initialize4FMHardware(const char *devType, int devNum, int M, int N)
{
	_4FM_error_t rc;
	int nDevNum = devNum;

	// open the first device found in the system
	rc = g_p4FM_OpenDeviceEx(&g_hDev, devType, &nDevNum, OPEN_MODE_COMPATIBILITY);
	if(rc != _4FM_OK) {
		printf("Could not find a 4DSP PMC device, exiting\n");
		return rc;
	}

	// reset the device
	rc = g_p4FM_ResetDevice(&g_hDev);
	if(rc != _4FM_OK) {
		printf("Could not reset device '%s', exiting (device has just been closed)\n", devType);
		return rc;
	}

	// extra sleep
	Sleep(100);

	// Set the synthesizer frequency
	rc = g_p4FM_SetClockSynth(&g_hDev, M, N);
	if(rc != _4FM_OK) {
		printf("Could not set synthesizer frequency on device '%s', exiting (M was %d and N was %d)\n", devType, M, N);
		return rc;
	}

	// reset the device
	rc = g_p4FM_ResetDevice(&g_hDev);
	if(rc != _4FM_OK) {
		printf("Could not reset device '%s', exiting (device has just been closed)\n", devType);
		return rc;
	}

	// extra sleep
	Sleep(100);

	// Set the DMA target to main FGPA
	rc = g_p4FM_SelectTarget(&g_hDev, tgMainFPGA);
	if(rc != _4FM_OK) {
		printf("Could not set DMA target on '%s'\n", devType);
		return rc;
	}

	//default return is _4FM_OK
	return rc;
}

/**
 * Loads a specified interface components. It also assigns interface offsets into global pointers using load4fmptrs() or loadethptrs()..
 *
 * @param	typeif	the type of interface to be used:
 *					- SIPIF_ETHAPI	( interface to the Ethernet API )
 *					- SIPIF_4FM		( interface to the PCI/PMC/XMC API )
 * @return  - 0 ( success )
 *			- a negative value indicating which function could not be found in the interface
 */
int load_interfacedll(unsigned char typeif)
{
	switch(typeif)
	{
	case SIPIF_ETHAPI: 
		return load_ethptrs();		
		break;

	case SIPIF_4FM:
		return load_4fmptrs();
		break;

	default:
		return SIPIF_ERR_UNEXPECTED_LAYER_ID;
	}
}


int sipif_init( unsigned char typeif, const char *devtype4FM, unsigned int devidx, unsigned int timeout, unsigned int M, unsigned int N)
{
	_4FM_error_t rc;

	// something common for all our interfaces
	g_timeout = timeout;
	g_typeif = typeif;

	// try to load the desired interface
	if(load_interfacedll(g_typeif)!=0) {
		return SIPIF_ERR_NO_INTERFACE_COMPS;
	}

	// make sure the 4FM handle is in a predifined state
#ifdef WIN32
	g_hDev.hDev = INVALID_HANDLE_VALUE;
#endif
	switch(g_typeif)
	{
	case SIPIF_ETHAPI: {
#ifdef WIN32
		if(g_pETH_GetDevicesEnumeration(API_ENUM_DISPLAY)==NULL) {
			g_pETH_ClosePort();	
			return SIPIF_ERR_NO_ETH_DRIVER_FOUND;
		}
		if(g_pETH_OpenDevice(devidx) == ERR_ETH_API_COULD_NOT_OPEN_DEVICE)	{
			g_pETH_ClosePort();
			return SIPIF_ERR_NO_ETH_DEVICE_FOUND;
		}		
		break;
#else
		return SIPIF_ERR_UNEXPECTED_LAYER_ID;
#endif
	}
	case SIPIF_4FM: {
		rc = Initialize4FMHardware(devtype4FM, devidx, M, N);
		if(rc!=_4FM_OK) {
			g_p4FM_CloseDevice(&g_hDev);
			return SIPIF_ERR_NO_4FM_DEVICE_FOUND;
		}

		rc = g_p4FM_SetTransferTimeout(&g_hDev, g_timeout);
		if(rc!=_4FM_OK) {
			g_p4FM_CloseDevice(&g_hDev);
			return SIPIF_ERR_UNABLE_SET_TIMEOUT;
		}

		rc = g_p4FM_SetTimeoutOnce(&g_hDev, g_timeout);
		if(rc!=_4FM_OK) {
			g_p4FM_CloseDevice(&g_hDev);
			return SIPIF_ERR_UNABLE_SET_TIMEOUT;
		}
		break;
	}
	default:
		return SIPIF_ERR_UNEXPECTED_LAYER_ID;
	}

	return SIPIF_ERR_OK;

}


int sipif_readsipreg(unsigned int addr, unsigned long *value)
{
	int rceth;
	_4FM_error_t rc4fm;

	// check if arguments are valid
	if(value==NULL) {
		return SIPIF_ERR_NULL_ARGUMENT;
	}

	switch(g_typeif)
	{
	case SIPIF_ETHAPI: {
#ifdef WIN32
		rceth = g_pETH_ReadSystemRegister(addr, value);
		if(rceth!=SIPIF_ERR_OK)
			return SIPIF_ERR_TIMEOUT;
		break;
#else
		return SIPIF_ERR_UNEXPECTED_LAYER_ID;
#endif
	}
	case SIPIF_4FM: {
		rc4fm = g_p4FM_Read(&g_hDev, addr, value, g_timeout);
		if(rc4fm!=_4FM_OK)	
			return SIPIF_ERR_TIMEOUT;
		break;
	}
	default:
		return SIPIF_ERR_UNEXPECTED_LAYER_ID;
	}

	return SIPIF_ERR_OK;

}
int sipif_writesipreg(unsigned int addr, unsigned long value)
{
	int rceth;
	_4FM_error_t rc4fm;

	switch(g_typeif)
	{
	case SIPIF_ETHAPI: {
#ifdef WIN32
		rceth = g_pETH_WriteSystemRegister(addr, value);
		if(rceth!=SIPIF_ERR_OK)
			return SIPIF_ERR_TIMEOUT;
		break;
#else
		return SIPIF_ERR_UNEXPECTED_LAYER_ID;
#endif
	}
	case SIPIF_4FM: {
		rc4fm = g_p4FM_Write(&g_hDev, addr, value, 0);
		if(rc4fm!=_4FM_OK)
			return SIPIF_ERR_TIMEOUT;
		break;
	}
	default:
		return SIPIF_ERR_UNEXPECTED_LAYER_ID;
	}
#ifdef __linux__
	Sleep(2);
#endif
	return SIPIF_ERR_OK;
}

int sipif_getdeviceenumeration(unsigned long mode)
{
#ifdef WIN32	
	PDEVNUM devenum;
#endif
	switch(g_typeif)
	{
	case SIPIF_ETHAPI: {
#ifdef WIN32
		// this is also called by sipif_init but we need to have the device enumeration before the
		// actually calling sipif_init
		if(load_interfacedll(g_typeif)!=0) {
			return SIPIF_ERR_NO_INTERFACE_COMPS;
		}
		devenum = g_pETH_GetDevicesEnumeration((ULONG)mode);
		if(devenum==NULL)
			return SIPIF_ERR_NO_ENUM;
		break;
#else
		return SIPIF_ERR_UNEXPECTED_LAYER_ID;
#endif
	}
	default:
		return SIPIF_ERR_UNEXPECTED_LAYER_ID;
	}

	return SIPIF_ERR_OK;

}

int sipif_readdata(void *buf, unsigned int size)
{
	int rceth;
	_4FM_error_t rc4fm;

	switch(g_typeif)
	{
	case SIPIF_ETHAPI: {
#ifdef WIN32
		rceth = g_pETH_ReadBlock((PUCHAR)buf, size, 0);
		if(rceth!=SIPIF_ERR_OK)
			return SIPIF_ERR_TIMEOUT;
		break;
#else
		return SIPIF_ERR_UNEXPECTED_LAYER_ID;
#endif
	}
	case SIPIF_4FM: {
		rc4fm = g_p4FM_ReceiveData(&g_hDev, buf, size);
		if(rc4fm!=_4FM_OK)
			return SIPIF_ERR_TIMEOUT;
		break;
	}
	default:
		return SIPIF_ERR_UNEXPECTED_LAYER_ID;
	}

	return SIPIF_ERR_OK;

}
int sipif_writedata(void *buf, unsigned int size)
{
	int rceth;
	_4FM_error_t rc4fm;

	switch(g_typeif)
	{
	case SIPIF_ETHAPI: {
#ifdef WIN32
		rceth = g_pETH_WriteBlock((PUCHAR)buf, size, 0);
		if(rceth!=SIPIF_ERR_OK)
			return SIPIF_ERR_TIMEOUT;
		break;
#else
		return SIPIF_ERR_UNEXPECTED_LAYER_ID;
#endif
					   }
	case SIPIF_4FM: {
		rc4fm = g_p4FM_SendData(&g_hDev, buf, size);
		if(rc4fm!=_4FM_OK)
			return SIPIF_ERR_TIMEOUT;
		break;
					}
	default:
		return SIPIF_ERR_UNEXPECTED_LAYER_ID;
	}

	return SIPIF_ERR_OK;

}

int sipif_free()
{
	switch(g_typeif)
	{
	case SIPIF_ETHAPI: 
#ifdef WIN32
		g_pETH_ClosePort();			
		break;
#else
		return SIPIF_ERR_UNEXPECTED_LAYER_ID;
#endif	
	case SIPIF_4FM: 
#ifdef WIN32
		if(g_hDev.hDev!=INVALID_HANDLE_VALUE)
#endif
			g_p4FM_CloseDevice(&g_hDev);
		break;
	default:
		return SIPIF_ERR_UNEXPECTED_LAYER_ID;
	}

	// free the dynamically loaded dll
#ifdef WIN32
	if(g_hDll!=NULL)
		FreeLibrary(g_hDll);
#endif

	return SIPIF_ERR_OK;
}
