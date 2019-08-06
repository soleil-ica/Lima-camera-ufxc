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
#include <sys/time.h>
#include <ctime>
#include <cstdint>

#include "constants.h"
#include "decode14b.h"
#include "decode2b.h"
#include "tools.h"

using namespace lima;
using namespace lima::Ufxc;
using namespace std;


//---------------------------
// @brief  Ctor
//---------------------------
/************************************************************************
 * \brief constructor
 ************************************************************************/
Camera::Camera(	const std::string& TCP_ip_address, unsigned long TCP_port,
				const std::string& SFP1_ip_address, unsigned long SFP1_port,
				const std::string& SFP2_ip_address, unsigned long SFP2_port,
				const std::string& SFP3_ip_address, unsigned long SFP3_port,
				unsigned long timeout_ms,
				bool is_geometrical_correction_enabled,
				bool is_stack_frames_sum_enabled)
{
	DEB_CONSTRUCTOR();

	m_detector_type = "undefined";
	m_detector_model = "undefined";
	m_detector_firmware_version = "undefined";
	m_detector_software_version = "undefined";
	m_module_firmware_version = "undefined";
	m_depth = 14;
	m_acq_frame_nb = 0;
	m_nb_frames = 1;
	m_pump_probe_trigger_acquisition_frequency = 1;
	m_pump_probe_nb_frames = 1;		
	m_is_geometrical_correction_enabled = is_geometrical_correction_enabled;		
    m_is_stack_frames_sum_enabled = is_stack_frames_sum_enabled;    
	DEB_TRACE()<<"m_is_geometrical_correction_enabled = "<<m_is_geometrical_correction_enabled;
	DEB_TRACE()<<"m_is_stack_frames_sum_enabled = "<<m_is_stack_frames_sum_enabled;
	try
	{
		ufxclib::DaqCnxConfig TCP_cnx, SFP1_cnx, SFP2_cnx, SFP3_cnx;
		TCP_cnx.ip_address = TCP_ip_address;
		TCP_cnx.configuration_port = TCP_port;
		TCP_cnx.socket_timeout_ms = timeout_ms;
		TCP_cnx.protocol = ufxclib::EnumProtocol::TCP;

		SFP1_cnx.ip_address = SFP1_ip_address;
		SFP1_cnx.configuration_port = SFP1_port;
		SFP1_cnx.socket_timeout_ms = timeout_ms;
		SFP1_cnx.protocol = ufxclib::EnumProtocol::UDP;

		SFP2_cnx.ip_address = SFP2_ip_address;
		SFP2_cnx.configuration_port = SFP2_port;
		SFP2_cnx.socket_timeout_ms = timeout_ms;
		SFP2_cnx.protocol = ufxclib::EnumProtocol::UDP;

		SFP3_cnx.ip_address = SFP3_ip_address;
		SFP3_cnx.configuration_port = SFP3_port;
		SFP3_cnx.socket_timeout_ms = timeout_ms;
		SFP3_cnx.protocol = ufxclib::EnumProtocol::UDP;

		//- prepare the registers
		SetHardwareRegisters();

		//- create the main ufxc object
		m_ufxc_interface = new ufxclib::UFXCInterface();

		//- connect to the DAQ/Detector
		m_ufxc_interface->open_connection(TCP_cnx, SFP1_cnx, SFP2_cnx, SFP3_cnx);

		//- set the registers to the DAQ
		m_ufxc_interface->get_config_acquisition_obj()->set_acquisition_registers_names(m_acquisition_registers);
		m_ufxc_interface->get_config_detector_obj()->set_detector_registers_names(m_detector_registers);
		m_ufxc_interface->get_daq_monitoring_obj()->set_monitoring_registers_names(m_monitor_registers);
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::Camera() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
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
	m_ufxc_interface->get_config_acquisition_obj()->stop_acquisition();
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
	AutoMutex aLock(m_cond.mutex());
	ufxclib::EnumDetectorStatus det_status;

	// getting the detector status
	det_status = m_ufxc_interface->get_daq_monitoring_obj()->get_detector_status();

	switch(det_status)
	{
		case ufxclib::EnumDetectorStatus::E_DET_READY:
			m_status = Camera::Ready;
			//DEB_TRACE() << "E_DET_READY";
			break;
		case ufxclib::EnumDetectorStatus::E_DET_BUSY:
			m_status = Camera::Busy;
			//DEB_TRACE() << "E_DET_BUSY";
			break;
		case ufxclib::EnumDetectorStatus::E_DET_DELAY_SCANNING:
		case ufxclib::EnumDetectorStatus::E_DET_CONFIGURING:
			m_status = Camera::Configuring;
			//DEB_TRACE() << "E_DET_CONFIGURING";
			break;
		case ufxclib::EnumDetectorStatus::E_DET_NOT_CONFIGURED:
			m_status = Camera::Ready;
			//DEB_TRACE() << "E_DET_NOT_CONFIGURED";
			break;
		case ufxclib::EnumDetectorStatus::E_DET_ERROR:
			m_status = Camera::Fault;
			DEB_TRACE() << "E_DET_ERROR";
			break;
	}

	status = m_status;
	aLock.unlock();
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
	DEB_TRACE() << "PACKET_DATA_LENGTH = " << PACKET_DATA_LENGTH;
	AutoMutex aLock(m_cond.mutex());
	ufxclib::EnumAcquisitionMode mode = m_ufxc_interface->get_config_acquisition_obj()->get_acq_mode();
	size_t IMAGE_DATA_SIZE = (m_ufxc_interface->get_data_receiver_obj()->get_frame_number_for_2_counters(mode) / COUNTER_NUMBER) * PACKET_DATA_LENGTH;
	aLock.unlock();
	DEB_TRACE() << "IMAGE_DATA_SIZE = " << IMAGE_DATA_SIZE;
	
	//allocate memory
	Timestamp t0_allocate = Timestamp::now();
	uint8_t** imgBuffer;
	int nb_frames = (m_depth == 14 ? m_nb_frames : m_pump_probe_nb_frames);

	try
	{
		imgBuffer = new uint8_t*[2 * nb_frames];
		for(int i = 0;i < (2 * nb_frames);i++)
			imgBuffer[i] = new uint8_t[IMAGE_DATA_SIZE];
	}
	catch(const std::bad_alloc& e)
	{
		DEB_ERROR() << "Allocation failed: " << e.what();
		THROW_HW_ERROR(Error) << e.what();
	}
	
	Timestamp t1_allocate = Timestamp::now();
	double delta_time_allocate = t1_allocate - t0_allocate;
	DEB_TRACE() << "---- Elapsed time of allocation memory = " << (int) (delta_time_allocate * 1000) << " (ms)";

	DEB_TRACE() << "nb. frames (requested) = " << nb_frames;

	Timestamp t0_get_all_images = Timestamp::now();
	////aLock.lock();
	size_t frames_number = 0;
	//get all images (two counters)
	m_ufxc_interface->get_data_receiver_obj()->get_all_images((char**) imgBuffer, mode, nb_frames, frames_number);
	//calculate the received images number for two counters
	size_t received_images_number = frames_number / m_ufxc_interface->get_data_receiver_obj()->get_frame_number_for_2_counters(mode);
	////aLock.unlock();
	Timestamp t1_get_all_images = Timestamp::now();
	double delta_time_get_all_images = t1_get_all_images - t0_get_all_images;
	DEB_TRACE() << "---- Elapsed time of get_all_images() = " << (int) (delta_time_get_all_images * 1000) << " (ms)";

	DEB_TRACE() << "nb. frames (received) = " << received_images_number;

	double delta_time_all_new_frame_ready = 0;
#ifdef USE_WRITE_FILE 
	Timestamp t0_write_file = Timestamp::now();
	std::stringstream filename("");
	std::ofstream output_file;
	//generate 1 file for all images
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d-%X", &tstruct);

	double delta_time_all_write_file = 0;
	Timestamp t1_write_file = Timestamp::now();
	filename.str("");
	filename << "/dev/shm/ufxc/ufxc-data-" << buf << ".dat";
	DEB_TRACE() << "filename = " << filename.str();
	output_file.open(filename.str(), std::ios::out | std::ofstream::binary);

	for(int i = 0;i < (received_images_number * 2);i++)
	{
		if(i % 2 == 0)
		{
			//Prepare Lima Frame Ptr 
			//DEB_TRACE() << "Prepare  Lima Frame Ptr("<<i/2<<")";			
			bptr = buffer_mgr.getFrameBufferPtr(i / 2);
		}

		Timestamp t0_write_file = Timestamp::now();
		for(int j = 0;j < IMAGE_DATA_SIZE;j++)
		{
			output_file << int((unsigned char) (imgBuffer[i][j])) << " ";
		}

		//next line for the 2nd counter
		output_file << std::endl;

		Timestamp t1_write_file = Timestamp::now();
		delta_time_all_write_file += (t1_write_file - t0_write_file);

		Timestamp t0_new_frame_ready = Timestamp::now();
		if(i % 2 == 1)
		{
			//Push the image buffer through Lima 
			//DEB_TRACE() << "Declare a Lima new Frame Ready (" << m_acq_frame_nb << ")";
			HwFrameInfoType frame_info;
			frame_info.acq_frame_nb = m_acq_frame_nb;
			buffer_mgr.newFrameReady(frame_info);
			m_acq_frame_nb++;
		}
		Timestamp t1_new_frame_ready = Timestamp::now();
		delta_time_all_new_frame_ready += (t1_new_frame_ready - t0_new_frame_ready);

	}

	output_file.close();//generate 1 file for all images
	DEB_TRACE() << "---- Elapsed time of all write file = " << (int) (delta_time_all_write_file * 1000) << " (ms)";

	DEB_TRACE() << "---- Elapsed time of all newFrameReady = " << (int) (delta_time_all_new_frame_ready * 1000) << " (ms)";
#endif

#ifdef USE_DECODE_IMAGE 
	double delta_time_all_decoding_image = 0;

	if(m_depth == 2)
	{
		int nb_loop = (m_is_stack_frames_sum_enabled?1:received_images_number);
		for(int i = 0;i < nb_loop;i++)
		{
			//Prepare Lima Frame Ptr 
			//DEB_TRACE() << "Prepare  Lima Frame Ptr("<<i<<")";			
			bptr = buffer_mgr.getFrameBufferPtr(i);

			Timestamp t0_decoding_image = Timestamp::now();
			DEB_TRACE() << "decoding 2 bits image...";
			unsigned width 	= (m_is_geometrical_correction_enabled?512+2:512);
			unsigned height = (m_is_stack_frames_sum_enabled?512:256);
			unsigned nb_bytes = (m_is_stack_frames_sum_enabled?4:1);//32bits if frames summed, 8 bits otherwise

			memset(bptr, 0, width * height * nb_bytes);

			if(m_is_stack_frames_sum_enabled)				
				decode_image2_pumpprobe((uint8_t**)imgBuffer, (uint32_t*)bptr, received_images_number, m_is_geometrical_correction_enabled);				
			else
				THROW_HW_ERROR(Error) << "This decoding mode is not yet implemented !";//decode_image2_twocnts((uint8_t*)imgBuffer[i*2], (uint8_t*)imgBuffer[i*2+1], (uint8_t*)bptr);
			
			Timestamp t1_decoding_image = Timestamp::now();
			delta_time_all_decoding_image += (t1_decoding_image - t0_decoding_image);

			Timestamp t0_new_frame_ready = Timestamp::now();

			//Push the image buffer through Lima 
			//DEB_TRACE() << "Declare a Lima new Frame Ready (" << m_acq_frame_nb << ")";
			HwFrameInfoType frame_info;
			frame_info.acq_frame_nb = m_acq_frame_nb;
			buffer_mgr.newFrameReady(frame_info);
			m_acq_frame_nb++;

			Timestamp t1_new_frame_ready = Timestamp::now();
			delta_time_all_new_frame_ready += (t1_new_frame_ready - t0_new_frame_ready);
		}
		DEB_TRACE() << "---- Elapsed time of all decoding images = " << (int) (delta_time_all_decoding_image * 1000) << " (ms)";
		DEB_TRACE() << "---- Elapsed time of all newFrameReady = " << (int) (delta_time_all_new_frame_ready * 1000) << " (ms)";
	}
	else //(m_depth == 14)
	{
		for(int i = 0;i < received_images_number;i++)
		{
			//Prepare Lima Frame Ptr 
			//DEB_TRACE() << "Prepare  Lima Frame Ptr("<<i<<")";			
			bptr = buffer_mgr.getFrameBufferPtr(i);

			Timestamp t0_decoding_image = Timestamp::now();

			if(i == 0)//display this once
				DEB_TRACE() << "decoding 14 bits image...";
			unsigned width 	= (m_is_geometrical_correction_enabled?512+2:512);
			unsigned height 	= 256;	
			unsigned nb_bytes = 2;//always 16 bits, no sum is available in this mode
			memset(bptr, 0, width * height*nb_bytes);
				
			decode_image14_twocnt((uint8_t*) imgBuffer[i * 2], (uint8_t*) imgBuffer[i * 2 + 1], (uint16_t*) bptr, m_is_geometrical_correction_enabled);

			Timestamp t1_decoding_image = Timestamp::now();
			delta_time_all_decoding_image += (t1_decoding_image - t0_decoding_image);

			Timestamp t0_new_frame_ready = Timestamp::now();

			//Push the image buffer through Lima 
			//DEB_TRACE() << "Declare a Lima new Frame Ready (" << m_acq_frame_nb << ")";
			HwFrameInfoType frame_info;
			frame_info.acq_frame_nb = m_acq_frame_nb;
			buffer_mgr.newFrameReady(frame_info);
			m_acq_frame_nb++;

			Timestamp t1_new_frame_ready = Timestamp::now();
			delta_time_all_new_frame_ready += (t1_new_frame_ready - t0_new_frame_ready);
		}
		DEB_TRACE() <<
 "---- Elapsed time of all decoding images = " << (int) (delta_time_all_decoding_image * 1000) << " (ms)";
		DEB_TRACE() << "---- Elapsed time of all newFrameReady = " << (int) (delta_time_all_new_frame_ready * 1000) << " (ms)";
	}
#endif 

	Timestamp t0_deallocate = Timestamp::now();
	//free deallocate double pointer
	if(imgBuffer)
	{
		for(int i = 0;i < (2 * nb_frames);i++)
		{
			delete[] imgBuffer[i];
		}
		delete[] imgBuffer;
	}

	Timestamp t1_deallocate = Timestamp::now();
	double delta_time_deallocate = t1_deallocate - t0_deallocate;
	DEB_TRACE() << "---- Elapsed time of deallocation memory = " << (int) (delta_time_deallocate * 1000) << " (ms)";

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
	AutoMutex aLock(m_cam.m_cond.mutex());

	while(!m_cam.m_quit)
	{
		while(m_cam.m_wait_flag && !m_cam.m_quit)
		{
			DEB_TRACE() << "Wait for start acquisition";
			m_cam.m_thread_running = false;
			m_cam.m_cond.broadcast();
			m_cam.m_cond.wait();
		}

		if(m_cam.m_quit)
			return;
		DEB_TRACE() << "AcqThread Running";
		m_cam.m_thread_running = true;
		m_cam.m_cond.broadcast();
		aLock.unlock();

		bool continueFlag = true;
		while(continueFlag && (!m_cam.m_nb_frames || m_cam.m_acq_frame_nb < m_cam.m_nb_frames))
		{
			// Check first if acq. has been stopped
			//DEB_TRACE() << "AcqThread : Check first if acq. has been stopped ";
			if(m_cam.m_wait_flag)
			{
				DEB_TRACE() << "AcqThread: has been stopped from user ";
				continueFlag = false;
				continue;
			}

			try
			{
				Camera::Status status;
				m_cam.getStatus(status);
				m_cam.m_cond.broadcast();
				if(status == Camera::Busy)
				{
					//DEB_TRACE()<<"Camera Busy ...";
					usleep(400);//UFXCGui use this amount of sleep !
					continue;
				}

				//Read Frame From API/Driver/Etc ... & Copy it into Lima Frame Ptr
				DEB_TRACE() << "Read Frame From API/Driver/Etc ... & Copy it into Lima Frame Ptr";
				m_cam.readFrame();
			}
			catch(const ufxclib::Exception& ue)
			{
				std::ostringstream err_msg;
				err_msg << "Error in AcqThread::threadFunction() :"
				 << "\nreason : " << ue.errors[0].reason
				 << "\ndesc : " << ue.errors[0].desc
				 << "\norigin : " << ue.errors[0].origin
				 << std::endl;
				DEB_ERROR() << err_msg;
				//now detector is ready
				m_cam.setStatus(Camera::Fault, false);
				REPORT_EVENT(err_msg.str())
				THROW_HW_ERROR(Error) << err_msg.str();
			}
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

		//now detector is ready
		m_cam.setStatus(Camera::Ready, false);
		DEB_TRACE() << " AcqThread::threadfunction() Setting thread running flag to false";
		aLock.lock();
		m_cam.m_thread_running = false;
		m_cam.m_wait_flag = true;
		aLock.unlock();
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
	m_cam.m_wait_flag = true;
	m_cam.m_quit = true;
	m_cam.m_cond.broadcast();
	aLock.unlock();
	join();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getImageType(ImageType& type)
{
	DEB_MEMBER_FUNCT();
	switch(m_depth)
	{
		case 2: 
			if(!m_is_stack_frames_sum_enabled)
				type = Bpp2;
			else
				type = Bpp32;
			break;		
		case 14: type = Bpp14;
			break;
		default:
			THROW_HW_ERROR(Error) << "This pixel format of the camera is not managed, only (2 & 14) bits cameras are already managed!";
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
		case Bpp2:
		{
			m_depth = 2;
		}
			break;

		case Bpp14:
		{
			m_depth = 14;
		}
			break;

		default:
			THROW_HW_ERROR(Error) << "This pixel format of the camera is not managed, only (2 & 14) bits cameras are already managed!";
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
	AutoMutex aLock(m_cond.mutex());
	//@BEGIN : Get Detector type from Driver/API	

	int width = 0;
	int height = 0;
	if(m_depth == 2 || m_is_stack_frames_sum_enabled)//@@TODO pour contourner un pb de taille image !
	{
		//unsigned width = m_ufxc_interface->get_config_acquisition_obj()->get_current_width();
		//unsigned height = m_ufxc_interface->get_config_acquisition_obj()->get_current_height();
		width 	= (m_is_geometrical_correction_enabled?512+2:512);
		height = (m_is_stack_frames_sum_enabled?512:256);
        size = Size(width, height);
	}
	else //if(m_depth == 14)
	{
		//unsigned width = m_ufxc_interface->get_config_acquisition_obj()->get_current_width();
		//unsigned height = m_ufxc_interface->get_config_acquisition_obj()->get_current_height();
		width 	= (m_is_geometrical_correction_enabled?512+2:512);
		height 	= 256;	
        size = Size(width, height);	
	}
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
HwEventCtrlObj* Camera::getEventCtrlObj()
{
	return &m_event_ctrl_obj;
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
		case ExtTrigSingle:
		case ExtTrigMult:
			valid_mode = true;
			break;
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
	AutoMutex aLock(m_cond.mutex());
	try
	{
		switch(mode)
		{
			case IntTrig:
				//do not write to hardware (raise exception if try to write to hardware) , but implicitly values are :
				//nbimages = nbFrames
				//nbtrigs = 1
				if(m_depth == 2)
				{
					THROW_HW_ERROR(NotSupported) << DEB_VAR1(mode);
				}
				if(m_depth == 14)
				{
					m_ufxc_interface->get_config_acquisition_obj()->set_acq_mode(ufxclib::EnumAcquisitionMode::software_raw);
					DEB_TRACE() << "Trigger Mode = software_raw (IntTrig - 14 bits)";
				}
				break;
			case ExtTrigSingle:
				//nbimages = nbFrames
				//nbtrigs = 1
				if(m_depth == 2)
				{
					THROW_HW_ERROR(NotSupported) << DEB_VAR1(mode);
				}
				if(m_depth == 14)
				{
					m_ufxc_interface->get_config_acquisition_obj()->set_acq_mode(ufxclib::EnumAcquisitionMode::external_raw);
					DEB_TRACE() << "Trigger Mode = external_raw (ExtTrigSingle - 14 bits)";
				}

				m_ufxc_interface->get_config_acquisition_obj()->set_images_number(m_nb_frames);
				m_ufxc_interface->get_config_acquisition_obj()->set_triggers_number(1);
				break;
			case ExtTrigMult:
				//nbimages = 1
				//nbtrigs = nbFames
				if(m_depth == 2)
				{
					m_ufxc_interface->get_config_acquisition_obj()->set_acq_mode(ufxclib::EnumAcquisitionMode::pump_and_probe_raw);
					m_ufxc_interface->get_config_acquisition_obj()->set_images_number(1);
					m_ufxc_interface->get_config_acquisition_obj()->set_triggers_number(m_pump_probe_nb_frames);
					DEB_TRACE() << "Trigger Mode = pump_and_probe_raw (ExtTrigMult - 2 bits)";
				}
				if(m_depth == 14)
				{
					m_ufxc_interface->get_config_acquisition_obj()->set_acq_mode(ufxclib::EnumAcquisitionMode::external_raw);
					m_ufxc_interface->get_config_acquisition_obj()->set_images_number(1);
					m_ufxc_interface->get_config_acquisition_obj()->set_triggers_number(m_nb_frames);
					DEB_TRACE() << "Trigger Mode = external_raw (ExtTrigMult - 14 bits)";
				}
				break;
			default:
				THROW_HW_ERROR(NotSupported) << DEB_VAR1(mode);
		}
		m_trigger_mode = mode;
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::setTrigMode() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
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
	AutoMutex aLock(m_cond.mutex());
	try
	{
		//UFXCLib use (ms), but lima use (second) as unit
		exp_time = m_ufxc_interface->get_config_acquisition_obj()->get_counting_time_ms();
		exp_time = exp_time / 1000;
		m_exp_time = exp_time;
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::setTrigMode() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
	DEB_RETURN() << DEB_VAR1(exp_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setExpTime(double exp_time)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Camera::setExpTime() " << DEB_VAR1(exp_time);
	AutoMutex aLock(m_cond.mutex());
	try
	{
		//UFXCLib use (ms), but lima use (second) as unit
		m_ufxc_interface->get_config_acquisition_obj()->set_counting_time_ms(exp_time * 1000);
		m_exp_time = exp_time;
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::setExpTime() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setLatTime(double lat_time)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Camera::setLatTime() " << DEB_VAR1(lat_time);
	AutoMutex aLock(m_cond.mutex());
	try
	{
		//UFXCLib use (ms), but lima use (second) as unit
		m_ufxc_interface->get_config_acquisition_obj()->set_waiting_time_ms(lat_time * 1000);
		m_lat_time = lat_time;
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::setLatTime() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getLatTime(double& lat_time)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		//UFXCLib use (ms), but lima use (second) as unit 
		lat_time = m_ufxc_interface->get_config_acquisition_obj()->get_waiting_time_ms();
		lat_time = lat_time / 1000;
		m_lat_time = lat_time;
		DEB_RETURN() << DEB_VAR1(lat_time);
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::getLatTime() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
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
	AutoMutex aLock(m_cond.mutex());
	try
	{
		if(m_nb_frames < 0)
		{
			THROW_HW_ERROR(Error) << "Number of frames to acquire has not been set";
		}

		TrigMode trigger_mode;
		getTrigMode(trigger_mode);
		if(trigger_mode == ExtTrigMult && m_depth == 2)
		{

			m_ufxc_interface->get_config_acquisition_obj()->set_images_number(1);
			m_ufxc_interface->get_config_acquisition_obj()->set_triggers_number(m_pump_probe_nb_frames);
			m_nb_frames = (m_is_stack_frames_sum_enabled?1:m_pump_probe_nb_frames);
		}
		else if(trigger_mode == ExtTrigMult && m_depth == 14)
		{
			m_ufxc_interface->get_config_acquisition_obj()->set_images_number(1);
			m_ufxc_interface->get_config_acquisition_obj()->set_triggers_number(nb_frames);
			m_nb_frames = nb_frames;
		}
		else
		{
			m_ufxc_interface->get_config_acquisition_obj()->set_images_number(nb_frames);
			m_ufxc_interface->get_config_acquisition_obj()->set_triggers_number(1);
			m_nb_frames = nb_frames;
		}
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::setNbFrames() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getNbFrames(int& nb_frames)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		if(m_depth == 2)
		{
			nb_frames = (m_is_stack_frames_sum_enabled?1:m_pump_probe_nb_frames);
			m_nb_frames = nb_frames;
		}
		else //if(m_depth == 14)
		{
			nb_frames = m_ufxc_interface->get_config_acquisition_obj()->get_images_number();
			m_nb_frames = nb_frames;
		}

	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::getNbFrames() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
bool Camera::is_thread_running()
{
	return m_thread_running;
}

///////////////////////////////////////////////////
// Ufxc specific stuff now
///////////////////////////////////////////////////////
void Camera::get_lib_version(std::string & version)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		version = m_ufxc_interface->get_lib_version();
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::get_lib_version() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::get_firmware_version(std::string & version)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		version = m_ufxc_interface->get_daq_monitoring_obj()->get_firmware_version();
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::get_firmware_version() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::get_detector_temperature(unsigned long& temp)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		temp = m_ufxc_interface->get_daq_monitoring_obj()->get_detector_temp();
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::get_detector_temperature() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::set_threshold_Low1(float thr)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		m_ufxc_interface->get_config_acquisition_obj()->set_low_1_threshold(thr);
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::set_threshold_Low1() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------	
void Camera::get_threshold_Low1(unsigned long& thr)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		thr = m_ufxc_interface->get_config_acquisition_obj()->get_low_1_threshold();
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::get_threshold_Low1() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::set_threshold_Low2(float thr)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		m_ufxc_interface->get_config_acquisition_obj()->set_low_2_threshold(thr);
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::set_threshold_Low2() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------	
void Camera::get_threshold_Low2(unsigned long& thr)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		thr = m_ufxc_interface->get_config_acquisition_obj()->get_low_2_threshold();
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::get_threshold_Low2() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::set_threshold_High1(float thr)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		m_ufxc_interface->get_config_acquisition_obj()->set_high_1_threshold(thr);
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::set_threshold_High1() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------	
void Camera::get_threshold_High1(unsigned long& thr)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		thr = m_ufxc_interface->get_config_acquisition_obj()->get_high_1_threshold();
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::get_threshold_High1() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::set_threshold_High2(float thr)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		m_ufxc_interface->get_config_acquisition_obj()->set_high_2_threshold(thr);
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::set_threshold_High2() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------	
void Camera::set_detector_config_file(const std::string& file_name)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		m_ufxc_interface->set_detector_config_file(file_name);
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::set_detector_config_file() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------	
void Camera::get_threshold_High2(unsigned long& thr)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		thr = m_ufxc_interface->get_config_acquisition_obj()->get_high_2_threshold();
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::get_threshold_High2() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : " << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}
/*******************************************************
 * \brief Set the Hardware registers in the DAQ system
 *******************************************************/
void Camera::SetHardwareRegisters()
{
	m_acquisition_registers[ufxclib::EnumAcquisitionConfigKey::DET_THRESHOLD_LOW_1] = "FMC.DET_THRESHOLD_LOW_1";
	m_acquisition_registers[ufxclib::EnumAcquisitionConfigKey::DET_THRESHOLD_LOW_2] = "FMC.DET_THRESHOLD_LOW_2";
	m_acquisition_registers[ufxclib::EnumAcquisitionConfigKey::DET_THRESHOLD_HIGH_1] = "FMC.DET_THRESHOLD_HIGH_1";
	m_acquisition_registers[ufxclib::EnumAcquisitionConfigKey::DET_THRESHOLD_HIGH_2] = "FMC.DET_THRESHOLD_HIGH_2";
	m_acquisition_registers[ufxclib::EnumAcquisitionConfigKey::ACQ_MODE] = "FMC.ACQ_MODE";
	m_acquisition_registers[ufxclib::EnumAcquisitionConfigKey::ACQ_COUNT_TIME] = "FMC.ACQ_COUNT_TIME";
	m_acquisition_registers[ufxclib::EnumAcquisitionConfigKey::ACQ_WAIT_TIME] = "FMC.ACQ_WAIT_TIME";
	m_acquisition_registers[ufxclib::EnumAcquisitionConfigKey::ACQ_NIMG] = "FMC.ACQ_NIMG";
	m_acquisition_registers[ufxclib::EnumAcquisitionConfigKey::ACQ_NTRIG] = "FMC.ACQ_NTRIG";
	m_acquisition_registers[ufxclib::EnumAcquisitionConfigKey::START_ACQ] = "FMC.StartAcq";
	m_acquisition_registers[ufxclib::EnumAcquisitionConfigKey::ABORT_ACQ] = "FMC.AbortAcq";
	m_acquisition_registers[ufxclib::EnumAcquisitionConfigKey::SFP_SOFT_RESET] = "SFP.SOFT_RESET";
	m_acquisition_registers[ufxclib::EnumAcquisitionConfigKey::FMC_SOFT_RESET] = "FMC.SOFT_RESET";

	m_detector_registers[ufxclib::EnumDetectorConfigKey::GLB_POL] = "FMC.GLB_POL";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::GLB_FS] = "FMC.GLB_FS";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::GLB_BCAS] = "FMC.GLB_BCAS";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::GLB_BKRUM] = "FMC.GLB_BKRUM";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::GLB_BTRIM] = "FMC.GLB_BTRIM";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::GLB_BREF] = "FMC.GLB_BREF";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::GLB_BSH] = "FMC.GLB_BSH";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::GLB_BDIS] = "FMC.GLB_BDIS";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::GLB_BGSH] = "FMC.GLB_BGSH";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::GLB_BR] = "FMC.GLB_BR";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::DETECTOR_CONFIG] = "FMC.DetectorConfig";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_BITLINE_NBR] = "FMC.PIXCONF_BITLINE_NBR";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_31_0_A] = "FMC.PIXCONF_COL_31_0_A";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_63_32_A] = "FMC.PIXCONF_COL_63_32_A";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_95_64_A] = "FMC.PIXCONF_COL_95_64_A";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_127_96_A] = "FMC.PIXCONF_COL_127_96_A";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_31_0_B] = "FMC.PIXCONF_COL_31_0_B";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_63_32_B] = "FMC.PIXCONF_COL_63_32_B";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_95_64_B] = "FMC.PIXCONF_COL_95_64_B";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_127_96_B] = "FMC.PIXCONF_COL_127_96_B";
	m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXLINE_CONFIG] = "FMC.PixLineConfig";


	m_monitor_registers[ufxclib::EnumMonitoringKey::TEMP_DAQ_PICO] = "SLOW.TEMP_DAQ_PICO";
	m_monitor_registers[ufxclib::EnumMonitoringKey::TEMP_DAQ_SFP] = "SLOW.TEMP_DAQ_SFP";
	m_monitor_registers[ufxclib::EnumMonitoringKey::FW_DELAY] = "FMC.FW_DELAY";
	m_monitor_registers[ufxclib::EnumMonitoringKey::TEMP_DAQ_PSU] = "SLOW.TEMP_DAQ_PSU";
	m_monitor_registers[ufxclib::EnumMonitoringKey::TEMP_DET] = "SLOW.TEMP_DET";
	m_monitor_registers[ufxclib::EnumMonitoringKey::ALIM_DET_P_HV_CURRENT] = "SLOW.ALIM_DET_P_HV_CURRENT";
	m_monitor_registers[ufxclib::EnumMonitoringKey::ALIM_DET_P_HV_CHIP] = "SLOW.ALIM_DET_P_HV_CHIP";
	m_monitor_registers[ufxclib::EnumMonitoringKey::ALIM_DET_1V2_CORE] = "SLOW.ALIM_DET_1V2_CORE";
	m_monitor_registers[ufxclib::EnumMonitoringKey::ALIM_DET_0V8_VDDM] = "SLOW.ALIM_DET_0V8_VDDM";
	m_monitor_registers[ufxclib::EnumMonitoringKey::ALIM_DET_1V2_DISC_A] = "SLOW.ALIM_DET_1V2_DISC_A";
	m_monitor_registers[ufxclib::EnumMonitoringKey::ALIM_DET_1V2_DISC_B] = "SLOW.ALIM_DET_1V2_DISC_B";
	m_monitor_registers[ufxclib::EnumMonitoringKey::ALIM_DET_1V2_VDDA_A] = "SLOW.ALIM_DET_1V2_VDDA_A";
	m_monitor_registers[ufxclib::EnumMonitoringKey::ALIM_DET_1V2_VDDA_B] = "SLOW.ALIM_DET_1V2_VDDA_B";
	m_monitor_registers[ufxclib::EnumMonitoringKey::DETECTOR_STATUS] = "FMC.DETECTOR_STATUS";
	m_monitor_registers[ufxclib::EnumMonitoringKey::DELAY_SCAN] = "FMC.Delay_scan";
	m_monitor_registers[ufxclib::EnumMonitoringKey::ABORT_DELAY] = "FMC.AbortAcq";
	m_monitor_registers[ufxclib::EnumMonitoringKey::FIRMWARE_VERSION] = "*IDN";
	m_monitor_registers[ufxclib::EnumMonitoringKey::EN_PIXCONF_SCANDELAY_SFP] = "FMC.EN_PIXCONF_SCANDELAY_SFP";
}
/*******************************************************
 * \brief Set trigger acquisition frequency for the pump and probe mode (2bits & ext triggger multi)
 *******************************************************/
void Camera::set_pump_probe_trigger_acquisition_frequency(float frequency)
{
	m_pump_probe_trigger_acquisition_frequency = frequency;
}
/*******************************************************
 * \brief get trigger acquisition frequency for the pump and probe mode (2bits & ext triggger multi)
 *******************************************************/
void Camera::get_pump_probe_trigger_acquisition_frequency(float& frequency)
{
	frequency = m_pump_probe_trigger_acquisition_frequency;
}
/*******************************************************
 * \brief set nb frames for the pump and probe mode (2bits & ext triggger multi)
 *******************************************************/
void Camera::set_pump_probe_nb_frames(float nb_frames)
{
	m_pump_probe_nb_frames = nb_frames;
}
/*******************************************************
 * \brief get nb frames for the pump and probe mode (2bits & ext triggger multi)
 *******************************************************/
void Camera::get_pump_probe_nb_frames(float& nb_frames)
{
	nb_frames = m_pump_probe_nb_frames;
}
