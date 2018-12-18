/**
 *  \file ConfigPortInterface.h
 *  \brief header file of ConfigPortInterface class
 *  \author Ayoub GHAMMOURI
 *  \version 0.1
 *  \date November 29 2018
 *  Created on: July 04  2018
 */

#ifndef UFXCLIB_CONFIGPORTINTERFACE_H_
#define UFXCLIB_CONFIGPORTINTERFACE_H_

#include "ufxc/DaqConnection.h"

/**
 * \namespace ufxclib
 */
namespace ufxclib
{
/**
 *  \class ConfigPortInterface
 *  \brief This class is used for all TCP connection with DAQ. All set/get DAQ registers passed through this class.
 */
class ConfigPortInterface: public DaqConnection
{

public:

    /**
    * \fn ConfigPortInterface()
    * \brief Default constructor
    * \param none
    * \return none
    */
    ConfigPortInterface();

    /**
    * \fn virtual ~ConfigPortInterface()
    * \brief destructor
    * \param none
	* \return none
    */
    virtual ~ConfigPortInterface();

    /**
    * \fn void open(T_UfxcLibCnx cnx)
    * \brief Open DAQ connection(s). This connection is used for all set/get DAQ registers values.
    * \param cnx : structure contains the connection parameters.
	* \return void
    */
    void open(T_UfxcLibCnx cnx);

    /**
    * \fn void close()
    * \brief Close DAQ connection(s) (if not already disconnected)
    * \param none
	* \return void
    */
    void close();

    /**
    * \fn template<class T> T read_register(std::string name)
    * \brief read the register value
    * \param name : register name
	* \return Register value
    */
    template<typename T>
    T read_register(const std::string& name)
    {
        yat::log_verbose("UFXCLib", "ConfigPortInterface::read_register() entering...");
        yat::log_verbose("UFXCLib", " --> register name: %s", name.c_str());

        std::string l_str = "";
        T l_result;

        if (!m_socket)
        {
            throw ufxclib::Exception("INTERNAL_ERROR",
                                      "Cannot access socket ptr!",
                                      "ConfigPortInterface::read_register");
        }

        try
        {
            /// write request on the underlying socket
            std::string l_request = name + std::string("?\n");
            m_socket->send(l_request);

            /// wait then get data from peer
            m_socket->wait_input_data(UFXC_BOX_WAIT);
            m_socket->receive(l_str);
        } 
        catch (const yat::SocketException & se)
        {
            throw ufxclib::Exception("SOCKET_ERROR", 
                                     se.text().c_str(),
                                     "ConfigPortInterface::read_register");
        }
        catch (...)
        {
            throw ufxclib::Exception("UNKNOWN_ERROR",
                                     "Cannot read register: unknown error!",
                                     "ConfigPortInterface::read_register");
        }

        /// Analyze answer
        std::string::size_type firstIdx;
        firstIdx = l_str.find_first_of(UFXC_OK_REQUEST_STRING);

        if (firstIdx == 0) /// good answer
        {
            std::string l_str_l = "";
            std::string l_str_r = "";
            yat::StringUtil::split(l_str, '=', &l_str_l, &l_str_r);
            l_str = l_str_r;
        } 
        else
        {
            firstIdx = l_str.find_first_of(UFXC_ERROR_STRING);
            if (firstIdx == 0)
            {
                /// error
                std::string l_err_str = "Cannot read register: " + name + " Returned: " + l_str;
                throw ufxclib::Exception("READ_ERROR", 
                                         l_err_str.c_str(),
                                         "ConfigPortInterface::read_register");
            } 
            else
            {
                /// unknown answer !!
                std::string l_err_str = "Cannot read register: " + name + " Unknown read error: " + l_str;
                throw ufxclib::Exception("UNKNOWN_READ_ERROR",
                                        l_err_str.c_str(),
                                        "ConfigPortInterface::read_register");
            }
        }

        l_result = yat::XString<T>::to_num(l_str);
        return l_result;
    }

    /**
    * \fn specialized std::string read_register_s(std::string name)
    * \brief read the register value
    * \param name : Register name
    * \return Register value (a string value)
    */
    //- TODO: use template specialization
    std::string read_register_s(const std::string& name)
    {
        yat::log_verbose("UFXCLib", "ConfigPortInterface::read_register_s<string>() entering...");
        yat::log_verbose("UFXCLib", " --> register name: %s", name.c_str());

        std::string l_str = "";

        if (!m_socket)
        {
            throw ufxclib::Exception("INTERNAL_ERROR",
                                    "Cannot access socket ptr!",
                                    "ConfigPortInterface::read_register_s");
        }

        try
        {
            /// write request on the underlying socket
            std::string l_request = name + std::string("?\n");
            m_socket->send(l_request);

            /// wait then get data from peer
            m_socket->wait_input_data(UFXC_BOX_WAIT);
            m_socket->receive(l_str);
        }
        catch (const yat::SocketException & se)
        {
            throw ufxclib::Exception("SOCKET_ERROR",
                                    se.text().c_str(),
                                    "ConfigPortInterface::read_register_s");
        }
        catch (...)
        {
            throw ufxclib::Exception("UNKNOWN_ERROR",
                                    "Cannot read register: unknown error!",
                                    "ConfigPortInterface::read_register_s");
        }

        /// Analyze answer
        std::string::size_type firstIdx;
        firstIdx = l_str.find_first_of(UFXC_OK_REQUEST_STRING);

        if (firstIdx == 0) /// good answer
        {
            std::string l_str_l = "";
            std::string l_str_r = "";
            yat::StringUtil::split(l_str, '=', &l_str_l, &l_str_r);
            l_str = l_str_r;
        }
        else
        {
            firstIdx = l_str.find_first_of(UFXC_ERROR_STRING);
            if (firstIdx == 0)
            {
                /// error
                std::string l_err_str = "Cannot read register: " + name + " Returned: " + l_str;
                throw ufxclib::Exception("READ_ERROR",
                                         l_err_str.c_str(),
                                         "ConfigPortInterface::read_register_s");
            }
            else
            {
                /// unknown answer !!
                std::string l_err_str = "Cannot read register: " + name + " Unknown read error: " + l_str;
                throw ufxclib::Exception("UNKNOWN_READ_ERROR",
                                         l_err_str.c_str(),
                                         "ConfigPortInterface::read_register_s");
            }
        }

        return l_str;
    }

    /**
    * \fn template<class T> void write_register(std::string name, T value)
    * \brief write value in the register
    * \param name : Register name
    * \param value : Register value
	* \return void
    */
    template<typename T>
    void write_register(std::string const& name, T value)
    {
        yat::log_verbose("UFXCLib", "ConfigPortInterface::write_register() entering...");
        yat::log_verbose("UFXCLib", " --> register name: %s", name.c_str());

        std::string l_str = "";
        
        if (!m_socket)
        {
            throw ufxclib::Exception("INTERNAL_ERROR",
                                     "Cannot access socket ptr!",
                                     "ConfigPortInterface::read_register");
        }

        try
        {
            /// write request on the underlying socket
            std::string l_request = name + "=" + yat::XString<T>::to_string(value) + "\n";
            m_socket->send(l_request);

            /// wait then get data from peer
            m_socket->wait_input_data(UFXC_BOX_WAIT);
            m_socket->receive(l_str);
        } 
        catch (const yat::SocketException & se)
        {
            throw ufxclib::Exception("SOCKET_ERROR",
                                     se.text().c_str(),
                                     "ConfigPortInterface::write_register");
        } 
        catch (...)
        {
            throw ufxclib::Exception("UNKNOWN_ERROR",
                                     "Cannot write register: unknown error!",
                                     "ConfigPortInterface::write_register");
        }

        /// Analyze answer
        std::string::size_type firstIdx;
        firstIdx = l_str.find_first_of(UFXC_OK_CMD_STRING);
        if (firstIdx != 0) /// error
        {
            firstIdx = l_str.find_first_of(UFXC_ERROR_STRING);
            if (firstIdx == 0)
            {
                /// error
                std::string l_err_str = "Cannot write register: " + name + " Returned: " + l_str;

                throw ufxclib::Exception("WRITE_ERROR", 
                                         l_err_str.c_str(),
                                         "ConfigPortInterface::write_register");
            } 
            else
            {
                /// unknown answer !!
                std::string l_err_str = "Cannot write register: " + name + " Unknown read error: " + l_str;

                throw ufxclib::Exception("UNKNOWN_WRITE_ERROR",
                                         l_err_str.c_str(),
                                         "ConfigPortInterface::write_register");
            }
        }
    }

private:

    /// Connection configuration
    T_UfxcLibCnx m_cnx_config;

};

} /// namespace ufxclib

#endif /// UFXCLIB_CONFIGPORTINTERFACE_H_

