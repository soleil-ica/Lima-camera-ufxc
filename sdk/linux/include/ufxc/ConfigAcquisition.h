/********************************************//**
 *  ConfigAcquisition.h
 ***********************************************/
/********************************************//**
 *  Created on: 30 juil. 2018
 *  Author: GHAMMOURI Ayoub
 *  Class: ConfigAcquisition
 *  Description:
 ***********************************************/

#ifndef UFXCLIB_SRC_CONFIGACQUISITION_H_
#define UFXCLIB_SRC_CONFIGACQUISITION_H_

/********************************************//**
 *  DEPENDENCIES
 ***********************************************/
#include "UFXC/UfxlibTypesAndConsts.h"
#include "UFXC/ConfigPortInterface.h"
#include "UFXC/CaptureManager.h"

namespace ufxclib {
class ConfigAcquisition
{
public:
	ConfigAcquisition(ConfigPortInterface * configPortInterface, CaptureManager * captureManager);

	virtual ~ConfigAcquisition();

	//!<Set Acquisition global configuration register names
	//!< @param map Register list
	void setAcquisitionRegisterNames(std::map<T_AcquisitionConfigKey, std::string> map)
		throw (ufxclib::Exception);

    //!< get low chip A discriminator threshold
    //!< @return low chip A discriminator threshold
	unsigned int getLowChipAThreshold()
	 throw (ufxclib::Exception);

    //!< set low chip A discriminator threshold
    //!< @param chip A threshold
	void setLowChipAThreshold(unsigned int threshold)
	 throw (ufxclib::Exception);

    //!< get low chip B discriminator threshold
    //!< @return low chip B discriminator threshold
	unsigned int getLowChipBThreshold()
	 throw (ufxclib::Exception);

    //!< set low chip B discriminator threshold
    //!< @param chip B threshold
	void setLowChipBThreshold(unsigned int threshold)
	 throw (ufxclib::Exception);

    //!< get high chip A discriminator threshold
    //!< @return high chip A discriminator threshold
	unsigned int getHighChipAThreshold()
	 throw (ufxclib::Exception);

    //!< set high chip A discriminator threshold
    //!< @param chip A threshold
	void setHighChipAThreshold(unsigned int threshold)
	 throw (ufxclib::Exception);

    //!< get high chip B discriminator threshold
    //!< @return high chip B discriminator threshold
	unsigned int getHighChipBThreshold()
	 throw (ufxclib::Exception);

    //!< set high chip B discriminator threshold
    //!< @param chip B threshold
	void setHighChipBThreshold(unsigned int threshold)
	 throw (ufxclib::Exception);

    //!< get acquisition mode (mode in [0; 5])
    //!< @return acquisition mode (mode in [0; 5])
	unsigned int getAcqMode()
	 throw (ufxclib::Exception);

    //!< set acquisition mode
    //!< @param mode
	void setAcqMode(unsigned int mode)
	 throw (ufxclib::Exception);

    //!< get detector counting time
    //!< @return detector counting time
	unsigned int getCountingTime()
	 throw (ufxclib::Exception);

    //!< set detector counting time
    //!< @param time
	void setCountingTime(unsigned int time)
	 throw (ufxclib::Exception);

    //!< get set waiting time
    //!< @return set waiting time
	unsigned int getWaitingTime()
	 throw (ufxclib::Exception);

    //!< set set waiting time
    //!< @param time
	void setWaitingTime(unsigned int time)
	 throw (ufxclib::Exception);

    //!< get number of images per trigger
    //!< @return number of images per trigger
	unsigned int getImageNumber()
	 throw (ufxclib::Exception);

    //!< set number of images per trigger
    //!< @param trigger
	void setImageNumber(unsigned int number)
	 throw (ufxclib::Exception);

    //!< get number of triggers
    //!< @return number of triggers
	unsigned int getTriggerNumber()
	 throw (ufxclib::Exception);

    //!< set number of triggers
    //!< @param threshold
	void setTriggerNumber(unsigned int number)
	 throw (ufxclib::Exception);

	//!< Start acquisition
	void StartAcquisition()
	 throw (ufxclib::Exception);

	//!< Stop acquisition
	void StopAcquisition()
	 throw (ufxclib::Exception);

	//!< Set A_L1 value
	//!< @param A_L1
	void setA_L1(float A_L1)
		throw (ufxclib::Exception);

	//!< Set A_L2 value
	//!< @param A_L2
	void setA_L2(float A_L2)
		throw (ufxclib::Exception);

	//!< Set B_L1 value
	//!< @param B_L1
	void setB_L1(float B_L1)
		throw (ufxclib::Exception);

	//!< Set B_L2 value
	//!< @param B_L2
	void setB_L2(float B_L2)
		throw (ufxclib::Exception);

	//!< Set A_H1 value
	//!< @param A_H1
	void setA_H1(float A_H1)
		throw (ufxclib::Exception);

	//!< Set A_H2 value
	//!< @param A_H2
	void setA_H2(float A_H2)
		throw (ufxclib::Exception);

	//!< Set B_H1 value
	//!< @param B_H1
	void setB_H1(float B_H1)
		throw (ufxclib::Exception);

	//!< Set B_H2 value
	//!< @param B_H2
	void setB_H2(float B_H2)
		throw (ufxclib::Exception);


private:
	std::map<T_AcquisitionConfigKey, std::string> m_Acquisition_registers;
	CaptureManager * m_captureManagerAcqui;
	ConfigPortInterface * m_configPortInterface;
	float m_A_L1, m_A_L2, m_B_L1, m_B_L2, m_A_H1, m_A_H2, m_B_H1, m_B_H2;

	//!< Reset SFPs in case of error
	//!< Reset data fifo, image frame count
	void SoftReset()
		throw (ufxclib::Exception);
};
} //!< namespace ufxclib

#endif //!< UFXCLIB_SRC_CONFIGACQUISITION_H_
