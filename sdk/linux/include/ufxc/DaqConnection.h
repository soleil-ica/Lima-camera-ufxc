/********************************************//**
 *  DaqConnection.h
 ***********************************************/
/********************************************//**
 *  Created on: 29 June 2018
 *  Author: GHAMMOURI Ayoub
 *  Class: DaqConnection
 *  Description: This class is used to manage the "control" TCP/IP socket to access to the DAQ board
 ***********************************************/


#ifndef DaqConnection_H_
#define DaqConnection_H_

/********************************************//**
 *  DEPENDENCIES
 ***********************************************/
#include <sstream>
#include <yat/network/ClientSocket.h>
#include <yat/utils/XString.h>
#include "UfxlibTypesAndConsts.h"

namespace ufxclib {

/********************************************//**
 *  API DEFINITION
 ***********************************************/

class DaqConnection
{

public:
  /**
   * Constructor.
   */
  DaqConnection();

  /**
   * Destructor.
   */
  virtual ~DaqConnection();

  /**
   * DAQ socket connection
   */
  void connect(std::string ip_address, unsigned int port, T_Protocol protocol = TCP)
    throw (ufxclib::Exception);
  /**
   * DAQ socket disconnection
   */
  void disconnect()
    throw (ufxclib::Exception);

  /**
   * checks if DAQ is connected
   */
  bool isConnected();

  /**
   * gets socket timeout (in ms)
   */
  unsigned int getTimeout();

  /**
   * sets socket timeout (in ms)
   */
  void setTimeout(unsigned int timeout_ms);

  yat::ClientSocket * getSocket()
  {
	  return m_socket;
  }

protected:
  /**
   * socket ptr
   */
  yat::ClientSocket * m_socket;

private:
  /**
   * socket timeout (ms)
   */
  unsigned int m_tmo;

  };
} //!< namespace ufxclib

#endif //!< DaqConnection_H_
