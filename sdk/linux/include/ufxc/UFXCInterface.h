/**
 *  \file UFXCInterface.h
 *  \brief header file of the UFXCInterface class
 *  \author Ayoub GHAMMOURI
 *  \version 0.1
 *  \date November 29 2018
 *  Created on: August 21 2018
 */

#ifndef UFXCLIB_UFXCINTERFACE_H_
#define UFXCLIB_UFXCINTERFACE_H_

#include <fstream>
#include <yat/file/FileName.h>

#include "ufxc/CaptureManager.h"
#include "ufxc/ConfigAcquisition.h"
#include "ufxc/ConfigDetector.h"
#include "ufxc/ConfigPortInterface.h"
#include "ufxc/DaqConnection.h"
#include "ufxc/DaqMonitoring.h"
#include "ufxc/DataReceiver.h"
#include "ufxc/UFXCLibTypesAndConsts.h"
#include "ufxc/UFXCException.h"

/**
 * \namespace ufxclib
 */
namespace ufxclib
{

/**
 * \class UFXCInterface
 * \brief This class is the first and the only interface with the final client.
 * For access to the functions of other classes you have to use this class.
 */
class UFXCInterface
{
public:

    /**
    * \fn UFXCInterface()
    * \brief default constructor
    * \param none
	* \return none
    */
    UFXCInterface();

    /**
    * \fn virtual ~UFXCInterface()
    * \brief destructor
    * \param none
	* \return none
    */
    virtual ~UFXCInterface();

    /**
    * \fn void open_connection(T_UfxcLibCnx tcpCnx, T_UfxcLibCnx SFPpCnx1, T_UfxcLibCnx SFPpCnx2, T_UfxcLibCnx SFPpCnx3)
    * \brief open one TCP connection and tree UDP connection with DAQ
    * \param tcpCnx struct contain the tcp connection parameters (IP, Port, ...).
    * \param SFPpCnx1 struct contain the SFP1 connection parameters (IP, Port, ...).
    * \param SFPpCnx2 struct contain the SFP2 connection parameters (IP, Port, ...).
    * \param SFPpCnx3 struct contain the SFP3 connection parameters (IP, Port, ...).
	* \return void
    */
    void open_connection(T_UfxcLibCnx tcpCnx, T_UfxcLibCnx SFPpCnx1, T_UfxcLibCnx SFPpCnx2, T_UfxcLibCnx SFPpCnx3);

    /**
    * \fn void close_connection()
    * \brief close the TCP connection and the tree UDP connection with DAQ
    * \param none
	* \return void
    */
    void close_connection();

    /**
     * \fn DaqMonitoring * get_daq_monitoring_obj()
     * \brief get a DaqMonitoring object to access to the members of this class.
     * \param none
 	* \return DaqMonitoring object
     */
    DaqMonitoring * get_daq_monitoring_obj();

    /**
     * \fn ConfigDetector * get_config_detector_obj()
     * \brief get a ConfigDetector object to access to the members of this class.
     * \param none
 	* \return ConfigDetector object
     */
    ConfigDetector * get_config_detector_obj();

    /**
     * \fn DataReceiver * get_data_receiver_obj()
     * \brief get a DataReceiver object to access to the members of this class.
     * \param none
 	* \return DataReceiver object
     */
    DataReceiver * get_data_receiver_obj();

    /**
     * \fn ConfigAcquisition * get_config_acquisition_obj()
     * \brief get a ConfigAcquisition object to access to the members of this class.
     * \param none
 	* \return ConfigAcquisition object
     */
    ConfigAcquisition * get_config_acquisition_obj();

    /**
    * \fn void set_detector_config_file(std::string file_path_and_name)
    * \brief this function read informations from the config file. And recode these informations in the lib.
    * The format of the information in the config file is in the form of "name = value".
    * \param file_path_and_name
	* \return void
    */
    void set_detector_config_file(std::string file_path_and_name);

    /**
     * \fn std::string get_UFXC_lib_version()
     * \brief get the library version
     * \param file_path_and_name
 	* \return library version
     */
    std::string get_UFXC_lib_version();

private:
    DaqMonitoring * m_daq_monitoring;
    ConfigDetector * m_config_detector;
    DataReceiver * m_data_receiver;
    ConfigAcquisition * m_config_acquisition;
    ConfigPortInterface * m_config_port_interface;
    DaqConnection * m_daq_connection_SFP1;
    DaqConnection * m_daq_connection_SFP2;
    DaqConnection * m_daq_connection_SFP3;
    CaptureManager * m_capture_manager;

    /**
    * \fn void split_text(const std::string &txt, std::vector<std::vector<std::uint32_t>> &vect, char ch)
    * \brief split a text to many strings. Convert string to uint32_t. And puts these values in a new vector.
    * \param txt text to split
    * \param vect it is an output vector filled with the words that constitutes the text.
    * \param ch separator between 2 strings of this text
	* \return void
    */
    void split_text(const std::string &txt, std::vector<std::vector<std::uint32_t>> &vect, char ch);
};
}/// namespace ufxclib
#endif /// UFXCLIB_UFXCINTERFACE_H_
