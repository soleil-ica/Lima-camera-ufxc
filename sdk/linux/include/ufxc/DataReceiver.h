/********************************************//**
 *  DataReceiver.h
 ***********************************************/
/********************************************//**
 *  Created on: 2 août 2018
 *  Author: GHAMMOURI Ayoub
 *  Class: DaqMonitoring
 *  Description: This class provide access to DAQ to read UDP data
 ***********************************************/
#ifndef UFXCLIB_SRC_DataReceiver_H_
#define UFXCLIB_SRC_DataReceiver_H_

/********************************************//**
 *  DEPENDENCIES
 ***********************************************/
#include "UFXC/ConfigPortInterface.h"
#include "UFXC/CaptureManager.h"

namespace ufxclib {

class DataReceiver : public yat::Thread{
public:

	DataReceiver()
	{

	}
	DataReceiver(ConfigPortInterface * configPortInterface, CaptureManager * captureManager);

	virtual ~DataReceiver();

	//!< Function call the join
	void exit();

	//!<provide access to FPGA registers dedicated to image reading
	void setSFPRegisterNames(std::map< T_SFPconfigKey, std::string> map, unsigned short sfp_nb) throw (ufxclib::Exception);
	//!< Get network configuration for specified SFP output
	//!< @return T_UfxcLibCnx
	T_UDPConfig  getSFPNetworkConfig( unsigned short sfp_nb) throw (ufxclib::Exception);
	//!< Set network configuration for specified SFP output
	// @param T_UfxcLibCnx param
	void setSFPNetworkConfig(T_UDPConfig  UDPCfg, unsigned short sfp_nb) throw (ufxclib::Exception);

	//!< function to get n images from 3 SFPs
	void getImage(unsigned char ImgBuffer[]) throw (ufxclib::Exception);

protected:
	//!< the thread entry point - called by yat::Thread::start_undetached
	virtual yat::Thread::IOArg run_undetached (yat::Thread::IOArg ioa) throw (ufxclib::Exception);

private:
	CaptureManager * m_captureManager;
	ConfigPortInterface * m_configPortInterface;
	std::map< T_SFPconfigKey, std::string> m_SFP_registers[3];
	unsigned int m_imgBufferSize;
	unsigned char * m_imgBuffer;
};

} //!< namespace ufxclib

#endif //!< UFXCLIB_SRC_DataReceiver_H_
