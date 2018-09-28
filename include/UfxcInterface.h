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

#ifndef UFXCINTERFACE_H
#define UFXCINTERFACE_H

#include "lima/Debug.h"

#include "UfxcDetInfoCtrlObj.h"
#include "UfxcSyncCtrlObj.h"
#include "lima/HwInterface.h"
#include "lima/HwBufferMgr.h"

namespace lima
{
namespace Ufxc
{
class Camera;
/*******************************************************************
 * \class Interface
 * \brief Ufxc hardware interface
 *******************************************************************/

class Interface: public HwInterface
{

DEB_CLASS_NAMESPC(DebModCamera, "UfxcInterface", "Ufxc");

public:
	Interface(Camera& cam);
	virtual ~Interface();

	//- From HwInterface
	virtual void getCapList(CapList&) const;
	virtual void reset(ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(StatusType& status);
	virtual int getNbHwAcquiredFrames();
	
    //! get the camera object to access it directly from client
    Camera& getCamera() { return m_cam;}

private:
	Camera& m_cam;
	CapList m_cap_list;
	DetInfoCtrlObj m_det_info;
	SyncCtrlObj m_sync;      
};

} // namespace Ufxc
} // namespace lima

#endif // UFXCINTERFACE_H
