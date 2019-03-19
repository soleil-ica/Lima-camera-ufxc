/**
 *  \file DaqConnection.h
 *  \brief header file of DaqConnection class
 *  \author Ayoub GHAMMOURI
 *  \version 0.1
 *  \date November 29 2018
 *  Created on: June 29 2018
 */

#ifndef UFXCLIB_DAQCONNECTION_H_
#define UFXCLIB_DAQCONNECTION_H_

#include <sstream>
#include <yat/network/ClientSocket.h>
#include <yat/utils/XString.h>

#include "ufxc/UFXCLibTypesAndConsts.h"

/**
 * \namespace ufxclib
 */
namespace ufxclib
{

/**
 *  \class DaqConnection
 *  \brief This class is used to manage the "control" TCP/UDP and socket to access to the DAQ board.
 *  This class is used for all TCP/UDP connection with DAQ. All UDP data passed through this class.
 */
class DaqConnection
{

public:

    /**
    * \fn DaqConnection()
    * \brief Default constructor
    * \param none
	* \return none
    */
    DaqConnection();

    /**
    * \fn virtual ~DaqConnection()
    * \brief destructor
    * \param none
	* \return none
    */
    virtual ~DaqConnection();

    /**
    * \fn void connect(std::string ip_address, yat::uint32 port, EnumProtocol protocol = TCP)
    * \brief This function is used to connect to the DAQ. We can use the UDP or the TCP protocole to connect to the DAQ.
    * \param ip_address: UDP or TCP ip. It is the first parameter to connect to the DAQ.
    * \param port: UDP or TCP port. It is the second parameter to connect to the DAQ.
    * \param protocol: UDP or TCP
	* \return void
    */
    void connect(std::string ip_address, yat::uint32 port, EnumProtocol protocol = TCP);

    /**
    * \fn void disconnect()
    * \brief This function is used to disconnect to the DAQ.
    * \param none
	* \return void
    */
    void disconnect();

    /**
    * \fn bool is_connected()
    * \brief This function is used to check if the application is connected to the DAQ.
    * \param none
	* \return bool : Connected if the function returned a true. Else disconnected
    */
    bool is_connected();

    /**
    * \fn yat::ClientSocket * get_socket()
    * \brief get socket pointer
    * \param none
	* \return a pointer to yat::ClientSocket
    */
    yat::ClientSocket * get_socket()
    {
        return m_socket;
    }

protected:

    /// socket ptr
    yat::ClientSocket * m_socket;

private:

    /// socket timeout_ms (ms)
    yat::uint32 m_tmo_ms;

};
} /// namespace ufxclib

#endif /// UFXCLIB_DAQCONNECTION_H_
