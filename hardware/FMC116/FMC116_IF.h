///////////////////////////////////////////////////////////////////////////////////
///@file FMC116_IF.h
///@author Pankil Butala (MCL, BU) 
///\brief FMC116_IF module to interface to accept commands over socket (header)
///
/// This defines the commands and array sizes needed to configure FMC116 over a socket
///
///////////////////////////////////////////////////////////////////////////////////
#ifndef _FMC116_IF_H_
#define _FMC116_IF_H_

// Socket 
#define SKT_PORT	(unsigned short)30002

// Command Config
#define IDX_CMD		0x00	
#define IDX_CHNL	0x01
#define IDX_LENLSB	0x02
#define IDX_LENMSB	0x03
#define PRELIM_LEN	0x04

// Commands
#define CMD_BURSTSIZE	0x10
#define CMD_DATA		0x20

// ADC Channel 
#define CHNL_1		0x01
#define CHNL_2		0x02
#define CHNL_3		0x04
#define CHNL_4		0x08

#endif
