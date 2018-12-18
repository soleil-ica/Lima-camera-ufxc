//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2014
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################

#include <sstream>
#include <iostream>
#include <string>
#include <math.h>
//#include <chrono>
#include <climits>
#include <iomanip>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include "lima/Exceptions.h"
#include "lima/Debug.h"
#include "lima/MiscUtils.h"
#include "UfxcCamera.h"

using namespace lima;
using namespace lima::Ufxc;
using namespace std;


//---------------------------
// @brief  Ctor
//---------------------------
/************************************************************************
 * \brief constructor
 ************************************************************************/
Camera::Camera(const std::string& config_ip_address, unsigned long config_port,
			   const std::string& SFP1_ip_address, unsigned long SFP1_port,
			   const std::string& SFP2_ip_address, unsigned long SFP2_port,
			   const std::string& SFP3_ip_address, unsigned long SFP3_port,
			   unsigned long timeout_ms)
{
	DEB_CONSTRUCTOR();

	m_detector_type = "undefined";
	m_detector_model = "undefined";
	m_detector_firmware_version = "undefined";
	m_detector_software_version = "undefined";
	m_module_firmware_version = "undefined";
	m_depth = 16;
	m_acq_frame_nb = 0;
	try
	{
		ufxclib::T_UfxcLibCnx tcpCnx, SFPpCnx1, SFPpCnx2, SFPpCnx3;
		tcpCnx.ip_address = config_ip_address;
		tcpCnx.configuration_port = config_port;
		tcpCnx.socket_timeout_ms = timeout_ms;
		tcpCnx.protocol = ufxclib::T_Protocol::TCP;

		SFPpCnx1.ip_address = SFP1_ip_address;
		SFPpCnx1.configuration_port = SFP1_port;
		SFPpCnx1.socket_timeout_ms = timeout_ms;
		SFPpCnx1.protocol = ufxclib::T_Protocol::UDP;

		SFPpCnx2.ip_address = SFP2_ip_address;
		SFPpCnx2.configuration_port = SFP2_port;
		SFPpCnx2.socket_timeout_ms = timeout_ms;
		SFPpCnx2.protocol = ufxclib::T_Protocol::UDP;

		SFPpCnx3.ip_address = SFP3_ip_address;
		SFPpCnx3.configuration_port = SFP3_port;
		SFPpCnx3.socket_timeout_ms = timeout_ms;
		SFPpCnx3.protocol = ufxclib::T_Protocol::UDP;

		//- prepare the registers
		SetHardwareRegisters();

		//- create the main ufxc object
		m_ufxc_interface = new ufxclib::UFXCInterface();

		//- connect to the DAQ/Detector
		m_ufxc_interface->open_connection(tcpCnx, SFPpCnx1, SFPpCnx2, SFPpCnx3);

		//- set the registers to the DAQ
		m_ufxc_interface->get_config_acquisition_obj()->set_acquisition_registers_names(m_acquisition_registers);
		m_ufxc_interface->get_config_detector_obj()->set_detector_registers_names(m_detector_registers);
		m_ufxc_interface->get_daq_monitoring_obj()->set_monitoring_registers_names(m_monitor_registers);

		//soft_reset() once when init device
		////m_ufxc_interface->get_config_acquisition_obj()->soft_reset();
		//fix the acquisition mode
		m_ufxc_interface->get_config_acquisition_obj()->set_acq_mode(ufxclib::T_AcquisitionMode::software_raw);//pump_and_probe software_raw		
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::Camera() :"
		 << "\nreason : "
		 << ue.errors[0].reason
		 << "\ndesc : "
		 << ue.errors[0].desc
		 << "\norigin : "
		 << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
	catch(...)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::Camera() : Unknown error" << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg;
	}

	m_acq_thread = new AcqThread(*this);
	m_acq_thread->start();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
Camera::~Camera()
{
	DEB_DESTRUCTOR();

	//delete the acquisition thread
	delete m_acq_thread;

	// releasing the detector control instance
	DEB_TRACE() << "Camera::Camera - releasing the detector control instance";

	if(m_ufxc_interface != NULL)
	{
		m_ufxc_interface->close_connection();

		delete m_ufxc_interface;
		m_ufxc_interface = NULL;
	}

}


//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::reset()
{
	DEB_MEMBER_FUNCT();
	stopAcq();
	//@BEGIN : other stuff on Driver/API
	//...
	//@END
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::prepareAcq()
{
	DEB_MEMBER_FUNCT();
	//@BEGIN : some stuff on Driver/API before start acquisition
	// Only snap is allowed
	if(m_nb_frames == 0LL)
		THROW_HW_ERROR(ErrorType::Error) << "Start mode is not allowed for this device! Please use Snap mode.";

	//@END	
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::startAcq()
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	m_acq_frame_nb = 0;
	StdBufferCbMgr& buffer_mgr = m_bufferCtrlObj.getBuffer();
	buffer_mgr.setStartTimestamp(Timestamp::now());
	DEB_TRACE() << "Ensure that Acquisition is Started  ";
	setStatus(Camera::Busy, false);
	//@BEGIN : Ensure that Acquisition is Started before return ...
	m_ufxc_interface->get_config_acquisition_obj()->start_acquisition();
	//@END

	//Start acquisition thread
	m_wait_flag = false;
	m_quit = false;
	m_cond.broadcast();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::stopAcq()
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	// Dont do anything if acquisition is idle.
	if(m_thread_running == true)
	{
		m_wait_flag = true;
		m_cond.broadcast();
		DEB_TRACE() << "stop requested ";
	}

	//@BEGIN : Ensure that Acquisition is Stopped before return ...	
	//...
	//@END
	//now detector is ready
	setStatus(Camera::Ready, false);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getStatus(Camera::Status& status)
{
	DEB_MEMBER_FUNCT();

	ufxclib::T_DetectorStatus det_status;

	// getting the detector status
	det_status = m_ufxc_interface->get_daq_monitoring_obj()->get_detector_status();

	switch(det_status)
	{
		case ufxclib::T_DetectorStatus::E_DET_READY:
			m_status = Camera::Ready;
			//DEB_TRACE() << "E_DET_READY";
			break;
		case ufxclib::T_DetectorStatus::E_DET_BUSY:
			m_status = Camera::Busy;
			//DEB_TRACE() << "E_DET_BUSY";
			break;
		case ufxclib::T_DetectorStatus::E_DET_DELAY_SCANNING:
		case ufxclib::T_DetectorStatus::E_DET_CONFIGURING:
			m_status = Camera::Configuring;
			//DEB_TRACE() << "E_DET_CONFIGURING";
			break;
		case ufxclib::T_DetectorStatus::E_DET_NOT_CONFIGURED:
			m_status = Camera::Ready;
//			DEB_TRACE() << "E_DET_NOT_CONFIGURED";
			break;			
		case ufxclib::T_DetectorStatus::E_DET_ERROR:
			m_status = Camera::Fault;
			DEB_TRACE() << "E_DET_ERROR";
			break;
	}

	status = m_status;

	DEB_RETURN() << DEB_VAR1(status);
}

//-----------------------------------------------------
// @brief set the new camera status
//-----------------------------------------------------
void Camera::setStatus(Camera::Status status, bool force)
{
	DEB_MEMBER_FUNCT();
	//AutoMutex aLock(m_cond.mutex());
	if(force || m_status != Camera::Fault)
		m_status = status;
	//m_cond.broadcast();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::readFrame(void)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Camera::readFrame() ";
	void* bptr;
	StdBufferCbMgr& buffer_mgr = m_bufferCtrlObj.getBuffer();
	//@BEGIN : 
	//get Acquisitions images
	size_t IMAGE_DATA_SIZE = (m_ufxc_interface->get_data_receiver_obj()->get_frame_number_for_2_counters(ufxclib::T_AcquisitionMode::software_raw) / COUNTER_NUMBER) * PACKET_DATA_LENGTH;
	//= 112*1024+6; 112 is the frames number in each image, 1024 is the data size of each image. 
	//header = 6 is the header size for each image and already extracted by the lib
	char ** imgBuffer;
	imgBuffer = new char *[2 * m_nb_frames];
	for(int i = 0;i < (2 * m_nb_frames);i++)
		imgBuffer[i] = new char[IMAGE_DATA_SIZE];
	DEB_TRACE()<<"nb. frames (requested) = "<<m_nb_frames;		
	size_t frames_number = 0;
	m_ufxc_interface->get_data_receiver_obj()->get_all_images(imgBuffer, ufxclib::T_AcquisitionMode::software_raw, m_nb_frames, frames_number);
	//!< calculate the received images number for two counters
	size_t received_images_number = frames_number / m_ufxc_interface->get_data_receiver_obj()->get_frame_number_for_2_counters(ufxclib::T_AcquisitionMode::software_raw);
	DEB_TRACE()<<"nb. frames (received) = "<<received_images_number<<"\n";
	std::stringstream filename("");	
	std::ofstream output_file;
	for(int i = 0;i < (received_images_number * 2);i++)
	{		
		if(i % 2 == 0)
		{
			//Prepare Lima Frame Ptr 
			DEB_TRACE() << "Prepare  Lima Frame Ptr("<<i/2<<")";			
			bptr = buffer_mgr.getFrameBufferPtr(i/2);	
			//memset(bptr, 0, 512*256*2);
			filename.str("");
			filename<<"/home/informatique/ica/noureddine/DeviceServers/ufxc_data_"<<i/2<<".dat";			
			DEB_TRACE()<<"filename = "<<filename.str();
			output_file.open(filename.str(), std::ios::out | std::ofstream::binary);
		}		
		
		for(int j = 0;j < IMAGE_DATA_SIZE;j++)
		{
			output_file << int((unsigned char) (imgBuffer[i][j])) << " ";
		}
		//next line for the 2nd counter
		output_file << std::endl;
		
		if(i % 2 == 1 )
		{			
			output_file.close();
			//Push the image buffer through Lima 
			DEB_TRACE() << "Declare a Lima new Frame Ready (" << m_acq_frame_nb << ")\n";
			HwFrameInfoType frame_info;
			frame_info.acq_frame_nb = m_acq_frame_nb;
			buffer_mgr.newFrameReady(frame_info);
			m_acq_frame_nb++;
		}
	}

	//@END	
	m_status = Camera::Busy;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
int Camera::getNbHwAcquiredFrames()
{
	DEB_MEMBER_FUNCT();
	return m_acq_frame_nb;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::AcqThread::threadFunction()
{
	DEB_MEMBER_FUNCT();
	//	AutoMutex aLock(m_cam.m_cond.mutex());


	while(!m_cam.m_quit)
	{
		while(m_cam.m_wait_flag && !m_cam.m_quit)
		{
			DEB_TRACE() << "Wait for start acquisition";
			m_cam.m_thread_running = false;
			AutoMutex lock(m_cam.m_cond.mutex());
			m_cam.m_cond.wait();
		}

		if(m_cam.m_quit)
			return;
		DEB_TRACE() << "AcqThread Running";
		m_cam.m_thread_running = true;

		//auto t1 = Clock::now();

		bool continueFlag = true;
		while(continueFlag && (!m_cam.m_nb_frames || m_cam.m_acq_frame_nb < m_cam.m_nb_frames))
		{
			// Check first if acq. has been stopped
			DEB_TRACE() << "AcqThread : Check first if acq. has been stopped ";
			if(m_cam.m_wait_flag)
			{
				DEB_TRACE() << "AcqThread: has been stopped from user ";
				continueFlag = false;
				continue;
			}

			Camera::Status status;
			m_cam.getStatus(status);
			while(status == Camera::Busy)
			{
				//refresh status
				m_cam.getStatus(status);
				usleep(10000);//wait 10ms
			}
			
			//Read Frame From API/Driver/Etc ... & Copy it into Lima Frame Ptr
			DEB_TRACE() << "Read Frame From API/Driver/Etc ... & Copy it into Lima Frame Ptr";				
			m_cam.readFrame();
		}
		//auto t2 = Clock::now();
		//DEB_TRACE() << "Delta t2-t1: " << std::chrono::duration_cast < std::chrono::nanoseconds > (t2 - t1).count() << " nanoseconds";

		//stopAcq only if this is not already done		
		DEB_TRACE() << "AcqThread : stopAcq only if this is not already done ";
		if(!m_cam.m_wait_flag)
		{
			DEB_TRACE() << " AcqThread: StopAcq";
			m_cam.stopAcq();
		}

		DEB_TRACE() << " AcqThread::threadfunction() Setting thread running flag to false";
		AutoMutex lock(m_cam.m_cond.mutex());
		//		aLock.lock();
		m_cam.m_thread_running = false;
		m_cam.m_wait_flag = true;
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
Camera::AcqThread::AcqThread(Camera& cam):
m_cam(cam)
{
	AutoMutex aLock(m_cam.m_cond.mutex());
	m_cam.m_wait_flag = true;
	m_cam.m_quit = false;
	aLock.unlock();
	pthread_attr_setscope(&m_thread_attr, PTHREAD_SCOPE_PROCESS);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
Camera::AcqThread::~AcqThread()
{
	AutoMutex aLock(m_cam.m_cond.mutex());
	m_cam.m_quit = true;
	m_cam.m_cond.broadcast();
	aLock.unlock();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getImageType(ImageType& type)
{
	DEB_MEMBER_FUNCT();
	switch(m_depth)
	{
		case 16: type = Bpp16;
			break;
		default:
			THROW_HW_ERROR(Error) << "This pixel format of the camera is not managed, only 16 bits cameras are already managed!";
			break;
	}
	return;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setImageType(ImageType type)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Camera::setImageType - " << DEB_VAR1(type);
	switch(type)
	{
		case Bpp16:
		{
			m_depth = 16;
		}
			break;
		default:
			THROW_HW_ERROR(Error) << "This pixel format of the camera is not managed, only 16 bits cameras are already managed!";
			break;
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getDetectorType(std::string& type)
{
	DEB_MEMBER_FUNCT();
	//@BEGIN : Get Detector type from Driver/API
	type = m_detector_type;
	//@END	
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getDetectorModel(std::string& model)
{
	DEB_MEMBER_FUNCT();
	//@BEGIN : Get Detector model from Driver/API
	model = m_detector_model;
	//@END		
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getDetectorImageSize(Size& size)
{
	DEB_MEMBER_FUNCT();
	//@BEGIN : Get Detector type from Driver/API	
	unsigned width = m_ufxc_interface->get_config_acquisition_obj()->get_current_width();
	unsigned height = m_ufxc_interface->get_config_acquisition_obj()->get_current_height();
	size = Size(width * 2, 256);
	//@END
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getPixelSize(double& sizex, double& sizey)
{
	DEB_MEMBER_FUNCT();
	//@BEGIN : Get Detector type from Driver/API			
	sizex = xPixelSize;
	sizey = yPixelSize;
	//@END
}

//-----------------------------------------------------
//
//-----------------------------------------------------
HwBufferCtrlObj* Camera::getBufferCtrlObj()
{
	return &m_bufferCtrlObj;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
bool Camera::checkTrigMode(TrigMode mode)
{
	DEB_MEMBER_FUNCT();
	bool valid_mode;

	switch(mode)
	{
		case IntTrig:
		case IntTrigMult:
		case ExtTrigSingle:
		case ExtTrigMult:
		case ExtGate:
			valid_mode = true;
			break;
		case ExtTrigReadout:
		default:
			valid_mode = false;
			break;
	}
	return valid_mode;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setTrigMode(TrigMode mode)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Camera::setTrigMode() " << DEB_VAR1(mode);
	DEB_PARAM() << DEB_VAR1(mode);
	switch(mode)
	{
		case IntTrig:
		case IntTrigMult:
			break;
		case ExtTrigSingle:
			break;
		case ExtTrigMult:
			break;
		case ExtGate:
			break;
		default:
			THROW_HW_ERROR(NotSupported) << DEB_VAR1(mode);
	}
	m_trigger_mode = mode;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getTrigMode(TrigMode& mode)
{
	DEB_MEMBER_FUNCT();
	mode = m_trigger_mode;
	DEB_RETURN() << DEB_VAR1(mode);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getExpTime(double& exp_time)
{
	DEB_MEMBER_FUNCT();
	//UFXCLib use (ms), but lima use (second) as unit
	exp_time = m_ufxc_interface->get_config_acquisition_obj()->get_counting_time_ms();
	exp_time = exp_time / 1000;
	m_exp_time = exp_time;
	DEB_RETURN() << DEB_VAR1(exp_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setExpTime(double exp_time)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Camera::setExpTime() " << DEB_VAR1(exp_time);
	//UFXCLib use (ms), but lima use (second) as unit
	m_ufxc_interface->get_config_acquisition_obj()->set_counting_time_ms(exp_time * 1000);
	m_exp_time = exp_time;
}


//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setLatTime(double lat_time)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Camera::setLatTime() " << DEB_VAR1(lat_time);
	//UFXCLib use (ms), but lima use (second) as unit
	m_ufxc_interface->get_config_acquisition_obj()->set_waiting_time_ms(lat_time * 1000);
	m_lat_time = lat_time;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getLatTime(double& lat_time)
{
	DEB_MEMBER_FUNCT();
	//UFXCLib use (ms), but lima use (second) as unit 
	lat_time = m_ufxc_interface->get_config_acquisition_obj()->get_waiting_time_ms();
	lat_time = lat_time / 1000;
	m_lat_time = lat_time;
	DEB_RETURN() << DEB_VAR1(lat_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getExposureTimeRange(double& min_expo, double& max_expo) const
{
	DEB_MEMBER_FUNCT();
	min_expo = 0.;
	max_expo = 10;//10s
	DEB_RETURN() << DEB_VAR2(min_expo, max_expo);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getLatTimeRange(double& min_lat, double& max_lat) const
{
	DEB_MEMBER_FUNCT();
	// --- no info on min latency
	min_lat = 0.;
	// --- do not know how to get the max_lat, fix it as the max exposure time
	max_lat = 10;//10s
	DEB_RETURN() << DEB_VAR2(min_lat, max_lat);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setNbFrames(int nb_frames)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Camera::setNbFrames() " << DEB_VAR1(nb_frames);
	if(m_nb_frames < 0)
	{
		THROW_HW_ERROR(Error) << "Number of frames to acquire has not been set";
	}
	m_ufxc_interface->get_config_acquisition_obj()->set_images_number(nb_frames);
	m_nb_frames = nb_frames;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getNbFrames(int& nb_frames)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Camera::getNbFrames";
	DEB_RETURN() << DEB_VAR1(m_nb_frames);
	nb_frames = m_ufxc_interface->get_config_acquisition_obj()->get_images_number();
	m_nb_frames = nb_frames;
}


///////////////////////////////////////////////////
// Ufxc specific stuff now
///////////////////////////////////////////////////////
void Camera::get_lib_version(std::string & version)
{
	version = m_ufxc_interface->get_UFXC_lib_version();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::get_firmware_version(std::string & version)
{
	version = m_ufxc_interface->get_daq_monitoring_obj()->get_firmware_version();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::get_detector_temperature(unsigned long temp)
{
	temp = m_ufxc_interface->get_daq_monitoring_obj()->get_detector_temp();
}
/*******************************************************
 * \brief Set the Hardware registers in the DAQ system
 *******************************************************/
void Camera::SetHardwareRegisters()
{
	m_acquisition_registers[ufxclib::T_AcquisitionConfigKey::DET_THRESHOLD_LOW_1] = "FMC.DET_THRESHOLD_LOW_1";
	m_acquisition_registers[ufxclib::T_AcquisitionConfigKey::DET_THRESHOLD_LOW_2] = "FMC.DET_THRESHOLD_LOW_2";
	m_acquisition_registers[ufxclib::T_AcquisitionConfigKey::DET_THRESHOLD_HIGH_1] = "FMC.DET_THRESHOLD_HIGH_1";
	m_acquisition_registers[ufxclib::T_AcquisitionConfigKey::DET_THRESHOLD_HIGH_2] = "FMC.DET_THRESHOLD_HIGH_2";
	m_acquisition_registers[ufxclib::T_AcquisitionConfigKey::ACQ_MODE] = "FMC.ACQ_MODE";
	m_acquisition_registers[ufxclib::T_AcquisitionConfigKey::ACQ_COUNT_TIME] = "FMC.ACQ_COUNT_TIME";
	m_acquisition_registers[ufxclib::T_AcquisitionConfigKey::ACQ_WAIT_TIME] = "FMC.ACQ_WAIT_TIME";
	m_acquisition_registers[ufxclib::T_AcquisitionConfigKey::ACQ_NIMG] = "FMC.ACQ_NIMG";
	m_acquisition_registers[ufxclib::T_AcquisitionConfigKey::ACQ_NTRIG] = "FMC.ACQ_NTRIG";
	m_acquisition_registers[ufxclib::T_AcquisitionConfigKey::StartAcq] = "FMC.StartAcq";
	m_acquisition_registers[ufxclib::T_AcquisitionConfigKey::AbortAcq] = "FMC.AbortAcq";
	m_acquisition_registers[ufxclib::T_AcquisitionConfigKey::SFP_SOFT_RESET] = "SFP.SOFT_RESET";
	m_acquisition_registers[ufxclib::T_AcquisitionConfigKey::FMC_SOFT_RESET] = "FMC.SOFT_RESET";

	m_detector_registers[ufxclib::T_DetectorConfigKey::GLB_POL] = "FMC.GLB_POL";
	m_detector_registers[ufxclib::T_DetectorConfigKey::GLB_FS] = "FMC.GLB_FS";
	m_detector_registers[ufxclib::T_DetectorConfigKey::GLB_BCAS] = "FMC.GLB_BCAS";
	m_detector_registers[ufxclib::T_DetectorConfigKey::GLB_BKRUM] = "FMC.GLB_BKRUM";
	m_detector_registers[ufxclib::T_DetectorConfigKey::GLB_BTRIM] = "FMC.GLB_BTRIM";
	m_detector_registers[ufxclib::T_DetectorConfigKey::GLB_BREF] = "FMC.GLB_BREF";
	m_detector_registers[ufxclib::T_DetectorConfigKey::GLB_BSH] = "FMC.GLB_BSH";
	m_detector_registers[ufxclib::T_DetectorConfigKey::GLB_BDIS] = "FMC.GLB_BDIS";
	m_detector_registers[ufxclib::T_DetectorConfigKey::GLB_BGSH] = "FMC.GLB_BGSH";
	m_detector_registers[ufxclib::T_DetectorConfigKey::GLB_BR] = "FMC.GLB_BR";
	m_detector_registers[ufxclib::T_DetectorConfigKey::DetectorConfig] = "FMC.DetectorConfig";
	m_detector_registers[ufxclib::T_DetectorConfigKey::PIXCONF_BITLINE_NBR] = "FMC.PIXCONF_BITLINE_NBR";
	m_detector_registers[ufxclib::T_DetectorConfigKey::PIXCONF_COL_31_0_A] = "FMC.PIXCONF_COL_31_0_A";
	m_detector_registers[ufxclib::T_DetectorConfigKey::PIXCONF_COL_63_32_A] = "FMC.PIXCONF_COL_63_32_A";
	m_detector_registers[ufxclib::T_DetectorConfigKey::PIXCONF_COL_95_64_A] = "FMC.PIXCONF_COL_95_64_A";
	m_detector_registers[ufxclib::T_DetectorConfigKey::PIXCONF_COL_127_96_A] = "FMC.PIXCONF_COL_127_96_A";
	m_detector_registers[ufxclib::T_DetectorConfigKey::PIXCONF_COL_31_0_B] = "FMC.PIXCONF_COL_31_0_B";
	m_detector_registers[ufxclib::T_DetectorConfigKey::PIXCONF_COL_63_32_B] = "FMC.PIXCONF_COL_63_32_B";
	m_detector_registers[ufxclib::T_DetectorConfigKey::PIXCONF_COL_95_64_B] = "FMC.PIXCONF_COL_95_64_B";
	m_detector_registers[ufxclib::T_DetectorConfigKey::PIXCONF_COL_127_96_B] = "FMC.PIXCONF_COL_127_96_B";
	m_detector_registers[ufxclib::T_DetectorConfigKey::PixLineConfig] = "FMC.PixLineConfig";


	m_monitor_registers[ufxclib::T_MonitoringKey::TEMP_DAQ_PICO] = "SLOW.TEMP_DAQ_PICO";
	m_monitor_registers[ufxclib::T_MonitoringKey::TEMP_DAQ_SFP] = "SLOW.TEMP_DAQ_SFP";
	m_monitor_registers[ufxclib::T_MonitoringKey::FW_DELAY] = "FMC.FW_DELAY";
	m_monitor_registers[ufxclib::T_MonitoringKey::TEMP_DAQ_PSU] = "SLOW.TEMP_DAQ_PSU";
	m_monitor_registers[ufxclib::T_MonitoringKey::TEMP_DET] = "SLOW.TEMP_DET";
	m_monitor_registers[ufxclib::T_MonitoringKey::ALIM_DET_P_HV_CURRENT] = "SLOW.ALIM_DET_P_HV_CURRENT";
	m_monitor_registers[ufxclib::T_MonitoringKey::ALIM_DET_P_HV_CHIP] = "SLOW.ALIM_DET_P_HV_CHIP";
	m_monitor_registers[ufxclib::T_MonitoringKey::ALIM_DET_1V2_CORE] = "SLOW.ALIM_DET_1V2_CORE";
	m_monitor_registers[ufxclib::T_MonitoringKey::ALIM_DET_0V8_VDDM] = "SLOW.ALIM_DET_0V8_VDDM";
	m_monitor_registers[ufxclib::T_MonitoringKey::ALIM_DET_1V2_DISC_A] = "SLOW.ALIM_DET_1V2_DISC_A";
	m_monitor_registers[ufxclib::T_MonitoringKey::ALIM_DET_1V2_DISC_B] = "SLOW.ALIM_DET_1V2_DISC_B";
	m_monitor_registers[ufxclib::T_MonitoringKey::ALIM_DET_1V2_VDDA_A] = "SLOW.ALIM_DET_1V2_VDDA_A";
	m_monitor_registers[ufxclib::T_MonitoringKey::ALIM_DET_1V2_VDDA_B] = "SLOW.ALIM_DET_1V2_VDDA_B";
	m_monitor_registers[ufxclib::T_MonitoringKey::DETECTOR_STATUS] = "FMC.DETECTOR_STATUS";
	m_monitor_registers[ufxclib::T_MonitoringKey::DELAY_SCAN] = "FMC.Delay_scan";
	m_monitor_registers[ufxclib::T_MonitoringKey::Abortdelay] = "FMC.AbortAcq";
	m_monitor_registers[ufxclib::T_MonitoringKey::FIRMWARE_VERSION] = "*IDN";
	m_monitor_registers[ufxclib::T_MonitoringKey::EN_PIXCONF_SCANDELAY_SFP] = "FMC.EN_PIXCONF_SCANDELAY_SFP";
}
