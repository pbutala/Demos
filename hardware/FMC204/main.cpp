/**
@file main.cpp
@author Arnaud Maye, 4DSP   
@brief FMC204 reference application
*************************************************************************/


// system includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//PB ADD
#include <Shlwapi.h>
#include "FMC204_IF.h"

#if defined WIN32

 // Include declarations for _aligned_malloc and _aligned_free 
 #include <malloc.h>
 #include <windows.h>
#else
 #include <unistd.h>
 #ifndef __int64
   #define __int64 long long
 #endif
 #ifndef ULONG
   #define ULONG unsigned long
 #endif
 #define INVALID_HANDLE_VALUE -1

static void *_aligned_malloc(size_t size, size_t alignment)  
{
	void *p;
	if (posix_memalign(&p, alignment, size))
		return NULL;
	return p; 
}

static void _aligned_free(void *p)
{
	free(p);
}

static void DeleteFile(const char *filename)
{
	unlink(filename);
}

#define Sleep(x)	(usleep((unsigned long long)(x*1000)))

#ifndef API_ENUM_DISPLAY
 #define API_ENUM_DISPLAY 1
#endif

#endif

// project includes
#include "sipif.h"

#include "cid.h"	
#include "sxdxrouter.h"
#include "i2cmaster.h"
#include "fmc204.h"
#include "ctgen.h"

#define CONSTELLATION_ID_FM680	0x89			/*!< firmware(constellation) ID for FM680-FMC204 is 137 */
#define CONSTELLATION_ID_VP680	0x99			/*!< firmware(constellation) ID for VM680-FMC204 is 153 */
#define CONSTELLATION_ID_ML605	0x5F			/*!< firmware(constellation) ID for ML605-FMC204 is 95 */
#define CONSTELLATION_ID_KC705	0xFB			/*!< firmware(constellation) ID for KC705-FMC204 is 251 */
#define CONSTELLATION_ID_VC707	0x160			/*!< firmware(constellation) ID for KC705-FMC204 is 251 */

#define ROUTER_S1D5_ID			0x13			/*!< router star ID as per the firmware source code */
#define FMC204_ID				0x2E			/*!< FMC204 star ID as per the firmware source code */
#define I2C_MASTER_ID			0x05			/*!< I2C master star ID as per the firmware source code */
#define CT_GEN_ID				0x43			/*!< fmc_ct_gen star ID as per the firmware source code */

#define SINE_WAVE				0				/*!< GenerateWaveform() generates sine wave */
#define SAW_WAVE				1				/*!< GenerateWaveform() generates saw wave */
#define DC_WAVE					2				/*!< GenerateWaveform() generates dc wave */

#define SYNTH_M					250				/*!< Reference value for M on the synthesizer frequency (f = M/N) */
#define SYNTH_N					2				/*!< Reference value for N on the synthesizer frequency (f = M/N) */
#define ASCII					0				/*!< Save16BitArrayToFile() saves the samples as ASCII */
#define BINARY					1				/*!< Save16BitArrayToFile() saves the samples as binary */
#define TIMEOUTDMA				2000			/*!< DMA tiemout is 2 seconds (2000 ms) */


//#define LOADFROMFILE						/*!< Application does not generate buffer but read it from file using GetBufferFromFile() */
//#define BUFFERFILENAME "ST_25MHz-0dB.txt"	/*!< "path to the DAC buffer file */
//#define BUFFERFILENAME "DT_25MHz-6dB.txt"	/*!< "path to the DAC buffer file */
//#define BUFFERFILENAME "DT_25MHz-12dB.txt"	/*!< "path to the DAC buffer file */


/**
 *  Generate a 16 bit waveform into a previously allocated memory buffer. This function can generate several data types and both
 *  signal period as well as amplitude are configurable.
 *
 *  @param buffer	pointer to a buffer about to receive the waveform data. This point to a previously allocated memory as big as
 *					numbersamples*2 ( byte size ) or numbersamples*1 ( sample size ).
 *  @param numbersamples	number of samples to be written on the buffer where one sample is as big as 2 bytes.
 *  @param period	period of the signal to generate.
 *  @param amplitude	amplitude of the signal to generate.
 *  @param datatype	decide what kind of data the function generates :
 *						- SINE_WAVE
 *						- SAW_WAVE
 *						- DC_WAVE
 *  @return 
 *						- -1 ( Unexpected NULL argument )
 *						- 0 ( Success )
 */
int GenerateWaveform16(unsigned short *buffer, unsigned int numbersamples, unsigned int period, unsigned int amplitude, unsigned char datatype)
{
	int ampl				= 0;
	double pi				= 3.1415926535;
	int tmp2				= 0x0;
	double x, y;

	// set our buffer with known value ( 0 ). Note the MUL(2) because memset takes a byte size
	memset(buffer,0, numbersamples*2);

	// make sure we are not going to hit the wall
	if(!buffer) {
		printf("GenerateWaveform() cannot receive a NULL first argument...\n");
		 
		return -1;
	}

	// proceed with the data generation
	switch(datatype)
	{
	case SINE_WAVE:
		for(unsigned int i=0; i < numbersamples/2; i++)
		{
			ampl=amplitude/2-1;
			x = (pi /period*2)*(i*2+0);
			y = sin( x );
			buffer[2*i+0] =  ((unsigned short)(y*ampl));
			x = (pi /period*2)*(i*2+1);
			y = sin( x );
			buffer[2*i+1] =  ((unsigned short)(y*ampl));
		}
		break;
	case SAW_WAVE:
		for(unsigned int i=0; i < numbersamples/2; i++)
		{
			tmp2 =(0xffff)& ((2*i)%period*(amplitude-1)/period*2);
			buffer[2*i+0] = (unsigned short)(tmp2);
			tmp2 = (0xffff)& ((2*i)%period*(amplitude-1)/period*2);
			buffer[2*i+1] = (unsigned short)(tmp2);
		}
		break;
	case DC_WAVE:
	default:
		for(unsigned int i=0; i < numbersamples/2; i++)
		{
			tmp2 =(0xffff) & (0x8000);
			buffer[2*i+0] = (unsigned short)(tmp2);
			buffer[2*i+1] = (unsigned short)(tmp2);
		}
	}
	 return 0;
}


/**
 *  Load a 16 bit sample array to file.
 *  The file is ASCII with one sample per line as the following :
 *  14883
 *  10134
 *  5136
 *  12
 *  -5113
 *  -10112
 *  -14863
 *  -19247
 *  -23158
 *
 *  @param filepath	pointer to a string representing the path where the buffer is located.
 *  @param buffer	pointer to a previously allocated buffer.
 *  @param szbuffer	size of the buffer in bytes. A sample is two bytes.
 *	@note the file is truncated if too big to fit in the buffer passed as argument and the buffer is padded with zeros if the file is too small
 *  @return 
 *						- Size of the buffer loaded from file.
 *						- 0 ( Error )
 */
size_t GetBufferFromFile(const char *filepath, void *buffer, size_t szbuffer)
{
	FILE *infile;
	char templine[1024];
	int index = 0;

	// the size buffer is in 16 bit samples, we mul 2
	szbuffer *= 2;

	// check for NULL arguments
	if(!filepath)
		return 0;
	if(!buffer)
		return 0;

	// get a 16 bit pointer out of our buffer passed as argument
	short *p16 = (short *)buffer;

	// let's zero the buffer. This ensure non processed samples going to be 0 in the buffer
	memset(buffer, 0, szbuffer);

	// try to open
	infile = fopen(filepath, "r");
	if(!infile) {
		printf("Could not open %s, returning...\n", filepath);
		 return 0;
	}

	// read from file
	while(fgets(templine, 1024, infile)!=NULL) {
		if((size_t)index<(szbuffer/2))
			p16[index++] = atoi(templine);
	}

	// close
	fclose(infile);

	return index*2;
}



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
 *  @ return
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
		printf("Save16BitArrayToFile() -> first argument cannot be NULL\n");
		 return -1;
	}

	if(!filename) {
		printf("Save16BitArrayToFile() -> third argument cannot be NULL\n");
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
		printf("Save16BitArrayToFile() -> Cannot open file '%s' with write access\n", filename);
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



/**
 *  \brief FMC204 Reference application (main).
 *
 *  This function demonstrates how to configure both digital to analog peripherals, upload waveforms to digital
 *  to analog convert chips, display FMC204 diagnostic, display FMC204 clock tree and grab data from both ADC 0
 *  and ADC 1.
 *
 *  Description of the software sequence :
 *	- Check and convert arguments passed to the application.
 *	- Open a ML605+FMC204 over ethernet using OpenDevice().
 *	- Read the constellation information from the ML605+FMC204 using cid_init().
 *	- Compute start offset of all FMC204 peripheral in the main constallation address space using cid_getstaroffset().
 *	- Configure the data routers with some defautl settings using sxdx_configurerouter().
 *	- Display FMC204 diagnostics using i2cmaster_getdiagnosticsFMC204().
 *  - If ML605 constellation is found in the hardware, we configure the clock trigger generation module (in external clock mode only).
 *	- Init all the FMC204 peripherals using FMC204_init().
 *	- Display all the freqencies part of the frequency tree using FMC204_freqcnt_getfrequency().
 *	- Configure burst size and burst number ( common for both ADC and DAC chips ) using FMC204_ctrl_configure_burst().
 *	- Generate a waveform and upload waveform to DAC0 using GenerateWaveform16(), sxdx_configurerouter(), FMC204_ctrl_prepare_wfm_load() and sipif_writedata() part of ethapi.
 *	- Generate a waveform and upload waveform to DAC1 using GenerateWaveform16(), sxdx_configurerouter(), FMC204_ctrl_prepare_wfm_load() and sipif_writedata() part of ethapi.
 *	- Generate a waveform and upload waveform to DAC2 using GenerateWaveform16(), sxdx_configurerouter(), FMC204_ctrl_prepare_wfm_load() and sipif_writedata() part of ethapi.
 *	- Generate a waveform and upload waveform to DAC3 using GenerateWaveform16(), sxdx_configurerouter(), FMC204_ctrl_prepare_wfm_load() and sipif_writedata() part of ethapi.

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
	int rc;
	//argc = 5;
	// Parse the application arguments
	if(argc!=5) {
		printf("Usage: FMCxxxApp.exe {interface type} {device type} {device index} {clock mode}\n\n");
		printf(" {interface type} can be either 0 (PCI) or 1 (Ethernet)\n");
		printf(" {device type} is a string defining the target hardware (VP680, ML605, ...)\n");
		printf(" {device index} is a PCI index or an Ethernet interface index\n");
		printf(" {clock mode} can be either 0 (Int. Clock) or 1 (Ext. Clock)\n");
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
		devIdx = 0;
		modeClock = 0;*/

		// translate interface type to the sipif values
		if(ifType==0)
			ifType = SIPIF_4FM;
		else
			ifType = SIPIF_ETHAPI;			
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Initialize the communication layer
	rc = sipif_init(ifType, devType, devIdx, TIMEOUTDMA, SYNTH_M, SYNTH_N);
	if(rc!=SIPIF_ERR_OK) {
		printf("Problem opening the hardware, sorry...\n");
		 return -2;
	}

	printf("Start of program\n");
	printf("--------------------------------------\n");

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Obtain and display the sip_cid informations to the console. This function also check that the constellation ID
	// obtained by the firmware match the value passed as argument
	rc  = cid_init(0);
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

	if( cid_getconstellationid() == CONSTELLATION_ID_FM680) {
		printf("Found FMC204 hardware on FM680\n\n");
	}
	else if( cid_getconstellationid() == CONSTELLATION_ID_VP680) {
		printf("Found FMC204 hardware on VP680\n\n");
	}
	else if( cid_getconstellationid() == CONSTELLATION_ID_ML605) {
		printf("Found FMC204 hardware on ML605\n\n");
	}
	else if( cid_getconstellationid() == CONSTELLATION_ID_KC705) {
		printf("Found FMC204 hardware on KC705\n\n");
	}
	else if( cid_getconstellationid() == CONSTELLATION_ID_VC707) {
		printf("Found FMC204 hardware on VC707\n\n");
	}
	
	else {
		printf("Constellation ID not supported by this software, exiting...\n");
		sipif_free();
		 return -3;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Read Star Offsets and compute sub mapping for stars
	ULONG size = 1;
	ULONG AddrSipRouterS1D5, AddrSipFMC204, AddrSipI2cMaster;
	if(cid_getstaroffset(ROUTER_S1D5_ID, &AddrSipRouterS1D5, &size)!=SIP_CID_ERR_OK) {
		printf("Could not obtain address for star type %d, exiting\n", ROUTER_S1D5_ID);
		sipif_free();
		 return -4;
	}
	if(cid_getstaroffset(FMC204_ID, &AddrSipFMC204, &size)!=SIP_CID_ERR_OK) {
		printf("Could not obtain address for star type %d, exiting\n", FMC204_ID);
		sipif_free();
		 return -6;
	}
	if(cid_getstaroffset(I2C_MASTER_ID, &AddrSipI2cMaster, &size)!=SIP_CID_ERR_OK) {
		printf("Could not obtain address for star type %d, exiting\n", I2C_MASTER_ID);
		sipif_free();
		 return -7;
	}

	// if we are dealing with a ML605 constellation
	if((cid_getconstellationid() == CONSTELLATION_ID_ML605)||(cid_getconstellationid() == CONSTELLATION_ID_KC705)) {
		unsigned long AddrCtGen;
		// Search for fmc_ct_gen star in the constellation
		if(cid_getstaroffset(CT_GEN_ID, &AddrCtGen, &size)!=SIP_CID_ERR_OK) {
			printf("Could not obtain address for star type %d, exiting\n", I2C_MASTER_ID);
			sipif_free();
			 return -8;
		}

		// If we are in external clock mode, we configure fmc_ct_gen star with the correct output frequency
		// otherwise we disable the output
		if(modeClock==1) {
			printf("Start setting clock...\n");
			if(ctgen_configure(AddrCtGen, OUT_2GBPS_1000MHZ)!=CTGEN_ERR_OK) {
			//if(ctgen_configure(AddrCtGen, OUT_2GBPS_125MHZ)!=CTGEN_ERR_OK) {
				printf("Could not configure the clock/trigger generator star, exiting\n");
				sipif_free();
				 return -9;
			}
			printf("Done setting clock...\n");
		} else {
			if(ctgen_configure(AddrCtGen, OUT_XGBPS_DISABLED)!=CTGEN_ERR_OK) {
				printf("Could not configure the clock/trigger generator star, exiting\n");
				sipif_free();
				 return -9;
			}
		}
	}


	// Calculate BAR of every peripheral mapped (sub mapping) to the FMC204 star's memory. This uses fixed offsets given by the FMC204 
	ULONG AddrSipFMC204Ctrl    = AddrSipFMC204 + 0x000;
	ULONG AddrSipFMC204DacPhy0 = AddrSipFMC204 + 0x020;
	ULONG AddrSipFMC204DacSpi0 = AddrSipFMC204 + 0x200;
	ULONG AddrSipFMC204ClkSpi  = AddrSipFMC204 + 0x300;
	ULONG AddrSipFMC204FreqCnt = AddrSipFMC204 + 0x600;
	ULONG AddrSipFMC204DacPhy1 = AddrSipFMC204 + 0x720; //Does not exist anymore
	ULONG AddrSipFMC204DacSpi1 = AddrSipFMC204 + 0x900;
	ULONG AddrSipFMC204Cpld    = AddrSipFMC204 + 0x920;
	ULONG AddrTempMon = AddrSipI2cMaster + 0x4800;

	// Configure I2C switch to either LPC or HPC connector on the KC705
	ULONG dword = 0;
	if(cid_getconstellationid()== CONSTELLATION_ID_KC705){
		sipif_writesipreg(AddrSipI2cMaster+0x7400, 0x02);	// Switch set to LPC		
		Sleep(10);
	}
	// Configure I2C switch to either HPC1 or HPC2 connector on the VC707
	
	if(cid_getconstellationid()== CONSTELLATION_ID_VC707){
		sipif_writesipreg(AddrSipI2cMaster+0x7400, 0x02); // Switch set to HPC_1	
		Sleep(10);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Detect FMC Presence
	if(FMC204_ctrl_probefmc(AddrSipFMC204Ctrl)!=FMC204_ERR_OK) {
		printf("Could not detect FMC204 hardware, exiting\n");
		sipif_free();
		 return -11;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Temperature/Voltages
	// we are only interested to display values to the console so we have all arguments receiving NULL as well as the last argument
	// set to 1
	if(i2cmaster_getdiagnosticsFMC204(AddrTempMon, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, I2CMASTER_DISPLAY_CONSOLE)!=I2CMASTER_FMC204_ERR_OK) {
		printf("Could not get FMC204 diagnostics, exiting\n");
		sipif_free();
		 return -10;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Init FMC204
	if(FMC204_init(AddrSipFMC204Cpld, AddrSipFMC204ClkSpi, AddrSipFMC204DacSpi0, AddrSipFMC204DacPhy0, AddrSipFMC204DacSpi1, 
		AddrSipFMC204DacPhy1, modeClock)!=FMC204_ERR_OK) {
			printf("Could not initialize FMC204, exiting\n");
			sipif_free();
			 return -11;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Measure and display all available frequencies in a loop.
	// Note that the first frequencies (ADC clocks) are going to display erroneous values if no
	// FMC is actually attached.
	printf("\n--- Measuring on-board frequencies ---\n");
	for(int i = 0; i < 4; i++) {
		if(FMC204_freqcnt_getfrequency(AddrSipFMC204FreqCnt, i, NULL, FMC204_FREQCNT_DISPLAY_CONSOLE)!=FMC204_FREQCNT_ERR_OK) {
			printf("Could not obtain frequency id%d from FMC204.FREQCNT\n", i);
			sipif_free();
			 return -12;
		}
	}
    printf("--------------------------------------\n\n");

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Configure burst size and burst number
	int BurstSize    = 0;			// samples
	unsigned char *CMDFRM = (unsigned char *)_aligned_malloc(PRELIM_LEN, 4096);

	char dirCurrent[1024];
	GetModuleFileName(NULL,dirCurrent,1024);
	PathRemoveFileSpec(dirCurrent);
	char filename[1024];
	char filenamebin[1024];
	char filenameascii[1024];

	
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
    local.sin_port=htons(SKT_PORT); //port to use

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

	printf("Waiting for data source...\n");
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
	unsigned int chnlNum = 0;
	unsigned long long routerValue = 0;
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
						if(FMC204_ctrl_configure_burst(AddrSipFMC204Ctrl, 1, BurstSize)!=FMC204_CTRL_ERR_OK) {
							printf("Could not configure burst size/length in FMC204.CTRL\n ");
							sipif_free();
							 return -13;
						}
						_aligned_free(CMDFRM);
						CMDFRM = (unsigned char *)_aligned_malloc(((2*BurstSize)>PRELIM_LEN?(2*BurstSize):PRELIM_LEN), 4096);
						break;
					case CMD_DATA:
						switch(DATACHNL){
						case CHNL_1: 
							pITER = &ITER1; 
							chnlNum = DAC0; 
						#ifdef WIN32
							routerValue = 0xFFFFFFFFFFFFFF00;
						#else
							routerValue = 0xFFFFFFFFFFFFFF00ULL;
						#endif
							break;
						case CHNL_2: 
							pITER = &ITER2; 
							chnlNum = DAC1; 
						#ifdef WIN32
							routerValue = 0xFFFFFFFFFFFF00FF;
						#else
							routerValue = 0xFFFFFFFFFFFF00FFULL;
						#endif
							break;
						case CHNL_3:
							pITER = &ITER3; 
							chnlNum = DAC2; 
						#ifdef WIN32
							routerValue = 0xFFFFFFFFFF00FFFF;
						#else
							routerValue = 0xFFFFFFFFFF00FFFFULL;
						#endif
							break;
						case CHNL_4: 
							pITER = &ITER4; 
							chnlNum = DAC3; 
						#ifdef WIN32
							routerValue = 0xFFFFFFFF00FFFFFF;
						#else
							routerValue = 0xFFFFFFFF00FFFFFFULL;
						#endif
							break;
						}
						strcpy(filenameascii,dirCurrent); 
						sprintf(filename,"\\dac%d_%d.txt",chnlNum,*pITER);
						strcat(filenameascii, filename);
						strcpy(filenamebin,dirCurrent); 
						sprintf(filename,"\\dac%d_%d.bin",chnlNum,*pITER);
						strcat(filenamebin, filename);

						DeleteFile(filenamebin);
						DeleteFile(filenameascii);
						Save16BitArrayToFile(CMDFRM, BurstSize, filenameascii, ASCII);
						Save16BitArrayToFile(CMDFRM, BurstSize, filenamebin, BINARY);
						// configure the router ( route data to DAC0's wave form memory )
					#ifdef WIN32
						if(sxdx_configurerouter(AddrSipRouterS1D5, routerValue)!=SXDXROUTER_ERR_OK) {
					#else
						if(sxdx_configurerouter(AddrSipRouterS1D5, routerValue)!=SXDXROUTER_ERR_OK) {
					#endif
							printf("Could not configure S1D5 router, exiting\n");
							sipif_free();
							 return -15;
						}
						// prepare the firmware to receive waveform data
						if(FMC204_ctrl_prepare_wfm_load(AddrSipFMC204Ctrl, chnlNum)!=FMC204_CTRL_ERR_OK) {
							printf("Could not prepare waveform upload, exiting\n");
							sipif_free();
							 return -16;
						}
						// send the data to the waveform memory
						if(sipif_writedata  (CMDFRM,  2*BurstSize)!=SIPIF_ERR_OK) {
							printf("Could not communicate with device %d.\n", devIdx);
							sipif_free();
							 return -17;
						}
						*pITER++;
						
						printf("Send data to channel %d\n",chnlNum);
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
							case CMD_ENCHNL:
								if(FMC204_ctrl_enable_channel(AddrSipFMC204Ctrl, ENABLED, DISABLED, DISABLED, ENABLED)!=FMC204_CTRL_ERR_OK) {
									printf("Could not enable, exiting\n");
									sipif_free();
									 return -26;
								}
								printf("Enabling all channels\n");
								break;
							case CMD_ARMDAC:
								// arm the DAC
								if(FMC204_ctrl_arm_dac(AddrSipFMC204Ctrl)!=FMC204_CTRL_ERR_OK) {
									printf("Could not arm, exiting\n");
									sipif_free();
									 return -27;
								}
								printf("Arming channels\n");
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
				BYTECOUNT= 0;
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
	//_aligned_free(BSData);
	_aligned_free(CMDFRM);
	//_aligned_free(pOutData);
	// wait user entry before closing the application
	//system("pause");
	return 0;

}
