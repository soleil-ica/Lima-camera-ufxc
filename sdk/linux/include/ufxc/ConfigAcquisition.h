/**
 *  \file ConfigAcquisition.h
 *  \brief header file of ConfigAcquisition class
 *  \author Ayoub GHAMMOURI
 *  \version 0.1
 *  \date November 29 2018
 *  Created on: July 30 2018
 */

#ifndef UFXCLIB_CONFIGACQUISITION_H_
#define UFXCLIB_CONFIGACQUISITION_H_

#include "ufxc/CaptureManager.h"
#include "ufxc/ConfigPortInterface.h"
#include "ufxc/UFXCLibTypesAndConsts.h"

/**
 * \namespace ufxclib
 */
namespace ufxclib
{
/**
 *  \class ConfigAcquisition
 *  \brief This class contains the necessary functions for the acquisition configuration.
 * It also contains two important functions: The first is the "start_acquisition" to start an acquisition and the second is the "stop_acquisition" to stop the acquisition.
 */
class ConfigAcquisition
{
public:

    /**
    * \fn ConfigAcquisition(ConfigPortInterface * config_port_interface, CaptureManager * capture_manager)
    * \brief constructor
    * \param config_port_interface : ConfigPortInterface object for all TCP communications with DAQ.
    * \param capture_manager : CaptureManager object to read UDP data.
	* \return none
    */
    ConfigAcquisition(ConfigPortInterface * config_port_interface, CaptureManager * capture_manager);

    /**
    * \fn virtual ~ConfigAcquisition()
    * \brief destructor
    * \param none
	* \return none
    */
    virtual ~ConfigAcquisition();

    /**
    * \fn void set_acquisition_registers_names(std::map<EnumAcquisitionConfigKey, std::string> map)
    * \brief Set Acquisition registers names
    * \param map : it is a map contain the list of registers names for the acquisition config
	* \return void
    */
    void set_acquisition_registers_names(std::map<EnumAcquisitionConfigKey, std::string> map);

    /**
    * \fn yat::uint32 get_low_1_threshold()
    * \brief get chip A threshold for the counter LOW
    * \param none
	* \return chip A threshold for the counter LOW
    */
    yat::uint32 get_low_1_threshold();

    /**
    * \fn void set_low_1_threshold(yat::uint32 threshold_low)
    * \brief set chip A threshold for the counter LOW
    * \param threshold_low : Low threshold
	* \return void
    */
    void set_low_1_threshold(yat::uint32 threshold_low);

    /**
    * \fn yat::uint32 get_low_2_threshold()
    * \brief get chip B threshold for the counter LOW
    * \param none
	* \return chip B threshold for the counter LOW
    */
    yat::uint32 get_low_2_threshold();

    /**
    * \fn void set_low_2_threshold(yat::uint32 threshold_low)
    * \brief set chip B threshold for the counter LOW
    * \param threshold_low : Low threshold
	* \return void
    */
    void set_low_2_threshold(yat::uint32 threshold_low);

    /**
    * \fn yat::uint32 get_high_1_threshold()
    * \brief get chip A threshold for the counter high
    * \param none
	* \return chip A threshold for the counter high
    */
    yat::uint32 get_high_1_threshold();

    /**
    * \fn void set_high_1_threshold(yat::uint32 threshold_high)
    * \brief set chip A threshold for the counter high
    * \param threshold_low : High threshold
	* \return void
    */
    void set_high_1_threshold(yat::uint32 threshold_high);

    /**
    * \fn yat::uint32 get_high_2_threshold()
    * \brief get chip B threshold for the counter high
    * \param none
	* \return chip B threshold for the counter high
    */
    yat::uint32 get_high_2_threshold();

    /**
    * \fn void set_high_2_threshold(yat::uint32 threshold_high)
    * \brief set chip B threshold for the counter high
    * \param threshold_low : High threshold
	* \return void
    */
    void set_high_2_threshold(yat::uint32 threshold_high);

    /**
    * \fn EnumAcquisitionMode get_acq_mode()
    * \brief get the acquisition mode. It is a value from 0 to 5
    * \param none
	* \return the acquisition mode. It is a value from 0 to 5
    */
    EnumAcquisitionMode get_acq_mode();

    /**
    * \fn void set_acq_mode(EnumAcquisitionMode mode)
    * \brief set the acquisition mode.
    * \param mode : It is a value from 0 to 5
	* \return void
    */
    void set_acq_mode(EnumAcquisitionMode mode);

    /**
    * \fn double get_counting_time_ms()
    * \brief get the counting time.
    * \param none
	* \return the counting time.
    */
    double get_counting_time_ms();

    /**
    * \fn void set_counting_time_ms(float time_ms)
    * \brief set the counting time.
    * \param time_ms : counting time in millisecond
	* \return void
    */
    void set_counting_time_ms(const double time_ms);

    /**
    * \fn double get_waiting_time_ms()
    * \brief get the waiting time.
    * \param none
	* \return waiting time in millisecond
    */
    double get_waiting_time_ms();

    /**
    * \fn void set_waiting_time_ms(float time_ms)
    * \brief set the waiting time.
    * \param time_ms : waiting time in millisecond
	* \return void
    */
    void set_waiting_time_ms(const double time_ms);

    /**
    * \fn std::size_t get_images_number()
    * \brief get the images number per trigger for one counter.
    * \param none
	* \return images number per trigger for one counter
    */
    std::size_t get_images_number();

    /**
    * \fn void set_images_number(std::size_t images_number)
    * \brief set the images number per trigger for one counter.
    * \param images_number : images number per trigger for one counter
	* \return void
    */
    void set_images_number(std::size_t images_number);

    /**
    * \fn std::size_t get_triggers_number()
    * \brief get the triggers number.
    * \param none
	* \return triggers number
    */
    std::size_t get_triggers_number();

    /**
    * \fn void set_triggers_number(std::size_t triggers_number)
    * \brief set the triggers number.
    * \param triggers_number : triggers number
	* \return void
    */
    void set_triggers_number(std::size_t triggers_number);

    /**
    * \fn void start_acquisition()
    * \brief start the acquisition:
    * The UFXCLib receive the images data from 3 SFPs via the UDP protocol.
    * This function start three threads in parallel to read data from SFPs.
    * Each thread creates a listener to receive data from one of the three UDP ports.
    * \param none
	* \return void
    */
    void start_acquisition();

    /**
    * \fn void stop_acquisition()
    * \brief THis function:
    * Stop the acquisition in progress started by the start_acquisition function.
    * Stop the read data in progress started by the get_data_receiver_obj function.
    * \param none
	* \return void
    */
    void stop_acquisition();

    /**
    * \fn void set_A_L1(float A_L1)
    * \brief set the linear function parameter correspond to the thip A, counter Low, threshold 1.
    * This function is called in set_detector_config_file function. Because we read this value from the config file in this function.
    * \param A_L1 : the linear function parameter correspond to the thip A, counter Low, threshold 1.
	* \return void
    */
    void set_A_L1(float A_L1);

    /**
    * \fn void set_A_L2(float A_L2)
    * \brief set the linear function parameter correspond to the thip A, counter Low, threshold 2.
    * This function is called in set_detector_config_file function. Because we read this value from the config file in this function.
    * \param A_L2 : the linear function parameter correspond to the thip A, counter Low, threshold 2.
	* \return void
    */
    void set_A_L2(float A_L2);

    /**
    * \fn void set_B_L1(float B_L1)
    * \brief set the linear function parameter correspond to the thip B, counter Low, threshold 1.
    * This function is called in set_detector_config_file function. Because we read this value from the config file in this function.
    * \param B_L1 : the linear function parameter correspond to the thip B, counter Low, threshold 1.
	* \return void
    */
    void set_B_L1(float B_L1);

    /**
    * \fn void set_B_L2(float B_L2)
    * \brief set the linear function parameter correspond to the thip B, counter Low, threshold 2.
    * This function is called in set_detector_config_file function. Because we read this value from the config file in this function.
    * \param B_L2 : the linear function parameter correspond to the thip B, counter Low, threshold 2.
	* \return void
    */
    void set_B_L2(float B_L2);

    /**
    * \fn void set_A_H1(float A_H1)
    * \brief set the linear function parameter correspond to the thip A, counter High, threshold 1.
    * This function is called in set_detector_config_file function. Because we read this value from the config file in this function.
    * \param A_H1 : the linear function parameter correspond to the thip A, counter High, threshold 1.
	* \return void
    */
    void set_A_H1(float A_H1);

    /**
    * \fn void set_A_H2(float A_H2)
    * \brief set the linear function parameter correspond to the thip A, counter High, threshold 2.
    * This function is called in set_detector_config_file function. Because we read this value from the config file in this function.
    * \param A_H2 : the linear function parameter correspond to the thip A, counter High, threshold 2.
	* \return void
    */
    void set_A_H2(float A_H2);

    /**
    * \fn void set_B_H1(float B_H1)
    * \brief set the linear function parameter correspond to the thip B, counter High, threshold 1.
    * This function is called in set_detector_config_file function. Because we read this value from the config file in this function.
    * \param B_H1 : the linear function parameter correspond to the thip B, counter High, threshold 1.
	* \return void
    */
    void set_B_H1(float B_H1);

    /**
    * \fn void set_B_H2(float B_H2)
    * \brief set the linear function parameter correspond to the thip B, counter High, threshold 2.
    * This function is called in set_detector_config_file function. Because we read this value from the config file in this function.
    * \param B_H2 : the linear function parameter correspond to the thip B, counter High, threshold 2.
	* \return void
    */
    void set_B_H2(float B_H2);

    /**
    * \fn std::size_t get_current_pixel_depth()
    * \brief get the current pixel depth. We have two types: 14 or 2 bits per counter.
    * \param none
	* \return current pixel depth
    */
    std::size_t get_current_pixel_depth();

    /**
    * \fn void soft_reset()
    * \brief Reset SFPs in case of error.
    * Reset data fifo and image frame count.
    * \param none
	* \return void
    */
    void soft_reset();

    /**
    * \fn void empty_buffer()
    * \brief this function likes the start_acquisition function for one image in the Software raw mode.
    * This function is called in the set_detector_config_file function to read the config image data.
    * \param none
	* \return void
    */
    void empty_buffer();

    /**
    * \fn std::size_t get_current_width()
    * \brief get the width image.
    * \param none
	* \return the width image
    */
    std::size_t get_current_width();

    /**
    * \fn std::size_t get_current_height()
    * \brief get the height image.
    * \param none
	* \return the height image
    */
	std::size_t get_current_height();

private:
    std::map<EnumAcquisitionConfigKey, std::string> m_acquisition_registers;
    CaptureManager * m_capture_manager_acqui;
    ConfigPortInterface * m_config_port_interface;
    /// The parameters of the linear function (value = f(energy)).
    /// For each chip, we have two thresholds. And for each image, we have two counters (High and Low)
    float m_A_L1, m_A_L2, m_B_L1, m_B_L2, m_A_H1, m_A_H2, m_B_H1, m_B_H2;
};
} /// namespace ufxclib

#endif /// UFXCLIB_CONFIGACQUISITION_H_
