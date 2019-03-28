/**
 *  \file DaqMonitoring.h
 *  \brief header file of DaqMonitoring class
 *  \author Ayoub GHAMMOURI
 *  \version 0.1
 *  \date November 29 2018
 *  Created on: August 02 2018
 */

#ifndef UFXCLIB_DATARECEIVER_H_
#define UFXCLIB_DATARECEIVER_H_

#include "ufxc/CaptureManager.h"
#include "ufxc/ConfigPortInterface.h"

/**
 * \namespace ufxclib
 */
namespace ufxclib
{

/**
 * \class DaqMonitoring
 * \brief This class provide access to DAQ to read UDP data
 */
class DataReceiver: public yat::Thread
{
public:

    /**
    * \fn DataReceiver()
    * \brief default constructor
    * \param none
	* \return none
    */
    //- FL: utile?
    DataReceiver()
    {

    }

    /**
    * \fn DataReceiver(ConfigPortInterface * config_port_interface, CaptureManager * capture_manager)
    * \brief Constructor
    * \param config_port_interface : ConfigPortInterface object for all TCP communications with DAQ.
    * \param capture_manager : CaptureManager object to recover images data read in the the udp listener.
	* \return none
    */
    DataReceiver(ConfigPortInterface * config_port_interface, CaptureManager * capture_manager);

    /**
    * \fn virtual ~DataReceiver()
    * \brief destructor
    * \param none
	* \return none
    */
    virtual ~DataReceiver();

    /**
    * \fn void exit()
    * \brief Function call the join
    * \param none
	* \return void
    */
    void exit();

    /**
    * \fn void set_SFP_registers_names(std::map<EnumSFPconfigKey, std::string> map, EnumSFPName sfp_name)
    * \brief Set SFP registers names
    * \param map : it is a map contain the list of registers names for the SFP config
    * \param sfp_name : it is the SFPs ports name (EnumSFPName::SFP1, EnumSFPName::SFP2 or EnumSFPName::SFP3). Maximum, we have three SFP port to receive images data.
	* \return void
    */
    void set_SFP_registers_names(std::map<EnumSFPconfigKey, std::string> map, EnumSFPName sfp_name);

    /**
    * \fn UDPConfig get_SFP_network_config(EnumSFPName sfp_name)
    * \brief Get network configuration for specified SFP output
    * \param sfp_name : it is the SFPs ports name (EnumSFPName::SFP1, EnumSFPName::SFP2 or EnumSFPName::SFP3). Maximum, we have three SFP port to receive images data.
	* \return structure to specify the udp network config.
    */
    UDPConfig get_SFP_network_config(EnumSFPName sfp_name);

    /**
    * \fn void set_SFP_network_config(UDPConfig udp_config, EnumSFPName sfp_name)
    * \brief Set network configuration for specified SFP output
    * \param udp_config : structure to specify the udp network config.
    * \param sfp_name : it is the SFPs ports name (EnumSFPName::SFP1, EnumSFPName::SFP2 or EnumSFPName::SFP3). Maximum, we have three SFP port to receive images data.
	* \return void
    */
    void set_SFP_network_config(UDPConfig udp_config, EnumSFPName sfp_name);

    /**
    * \fn void get_all_images(char ** data_images_table, EnumAcquisitionMode acq_mode, std::size_t images_nb, std::size_t & frame_number)
    * \brief read all images data from the udp listeners buffers (from 3 SFPs) and puts this data in the data_images_table buffer.
    * \param data_images_table : buffer located by client. The function puts all images data in this buffer.
    * \param acq_mode : acquisition mode. This parameter determine  how many bits per pixel
    * \param images_nb : it is an input parameter represent the images number.
    * \param frame_number : it is an output parameter to get the frames number of all images.
	* \return void
    */
    void get_all_images(char ** data_images_table, EnumAcquisitionMode acq_mode, std::size_t images_nb, std::size_t & frame_number);

    /**
    * \fn void get_all_frames(yat::Socket::Data images_buffer_frames[], std::size_t & frame_number)
    * \brief read all frames data from the udp listeners buffers (from 3 SFPs) and puts this data in the images_buffer_frames buffer.
    * \param images_buffer_frames[] : it is an in/OUt buffer. The function puts all of frames data in this buffer.
    * \param frame_number : it is an output parameter to get the frames number of all images.
	* \return void
    */
    void get_all_frames(yat::Socket::Data images_buffer_frames[], std::size_t & frame_number);

    /**
    * \fn void ordering_data(yat::Socket::Data img_raw_Two_chip[], char **data_images_table, EnumAcquisitionMode acq_mode, std::size_t images_nb, std::size_t & frame_number)
    * \brief order the images frames received from 3 SFPs and puts them in the data_images_table buffer.
    *  This function orders the frames of each image. And it orders the images of each acquisition.
    * \param img_raw_Two_chip[] : It is an input buffer contains all images frames.
    * \param data_images_table : buffer located by client. The function puts all images data in this buffer.
    * \param acq_mode : acquisition mode. This parameter determine  how many bits per pixel
    * \param images_nb : it is an input parameter represent the images number.
    * \param frame_number : get the frames number of all images.
	* \return void
    */
    void ordering_data(yat::Socket::Data img_raw_Two_chip[], char **data_images_table, EnumAcquisitionMode acq_mode, std::size_t images_nb, std::size_t & frame_number);

    /**
    * \fn std::size_t get_frame_number_for_2_counters(EnumAcquisitionMode acq_mode)
    * \brief get the frames number for two counters
    * \param acq_mode : acquisition mode. This parameter determine  how many bits per pixel
	* \return the frames number
    */
    std::size_t get_frame_number_for_2_counters(EnumAcquisitionMode acq_mode);

    /**
    * \fn void get_pixel_config_image (char ** data_image_table, std::size_t & frame_number)
    * \brief read the config image data from the udp listeners buffers (from 3 SFPs) and puts this data in the data_image_table buffer.
    * this function call the get_all_images with ((images_nb = 1) && (acq_mode = ufxclib::EnumAcquisitionMode::software_raw))
    * \param data_image_table : buffer located by client. The function puts all images data in this buffer.
    * \param frame_number : it is an output parameter to get the frames number of all images.
	* \return void
    */
    void get_pixel_config_image (char ** data_image_table, std::size_t & frame_number);

protected:
    /**
    * \fn virtual yat::Thread::IOArg run_undetached(yat::Thread::IOArg ioa)
    * \brief the thread entry point - called by yat::Thread::start_undetached.
    * It is the thread to read the two buffers filled in the UDP listener (images buffers).
    * \param ioa
	* \return none
    */
    virtual yat::Thread::IOArg run_undetached(yat::Thread::IOArg ioa);

private:
    CaptureManager * m_capture_manager;
    ConfigPortInterface * m_config_port_interface;
    std::map<EnumSFPconfigKey, std::string> m_SFP_registers[3];
    yat::Socket::Data * m_images_buffer_frames;
    std::size_t * m_frames_number;
};

} /// namespace ufxclib

#endif /// UFXCLIB_DATARECEIVER_H_
