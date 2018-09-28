//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2018
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

#ifndef UFXCCAMERA_H
#define UFXCCAMERA_H

#include "lima/Debug.h"
#include "lima/Constants.h"
#include "lima/HwBufferMgr.h"
#include "lima/ThreadUtils.h"


extern "C"
{
#include "mx_util.h"
#include "mx_record.h"
#include "mx_image.h"
#include "mx_area_detector.h"
#include "mx_driver.h"
#include "mx_version.h"
}

namespace lima {
namespace Ufxc {
    
class Camera
{
    DEB_CLASS_NAMESPC(DebModCamera, "Camera", "Ufxc");
    friend class Interface;
public:

    enum Status
    {
        Ready, Exposure, Readout, Latency, Fault
    };

    Camera(const std::string& camName, const std::string& databaseFile);
    ~Camera();

    //-- Related to Ufxc specific features
    void getExpMultiplier(double& exp_mult);
    void setExpMultiplier(double exp_mult);
    void getLatencyTime(double& period_time);
    void setLatencyTime(double period_time);
    void getGapMultiplier(double& gap_mult);
    void setGapMultiplier(double gap_mult);
    void getMxLibraryVersion(std::string& version);
    void getInternalAcqMode(std::string& acq_mode);
    void setInternalAcqMode(const std::string& mode);
    void getReadoutDelayTime(double& readout_delay);
    void setReadoutDelayTime(double readout_delay);
    void getReadoutSpeed(bool& readout_speed);
    void setReadoutSpeed(bool readout_speed);
    void setExtraParam(const std::string& name, const std::string& val);
    std::string getExtraParam(const std::string& name);
    void getInitialDelayTime(double& initial_delay);
    void setInitialDelayTime(double initial_delay);
    void setCorrectionFlags(unsigned long);
    double computeTimestamp(MX_IMAGE_FRAME* image_frame, long num_exp);

    //-- Related to Acquisition
    void startAcq();
    void stopAcq();
    void prepareAcq();
    Status getStatus();
    bool isBusy(); // return the state of the detector : true if detector is processing/running

    //-- Related to Synch control object
    void setTrigMode(TrigMode mode);
    void getTrigMode(TrigMode& mode);
    void getExpTime(double& exp_time);
    void setExpTime(double exp_time);
    int getNbHwAcquiredFrames();
    void setNbFrames(int nb_frames);
    void getNbFrames(int& nb_frames);

    //-- Related to Bin control object
    void setBin(const Bin& bin);
    void getBin(Bin& bin);
    void checkBin(Bin& bin);

    //-- Related to Roi control object
    void checkRoi(const Roi& set_roi, Roi& hw_roi);
    void setRoi(const Roi& set_roi);
    void getRoi(Roi& hw_roi);

    //-- Related to BufferCtrl	object

    HwBufferCtrlObj* getBufferCtrlObj()
    {
        return &m_buffer_ctrl_obj;
    };

    //-- Related to DetInfo object
    void getDetectorModel(std::string& model);
    unsigned short getMaxWidth();
    unsigned short getMaxHeight();
    void setImageType(ImageType type);
    void getImageType(ImageType& type);

private:
    void _open(); // open (reserves) the camera
    void _close(); // close (release) the camera
    void _armDetector(); // do arm/trigger on detector

    //CmdThread class, used to handle some specific tasks (startAcq, stopAcq, ...)

    class CameraThread : public CmdThread
    {
        DEB_CLASS_NAMESPC(DebModCamera, "CameraThread", "Ufxc");
    public:

        enum
        { // Status
            Ready = MaxThreadStatus, Exposure, Readout, Latency,
        };

        enum
        { // Cmd
            StartAcq = MaxThreadCmd, StopAcq, StartMeasureDark, StartMeasureFloodField,
        };

        CameraThread(Camera& cam);

        virtual void start();

        int getNbHwAcquiredFrames();
        bool m_force_stop;

    protected:
        virtual void init();
        virtual void execCmd(int cmd);
    private:
        void execStartAcq();
        void execStartMeasureDark();
        void execStartMeasureFloodField();
        Camera* m_cam;
    };
    friend class CameraThread;

    /* Lima buffer control object */
    SoftBufferCtrlObj m_buffer_ctrl_obj;
    unsigned short* m_frame;
    unsigned short* m_pr_buffer;

    /*camera stuff*/
    Camera::Status m_state;
    std::string m_name;
    std::string m_database_file_name;
    double m_exposure_time;
    double m_exposure_multiplier;
    double m_latency_time;
    double m_gap_multiplier;
    double m_initial_delay_time;
    double m_readout_delay_time;
    double m_ccd_readout_time;
    double m_time_stamp_0;
    double m_time_stamp;
    long   m_readout_speed;

    unsigned long m_correction_flags;
    int m_nb_frames;
    int m_trigger_mode;
    long m_max_width;
    long m_max_height;
    long m_depth;
    std::string m_acq_mode_name;
    std::string m_status;
    Size m_frame_size;

    /* main acquisition thread*/
    CameraThread m_thread;
    int m_acq_frame_nb;

    /*Mx stuff*/
    struct mx_record_type* m_mx_record;

    /*mutex stuff*/
    mutable Cond m_cond;

    long m_binning_x;
    long m_binning_y;

};

} // namespace Ufxc
} // namespace lima

#endif // UFXCCAMERA_H
