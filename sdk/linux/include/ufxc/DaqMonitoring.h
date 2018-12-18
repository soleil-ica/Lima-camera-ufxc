/**
 *  \file DaqMonitoring.h
 *  \brief header file of DaqMonitoring class
 *  \author Ayoub GHAMMOURI
 *  \version 0.1
 *  \date November 29 2018
 *  Created on: July 23 2018
 */

#ifndef UFXCLIB_DAQMONITORING_H_
#define UFXCLIB_DAQMONITORING_H_

#include "ufxc/ConfigPortInterface.h"
#include "ufxc/UFXCLibTypesAndConsts.h"

/**
 * \namespace ufxclib
 */
namespace ufxclib
{
/**
 *  \class DaqMonitoring
 *  \brief This class provide access to monitoring registers
 */
class DaqMonitoring
{
public:

    /**
    * \fn DaqMonitoring(ConfigPortInterface * config_port_interface)
    * \brief Constructor
    * \param config_port_interface : ConfigPortInterface object for all TCP communications with DAQ.
	* \return none
    */
    DaqMonitoring(ConfigPortInterface * config_port_interface);

    /**
    * \fn virtual ~DaqMonitoring()
    * \brief destructor
    * \param none
	* \return none
    */
    virtual ~DaqMonitoring();

    /**
    * \fn yat::uint32 get_SFP_temps()
    * \brief Get DAQ board temperature sensor readout (SFP transceivers region).
    * \param none
	* \return FFP temperature in deg C
    */
    yat::uint32 get_SFP_temps();

    /**
    * \fn yat::uint32 get_picozed_temp()
    * \brief Get DAQ board temperature sensor readout (Picozed region).
    * \param none
	* \return picozed temperature in deg C
    */
    yat::uint32 get_picozed_temp();

    /**
    * \fn yat::uint32 get_power_temp()
    * \brief Get DAQ board temperature sensor readout (power supply region).
    * \param none
	* \return power temperature in deg C
    */
    yat::uint32 get_power_temp();

    /**
    * \fn yat::uint32 get_detector_temp()
    * \brief Get Detector board temperature sensor readout.
    * \param none
	* \return detector temperature in deg C
    */
    yat::uint32 get_detector_temp();

    /**
    * \fn yat::uint32 get_sensor_current()
    * \brief Get sensor current value
    * \param none
	* \return sensor current value
    */
    yat::uint32 get_sensor_current();

    /**
    * \fn  yat::uint32 get_sensor_high_voltage()
    * \brief get sensor high voltage value
    * \param none
	* \return sensor current value
    */
    yat::uint32 get_sensor_high_voltage();

    /**
    * \fn yat::uint32 get_core_voltage()
    * \brief get pixel matrix core voltage
    * \param none
	* \return pixel matrix core voltage
    */
    yat::uint32 get_core_voltage();

    /**
    * \fn yat::uint32 get_VDDM_voltage()
    * \brief get VDDM voltage
    * \param none
	* \return VDDM voltage
    */
    yat::uint32 get_VDDM_voltage();

    /**
    * \fn yat::uint32 get_1_disc_voltage()
    * \brief get chip A discriminator voltage
    * \param none
	* \return chip A discriminator voltage
    */
    yat::uint32 get_1_disc_voltage();

    /**
    * \fn yat::uint32 get_2_disc_voltage()
    * \brief get chip B discriminator voltage
    * \param none
	* \return chip B discriminator voltage
    */
    yat::uint32 get_2_disc_voltage();

    /**
    * \fn yat::uint32 get_1_front_voltage()
    * \brief get chip A frontend voltage
    * \param none
	* \return chip A frontend voltage
    */
    yat::uint32 get_1_front_voltage();

    /**
    * \fn yat::uint32 get_2_front_voltage()
    * \brief get chip B frontend voltage
    * \param none
	* \return chip B frontend voltage
    */
    yat::uint32 get_2_front_voltage();

    /**
    * \fn T_DetectorStatus get_detector_status()
    * \brief get detector status value.
    * \param none
	* \return detector status
    */
    T_DetectorStatus get_detector_status();

    /**
    * \fn yat::uint32 get_delay_comp()
    * \brief Get cable delay compensation. Compensation of the cable delay expressed in number of clock periods.
    * \param none
	* \return cable delay compensation
    */
    yat::uint32 get_delay_comp();

    /**
    * \fn void set_delay_comp(yat::uint32 delay)
    * \brief set cable delay compensation
    * \param delay : delay value
	* \return void
    */
    void set_delay_comp(yat::uint32 delay);

    /**
    * \fn void start_delay_scan()
    * \brief Start delay scan
    * \param none
	* \return void
    */
    void start_delay_scan();

    /**
    * \fn void stop_delay_scan()
    * \brief Stop delay scan
    * \param none
	* \return void
    */
    void stop_delay_scan();

    /**
    * \fn void set_monitoring_registers_names(std::map<T_MonitoringKey, std::string> map)
    * \brief Set monitoring registers names
    * \param map : it is a map contain the list of registers names for the monitoring config
	* \return void
    */
    void set_monitoring_registers_names(std::map<T_MonitoringKey, std::string> map);

    /**
    * \fn std::string get_firmware_version()
    * \brief Get firmware version
    * \param none
	* \return std::string contain the firmware version
    */
    std::string get_firmware_version();

    /**
    * \fn void enable_config_feedback()
    * \brief enable config feedback. If this fuction is called, the DAQ sends the image config data after each set_detector_config_file.
    * \param none
	* \return void
    */
    void enable_config_feedback();

    /**
    * \fn void disable_config_feedback()
    * \brief disable config feedback. If this fuction is called, the DAQ don't send the image config data.
    * \param none
	* \return void
    */
    void disable_config_feedback();

    /**
    * \fn uint8_t get_pixel_config_status()
    * \brief get pixel configuration status (enable or disable).
    * \param none
	* \return enable or disable (1 or 0)
    */
    uint8_t get_pixel_config_status();

private:
    std::map<T_MonitoringKey, std::string> m_monitor_registers;
    ConfigPortInterface * m_config_port_interface;
    /**
    * \fn T_DetectorStatus convert_string_to_T_DetectorStatus(std::string status)
    * \brief Convert string to T_DetectorStatus enum
    * \param status : string status
	* \return status
    */
    T_DetectorStatus convert_string_to_T_DetectorStatus(std::string status);

};
} /// namespace ufxclib

#endif /// UFXCLIB_DAQMONITORING_H_
