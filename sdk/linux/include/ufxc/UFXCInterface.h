/********************************************//**
 *  UFXCInterface.h
 ***********************************************/
/********************************************//**
 *  Created on: 21 août 2018
 *  Author: GHAMMOURI Ayoub
 *  Class: UFXCInterface
 *  Description:
 ***********************************************/

#ifndef UFXCLIB_SRC_UFXCINTERFACE_H_
#define UFXCLIB_SRC_UFXCINTERFACE_H_

/********************************************//**
 *  DEPENDENCIES
 ***********************************************/
#include "UFXC/ConfigPortInterface.h"
#include "UFXC/DaqMonitoring.h"
#include "UFXC/ConfigDetector.h"
#include "UFXC/ConfigAcquisition.h"
#include "UFXC/UFXCException.h"
#include "UFXC/UfxlibTypesAndConsts.h"
#include <fstream>
#include <yat/file/FileName.h>
#include "UFXC/CaptureManager.h"
#include "UFXC/DaqConnection.h"
#include "UFXC/DataReceiver.h"

namespace ufxclib
{
	class UFXCInterface
	{
		public:
			UFXCInterface();
			virtual ~UFXCInterface();

			//!< for open TCP/UDP connection
			//!< @param TCP IP, and three the UDP IP
			void openCom(T_UfxcLibCnx tcpCnx,  T_UfxcLibCnx SFPpCnx1, T_UfxcLibCnx SFPpCnx2, T_UfxcLibCnx SFPpCnx3) throw (ufxclib::Exception);

			//!< close TCP/UDP connection
			void closeComm() throw (ufxclib::Exception);

			//!< return a DaqMonitoring object
			DaqMonitoring * getDaqMonitoringObj() throw (ufxclib::Exception);

			//!< return a ConfigDetector object
			ConfigDetector * getConfigDetectorObj() throw (ufxclib::Exception);

			//!< return a DataReceiver object
			DataReceiver * getDataReceiverObj() throw (ufxclib::Exception);

			//!< return a ConfigAcquisition object
			ConfigAcquisition * getConfigAcquisitionObj() throw (ufxclib::Exception);

			//!<The client fills a text file containing the detector configuration
			//!<Then the client calls the setDetectorConfigFile() function.
			//!<Set detector configuration by file:
			void setDetectorConfigFile (std::string file_path_and_name) throw (ufxclib::Exception);

		private:
			DaqMonitoring * m_daqMonitoring;
			ConfigDetector * m_configDetector;
			DataReceiver * m_dataReceiver;
			ConfigAcquisition * m_configAcquisition;
			ConfigPortInterface * m_configPortInterface;
			DaqConnection * m_daqConnectionSFP1;
			DaqConnection * m_daqConnectionSFP2;
			DaqConnection * m_daqConnectionSFP3;
			CaptureManager * m_captureManager;
	};
}//!< namespace ufxclib
#endif //!< UFXCLIB_SRC_UFXCINTERFACE_H_
