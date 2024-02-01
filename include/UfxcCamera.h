//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
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
//
// UfxcCamera.h
// Created on: November 28, 2018
// Author: Arafat NOUREDDINE

#ifndef UFXCCAMERA_H_
#define UFXCCAMERA_H_


#include <ostream>
#include <map>
#include "UfxcCompatibility.h"
#include "lima/HwBufferMgr.h"
#include "lima/HwInterface.h"
#include "lima/HwEventCtrlObj.h"
#include "lima/HwMaxImageSizeCallback.h"
#include "lima/Debug.h"
#include "ufxc/UFXCInterface.h"

#define REPORT_EVENT(desc) { \
    Event *my_event = new Event(Hardware,Event::Info, Event::Camera, Event::Default,desc); \
    m_cam.getEventCtrlObj()->reportEvent(my_event);  \
} \

using namespace std;

namespace lima
{
namespace Ufxc
{

const int xPixelSize = 75e-6; // pixel size is 75 micron
const int yPixelSize = 75e-6; // pixel size is 75 micron

class BufferCtrlObj;

/*******************************************************************
 * \class Camera
 * \brief object controlling the Ufxc camera
 *******************************************************************/
class LIBUFXC_API Camera
{
    DEB_CLASS_NAMESPC(DebModCamera, "Camera", "Ufxc");

public:

    // status values
    enum Status
    {
        Init       , // starting the plugin
        Ready      , // ready to start acquisition
        Busy       , // acquisition is running 
        Fault      , // acquisition stopped externally or unexpected error 
        Configuring, // detector is configuring  
    } ;

    // acquisition modes
    enum CountingModes
    {
        Continuous_2  = 0, 
        Continuous_4     , 
        Continuous_8     , 
        Continuous_14    , 
        Standard_14      , 
        LongCounter_28   ,
        PumpProbeProbe_32, 
        SelectDefault    ,
    };

    //==================================================================
    // constructor
    Camera( const std::string&  Ufxc_Model     ,                            //- Detector model (label) 
            const std::string&  TCP_ip_address , unsigned long TCP_port ,   //- IP Address and port for the config
            const std::string&  SFP1_ip_address, unsigned long SFP1_port,   //- IP Address and port for the SFP1
            const std::string&  SFP2_ip_address, unsigned long SFP2_port,   //- IP Address and port for the SFP2
            const std::string&  SFP3_ip_address, unsigned long SFP3_port,   //- IP Address and port for the SFP3
            unsigned long       SFP_MTU        ,                            //- MTU value of the SFP ports
            unsigned long       timeout_ms     ,                            //- timeout in ms
            unsigned long       pixel_depth    ,                            //- pixel depth from the generic device properties
            std::string         counting_mode  );                           //- counting mode from the specific properties

    virtual ~Camera();

    void init();
    void reset();
    void prepareAcq();
    void startAcq();
    void stopAcq();
    void getStatus(Camera::Status& status);
    int  getNbHwAcquiredFrames();

    // -- detector info object
    void getImageType(ImageType& type);
    void setImageType(ImageType type);

    void getDetectorType(std::string& type);
    void getDetectorModel(std::string& model);
    void getDetectorImageSize(Size& size);
    void getPixelSize(double& sizex, double& sizey);

    // -- Buffer control object
    HwBufferCtrlObj* getBufferCtrlObj();
    HwEventCtrlObj*  getEventCtrlObj();

    //-- Synch control object
    void setTrigMode(TrigMode mode);
    void getTrigMode(TrigMode& mode);
    bool checkTrigMode(TrigMode mode);

    void setExpTime(double exp_time);
    void getExpTime(double& exp_time);

    void setLatTime(double lat_time);
    void getLatTime(double& lat_time);

    void getExposureTimeRange(double& min_expo, double& max_expo) const;
    void getLatTimeRange(double& min_lat, double& max_lat) const;

    void setNbFrames(int nb_frames);
    void getNbFrames(int& nb_frames);

    bool is_thread_running();
    
    ///////////////////////////////
    // -- ufxc specific functions
    ///////////////////////////////
    void get_lib_version(std::string & version);
    void get_firmware_version(std::string & version);
    void get_detector_temperature(unsigned long& temp);
    void set_threshold_Low1(float thr);
    void get_threshold_Low1(unsigned long& thr);
    void set_threshold_Low2(float thr);
    void get_threshold_Low2(unsigned long& thr);
    void set_threshold_High1(float thr);
    void get_threshold_High1(unsigned long& thr);
    void set_threshold_High2(float thr);
    void get_threshold_High2(unsigned long& thr);
    void set_detector_config_file(const std::string& file_name);    
    void set_pump_probe_trigger_acquisition_frequency(float frequency);
    void get_pump_probe_trigger_acquisition_frequency(float& frequency);
    void set_pump_probe_nb_frames(int nb_frames);
    void get_pump_probe_nb_frames(int& nb_frames); 
    void setCountingMode(CountingModes mode);
    void getCountingMode(CountingModes& mode);
    void setGeometricalCorrection(bool enabled);
    void getGeometricalCorrection(bool& enabled);
    TrigMode getDefaultTrigMode() const;
    std::string getTrigLabel(TrigMode& mode) const;

    unsigned long getCountingModePixelDepth(enum lima::Ufxc::Camera::CountingModes mode);

    bool convertCountingModeLabel(std::string & in_out_mode_label,
                                  enum lima::Ufxc::Camera::CountingModes & out_counting_mode,
                                  std::string & out_error_message);

    bool convertCountingModeEnum(enum lima::Ufxc::Camera::CountingModes & in_counting_mode,
                                 std::string & out_mode_label   ,
                                 std::string & out_error_message);

    ImageType getImageTypeOfCountingMode(enum lima::Ufxc::Camera::CountingModes mode);
    bool checkTrigModeOfCountingMode(TrigMode trig_mode, enum lima::Ufxc::Camera::CountingModes counting_mode);
    TrigMode getDefaultTrigModeOfCountingMode(CountingModes counting_mode) const;

    void computePumpProbeNbFrames(double in_exposure);

    void getCorrectLatTime     (double& lat_time) const;
    void getCorrectExposureTime(double& exp_time) const;

private:
    //get frame from API/Driver/etc ...
    bool readFrames(void);
    void setStatus(Camera::Status status, bool force);
    void internalStopAcq(); // called only by the acquisition thread
    //////////////////////////////
    // -- ufxc specific members
    //////////////////////////////
    void SetHardwareRegisters();

    class AcqThread;

    AcqThread *         m_acq_thread;
    TrigMode            m_trigger_mode;
    double              m_exp_time;
    double              m_lat_time;
    ImageType           m_image_type;
    int                 m_nb_frames; // nos of frames to acquire
    bool                m_thread_running;
    bool                m_wait_flag;
    bool                m_quit;
    int                 m_acq_frame_nb; // nb of frames acquired
    mutable             Cond m_cond;
    long                m_depth; // depth value can be 2/4/8/14/28/32
    int                 m_pump_probe_nb_frames;
    bool                m_is_geometrical_correction_enabled;    
    Camera::Status      m_status;

    Camera::CountingModes m_counting_mode;

    // UFXC lib main object
    ufxclib::UFXCInterface* m_ufxc_interface;
    // Registers configuration
    std::map<ufxclib::EnumAcquisitionConfigKey, std::string> m_acquisition_registers;
    std::map<ufxclib::EnumDetectorConfigKey, std::string> m_detector_registers;
    std::map<ufxclib::EnumMonitoringKey, std::string> m_monitor_registers;

    // detector type
    std::string m_detector_type;

    // detector model
    std::string m_detector_model;

    // detector firmware version
    std::string m_detector_firmware_version;

    // detector software version
    std::string m_detector_software_version;

    // module firmware version
    std::string m_module_firmware_version;

    // Buffer control object
    SoftBufferCtrlObj   m_bufferCtrlObj;

    // Lima event control object
    HwEventCtrlObj      m_event_ctrl_obj;

    // for the dynamic change of size or/and pixel depth
    #define UFXCCAMERA_USE_DYNAMIC_COUNTING_MODE_CHANGE

    #include "UfxcImageSize.h"
} ;

/*******************************************************************
 * \class AcqThread
 * \brief Thread of acquisition
 *******************************************************************/
class Camera::AcqThread : public Thread
{
    DEB_CLASS_NAMESPC(DebModCamera, "Camera", "AcqThread");
public:
    AcqThread(Camera &aCam);
    virtual ~AcqThread();

protected:
    virtual void threadFunction();

private:
    Camera& m_cam;
} ;

} // namespace Ufxc
} // namespace lima

#endif /* UFXCCAMERA_H_ */
