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

#include "UfxcCamera.h"
#include "UfxcDetInfoCtrlObj.h"
#include <limits>

using namespace lima;
using namespace lima::Ufxc;

/*******************************************************************
 * \brief DetInfoCtrlObj constructor
 *******************************************************************/
DetInfoCtrlObj::DetInfoCtrlObj(Camera& cam) : m_cam(cam)
{
	DEB_CONSTRUCTOR();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
DetInfoCtrlObj::~DetInfoCtrlObj()
{
	DEB_DESTRUCTOR();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::getMaxImageSize(Size& size)
{
	DEB_MEMBER_FUNCT();
	// get the max image size
	getDetectorImageSize(size);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::getDetectorImageSize(Size& size)
{
	DEB_MEMBER_FUNCT();
	// get the max image size of the detector
    
	unsigned short width = m_cam.getMaxWidth();
	unsigned short height = m_cam.getMaxHeight();

	size = Size(width, height);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::getDefImageType(ImageType& image_type)
{
	DEB_MEMBER_FUNCT();
	m_cam.getImageType(image_type);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::getCurrImageType(ImageType& image_type)
{
	DEB_MEMBER_FUNCT();
	m_cam.getImageType(image_type);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::setCurrImageType(ImageType image_type)
{
	DEB_MEMBER_FUNCT();
	m_cam.setImageType(image_type);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::getPixelSize(double& x_size, double& y_size)
{
	DEB_MEMBER_FUNCT();
	x_size = y_size = std::numeric_limits<double>::quiet_NaN();;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::getDetectorType(std::string& type)
{
	DEB_MEMBER_FUNCT();
	type = "UFXC";

}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::getDetectorModel(std::string& model)
{
	DEB_MEMBER_FUNCT();
	m_cam.getDetectorModel(model);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void DetInfoCtrlObj::unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
}


