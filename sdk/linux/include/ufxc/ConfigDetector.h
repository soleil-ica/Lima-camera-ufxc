/********************************************//**
 *  ConfigDetector.h
 ***********************************************/
/********************************************//**
 *  Created on: 30 juil. 2018
 *  Author: GHAMMOURI Ayoub
 *  Class: ConfigDetector
 *  Description:
 ***********************************************/

#ifndef UFXCLIB_SRC_CONFIGDETECTOR_H_
#define UFXCLIB_SRC_CONFIGDETECTOR_H_

/********************************************//**
 *  DEPENDENCIES
 ***********************************************/
#include "UFXC/UfxlibTypesAndConsts.h"
#include "UFXC/ConfigPortInterface.h"

namespace ufxclib {
class ConfigDetector {
public:
	ConfigDetector(ConfigPortInterface * configPortInterface);

	virtual ~ConfigDetector();

	//!<Set detector global configuration register names
	//!< @param map Register list
	void setDetectorRegisterNames(std::map<T_DetectorConfigKey, std::string> map)
	  throw (ufxclib::Exception);

    //!< get CSA polarity
    //!< @return CSA polarity
	bool getCSAPolarity()
		throw (ufxclib::Exception);

    //!< set CSA polarity
	// @param bool
	void setCSAPolarity(bool polarity)
		throw (ufxclib::Exception);

    //!< get shaper feedback control
    //!< @return shaper feedback control
	bool getShaperFeedback()
		throw (ufxclib::Exception);

    //!< set shaper feedback control
	// @param bool
	void setShaperFeedback(bool feedback)
		throw (ufxclib::Exception);

    //!< get CSA current
    //!< @return CSA current
	unsigned int getBCAS()
		throw (ufxclib::Exception);

    //!< set CSA current
    //!< @param BCAS value
	void setBCAS(unsigned int bcas)
		throw (ufxclib::Exception);

    //!< get CSA feedback
    //!< @return CSA feedback
	unsigned int getBKRUM()
		throw (ufxclib::Exception);

    //!< set CSA feedback
    //!< @param BKRUM value
	void setBKRUM (unsigned int bkrum)
		throw (ufxclib::Exception);

    //!< get Trim DAC current in input transistor
    //!< @return Trim DAC current
	unsigned int getBTRIM()
		throw (ufxclib::Exception);

    //!< set Trim DAC current
    //!< @param BTRIM value
	void setBTRIM(unsigned int btrim)
		throw (ufxclib::Exception);

    //!< get CSA current
    //!< @return CSA current
	unsigned int getBREF()
		throw (ufxclib::Exception);

    //!< set CSA current
    //!< @param BREF value
	void setBREF(unsigned int bref)
		throw (ufxclib::Exception);

    //!< get shaper current
    //!< @return shaper current
	unsigned int getBSH()
		throw (ufxclib::Exception);

    //!< set shaper current
    //!< @param BSH value
	void setBSH(unsigned int bsh)
		throw (ufxclib::Exception);

    //!< get discriminator current
    //!< @return discriminator current
	unsigned int getBDIS()
		throw (ufxclib::Exception);

    //!< set discriminator current
    //!< @param BDIS value
	void setBDIS(unsigned int bdis)
		throw (ufxclib::Exception);

    //!< get shaper feedback control
    //!< @return shaper feedback control
	unsigned int getBGSH()
		throw (ufxclib::Exception);

    //!< set shaper feedback control
    //!< @param BGSH value
	void setBGSH(unsigned int bgsh)
		throw (ufxclib::Exception);

    //!< get reference level DAC
    //!< @return reference level DAC
	unsigned int getBR()
		throw (ufxclib::Exception);

    //!< set reference level DAC
    //!< @param BR value
	void setBR(unsigned int br)
		throw (ufxclib::Exception);

	//!< Apply detector configuration:
	void applyDetectorConfig()
		throw (ufxclib::Exception);

	//TODO:
//	•	Set pixel configuration:
//	setPixelConfig(unsigned int [][] pixels) // pixels is a pointer to a 2D table

private:
	std::map<T_DetectorConfigKey, std::string> m_Detector_registers;
	ConfigPortInterface * m_configPortInterface;
};
} //!< namespace ufxclib

#endif ///!< UFXCLIB_SRC_CONFIGDETECTOR_H_
