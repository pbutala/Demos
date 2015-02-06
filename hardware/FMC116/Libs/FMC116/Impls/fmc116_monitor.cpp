///////////////////////////////////////////////////////////////////////////////////
///@file FMC116_monitor.cpp
///@author Arnaud Maye (4DSP) 
///\brief FMC116_monitor module to interface with the FMC116 star (implementation)
///
/// This module is in charge of controlling the monitoring device populated
/// on the FMC116 printed circuit board.
///
///////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "FMC116_monitor.h"
#include "sipif.h"


int FMC116_monitor_getdiags(unsigned long bar_mon) 
{
	ULONG dev_id, man_id, silicon_rev;
	float chipTemp = 0;
	float vdd = 0;
	float ain1 = 0;
	float ain2 = 0;
	float ain3 = 0;
	float ain4 = 0;
	float ain5 = 0;
	float ain6 = 0;
	float ain7 = 0;
	float ain8 = 0;

	int rc = FMC116_monitor_getDeviceDiags(bar_mon, FMC116_ADT_DEV, &dev_id, &man_id, &silicon_rev, &chipTemp,
		&vdd, &ain1, &ain2, &ain3, &ain4, &ain5, &ain6, &ain7, &ain8);
	
	if(rc == -3){
		printf("Could not read from ADT7411 #1\n");
		return -3;
	}

	printf("\n-------- Onboard Monitoring ---------\n");
	printf("ADT7411 #1 \nDevice ID       : 0x%2.2X \nManufacturer ID : 0x%2.2X \nSilicon Revision: 0x%2.2X\n", dev_id, man_id, silicon_rev);

	//Onchip TEMP
	printf("TEMP (  ADCcd ) : %5.2fC \n", chipTemp);

	printf("VDD  ( 3.3V   ) : %5.2fV \n", vdd);
	printf("AIN1 ( 1.8V[2]) : %5.2fV \n", ain1);
	printf("AIN2 ( 1.8V[3]) : %5.2fV \n", ain2);
	printf("AIN3 ( 1.8V[D]) : %5.2fV \n", ain3);
	printf("AIN4 (-3.3V   ) : %5.2fV \n", ain4);
	printf("AIN5 ( 1.8V[1]) : %5.2fV \n", ain5);
	printf("AIN6 ( 1.8V[0]) : %5.2fV \n", ain6);
	printf("AIN7 ( 3.3V[C]) : %5.2fV \n", ain7);
	printf("AIN8 (+3.3V   ) : %5.2fV \n", ain8);

	printf("-------------------------------------\n\n");

	return FMC116_MON_ERR_OK;
}

int FMC116_monitor_getDeviceDiags(unsigned long bar_mon, int deviceNum, ULONG* dev_id, ULONG* man_id, ULONG* silicon_rev,
								  float* chipTemp, float* vdd, float* ain1, float* ain2,
								  float* ain3, float* ain4, float* ain5, float* ain6, float* ain7, float* ain8)
{
	// check to make sure device number is correct
	if(deviceNum != FMC116_ADT_DEV)
		return -3;

	ULONG lsb, msb, test;
	float Vref, Vtmp;

	// set the starting register to offset from based on the ADT device
	unsigned long start_reg = bar_mon-0x10000+0x14800;

	// Read IDs from ADT
	if(sipif_readsipreg(start_reg+0x4D, dev_id) == SIPIF_ERR_TIMEOUT) {
		sipif_free();
		return -3;
	}
	if(sipif_readsipreg(start_reg+0x4E, man_id) == SIPIF_ERR_TIMEOUT) {
		sipif_free(); 
		return -3;
	}
	if(sipif_readsipreg(start_reg+0x4F, silicon_rev) == SIPIF_ERR_TIMEOUT) {
		sipif_free();
		return -3;
	}

	//Control configuration 1
	sipif_writesipreg(start_reg+0x18, 0x29);
	Sleep(100);
	
	//Control configuration 3	
	sipif_writesipreg(start_reg+0x1A, 0x11); //Ext VREF
	//sipif_writesipreg(start_reg+0x1A, 0x01); //Int VREF
	Sleep(100);

	//Onchip TEMP
	sipif_readsipreg(start_reg+0x03, &test); //LSB
	lsb = (test >> 0) && 0x3;
	sipif_readsipreg(start_reg+0x07, &test); //MSB
	msb = test << 2;
	*chipTemp = (msb + lsb) / 4.0f;

	//Onchip VDD
	sipif_readsipreg(start_reg+0x03, &test); //LSB
	lsb = (test >> 2) && 0x3;
	sipif_readsipreg(start_reg+0x06, &test); //MSB
	msb = test << 2;
	*vdd = (msb + lsb) * 3.11f * 2.197f / 1000.0f;
	Vref = *vdd;
	//Vref = 2.25f;
	//Vref = 3.3f;

	//AIN1
	sipif_readsipreg(start_reg+0x04, &test); //LSB
	lsb = (test >> 0) && 0x3;
	sipif_readsipreg(start_reg+0x08, &test); //MSB
	msb = test << 2;
	*ain1 = (msb + lsb) * Vref / 1024.0f;

	//AIN2
	sipif_readsipreg(start_reg+0x04, &test); //LSB
	lsb = (test >> 2) && 0x3;
	sipif_readsipreg(start_reg+0x09, &test); //MSB
	msb = test << 2;
	*ain2 = (msb + lsb) * Vref / 1024.0f;

	//AIN3
	sipif_readsipreg(start_reg+0x04, &test); //LSB
	lsb = (test >> 4) && 0x3;
	sipif_readsipreg(start_reg+0x0A, &test); //MSB
	msb = test << 2;
	*ain3 = (msb + lsb) * Vref / 1024.0f;

	//AIN4
	sipif_readsipreg(start_reg+0x04, &test); //LSB
	lsb = (test >> 6) && 0x3;
	sipif_readsipreg(start_reg+0x0B, &test); //MSB
	msb = test << 2;
	Vtmp = (msb + lsb) * Vref / 1024.0f;
	*ain4 = 5.7f * Vtmp - 4.7f * *vdd;

	//AIN5
	sipif_readsipreg(start_reg+0x05, &test); //LSB
	lsb = (test >> 0) && 0x3;
	sipif_readsipreg(start_reg+0x0C, &test); //MSB
	msb = test << 2;
	*ain5 = (msb + lsb) * Vref / 1024.0f;

	//AIN6
	sipif_readsipreg(start_reg+0x05, &test); //LSB
	lsb = (test >> 2) && 0x3;
	sipif_readsipreg(start_reg+0x0D, &test); //MSB
	msb = test << 2;
	*ain6 = (msb + lsb) * Vref / 1024.0f;

	//AIN7
	sipif_readsipreg(start_reg+0x05, &test); //LSB
	lsb = (test >> 4) && 0x3;
	sipif_readsipreg(start_reg+0x0E, &test); //MSB
	msb = test << 2;
	*ain7 = 2 * (msb + lsb) * Vref / 1024.0f;

	//AIN8
	sipif_readsipreg(start_reg+0x05, &test); //LSB
	lsb = (test >> 6) && 0x3;
	sipif_readsipreg(start_reg+0x0F, &test); //MSB
	msb = test << 2;
	*ain8 = 2 * (msb + lsb) * Vref / 1024.0f;

	return FMC116_MON_ERR_OK;
}