/********************************************//**
 *  CaptureManager.h
 ***********************************************/
/********************************************//**
 *  Created on: 21 août 2018
 *  Author: GHAMMOURI Ayoub
 *  Class: CaptureManager
 *  Description:
 ***********************************************/
#ifndef UFXCLIB_SRC_CAPTUREMANAGER_H_
#define UFXCLIB_SRC_CAPTUREMANAGER_H_

/********************************************//**
 *  DEPENDENCIES
 ***********************************************/
#include "UFXC/DataCaptureListener.h"
#include "UFXC/DaqConnection.h"

namespace ufxclib
{
	class CaptureManager : public DataCaptureListener
	{
		public:
			CaptureManager()
			{

			}
			CaptureManager(DaqConnection * daqConnectionSFP1, DaqConnection * daqConnectionSFP2, DaqConnection * daqConnectionSFP3);
			virtual ~CaptureManager();

			//!< start receiving data from 3 SFPs
			void startCapture() throw (ufxclib::Exception);

			//!< stop receiving data from 3 SFPs
			void stopCapture() throw (ufxclib::Exception);

			//!< int config before receiving data from 3 SFPs
			void initCapture() throw (ufxclib::Exception);

			//!< read the buffers if its not empty
			void readDataFromBuffers(unsigned char ImgBuffer[], unsigned int size) throw (ufxclib::Exception);

			//!< return a m_dataCaptureListenerSFP1 pointer
			DataCaptureListener * getDataCaptureListenerSFP1() throw (ufxclib::Exception);

			//!< return a m_dataCaptureListenerSFP2 pointer
			DataCaptureListener * getDataCaptureListenerSFP2() throw (ufxclib::Exception);

			//!< return a m_dataCaptureListenerSFP3 pointer
			DataCaptureListener * getDataCaptureListenerSFP3() throw (ufxclib::Exception);

			//!< set the buffer size max
			//!< @param the max size of each buffer
			void setbufferOneSizeMax(yat::uint32 bufferSizeMax);

			//!< get the buffer size max
			yat::uint32 getbufferOneSizeMax();
		private:
			DaqConnection * m_daqConnectionSFP1;
			DaqConnection * m_daqConnectionSFP2;
			DaqConnection * m_daqConnectionSFP3;

			DataCaptureListener * m_dataCaptureListenerSFP1;
			DataCaptureListener * m_dataCaptureListenerSFP2;
			DataCaptureListener * m_dataCaptureListenerSFP3;
			yat::uint32 m_dequeSizeMax;

	};

}//!< namespace ufxclib
#endif //!< UFXCLIB_SRC_CAPTUREMANAGER_H_
