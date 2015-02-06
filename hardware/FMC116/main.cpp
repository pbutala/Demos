/**
@file main.cpp
@author Arnaud Maye, 4DSP
@brief FMC116 reference application
*************************************************************************/


// system includes
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//PD ADD
#include <Shlwapi.h>

// project includes
#include "sipif.h"

#include "cid.h"
#include "sxdxrouter.h"
#include "i2cmaster.h"
#include "FMC116.h"
#include "ctgen.h"
#include "FMC116_IF.h"

// PB added to create Winsock server
// END

#define CID_ML605_FMC116			198				/*!< firmware(constellation) ID for FMC116 */
#define CID_ML605_FMC112			223				/*!< firmware(constellation) ID for FMC112 */
#define CID_KC705_FMC112			228				/*!< firmware(constellation) ID for FMC112 */
#define CID_VC707_FMC112			249             /*!< firmware(constellation) ID for FMC112 */
#define CID_KC705_FMC116			225				/*!< firmware(constellation) ID for FMC116 */
#define CID_KC705_FMC116_PCIE		270				/*!< firmware(constellation) ID for FMC116 */
#define CID_VC707_FMC116			248				/*!< firmware(constellation) ID for FMC116 */
#define CID_VP680_FMC116			239				/*!< firmware(constellation) ID for FMC116 */
#define CID_ZC702_FMC112			326				/*!< firmware(constellation) ID for FMC116 */

#define FMC116_ID					0x6C			/*!< FMC116 star ID as per the firmware source code */
#define FMC112_ID					0x80			/*!< FMC112 star ID as per the firmware source code */

#define ROUTER_S16D1_ID				0x6D			/*!< router star ID as per the firmware source code ( used by FMC116 firmware ) */
#define FIFO64K_ID					0x3D			/*!< 64kB FIFO star ID as per the firmware source code */
#define I2C_MASTER_ID				0x05			/*!< I2C master star ID as per the firmware source code */
#define CT_GEN_ID					0x43			/*!< Clock-trigger generation as per the firmware source code */

#define SYNTH_M						250				/*!< Reference value for M on the synthesizer frequency (f = M/N) */
#define SYNTH_N						2				/*!< Reference value for N on the synthesizer frequency (f = M/N) */
#define TIMEOUTDMA					2000			/*!< Timeout value is 2000 ms. */
#define ASCII						0				/*!< Save16BitArrayToFile() saves the samples as ASCII */
#define BINARY						1				/*!< Save16BitArrayToFile() saves the samples as binary */

#define CUR_INTERFACE				(SIPIF_ETHAPI)		/*!< The interface in use for this project */
#define BUFFER_SIZE					1024			/*in number of BYTES */

// Save a buffer to a file.
#ifndef Save16BitArrayToFile
/**
 *  Save a 16 bit sample array to file.
 *
 *  @param buf	pointer to a buffer about to receive the waveform data. This point to a previously allocated memory as big as
 *					bufsize*2 ( byte size ) or bufsize*1 ( sample size ).
 *  @param bufsize	number of samples to be written on the buffer where one sample is as big as 2 bytes.
 *  @param filename	pointer to a string representing the filename/path
 *  @param mode	decide if the function writes in ASCII or binary representation:
 *				- BINARY
 *				- ASCII
 *  @return
 *						- -1, -2 ( Unexpected NULL argument )
 *						- 0 ( Success )
 */
static ULONG Save16BitArrayToFile(void *buf, int bufsize, const char *filename, int mode)
{
	int i;
	FILE *fOutFile;
	char sOpenMode[55];


	// these pointers cannot be NULL
	if(!buf) {
		return -1;
	}

	if(!filename) {
		return -2;
	}

	// cast our stamp less pointer to a short
	short *buf16 = (short *)buf;

	// open the file given as argument
	if(mode==ASCII)
		sprintf(sOpenMode, "a");
	else
		sprintf(sOpenMode, "ab");

	fOutFile = fopen(filename, sOpenMode);
	if(fOutFile==NULL) {
		return -3;
	}

	// write to file either as ASCII or BINARY
	if(mode == BINARY)
		fwrite(buf, 2, bufsize, fOutFile);
	else // -> ASCII
	{
		// Here we don't take risk. We pass a short casted as an int,
		// and we use the normalized int -> short format converter (%hi)
		for(i = 0; i < bufsize; i++)
			fprintf(fOutFile, "%hi\n", (int)buf16[i]);
	}

	fclose(fOutFile);

	return 0;
}
#endif
/**
 *  \brief FMC116 Reference application (main).
 *
 *  This function demonstrates how to configure both digital to analog peripherals, upload waveforms to digital
 *  to analog convert chips, display FMC116 diagnostic, display FMC116 clock tree and grab data from the ADC 0
 *  and ADC 1.
 *
 *  Description of the software sequence :
 *	- Check and convert arguments passed to the application.
 *	- Open a ML605+FMC116 over ethernet using OpenDevice().
 *	- Read the constellation information from the ML605+FMC116 using cid_init().
 *	- Compute start offset of all FMC116 peripheral in the main constallation address space using cid_getstaroffset().
 *	- Configure the data routers with some defautl settings using sxdx_configurerouter().
 *	- Display FMC116 diagnostics using FMC116_getdiagnostics().
 *	- Init all the FMC116 peripherals using FMC116_init().
 *	- Display all the freqencies part of the frequency tree using FMC116_freqcnt_getfrequency().
 *	- Configure burst size using FMC116_ctrl_configure_burst().
 *	- Grab {n} times a burst from ADC{n} using 	sxdx_configurerouter(), FMC116_ctrl_enable_channel(), FMC116_ctrl_arm(), FMC116_ctrl_sw_trigger() and Save16BitArrayToFile().
 *
 *  @param argc the command line
 *  @param argv the number of options in the command line.
 *  @return 0 ( success ) or any other error code.
 */
int main(int argc, char* argv[])
{
	int devIdx;
	int modeClock;
	int ifType;
	const char *devType;

	const char *deviceFW;
	int FMCConstID = 0;
	int FMCnbrch = 16; 
	int modeML605 = 0;
	int modeKC705 = 0;
	int modeVC707 = 0;
    int modeZC702 = 0;

	unsigned int routerID;
	//argc = 5;
	//char rcvBuf[BUFFER_SIZE];
	// Parse the application arguments
	if(argc!=5) {
		printf("Usage: FMC116App.exe {interface type} {device type} {device index} {clock mode}\n\n");
		printf(" {interface type} can be either 0 (PCI) or 1 (Ethernet) or 2 (TCPIP)\n");
		printf(" {device type} is a string defining the target hardware (VP680, ML605, KC705, VC707 ...)\n");
		printf(" {device type} is an ip address when using TCPIP interface\n");
		printf(" {device index} is a PCI index or an Ethernet interface index or a TCPIP port when using TCPIP interface\n");
		printf(" {clock mode}\n");
		printf("	0  Internal Clock with Interal Reference\n");
		printf("	1  External Clock\n");
		printf("	2  Internal Clock with Exteral Reference\n");
		printf("\n");
		printf("\n");
		printf(" List of NDIS interfaces found in the system {device index}:\n");
		printf(" -----------------------------------------------------------\n");
		if(sipif_getdeviceenumeration(API_ENUM_DISPLAY)!=SIPIF_ERR_OK) {
			printf("Could not obtain NDIS(Ethernet) device enumeration...\n Check if the 4dspnet driver installed or if the service started?\n");
			printf("You can discard this error if you do not have any Ethernet based product in use.");
		}
		sipif_free();
		return -1;
	} else {
		// Convert arguments
		ifType = atoi(argv[1]);
		devType = (const char *)argv[2];
		devIdx = atoi(argv[3]);
		modeClock = atoi(argv[4]);
		/*ifType = 1;
		devType = "ML605";
		devIdx = 1;
		modeClock = 0;*/

		// translate interface type to the sipif values
		if(ifType==0)
			ifType = SIPIF_4FM;
		else if(ifType==1)
			ifType = SIPIF_ETHAPI;	
		else
			ifType = SIPIF_TCPIP_V4;	
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Open one of the device from a given device ID argument
	if(sipif_init(ifType, devType, devIdx, TIMEOUTDMA, SYNTH_M, SYNTH_N) != SIPIF_ERR_OK) {
		printf("Could not open device %d\n", devIdx);
		sipif_free();
		return -2;
	}


	printf("Start of program\n");
	printf("--------------------------------------\n");


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Obtain and display the sip_cid informations to the console. This function also check that the constellation ID
	// obtained by the firmware match the value passed as argument and this is why we pass 0 as we do not want the check
	// to happen here
	int rc  = cid_init(0);
	if(rc<1) {
		printf("Could not obtain sipcid table (error %x), exiting\n", rc);
		sipif_free();
		return -3;
	}
	printf("Constellation ID : %d\n", cid_getconstellationid());
	printf("Number of Stars  : %d\n", cid_getnbrstar());
	printf("Software Build   : 0x%8.8X\n", cid_getswbuildcode());
	printf("Firmware Build   : 0x%8.8X\n", cid_getfwbuildcode());
	printf("Firmware Version : %d.%d\n", cid_getfirmwareversion()>>16, cid_getfirmwareversion()&0xFFFF);
	printf("--------------------------------------\n");
	printf("\n");

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Display which FMC116 card is supported by the firmware and set up a few variable for a given constellation IDs
	switch(cid_getconstellationid())
	{
	case CID_ML605_FMC116:
		deviceFW	= "FMC116 on ML605\n";
		FMCConstID	= FMC116_ID;
		FMCnbrch	= 16;
		routerID	= ROUTER_S16D1_ID;
		modeML605	= 1;
		break;
		
	case CID_KC705_FMC116_PCIE:
		deviceFW	= "FMC116 PCIe on KC705\n";
		FMCConstID	= FMC116_ID;
		FMCnbrch	= 16;
		routerID	= ROUTER_S16D1_ID;
		modeKC705	= 0;
		break;
	case CID_KC705_FMC116:
		deviceFW	= "FMC116 on KC705\n";
		FMCConstID	= FMC116_ID;
		FMCnbrch	= 16;
		routerID	= ROUTER_S16D1_ID;
		modeKC705	= 1;
		break;
	case CID_VC707_FMC116:
		deviceFW	= "FMC116 on VC707\n";
		FMCConstID	= FMC116_ID;
		FMCnbrch	= 16;
		routerID	= ROUTER_S16D1_ID;
		modeVC707	= 1;
		break;
	case CID_ML605_FMC112:
		deviceFW	= "FMC112 on ML605\n";
		FMCConstID	= FMC112_ID;
		FMCnbrch	= 12;
		routerID	= ROUTER_S16D1_ID;
		modeML605	= 1;
		break;
	case CID_KC705_FMC112:
		deviceFW	= "FMC112 on KC705\n";
		FMCConstID	= FMC112_ID;
		FMCnbrch	= 12;
		routerID	= ROUTER_S16D1_ID;
		modeKC705	= 1;
		break;
	case CID_VC707_FMC112:
		deviceFW	= "FMC112 on VC707\n";
		FMCConstID	= FMC112_ID;
		FMCnbrch	= 12;
		routerID	= ROUTER_S16D1_ID;
		modeVC707	= 1;
		break;
	case CID_VP680_FMC116:
		deviceFW	= "FMC116 on VP680\n";
		FMCConstID	= FMC116_ID;
		FMCnbrch	= 16;
		routerID	= ROUTER_S16D1_ID;
		modeML605	= 0;
		break;
	case CID_ZC702_FMC112:
		deviceFW	= "FMC112 on ZC702\n";
		FMCConstID	= FMC112_ID;
		FMCnbrch	= 12;
		routerID	= ROUTER_S16D1_ID;
		modeZC702	= 1;
		break;		
	default:
		printf("The firmware ID of the current firmware is unknown, cannot continue, sorry...\n");
		sipif_free();
		return -4;
	}
	printf("FMC supported    : %s\n", deviceFW);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Read Star Offsets and compute sub mapping for stars
	ULONG size = 1;
	ULONG AddrSipRouter, AddrSipFMC116, AddrSipI2cMaster;
	if(cid_getstaroffset(routerID, &AddrSipRouter, &size)!=SIP_CID_ERR_OK) {
		printf("Could not obtain address for star type %d, exiting\n", ROUTER_S16D1_ID);
		sipif_free();
		return -5;
	}
	if(cid_getstaroffset(FMCConstID, &AddrSipFMC116, &size)!=SIP_CID_ERR_OK) {
		printf("Could not obtain address for star type %d, exiting\n", FMC116_ID);
		sipif_free();
		return -6;
	}
	if(cid_getstaroffset(I2C_MASTER_ID, &AddrSipI2cMaster, &size)!=SIP_CID_ERR_OK) {
		printf("Could not obtain address for star type %d, exiting\n", I2C_MASTER_ID);
		sipif_free();
		return -7;
	}
	// if we are dealing with a ML605, KC705 or VC707 constellation
	if(modeML605||modeKC705||modeVC707) {
		unsigned long AddrCtGen;
		// Search for fmc_ct_gen star in the constellation
		if(cid_getstaroffset(CT_GEN_ID, &AddrCtGen, &size)!=SIP_CID_ERR_OK) {
			printf("Could not obtain address for star type %d, exiting\n", I2C_MASTER_ID);
			sipif_free();
			return -8;
		}
		// If we are in external clock mode, we configure fmc_ct_gen star with the correct output frequency
		// otherwise we disable the output
		if(modeClock==FMC116_EXTCLK) {
			if(ctgen_configure(AddrCtGen, OUT_2GBPS_125MHZ)!=CTGEN_ERR_OK) {
				printf("Could not configure the clock/trigger generator star, exiting\n");
				sipif_free();
				return -9;
			}
		} else {
			if(ctgen_configure(AddrCtGen, OUT_XGBPS_DISABLED)!=CTGEN_ERR_OK) {
				printf("Could not configure the clock/trigger generator star, exiting\n");
				sipif_free();
				return -9;
			}
		}
	}


	// Calculate BAR of every peripheral mapped (sub mapping) to the FMC116 star's memory. This uses fixed offsets given by the FMC116
	ULONG AddrSipFMC116Ctrl    = AddrSipFMC116 + 0x000;
	ULONG AddrSipFMC116AdcPhy  = AddrSipFMC116 + 0x010;
	ULONG AddrSipFMC116AdcSpi0 = AddrSipFMC116 + 0x100;
	ULONG AddrSipFMC116AdcSpi1 = AddrSipFMC116 + 0x110;
	ULONG AddrSipFMC116AdcSpi2 = AddrSipFMC116 + 0x120;
	ULONG AddrSipFMC116AdcSpi3 = AddrSipFMC116 + 0x130;
	ULONG AddrSipFMC116ClkSpi  = AddrSipFMC116 + 0x300;
	ULONG AddrSipFMC116FreqCnt = AddrSipFMC116 + 0x600;
	ULONG AddrSipFMC116DacSpi0 = AddrSipFMC116 + 0x700;
	ULONG AddrSipFMC116DacSpi1 = AddrSipFMC116 + 0x800;
	ULONG AddrSipFMC116Cpld    = AddrSipFMC116 + 0x920;
	ULONG AddrSipFMC116Monitor = AddrSipI2cMaster;

    // Configure I2C switch to either LPC or HPC connector on the KC705
	ULONG dword = 0;
	if((cid_getconstellationid()== CID_KC705_FMC116)||(cid_getconstellationid()== CID_KC705_FMC116_PCIE)){
		sipif_writesipreg(AddrSipI2cMaster+0x7400, 0x02);	//reset
		Sleep(10);
	} else if(cid_getconstellationid()== CID_KC705_FMC112){
		sipif_writesipreg(AddrSipI2cMaster+0x7400, 0x04);	//reset
		Sleep(10);
	};
    // Configure I2C switch to HPC_1 connector on the VC707
	if(cid_getconstellationid()== CID_VC707_FMC116){
		sipif_writesipreg(AddrSipI2cMaster+0x7400, 0x02);	//reset
		Sleep(10);
	} else if(cid_getconstellationid()== CID_VC707_FMC112){
		sipif_writesipreg(AddrSipI2cMaster+0x7400, 0x02);	//reset
		Sleep(10);
	}
	// Configure I2C switch to FMC_1_LPC connector on the ZC702
	if(cid_getconstellationid()== CID_ZC702_FMC112){
		sipif_writesipreg(AddrSipI2cMaster+0x7400, 0x20);	//reset
		Sleep(10);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Detect FMC Presence
	if(FMC116_ctrl_probefmc(AddrSipFMC116Ctrl)!=FMC116_ERR_OK) {
		printf("Could not detect FMC116 hardware, exiting\n");
		sipif_free();
		return -11;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Temperature/Voltages monitoring
	printf("---  Measuring on-board voltages   ---\n");
	if(FMC116_monitor_getdiags(AddrSipFMC116Monitor)!=FMC116_MON_ERR_OK) {
		printf("An error occurred in the FMC116 diagnostics function, exiting\n");
		sipif_free();
		return -9;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Init FMC116
	if(FMC116_init(AddrSipFMC116Cpld, AddrSipFMC116ClkSpi, AddrSipFMC116AdcSpi0, AddrSipFMC116AdcSpi1, AddrSipFMC116AdcSpi2, AddrSipFMC116AdcSpi3,
		AddrSipFMC116AdcPhy, AddrSipFMC116DacSpi0, AddrSipFMC116DacSpi1, AddrSipFMC116Monitor, modeClock, FMCnbrch)!=FMC116_ERR_OK) {
		printf("Could not initialize FMC116\n");
		sipif_free();
		return -10;
	}
    printf("\n");

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Measure and display all available frequencies in a loop.
	// Note that the first frequencies (ADC clocks) are going to display erroneous values if no
	// FMC is actually attached.
    printf("--------------------------------------\n");
	printf("--- Measuring on-board frequencies ---\n");
	for(int i = 0; i < 7; i++) {
		if (i==4 && FMCnbrch==12) i++; //Skip Clock ADC 3 for FMC112
		if(FMC116_freqcnt_getfrequency(AddrSipFMC116FreqCnt, i, NULL, FMC116_FREQCNT_DISPLAY_CONSOLE)!=FMC116_FREQCNT_ERR_OK) {
			printf("Could not obtain frequency id%d from FMC116.FREQCNT\n", i);
			sipif_free();
			return -11;
		}
	}
    printf("--------------------------------------\n\n");

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Configure burst size and burst number
	int BurstSize    = 0;		// samples
	char dirCurrent[1024];
	GetModuleFileName(NULL,dirCurrent,1024);
	PathRemoveFileSpec(dirCurrent);
	char filename[1024];
	char filenamebin[1024];
	char filenameascii[1024];
	unsigned char *CMDFRM = (unsigned char *)_aligned_malloc(PRELIM_LEN, 4096);
	
		
	/****************************************************************************************************/
	// PB ADD TO START SERVER
    SOCKET server;
    WSADATA wsaData;
    sockaddr_in local;
    int wsaret=WSAStartup(0x101,&wsaData);
    if(wsaret!=0)
    {
        WSACleanup();
        return 0;
    }

    //Now we populate the sockaddr_in structure
    local.sin_family=AF_INET; //Address family
    local.sin_addr.s_addr=INADDR_ANY; //Wild card IP address
    local.sin_port=htons((u_short)SKT_PORT); //port to use

    //the socket function creates our SOCKET
    server=socket(AF_INET,SOCK_STREAM,0);

    //If the socket() function fails we exit
    if(server==INVALID_SOCKET)
    {
        WSACleanup();
        return 0;
    }
    if(bind(server,(sockaddr*)&local,sizeof(local))!=0)
    {
        return 0;
    }
    //listen instructs the socket to listen for incoming 
    //connections from clients. The second arg is the backlog
    if(listen(server,1)!=0)
    {
        WSACleanup();
        return 0;
    }

    //we will need variables to hold the client socket.
    //thus we declare them here.
    SOCKET client;
    sockaddr_in from;
    int fromlen=sizeof(from);
	
	printf("Waiting for data sink...\n");
	//accept() will accept an incoming
    //client connection
    client=accept(server,(struct sockaddr*)&from,&fromlen);
	if (client == INVALID_SOCKET) 
	{
        WSACleanup();
		return 0;
	}
	bool FLG_PRELIM0_DATA1 = false;
	unsigned int BYTECOUNT = 0;
	unsigned int DATALENGTH = PRELIM_LEN;
	unsigned char DATACHNL = 0;
	unsigned char DATACMD = 0;
	int iResult;
	unsigned int ITER1 = 0;
	unsigned int ITER2 = 0;
	unsigned int ITER3 = 0;
	unsigned int ITER4 = 0;
	unsigned int *pITER;
	int chnlNum = 0;
	unsigned __int64 routerword;
	int ChannelEnable;
	// Get burst size
	printf("Server online...\n");
	do{
		iResult = recv(client, ((char*)CMDFRM)+BYTECOUNT, DATALENGTH-BYTECOUNT, 0);
		BYTECOUNT+=iResult;
		if(iResult > 0) {
			if(BYTECOUNT == DATALENGTH){
				if(FLG_PRELIM0_DATA1){
					switch(DATACMD){
					case CMD_BURSTSIZE:
						BurstSize = (CMDFRM[1]<<8) + CMDFRM[0];
						printf("Setting BurstSize = %d\n",BurstSize);
						// Configure Burst Size
						if(FMC116_ctrl_configure_burst(AddrSipFMC116Ctrl, 1, BurstSize)!=FMC116_CTRL_ERR_OK) {
							printf("Could not configure burst size/length in FMC116.CTRL\n ");
							sipif_free();
							closesocket(client);
							WSACleanup();
							return -12;
						}
						_aligned_free(CMDFRM);
						CMDFRM = (unsigned char *)_aligned_malloc(((2*BurstSize)>PRELIM_LEN?(2*BurstSize):PRELIM_LEN), 4096);
						break;
					default:
						break;
					}
					DATALENGTH = PRELIM_LEN;
					FLG_PRELIM0_DATA1 = false;
				}
				else{
					// Get Command
					DATACMD = CMDFRM[IDX_CMD];
					// Get Command Channel
					DATACHNL = CMDFRM[IDX_CHNL];
					// Get Command Length
					DATALENGTH = (CMDFRM[IDX_LENMSB]<<8) + CMDFRM[IDX_LENLSB];
					if (DATALENGTH == 0){
						switch(DATACMD){
							case CMD_DATA:
								switch(DATACHNL){
								case CHNL_1:
									chnlNum = 0;
									pITER = &ITER1;
									break;
								case CHNL_2:
									chnlNum = 1;
									pITER = &ITER2;
									break;
								case CHNL_3:
									chnlNum = 2;
									pITER = &ITER3;
									break;
								case CHNL_4:
									chnlNum = 3;
									pITER = &ITER4;
									break;
								default:
									printf("Incorrect channel (%x) specified\n",DATACHNL);
									break;
								}
								strcpy(filenameascii,dirCurrent); 
								sprintf(filename,"\\adc%d_%d.txt",chnlNum,*pITER);
								strcat(filenameascii, filename);
								strcpy(filenamebin,dirCurrent); 
								sprintf(filename,"\\adc%d_%d.bin",chnlNum,*pITER);
								strcat(filenamebin, filename);
								DeleteFile(filenamebin);
								DeleteFile(filenameascii);

								// compute the router configuration for a given loop
								routerword = 0xFFFFFFFFFFFFFF00 | chnlNum;

								//ChannelEnable
								ChannelEnable = (0x01 << chnlNum);

								/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
								// Read a burst from ADC0 and save to file
								// route data from ADC0's FIFO
								if(sxdx_configurerouter(AddrSipRouter, routerword)!=SXDXROUTER_ERR_OK) {
									printf("Could not configure S3D1 router, exiting\n");
									sipif_free();
									_aligned_free(CMDFRM);
									return -13;
								}

								if(FMC116_ctrl_enable_channel(AddrSipFMC116Ctrl, ChannelEnable, 0)!=FMC116_CTRL_ERR_OK) {
									printf("Could not enable, exiting\n");
									sipif_free();
									_aligned_free(CMDFRM);
									return -21;
								}

								// arm the FMC116
								if(FMC116_ctrl_arm(AddrSipFMC116Ctrl)!=FMC116_CTRL_ERR_OK) {
									printf("Could not arm FMC116, exiting\n");
									sipif_free();
									_aligned_free(CMDFRM);
									return -22;
								}

								Sleep(2);

								// send a software trigger to the ADC block
								if(FMC116_ctrl_sw_trigger(AddrSipFMC116Ctrl)!=FMC116_CTRL_ERR_OK) {
									printf("Could not send software trigger to ADC0, exiting\n");
									sipif_free();
									_aligned_free(CMDFRM);
									return -23;
								}

								// Read data from the pipe
								printf("Retrieve %d samples from ADC%d\n", BurstSize,chnlNum);
								if(sipif_readdata  (CMDFRM,  2*BurstSize)!=SIPIF_ERR_OK) {
									printf("Could not communicate with device %d\n", devIdx);
									sipif_free();
									_aligned_free(CMDFRM);
									return -24;
								}
								*pITER++;
								
								send(client, (const char *)CMDFRM, 2*BurstSize, 0);
			
								Save16BitArrayToFile(CMDFRM, BurstSize, filenameascii, ASCII);
								Save16BitArrayToFile(CMDFRM, BurstSize, filenamebin, BINARY);
								break;
							default:
								break;
						}
						DATALENGTH = PRELIM_LEN;
						FLG_PRELIM0_DATA1 = false;
					}
					else{
						FLG_PRELIM0_DATA1 = true;
					}
				}
				BYTECOUNT = 0;
			}
		}
		else if (iResult == 0)
				printf("Connection closing...\n");
		else  
		{
			printf("Recv failed with error: %d\n", WSAGetLastError());
			closesocket(client);
			WSACleanup();
			return 1;
		}
	} while(iResult > 0);
    closesocket(server);
    WSACleanup();
	// Close the device
	printf("\nEnd of program.\n\n\n");
	sipif_free();
	_aligned_free(CMDFRM);
	//system("pause");
	return 0;
}