/********************************************//**
 *  DaqMonitoring.h
 ***********************************************/
/********************************************//**
 *  Created on: 23 juil. 2018
 *  Author: GHAMMOURI Ayoub
 *  Class: DaqMonitoring
 *  Description: This class provide access to monitoring registers
 ***********************************************/

#ifndef UFXCLIB_DAQMONITORING_H_
#define UFXCLIB_DAQMONITORING_H_

/********************************************//**
 *  DEPENDENCIES
 ***********************************************/
#include "UFXC/ConfigPortInterface.h"
#include "UFXC/UfxlibTypesAndConsts.h"

namespace ufxclib {


class DaqMonitoring {
public:
	//!< Constructor.
	DaqMonitoring(ConfigPortInterface * configPortInterface);
	//!< Destructor.
	virtual ~DaqMonitoring();

    //!< get sfp temperature
    //!< @return SFP temperature
	unsigned int getSFPTemps()
	  throw (ufxclib::Exception);

    //!< get picozed temperature
    //!< @return Picozed temperature
    unsigned int getPicozedTemp()
	  throw (ufxclib::Exception);

    //!< get Power temperature
    //!< @return Power temperature
    unsigned int getPowerTemp()
	  throw (ufxclib::Exception);

    //!< get detector temperature
    //!< @return detector temperature
    unsigned int getDetectorTemp()
	  throw (ufxclib::Exception);

    //!< get sensor current value
    //!< @return sensor current value
    unsigned int getSensorCurrent()
	  throw (ufxclib::Exception);

    //!< get sensor high voltage value
    //!< @return sensor high voltage value
    unsigned int getSensorHighVoltage()
	  throw (ufxclib::Exception);

    //!< get pixel matrix core voltage
    //!< @return pixel matrix core voltage
    unsigned int getCoreVoltage()
      throw (ufxclib::Exception);

    //!< get VDDM voltage
    //!< @return VDDM voltage
    unsigned int getVDDMVoltage()
	  throw (ufxclib::Exception);

    //!< get chip A discriminator voltage
    //!< @return chip A discriminator voltage
    unsigned int getChipADiscVoltage()
	  throw (ufxclib::Exception);

    //!< get chip B discriminator voltage
    //!< @return chip B discriminator voltage
    unsigned int getChipBDiscVoltage()
	  throw (ufxclib::Exception);

    //!< get chip A frontend voltage
    //!< @return chip A frontend voltage
    unsigned int getChipAFrontVoltage()
	  throw (ufxclib::Exception);

    //!< get chip B frontend voltage
    //!< @return chip B frontend voltage
    unsigned int getChipBFrontVoltage()
	  throw (ufxclib::Exception);

    //!< get detector status value
    //!< @return detector status value
    T_DetectorStatus getDetectorStatus()
	  throw (ufxclib::Exception);

    //!< get cable delay compensation
    //!< @return cable delay compensation
    unsigned int getDelayComp()
    throw (ufxclib::Exception);

    //!< set cable delay compensation
	// @param delay value
	void setDelayComp(unsigned int delay)
    throw (ufxclib::Exception);

    //!< Start delay scan
    void StartDelayScan()
      throw (ufxclib::Exception);

    //!< Stop delay scan
    void StopDelayScan()
      throw (ufxclib::Exception);

	//!< 	Set monitoring function register names
	//!< @param map Register list
	void setMonitoringRegisterNames(std::map<T_MonitoringKey, std::string> map)
	  throw (ufxclib::Exception);
private:
	  //!< Monitoring functions register names
	  std::map<T_MonitoringKey, std::string> m_Monitor_registers;

	  ConfigPortInterface * m_configPortInterface;

};
} //!< namespace ufxclib

#endif //!< UFXCLIB_DAQMONITORING_H_
