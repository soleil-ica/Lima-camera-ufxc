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
#include "UfxcInterface.h"
#include "UfxcCamera.h"

using namespace lima;
using namespace lima::Ufxc;

#ifndef __linux__
#    define DEF_FNID static char *fnId = __FUNCTION__;
#else
#    define DEF_FNID const char *fnId __attribute__((unused)) = __FUNCTION__;
#endif

//-----------------------------------------------------
//
//-----------------------------------------------------
SyncCtrlObj::SyncCtrlObj(Camera& cam):m_cam(cam)
{
	DEB_CONSTRUCTOR();
    m_first_read_of_valid_range = true;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
SyncCtrlObj::~SyncCtrlObj()
{
	DEB_DESTRUCTOR();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
bool SyncCtrlObj::checkTrigMode(TrigMode trig_mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(trig_mode);

	return m_cam.checkTrigMode(trig_mode);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(trig_mode);

	if(!checkTrigMode(trig_mode))
	{
		THROW_HW_ERROR(InvalidValue) << "Invalid "
		 << DEB_VAR1(trig_mode);
	}
	m_cam.setTrigMode(trig_mode);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::getTrigMode(TrigMode& trig_mode)
{
	DEB_MEMBER_FUNCT();
	m_cam.getTrigMode(trig_mode);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::setExpTime(double exp_time)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(exp_time);
    updateValidRanges();
    m_cam.setExpTime(exp_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::getExpTime(double& exp_time)
{
	DEB_MEMBER_FUNCT();
	m_cam.getExpTime(exp_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::setLatTime(double lat_time)
{
	DEB_MEMBER_FUNCT();
    updateValidRanges();
	m_cam.setLatTime(lat_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::getLatTime(double& lat_time)
{
	DEB_MEMBER_FUNCT();
	m_cam.getLatTime(lat_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::setNbHwFrames(int nb_frames)
{
	DEB_MEMBER_FUNCT();
	m_cam.setNbFrames(nb_frames);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::getNbHwFrames(int& nb_frames)
{
	DEB_MEMBER_FUNCT();
	m_cam.getNbFrames(nb_frames);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
	DEB_MEMBER_FUNCT();
    DEF_FNID;

	double min_time;
	double max_time;
	m_cam.getExposureTimeRange(min_time, max_time);
	valid_ranges.min_exp_time = min_time;
	valid_ranges.max_exp_time = max_time;

	m_cam.getLatTimeRange(min_time, max_time);
	valid_ranges.min_lat_time = min_time;
	valid_ranges.max_lat_time = max_time;

    if(m_first_read_of_valid_range)
    {
        m_current_valid_ranges      = valid_ranges;
        m_first_read_of_valid_range = false;
    }
}

//-----------------------------------------------------
// manages the update of valid ranes (only if needed)
//-----------------------------------------------------
void SyncCtrlObj::updateValidRanges()
{
	DEB_MEMBER_FUNCT();
    DEF_FNID;

    ValidRangesType valid_ranges;
    getValidRanges(valid_ranges);

    // managing the update of valid ranges (only if needed)
    if((valid_ranges.min_exp_time != m_current_valid_ranges.min_exp_time) || 
       (valid_ranges.max_exp_time != m_current_valid_ranges.max_exp_time) || 
       (valid_ranges.min_lat_time != m_current_valid_ranges.min_lat_time) || 
       (valid_ranges.max_lat_time != m_current_valid_ranges.max_lat_time))
    {
        m_current_valid_ranges = valid_ranges;
        validRangesChanged(m_current_valid_ranges); // calling ... callback
        DEB_TRACE() << fnId << ": callback - new valid_ranges: " << DEB_VAR1(m_current_valid_ranges);
    }
}

//-----------------------------------------------------

