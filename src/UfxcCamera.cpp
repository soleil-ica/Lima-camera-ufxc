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

using namespace lima;
using namespace lima::Ufxc;
using namespace std;
using namespace ufxclib;

//-------------------------------------------------------------------------
// COUNTING MODES MANAGEMENT
//-------------------------------------------------------------------------
// labels of counting modes for reading the attribute
static const std::vector<std::string> 
       TANGO_COUNTING_MODE_READ_LABELS{"CONTINUOUS_2"       , 
                                       "CONTINUOUS_4"       , 
                                       "CONTINUOUS_8"       , 
                                       "CONTINUOUS_14"      , 
                                       "STANDARD_14"        ,
                                       "LONG_COUNTER_28"    , 
                                       "PUMP_PROBE_PROBE_32"};

static const std::vector<enum lima::Ufxc::Camera::CountingModes> 
        TANGO_COUNTING_MODE_READ_LABELS_TO_TYPE{lima::Ufxc::Camera::CountingModes::Continuous_2     ,
                                                lima::Ufxc::Camera::CountingModes::Continuous_4     ,
                                                lima::Ufxc::Camera::CountingModes::Continuous_8     ,
                                                lima::Ufxc::Camera::CountingModes::Continuous_14    ,
                                                lima::Ufxc::Camera::CountingModes::Standard_14      ,
                                                lima::Ufxc::Camera::CountingModes::LongCounter_28   ,
                                                lima::Ufxc::Camera::CountingModes::PumpProbeProbe_32};

// labels of acquisition modes for writing the attribute
static const std::vector<std::string> 
       TANGO_COUNTING_MODE_WRITE_LABELS{"CONTINUOUS_2"       , "C2"  , 
                                        "CONTINUOUS_4"       , "C4"  ,
                                        "CONTINUOUS_8"       , "C8"  ,
                                        "CONTINUOUS_14"      , "C14" ,
                                        "STANDARD_14"        , "S14" ,
                                        "LONG_COUNTER_28"    , "L28" , 
                                        "PUMP_PROBE_PROBE_32", "P32" };

static const std::vector<enum lima::Ufxc::Camera::CountingModes> TANGO_COUNTING_MODE_WRITE_LABELS_TO_TYPE
    {lima::Ufxc::Camera::CountingModes::Continuous_2     , lima::Ufxc::Camera::CountingModes::Continuous_2     ,
     lima::Ufxc::Camera::CountingModes::Continuous_4     , lima::Ufxc::Camera::CountingModes::Continuous_4     ,
     lima::Ufxc::Camera::CountingModes::Continuous_8     , lima::Ufxc::Camera::CountingModes::Continuous_8     ,
     lima::Ufxc::Camera::CountingModes::Continuous_14    , lima::Ufxc::Camera::CountingModes::Continuous_14    ,
     lima::Ufxc::Camera::CountingModes::Standard_14      , lima::Ufxc::Camera::CountingModes::Standard_14      ,
     lima::Ufxc::Camera::CountingModes::LongCounter_28   , lima::Ufxc::Camera::CountingModes::LongCounter_28   ,
     lima::Ufxc::Camera::CountingModes::PumpProbeProbe_32, lima::Ufxc::Camera::CountingModes::PumpProbeProbe_32};

//-----------------------------------------------------
// convertCountingModeLabel
//-----------------------------------------------------
// Converts a counting mode label to the corresponding enum.
// Changes also the label to the official label.
// return false in case of error with a filled message in out_error_message
bool Camera::convertCountingModeLabel(std::string & in_out_mode_label,
                                      enum lima::Ufxc::Camera::CountingModes & out_counting_mode,
                                      std::string & out_error_message)
{
    bool result = true;

    // we need to convert the acquisition mode string to the enum acquisition mode
    enum lima::Ufxc::Camera::CountingModes in_counting_mode;
    const std::vector<string>::const_iterator 
        iterator = find(TANGO_COUNTING_MODE_WRITE_LABELS.begin(), 
                        TANGO_COUNTING_MODE_WRITE_LABELS.end  (),
                        in_out_mode_label);
    // found it
    if (iterator != TANGO_COUNTING_MODE_WRITE_LABELS.end()) 
    {
        // calculation gives the index
        out_counting_mode = TANGO_COUNTING_MODE_WRITE_LABELS_TO_TYPE[iterator - TANGO_COUNTING_MODE_WRITE_LABELS.begin()];
        result = convertCountingModeEnum(out_counting_mode, in_out_mode_label, in_out_mode_label);
    }
    else
    {
        std::stringstream message;
        message.str("");
        message << "Incorrect counting mode: " << in_out_mode_label << std::endl;
        message << "Available counting modes are:" << std::endl;

        for(size_t index = 0 ; index < TANGO_COUNTING_MODE_READ_LABELS.size() ; index++)
        {
            message << TANGO_COUNTING_MODE_READ_LABELS[index] << std::endl;
        }

        out_error_message = message.str();
        result = false;
    }

    return result;
}

//-----------------------------------------------------
// convertCountingModeEnum
//-----------------------------------------------------
// Converts a counting mode enum to the corresponding label.
// return false in case of error with a filled message in out_error_message
bool Camera::convertCountingModeEnum(enum lima::Ufxc::Camera::CountingModes & in_counting_mode,
                                     std::string & out_mode_label   ,
                                     std::string & out_error_message)
{
    bool result = true;

    // searching the label of the acquisition mode
    const std::vector<enum lima::Ufxc::Camera::CountingModes>::const_iterator 
        iterator = find(TANGO_COUNTING_MODE_READ_LABELS_TO_TYPE.begin(), 
                        TANGO_COUNTING_MODE_READ_LABELS_TO_TYPE.end  (),
                        in_counting_mode);
    // found it
    if (iterator != TANGO_COUNTING_MODE_READ_LABELS_TO_TYPE.end()) 
    {
        // calculation gives the index
        out_mode_label = TANGO_COUNTING_MODE_READ_LABELS[iterator - TANGO_COUNTING_MODE_READ_LABELS_TO_TYPE.begin()];
    }
    else
    {
        std::ostringstream MsgErr;
        MsgErr << "Impossible to found the counting mode: " << in_counting_mode << std::endl;
        out_error_message = MsgErr.str();
        result = false;
    }

    return result;
}

//-------------------------------------------------------------------------
// @brief  Ctor
//-------------------------------------------------------------------------
/************************************************************************
 * \brief constructor
 ************************************************************************/
Camera::Camera(	const std::string& Ufxc_Model     ,
                const std::string& TCP_ip_address , unsigned long TCP_port ,
				const std::vector<std::string>& SFP_ip_addresses,
				const std::vector<unsigned long>& SFP_ports,
                unsigned long      SFP_MTU        ,
                unsigned long      timeout_ms     ,
                unsigned long      pixel_depth    ,
                std::string        counting_mode  )
{
	DEB_CONSTRUCTOR();

	try
	{
        setStatus(Camera::Init, true);

        m_detector_type = "undefined";
	    m_detector_model = "undefined";
	    m_detector_firmware_version = "undefined";
	    m_detector_software_version = "undefined";
	    m_module_firmware_version = "undefined";
	    m_depth = pixel_depth; // given by the lima factory
	    m_acq_frame_nb = 0;
	    m_nb_frames = 1;
	    m_pump_probe_nb_frames = 1;
	    m_is_geometrical_correction_enabled = false;		

        // determine which counting mode should be used -> if unknown label -> CountingModes::SelectDefault
        std::string error_message;

        if(!convertCountingModeLabel(counting_mode, m_counting_mode, error_message))
        {
            m_counting_mode = CountingModes::SelectDefault;
    	    DEB_ERROR() << error_message;
        }

		ufxclib::DaqCnxConfig TCP_cnx;
		TCP_cnx.ip_address          = TCP_ip_address;
		TCP_cnx.configuration_port  = TCP_port;
		TCP_cnx.socket_timeout_ms   = timeout_ms;
		TCP_cnx.protocol            = ufxclib::EnumProtocol::TCP;

		std::vector<ufxclib::DaqCnxConfig> SFP_cnx_lst;
		for (size_t i=0 ; i<SFP_ip_addresses.size() ; i++ )
		{
			ufxclib::DaqCnxConfig SFP_cnx;
			SFP_cnx.ip_address         = SFP_ip_addresses[i];
			SFP_cnx.configuration_port = SFP_ports[i];
			SFP_cnx.socket_timeout_ms  = timeout_ms;
			SFP_cnx.protocol           = ufxclib::EnumProtocol::UDP;
			SFP_cnx_lst.push_back(SFP_cnx);
		}

		//- create the main ufxc object
		m_ufxc_interface = new ufxclib::UFXCInterface();

        // convert the Ufxc_Model (label) to the detector type used by the SDK
        ufxclib::EnumDetectorType sdk_detector_type = m_ufxc_interface->get_detector_type_from_label(Ufxc_Model);

		//- prepare the registers
		SetHardwareRegisters(sdk_detector_type);

		//- connect to the DAQ/Detector
		m_ufxc_interface->open_connection(sdk_detector_type, TCP_cnx, SFP_cnx_lst, SFP_MTU);

		//- set the registers to the DAQ
		m_ufxc_interface->set_acquisition_registers_names(m_acquisition_registers);
		m_ufxc_interface->set_detector_registers_names   (m_detector_registers);
		m_ufxc_interface->set_monitoring_registers_names (m_monitor_registers);
        m_ufxc_interface->set_geometrical_correction(m_is_geometrical_correction_enabled);

	    m_detector_type  = m_ufxc_interface->get_detector_name();
	    m_detector_model = m_ufxc_interface->get_detector_type();

        setCountingMode(m_counting_mode);

        // at start, using the default trigger mode to set the counting mode
        // the real trigger mode will be used when we start an acquisition by Lima
        setTrigMode(getDefaultTrigMode());
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
	if(m_thread_running == true)
	{
	    m_ufxc_interface->stop_acquisition();
    }

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

    if((m_counting_mode == CountingModes::PumpProbeProbe_32)&&(m_nb_frames != 1LL))
		THROW_HW_ERROR(Error) << "Incorrect number of frames in Pump Probe Probe mode! Should be set to 1.";
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

    // we need to force the busy state.
    // if not critical error occured during the previous acquisition
    // we are able to start another acqusition.
	setStatus(Camera::Busy, true);

	//@BEGIN : Ensure that Acquisition is Started before return ...
	m_ufxc_interface->start_acquisition();
	//@END

	//Start acquisition thread
	m_wait_flag = false;
	m_quit = false;
	m_cond.broadcast();
}

//-----------------------------------------------------
// called only by the acquisition thread
//-----------------------------------------------------
void Camera::internalStopAcq()
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());

    //@BEGIN : Ensure that Acquisition is Stopped before return ...	
    m_ufxc_interface->stop_acquisition();
    //@END

    m_wait_flag = true;
	m_cond.broadcast();
	DEB_TRACE() << "internal stop requested ";
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::stopAcq()
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());

    // Don't do anything if acquisition is idle.
	if(m_thread_running)
	{
        // do not call internalStopAcq in this method because the lock is not a recursive one!
        //@BEGIN : Ensure that Acquisition is Stopped before return ...	
        m_ufxc_interface->stop_acquisition();
        //@END

        m_wait_flag = true;
	    m_cond.broadcast();
	    DEB_TRACE() << "stop requested ";

        //now detector is ready
	    setStatus(Camera::Ready, false);
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getStatus(Camera::Status& status)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());

    // managing the error state which could be forced by the acquisition thread
    if(m_status != Camera::Fault)
    {
    	ufxclib::EnumDetectorStatus det_status;

        // getting the detector status
		try
		{
		    det_status = m_ufxc_interface->get_detector_status();
		
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
		}
		catch(const ufxclib::Exception& e)
		{
			m_status = Camera::Fault;
		}
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
bool Camera::readFrames(void)
{
    DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Camera::readFrames() ";

    double frame_time = m_ufxc_interface->get_counting_time_ms() + m_ufxc_interface->get_waiting_time_ms();
    bool fast_acquisition = (frame_time < 100); // if the frame time is inferior of 100 ms, we need te treat the frames faster

    // reading the Lima frame size in bytes
	StdBufferCbMgr& buffer_mgr     = m_bufferCtrlObj.getBuffer();
    lima::FrameDim  frame_dim      = buffer_mgr.getFrameDim();
    int             frame_mem_size = frame_dim.getMemSize();
    Size            frame_size     = frame_dim.getSize();
    int             frame_depth    = frame_dim.getDepth();
    bool            incoherent_frame_index = false; // will be set to true if there is at least one incoherent frame received (frame lost)
    std::size_t     built_images_nb;

	DEB_TRACE() << "Camera::readFrames() - starting acquisition - size (" 
                << frame_size.getWidth () << ", " 
                << frame_size.getHeight() << ") - depth (" 
                << frame_depth << ")";

    // register the plugin as an acquisition customer
    m_ufxc_interface->register_acquisition_customer("Lima Ufxc Plugin");

    // read all the frames or until there is a stop/error
    while(!m_ufxc_interface->end_of_transfer())
    {
        // waiting for new images to receive
        m_ufxc_interface->waiting_built_images();

        // getting the images
        built_images_nb = m_ufxc_interface->get_built_images_nb();

        while(built_images_nb)
        {
            std::size_t image_index = m_ufxc_interface->get_first_built_image_index();

            if((!incoherent_frame_index)&&(image_index != m_acq_frame_nb))
            {
    	        DEB_ERROR() << "---- received frame index " << image_index << " is incoherent with the needed frame index " << m_acq_frame_nb;
                incoherent_frame_index = true;
            }

	        // preparing Lima Frame Ptr 
	        void * bptr = buffer_mgr.getFrameBufferPtr(m_acq_frame_nb);

            if(!m_ufxc_interface->fill_image_buffer(reinterpret_cast<char *>(bptr), frame_mem_size))
            {
                // A problem occured, it is safer to stop the acquisition.
                // We will exit from the loop.
    	        DEB_ERROR() << "---- Error during fill image buffer of image " << m_acq_frame_nb;
        	    m_ufxc_interface->stop_acquisition();
            }
            else
            {
		        // pushing the image buffer through Lima 
		        HwFrameInfoType frame_info;
		        frame_info.acq_frame_nb = m_acq_frame_nb;
		        buffer_mgr.newFrameReady(frame_info);
		        m_acq_frame_nb++;
            }

            built_images_nb--;

            // if this is a slow acquisition, we need to check if there is an error/stop
            // for a fast management of acquisition end.
            if((!fast_acquisition) && (built_images_nb) && (m_ufxc_interface->end_of_transfer()))
            {
                break;
            }
        }
    }

    DEB_TRACE() << "received images number (" << m_acq_frame_nb << ")";

    // unregister the plugin as an acquisition customer
    m_ufxc_interface->unregister_acquisition_customer("Lima Ufxc Plugin");

    // Logging acquisition stats (speed performance and memory use)
    m_ufxc_interface->log_acquisition_stats();

    // return true if ok, false if there was problem during the acquisition, stop or error (timeout for example)
    return (!m_ufxc_interface->failed_acquisition());
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

	    bool acquisition_ok;

		try
		{
		    // read Frames From API/Driver/Etc ... & Copy it into Lima Frame Ptr
		    DEB_TRACE() << "Read Frame From API/Driver/Etc ... & Copy it into Lima Frame Ptr";
		    acquisition_ok = m_cam.readFrames();
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

			//now detector is in fault
			m_cam.setStatus(Camera::Fault, true);

			REPORT_EVENT(err_msg.str());
			THROW_HW_ERROR(Error) << err_msg.str();
		}

		//stopAcq only if this is not already done		
	    bool stopped_by_user = m_cam.m_wait_flag; // making a copy because m_cam.stopAcq will change the value

        DEB_TRACE() << "AcqThread : stopAcq only if this is not already done ";
        
        if(!m_cam.m_wait_flag)
		{
            DEB_TRACE() << " AcqThread: StopAcq";
            m_cam.internalStopAcq();
		}
        else
	    // already stopped
	    {
            DEB_TRACE() << "AcqThread: has been stopped by user ";
	    }

        // managing a failure only if a stop was not done
        if((!stopped_by_user) && (!acquisition_ok))
        {
            std::ostringstream err_msg;
            err_msg << "Failed acquisition! Received only " << m_cam.m_acq_frame_nb << " image(s). Stopped by user="
				<< (stopped_by_user?"True":"False") << ", acquisition ok=" << (acquisition_ok?"True":"False") << std::endl;
            DEB_ERROR() << err_msg;

            //now detector is in fault
            m_cam.setStatus(Camera::Fault, true);

		    DEB_TRACE() << " AcqThread::threadfunction() Setting thread running flag to false";
		    aLock.lock();
		    m_cam.m_thread_running = false;
		    m_cam.m_wait_flag = true;
		    aLock.unlock();

            REPORT_EVENT(err_msg.str());
        }
        else
        {
		    //now detector is ready
		    m_cam.setStatus(Camera::Ready, false);

		    DEB_TRACE() << " AcqThread::threadfunction() Setting thread running flag to false";
		    aLock.lock();
		    m_cam.m_thread_running = false;
		    m_cam.m_wait_flag = true;
		    aLock.unlock();
        }
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
void Camera::setCountingMode(CountingModes mode)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Camera::setCountingMode - " << DEB_VAR1(mode);
    AutoMutex aLock(m_cond.mutex());

    // 2 bits, only continuous is allowed
    if(m_depth == 2)
    {
        if(mode != CountingModes::Continuous_2)
            mode = CountingModes::Continuous_2; // default
    }
    else
    // 4 bits, only continuous is allowed
    if(m_depth == 4)
    {
        if(mode != CountingModes::Continuous_4)
            mode = CountingModes::Continuous_4; // default
    }
    else
    // 8 bits, only continuous is allowed
    if(m_depth == 8)
    {
        if(mode != CountingModes::Continuous_8)
            mode = CountingModes::Continuous_8; // default
    }
    else
    // 14 bits, only standard, continuous and long counter are allowed
    if(m_depth == 14)
    {
        if((mode != CountingModes::Standard_14  ) && 
           (mode != CountingModes::Continuous_14))
            mode = CountingModes::Standard_14; // default
    }
    else
    // 28 bits, only long counter is allowed
    if(m_depth == 28)
    {
        if(mode != CountingModes::LongCounter_28)
            mode = CountingModes::LongCounter_28; // default
    }
    else
    // 32 bits, only pump-probe-probe is allowed
    if(m_depth == 32)
    {
        if(mode != CountingModes::PumpProbeProbe_32)
            mode = CountingModes::PumpProbeProbe_32; // default
    }
    else
    {
        DEB_ERROR() << "Camera::setCountingMode - pixel depth " << m_depth << "is not managed!";
    }

    m_counting_mode = mode;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getCountingMode(CountingModes& mode)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
    mode = m_counting_mode;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
unsigned long Camera::getCountingModePixelDepth(CountingModes mode)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Camera::getCountingModePixelDepth - " << DEB_VAR1(mode);

    unsigned long depth = 0; 

    switch(mode)
    {
        case CountingModes::Continuous_2     : depth = 2 ; break;
        case CountingModes::Continuous_4     : depth = 4 ; break;
        case CountingModes::Continuous_8     : depth = 8 ; break;
        case CountingModes::Continuous_14    : depth = 14; break;
        case CountingModes::Standard_14      : depth = 14; break;
        case CountingModes::LongCounter_28   : depth = 28; break;
        case CountingModes::PumpProbeProbe_32: depth = 32; break;

	    default:
            DEB_ERROR() << "Camera::getCountingModePixelDepth - counting mode " << mode << "is not managed!";
		    break;
    }

    return depth;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setGeometricalCorrection(bool enabled)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Camera::setGeometricalCorrection - " << DEB_VAR1(enabled);
	AutoMutex aLock(m_cond.mutex());
    m_ufxc_interface->set_geometrical_correction(enabled);
    m_is_geometrical_correction_enabled = m_ufxc_interface->get_geometrical_correction();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getGeometricalCorrection(bool& enabled)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
    m_is_geometrical_correction_enabled = m_ufxc_interface->get_geometrical_correction();
    enabled = m_is_geometrical_correction_enabled;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
ImageType Camera::getImageTypeOfCountingMode(CountingModes mode)
{
	DEB_MEMBER_FUNCT();

    ImageType type;

    switch(mode)
    {
        case CountingModes::Continuous_2     : type = Bpp2 ; break;
        case CountingModes::Continuous_4     : type = Bpp4 ; break;
        case CountingModes::Continuous_8     : type = Bpp8 ; break;
        case CountingModes::Continuous_14    : type = Bpp14; break;
        case CountingModes::Standard_14      : type = Bpp14; break;
        case CountingModes::LongCounter_28   : type = Bpp28; break;
        case CountingModes::PumpProbeProbe_32: type = Bpp32; break;

	    default:
		    THROW_HW_ERROR(Error) << "This pixel format of the camera is not managed, only (2, 4, 8, 14, 28, 32) bits cameras are managed!";
		    break;
    }

    return type;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getImageType(ImageType& type)
{
	DEB_MEMBER_FUNCT();

    type = getImageTypeOfCountingMode(m_counting_mode);
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
		case Bpp2 : m_depth = 2 ; break;
		case Bpp4 : m_depth = 4 ; break;
		case Bpp8 : m_depth = 8 ; break;
		case Bpp14: m_depth = 14; break;
		case Bpp28: m_depth = 28; break;
		case Bpp32: m_depth = 32; break;

		default:
		    THROW_HW_ERROR(Error) << "This pixel format of the camera is not managed, only (2, 4, 8, 14, 28, 32) bits cameras are managed!";
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
    size = Size(m_ufxc_interface->get_current_width(), m_ufxc_interface->get_current_height());
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

yat::uint8 Camera::get_detector_chips_count(const std::string& Ufxc_Model)
{
	// convert the Ufxc_Model (label) to the detector type used by the SDK
	ufxclib::EnumDetectorType sdk_detector_type = UFXCInterface::get_detector_type_from_label(Ufxc_Model);

	return UFXCInterface::get_detector_chips_count_from_type(sdk_detector_type);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
bool Camera::checkTrigModeOfCountingMode(TrigMode trig_mode, CountingModes counting_mode)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Camera::checkTrigModeOfCountingMode() " << DEB_VAR1(trig_mode    ) << ", " 
                                                            << DEB_VAR1(counting_mode);
	bool valid_mode = true;

	switch(counting_mode)
	{
		case CountingModes::Continuous_2 : 
		case CountingModes::Continuous_4 : 
		case CountingModes::Continuous_8 : 
		case CountingModes::Continuous_14 : 
		case CountingModes::Standard_14 : 
		case CountingModes::LongCounter_28 : 
		if( (trig_mode != IntTrig) && (trig_mode != ExtTrigSingle) )
		{
			valid_mode = false;
		}
		break;

		case CountingModes::PumpProbeProbe_32 : 
		if(trig_mode != ExtTrigMult)
		{
			valid_mode = false;
		}
		break;

		default : 
		DEB_ERROR() << "Camera::checkTrigModeOfCountingMode - counting mode " << counting_mode << "is not managed!";
		break;
	}

	return valid_mode;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
bool Camera::checkTrigMode(TrigMode mode)
{
	DEB_MEMBER_FUNCT();
    return ((mode == IntTrig      ) || 
            (mode == ExtTrigSingle) ||
            (mode == ExtTrigMult  ));
}

//-----------------------------------------------------
// get default trigger mode for the counting mode
//-----------------------------------------------------
TrigMode Camera::getDefaultTrigModeOfCountingMode(CountingModes counting_mode) const
{
	DEB_MEMBER_FUNCT();

    TrigMode result;

    if((counting_mode == CountingModes::Continuous_2  ) ||
       (counting_mode == CountingModes::Continuous_4  ) ||
       (counting_mode == CountingModes::Continuous_8  ) ||
       (counting_mode == CountingModes::Continuous_14 ) ||
       (counting_mode == CountingModes::Standard_14   ) ||
       (counting_mode == CountingModes::LongCounter_28))
    {
        result = IntTrig;
    }
    else
    if(counting_mode == CountingModes::PumpProbeProbe_32)
    {
        result = ExtTrigMult;
    }
    else
    {
        DEB_ERROR() << "Camera::getDefaultTrigModeOfCountingMode - counting mode " << counting_mode << "is not managed!";
    }

	return result;
}

//-----------------------------------------------------
// get default trigger mode for the current counting mode
//-----------------------------------------------------
TrigMode Camera::getDefaultTrigMode() const
{
    DEB_MEMBER_FUNCT();
    return getDefaultTrigModeOfCountingMode(m_counting_mode);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setTrigMode(TrigMode mode)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << "Camera::setTrigMode() " << DEB_VAR1(mode)    << ", " 
                                            << DEB_VAR1(m_depth) << ", " 
                                            << DEB_VAR1(m_counting_mode);
	DEB_PARAM() << DEB_VAR1(mode);

    bool incoherence = false;
    ufxclib::EnumAcquisitionMode acq_mode;

	try
	{
        // beware, it is not a recursive mutex
        {
            AutoMutex aLock(m_cond.mutex());

            if(m_counting_mode == CountingModes::Continuous_2)
            {
                if(mode == IntTrig      ) acq_mode = EnumAcquisitionMode::software_continuous_2_raw; else
                if(mode == ExtTrigSingle) acq_mode = EnumAcquisitionMode::external_continuous_2_raw; else
                    incoherence = true;
            }
            else
            if(m_counting_mode == CountingModes::Continuous_4)
            {
                if(mode == IntTrig      ) acq_mode = EnumAcquisitionMode::software_continuous_4_raw; else
                if(mode == ExtTrigSingle) acq_mode = EnumAcquisitionMode::external_continuous_4_raw; else
                    incoherence = true;
            }
            else
            if(m_counting_mode == CountingModes::Continuous_8)
            {
                if(mode == IntTrig      ) acq_mode = EnumAcquisitionMode::software_continuous_8_raw; else
                if(mode == ExtTrigSingle) acq_mode = EnumAcquisitionMode::external_continuous_8_raw; else
                    incoherence = true;
            }
            else
            if(m_counting_mode == CountingModes::Continuous_14)
            {
                if(mode == IntTrig      ) acq_mode = EnumAcquisitionMode::software_continuous_14_raw; else
                if(mode == ExtTrigSingle) acq_mode = EnumAcquisitionMode::external_continuous_14_raw; else
                    incoherence = true;
            }
            else
            if(m_counting_mode == CountingModes::Standard_14)
            {
                if(mode == IntTrig      ) acq_mode = EnumAcquisitionMode::software_14_raw; else
                if(mode == ExtTrigSingle) acq_mode = EnumAcquisitionMode::external_14_raw; else
                if(mode == ExtTrigMult  ) acq_mode = EnumAcquisitionMode::external_14_raw; else
                    incoherence = true;
            }
            else
            if(m_counting_mode == CountingModes::LongCounter_28)
            {
                if(mode == IntTrig      ) acq_mode = EnumAcquisitionMode::software_long_counter_14_raw; else
                if(mode == ExtTrigSingle) acq_mode = EnumAcquisitionMode::external_long_counter_14_raw; else
                if(mode == ExtTrigMult  ) acq_mode = EnumAcquisitionMode::external_long_counter_14_raw; else
                    incoherence = true;
            }
            else
            if(m_counting_mode == CountingModes::PumpProbeProbe_32)
            {
                if(mode == ExtTrigMult  ) acq_mode = EnumAcquisitionMode::pump_and_probe_2_raw; else
                    incoherence = true;
            }

            if(incoherence)
            {
		        THROW_HW_ERROR(NotSupported) << DEB_VAR1(mode);
            }

    	    m_trigger_mode = mode;
            m_ufxc_interface->set_acq_mode(acq_mode);
        }

		// DET-407
		// setTrigMode is called before setExpTime so the previous m_exp_time used value (to compute nb frames) can be false.
		if(m_counting_mode == lima::Ufxc::Camera::CountingModes::PumpProbeProbe_32)
		{
        	computePumpProbeNbFrames(m_exp_time);
		}
        // recursive lock problem - can not be called with a locked mutex
        setNbFrames(m_nb_frames);
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
// NEVER USED.
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
std::string Camera::getTrigLabel(TrigMode& mode) const
{
	DEB_MEMBER_FUNCT();

    std::string triggerName = "";

    switch(mode)
    {
        case IntTrig: triggerName        = "INTERNAL_SINGLE"     ; break;
        case ExtTrigSingle: triggerName  = "EXTERNAL_SINGLE"     ; break;
        case ExtTrigMult: triggerName    = "EXTERNAL_MULTI"      ; break;
        case ExtGate: triggerName        = "EXTERNAL_GATE"       ; break;
        case IntTrigMult: triggerName    = "INTERNAL_MULTI"      ; break;
        case ExtStartStop: triggerName   = "EXTERNAL_START_STOP" ; break;
        case ExtTrigReadout: triggerName = "EXTERNAL_READOUT"    ; break;
        default: triggerName = "ERROR"; break;
    }
    
    return triggerName;
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
		exp_time   = m_ufxc_interface->get_counting_time_ms();
		exp_time   = exp_time / 1000;
		m_exp_time = exp_time;
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::getExpTime() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : "   << ue.errors[0].desc
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
    {
        AutoMutex aLock(m_cond.mutex());
	    try
	    {
		    if(exp_time < 0.0)
		    {
			    THROW_HW_ERROR(Error) << "Incorrect exposure time!";
		    }
            
            //UFXCLib use (ms), but lima use (second) as unit
		    m_ufxc_interface->set_counting_time_ms(exp_time * 1000);
		    m_exp_time = exp_time;
	    }
	    catch(const ufxclib::Exception& ue)
	    {
		    std::ostringstream err_msg;
		    err_msg << "Error in Camera::setExpTime() :"
		     << "\nreason : " << ue.errors[0].reason
		     << "\ndesc : "   << ue.errors[0].desc
		     << "\norigin : " << ue.errors[0].origin
		     << std::endl;
		    DEB_ERROR() << err_msg;
		    THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	    }
    }

	//only in mode pump & probe
    // recursive lock problem - can not be called with a locked mutex
	if(m_counting_mode == lima::Ufxc::Camera::CountingModes::PumpProbeProbe_32)
	{
        computePumpProbeNbFrames(m_exp_time);

        // updating again the frames number.
        // In Lima, setExpTime is called after setTrigMode and setNbFrames,
        // so the previous set value can be false.
        setNbFrames(m_nb_frames); 
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
		m_ufxc_interface->set_waiting_time_ms(lat_time * 1000);
		m_lat_time = lat_time;
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::setLatTime() :"
                << "\nreason : " << ue.errors[0].reason
                << "\ndesc : "   << ue.errors[0].desc
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
		lat_time   = m_ufxc_interface->get_waiting_time_ms() / 1000.0;
		m_lat_time = lat_time;
		DEB_RETURN() << DEB_VAR1(lat_time);
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::getLatTime() :"
                << "\nreason : " << ue.errors[0].reason
                << "\ndesc : "   << ue.errors[0].desc
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
	try
	{
		//UFXCLib uses (ms), but lima uses (second) as unit 
		min_expo = m_ufxc_interface->get_min_exposure_time_ms() / 1000.0;
        max_expo = m_ufxc_interface->get_max_exposure_time_ms() / 1000.0;
	    DEB_RETURN() << DEB_VAR2(min_expo, max_expo);
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::getExposureTimeRange() :"
                << "\nreason : " << ue.errors[0].reason
                << "\ndesc : "   << ue.errors[0].desc
                << "\norigin : " << ue.errors[0].origin
                << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getLatTimeRange(double& min_lat, double& max_lat) const
{
	DEB_MEMBER_FUNCT();

	try
	{
		//UFXCLib uses (ms), but lima uses (second) as unit 
		min_lat = m_ufxc_interface->get_min_latency_time_ms() / 1000.0;
        max_lat = m_ufxc_interface->get_max_latency_time_ms() / 1000.0;
	    DEB_RETURN() << DEB_VAR2(min_lat, max_lat);
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::getLatTimeRange() :"
                << "\nreason : " << ue.errors[0].reason
                << "\ndesc : "   << ue.errors[0].desc
                << "\norigin : " << ue.errors[0].origin
                << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getCorrectLatTime(double& lat_time) const
{
	DEB_MEMBER_FUNCT();

	try
	{
		//UFXCLib uses (ms), but lima uses (second) as unit 
        double min_lat = m_ufxc_interface->get_min_latency_time_ms() / 1000.0;
        double max_lat = m_ufxc_interface->get_max_latency_time_ms() / 1000.0;

        if(lat_time < min_lat) lat_time = min_lat;
        if(lat_time > max_lat) lat_time = max_lat;
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::getCorrectLatTime() :"
                << "\nreason : " << ue.errors[0].reason
                << "\ndesc : "   << ue.errors[0].desc
                << "\norigin : " << ue.errors[0].origin
                << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getCorrectExposureTime(double& exp_time) const
{
	DEB_MEMBER_FUNCT();
	try
	{
		//UFXCLib uses (ms), but lima uses (second) as unit 
		double min_expo = m_ufxc_interface->get_min_exposure_time_ms() / 1000.0;
        double max_expo = m_ufxc_interface->get_max_exposure_time_ms() / 1000.0;

        if(exp_time < min_expo) exp_time = min_expo;
        if(exp_time > max_expo) exp_time = max_expo;
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::getCorrectExposureTime() :"
                << "\nreason : " << ue.errors[0].reason
                << "\ndesc : "   << ue.errors[0].desc
                << "\norigin : " << ue.errors[0].origin
                << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}
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
		if(nb_frames < 0)
		{
			THROW_HW_ERROR(Error) << "Number of frames to acquire has not been set";
		}

        std::size_t images_number   = 0;
        std::size_t triggers_number = 0;

		TrigMode trigger_mode;
		getTrigMode(trigger_mode);

        if((trigger_mode == IntTrig) || (trigger_mode == ExtTrigSingle))
        {
	        images_number   = nb_frames;
	        triggers_number = 1;
	        m_nb_frames     = nb_frames;
        }
        else
        if(trigger_mode == ExtTrigMult)
        {
	        if(m_counting_mode == CountingModes::PumpProbeProbe_32)
            {
                images_number   = 1;
	            triggers_number = m_pump_probe_nb_frames;
	            m_nb_frames     = 1;
            }
            else
            {
                images_number   = 1;
	            triggers_number = nb_frames;
	            m_nb_frames     = nb_frames;
            }
        }

        m_ufxc_interface->set_images_number  (images_number  );
        m_ufxc_interface->set_triggers_number(triggers_number);
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
        if(m_counting_mode == CountingModes::PumpProbeProbe_32)
        {
		    m_nb_frames = 1;
        }
        else
        {
            std::size_t images_number   = m_ufxc_interface->get_images_number  ();
            std::size_t triggers_number = m_ufxc_interface->get_triggers_number();
            m_nb_frames = images_number * triggers_number;
        }

        nb_frames = m_nb_frames;
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::getNbFrames() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : "   << ue.errors[0].desc
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
		version = m_ufxc_interface->get_firmware_version();
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
		temp = m_ufxc_interface->get_detector_temp();
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
void Camera::set_threshold_Low(size_t index, float thr)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		m_ufxc_interface->set_low_threshold(index, thr);
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::set_threshold_Low() :"
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
void Camera::get_threshold_Low(size_t index, unsigned long& thr)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		thr = m_ufxc_interface->get_low_threshold(index);
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::get_threshold_Low() :"
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
void Camera::set_threshold_High(size_t index, float thr)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		m_ufxc_interface->set_high_threshold(index, thr);
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::set_threshold_High() :"
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
void Camera::get_threshold_High(size_t index, unsigned long& thr)
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	try
	{
		thr = m_ufxc_interface->get_high_threshold(index);
	}
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::get_threshold_High() :"
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

/*******************************************************
 * \brief Set the Hardware registers in the DAQ system
 *******************************************************/
void Camera::SetHardwareRegisters(ufxclib::EnumDetectorType sdk_detector_type)
{
	yat::uint8 detector_chips_count = m_ufxc_interface->get_detector_chips_count_from_type(sdk_detector_type);
	for(yat::uint8 index=0 ;  index<detector_chips_count ; index++)
	{
		std::stringstream ssACQ_CONF_KEY_DET_THRESHOLD_LOW;
		ssACQ_CONF_KEY_DET_THRESHOLD_LOW << ACQ_CONF_KEY_DET_THRESHOLD_LOW << index+1;
		std::stringstream ssACQ_CONF_KEY_DET_THRESHOLD_HIGH;
		ssACQ_CONF_KEY_DET_THRESHOLD_HIGH << ACQ_CONF_KEY_DET_THRESHOLD_HIGH << index+1;
		
		std::stringstream ssACQ_CONF_VALUE_DET_THRESHOLD_LOW;
		ssACQ_CONF_VALUE_DET_THRESHOLD_LOW << "FMC.DET_THRESHOLD_LOW_" << index+1;
		std::stringstream ssACQ_CONF_VALUE_DET_THRESHOLD_HIGH;
		ssACQ_CONF_VALUE_DET_THRESHOLD_HIGH << "FMC.DET_THRESHOLD_HIGH_" << index+1;

		m_acquisition_registers[ssACQ_CONF_KEY_DET_THRESHOLD_LOW.str()] = ssACQ_CONF_VALUE_DET_THRESHOLD_LOW.str();
		m_acquisition_registers[ssACQ_CONF_KEY_DET_THRESHOLD_HIGH.str()] = ssACQ_CONF_VALUE_DET_THRESHOLD_HIGH.str();
	}

	m_acquisition_registers[ufxclib::ACQ_CONF_KEY_ACQ_MODE] = "FMC.ACQ_MODE";
	m_acquisition_registers[ufxclib::ACQ_CONF_KEY_ACQ_COUNT_TIME] = "FMC.ACQ_COUNT_TIME";
	m_acquisition_registers[ufxclib::ACQ_CONF_KEY_ACQ_WAIT_TIME] = "FMC.ACQ_WAIT_TIME";
	m_acquisition_registers[ufxclib::ACQ_CONF_KEY_ACQ_NIMG] = "FMC.ACQ_NIMG";
	m_acquisition_registers[ufxclib::ACQ_CONF_KEY_ACQ_NTRIG] = "FMC.ACQ_NTRIG";
	m_acquisition_registers[ufxclib::ACQ_CONF_KEY_SFP_SOFT_RESET] = "SFP.SOFT_RESET";
	if( sdk_detector_type==ufxclib::DETECTOR_U8DEM )
	{
		m_acquisition_registers[ufxclib::ACQ_CONF_KEY_START_ACQ] = "FMC.STARTACQ";
		m_acquisition_registers[ufxclib::ACQ_CONF_KEY_ABORT_ACQ] = "FMC.ABORTACQ";
		m_acquisition_registers[ufxclib::ACQ_CONF_KEY_FMC_SOFT_RESET] = "UFXSTATUS.SOFT_RESET";
	}
	else
	{
		m_acquisition_registers[ufxclib::ACQ_CONF_KEY_START_ACQ] = "FMC.StartAcq";
		m_acquisition_registers[ufxclib::ACQ_CONF_KEY_ABORT_ACQ] = "FMC.AbortAcq";
		m_acquisition_registers[ufxclib::ACQ_CONF_KEY_FMC_SOFT_RESET] = "FMC.SOFT_RESET";
	}

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
	m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_BITLINE_NBR] = "FMC.PIXCONF_BITLINE_NBR";
	if( sdk_detector_type==ufxclib::DETECTOR_U8DEM )
	{
		m_detector_registers[ufxclib::EnumDetectorConfigKey::DETECTOR_CONFIG] = "FMC.DETECTORCONFIG";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXLINE_CONFIG] = "FMC.PIXLINECONFIG";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_31_0_1] = "FMC.PIXCONF_COL_31_0_1";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_63_32_1] = "FMC.PIXCONF_COL_63_32_1";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_95_64_1] = "FMC.PIXCONF_COL_95_64_1";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_127_96_1] = "FMC.PIXCONF_COL_127_96_1";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_31_0_2] = "FMC.PIXCONF_COL_31_0_2";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_63_32_2] = "FMC.PIXCONF_COL_63_32_2";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_95_64_2] = "FMC.PIXCONF_COL_95_64_2";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_127_96_2] = "FMC.PIXCONF_COL_127_96_2";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_31_0_3] = "FMC.PIXCONF_COL_31_0_3";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_63_32_3] = "FMC.PIXCONF_COL_63_32_3";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_95_64_3] = "FMC.PIXCONF_COL_95_64_3";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_127_96_3] = "FMC.PIXCONF_COL_127_96_3";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_31_0_4] = "FMC.PIXCONF_COL_31_0_4";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_63_32_4] = "FMC.PIXCONF_COL_63_32_4";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_95_64_4] = "FMC.PIXCONF_COL_95_64_4";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_127_96_4] = "FMC.PIXCONF_COL_127_96_4";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_31_0_5] = "FMC.PIXCONF_COL_31_0_5";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_63_32_5] = "FMC.PIXCONF_COL_63_32_5";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_95_64_5] = "FMC.PIXCONF_COL_95_64_5";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_127_96_5] = "FMC.PIXCONF_COL_127_96_5";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_31_0_6] = "FMC.PIXCONF_COL_31_0_6";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_63_32_6] = "FMC.PIXCONF_COL_63_32_6";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_95_64_6] = "FMC.PIXCONF_COL_95_64_6";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_127_96_6] = "FMC.PIXCONF_COL_127_96_6";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_31_0_7] = "FMC.PIXCONF_COL_31_0_7";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_63_32_7] = "FMC.PIXCONF_COL_63_32_7";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_95_64_7] = "FMC.PIXCONF_COL_95_64_7";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_127_96_7] = "FMC.PIXCONF_COL_127_96_7";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_31_0_8] = "FMC.PIXCONF_COL_31_0_8";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_63_32_8] = "FMC.PIXCONF_COL_63_32_8";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_95_64_8] = "FMC.PIXCONF_COL_95_64_8";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_127_96_8] = "FMC.PIXCONF_COL_127_96_8";
	}
	else
	{
		m_detector_registers[ufxclib::EnumDetectorConfigKey::DETECTOR_CONFIG] = "FMC.DetectorConfig";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXLINE_CONFIG] = "FMC.PixLineConfig";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_31_0_A] = "FMC.PIXCONF_COL_31_0_A";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_63_32_A] = "FMC.PIXCONF_COL_63_32_A";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_95_64_A] = "FMC.PIXCONF_COL_95_64_A";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_127_96_A] = "FMC.PIXCONF_COL_127_96_A";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_31_0_B] = "FMC.PIXCONF_COL_31_0_B";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_63_32_B] = "FMC.PIXCONF_COL_63_32_B";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_95_64_B] = "FMC.PIXCONF_COL_95_64_B";
		m_detector_registers[ufxclib::EnumDetectorConfigKey::PIXCONF_COL_127_96_B] = "FMC.PIXCONF_COL_127_96_B";
	}
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
	if( sdk_detector_type==ufxclib::DETECTOR_U8DEM )
	{
		m_monitor_registers[ufxclib::EnumMonitoringKey::DETECTOR_STATUS] = "UFXSTATUS.DETECTOR_STATUS";
		m_monitor_registers[ufxclib::EnumMonitoringKey::EN_PIXCONF_SCANDELAY_SFP] = "FMC.EN_PIXCONF_SFP";
		m_monitor_registers[ufxclib::EnumMonitoringKey::DELAY_SCAN] = "FMC.Delay_scan";
		m_monitor_registers[ufxclib::EnumMonitoringKey::ABORT_DELAY] = "FMC.ABORTACQ";
	}
	else
	{
		m_monitor_registers[ufxclib::EnumMonitoringKey::DETECTOR_STATUS] = "FMC.DETECTOR_STATUS";
		m_monitor_registers[ufxclib::EnumMonitoringKey::EN_PIXCONF_SCANDELAY_SFP] = "FMC.EN_PIXCONF_SCANDELAY_SFP";
		m_monitor_registers[ufxclib::EnumMonitoringKey::DELAY_SCAN] = "FMC.Delay_scan";
		m_monitor_registers[ufxclib::EnumMonitoringKey::ABORT_DELAY] = "FMC.AbortAcq";
	}
	m_monitor_registers[ufxclib::EnumMonitoringKey::FIRMWARE_VERSION] = "*IDN";
}
/*******************************************************
 * \brief Set trigger acquisition frequency for the pump and probe mode (2bits & ext triggger multi)
 *******************************************************/
void Camera::set_pump_probe_trigger_acquisition_frequency(float frequency)
{
	AutoMutex aLock(m_cond.mutex());
    m_ufxc_interface->set_pump_probe_frequency_Hz(static_cast<double>(frequency));
}
/*******************************************************
 * \brief get trigger acquisition frequency for the pump and probe mode (2bits & ext triggger multi)
 *******************************************************/
void Camera::get_pump_probe_trigger_acquisition_frequency(float& frequency)
{
	AutoMutex aLock(m_cond.mutex());
	frequency = static_cast<float>(m_ufxc_interface->get_pump_probe_frequency_Hz());
}
/*******************************************************
 * \brief set nb frames for the pump and probe mode (2bits & ext triggger multi)
 *******************************************************/
void Camera::set_pump_probe_nb_frames(int nb_frames)
{
	AutoMutex aLock(m_cond.mutex());
    m_pump_probe_nb_frames = nb_frames;
}
/*******************************************************
 * \brief get nb frames for the pump and probe mode (2bits & ext triggger multi)
 *******************************************************/
void Camera::get_pump_probe_nb_frames(int& nb_frames)
{
	AutoMutex aLock(m_cond.mutex());
    nb_frames = m_pump_probe_nb_frames;
}

/*******************************************************
 * \brief compute pump probe nb frames
 *******************************************************/
void Camera::computePumpProbeNbFrames(double in_exposure)
{
	DEB_MEMBER_FUNCT();

	try
	{
        int nb_frames_pump_probe;

        // beware, it is not a recursive mutex
        {
            AutoMutex aLock(m_cond.mutex());

            double acquisition_frequency = m_ufxc_interface->get_pump_probe_frequency_Hz();

            nb_frames_pump_probe = static_cast<int> (round(in_exposure * acquisition_frequency / 2)*2);

            DEB_TRACE() << "in_exposure = "           << in_exposure          ;

            DEB_TRACE() << "acquisition_frequency = " << acquisition_frequency;
            DEB_TRACE() << "nb_frames_pump_probe = "  << nb_frames_pump_probe ;
        }

        // recursive lock problem - can not be called with a locked mutex

		set_pump_probe_nb_frames(nb_frames_pump_probe);
    }
	catch(const ufxclib::Exception& ue)
	{
		std::ostringstream err_msg;
		err_msg << "Error in Camera::computePumpProbeData() :"
		 << "\nreason : " << ue.errors[0].reason
		 << "\ndesc : "   << ue.errors[0].desc
		 << "\norigin : " << ue.errors[0].origin
		 << std::endl;
		DEB_ERROR() << err_msg;
		THROW_HW_FATAL(ErrorType::Error) << err_msg.str();
	}

}
