/********************************************//**
 *  ConfigPortInterface.h
 ***********************************************/
/********************************************//**
 *  Created on: 04 July 2018
 *  Author: GHAMMOURI Ayoub
 *  Class: ConfigPortInterface
 *  Description:
 ***********************************************/

#ifndef CONFIGPORTINTERFACE_H_
#define CONFIGPORTINTERFACE_H_

/********************************************//**
 *  DEPENDENCIES
 ***********************************************/
#include "DaqConnection.h"

namespace ufxclib {
/********************************************//**
* Configuration registers
 ***********************************************/
const std::string kUFXC_REGISTER_LENGTH = ".LENGTH?\n";
const std::string kUFXC_REGISTER_MAX_LENGTH = ".MAX_LENGTH?\n";


/********************************************//**
 *  API DEFINITION
 ***********************************************/
class ConfigPortInterface : public DaqConnection
{

public:
  //!< Constructor.
  ConfigPortInterface();
  ConfigPortInterface(yat::ClientSocket * socket);

  //!< Destructor.
  virtual ~ConfigPortInterface();

  //!< Open DAQ box connection(s)
  //!< @param cnx DAQ box connection(s) definition
  //!< Opens the CONFIG connection.
  //!< Opens the DATA connection if data port is not null.
  void open(T_UfxcLibCnx cnx)
    throw (ufxclib::Exception);

  //!< Close connection(s) (if not already disconnected)
  void close()
    throw (ufxclib::Exception);


  //!< Read register
  //!< @param name Register name
  //!< @return Register value
  template <class T>
  T readRegister(std::string name)
    throw (ufxclib::Exception)
  {
    std::string l_str = "";
    T l_result;

    if (!m_socket)
    {
        throw ufxclib::Exception("INTERNAL_ERROR",
                     "Cannot access socket ptr!",
                    "ConfigPortInterface::readRegister");
    }
    try
    {
      //!< write request on the underlying socket
      std::string l_request = name + std::string("?\n");
      m_socket->send(l_request);

      //!< wait then get data from peer
      m_socket->wait_input_data(kUFXC_BOX_WAIT);
      m_socket->receive(l_str);
    }
    catch (const yat::SocketException & se)
    {
      throw ufxclib::Exception("SOCKET_ERROR",
        se.text().c_str(),
        "ConfigPortInterface::readRegister");
    }
    catch (...)
    {
      throw ufxclib::Exception("UNKNOWN_ERROR",
        "Cannot read register: unknown error!",
        "ConfigPortInterface::readRegister");
    }

    //!< Analyze answer
    std::string::size_type firstIdx;
    firstIdx = l_str.find_first_of(kUFXC_OK_REQUEST_STRING);

    if (firstIdx == 0) //!< good answer
    {
      std::string l_str_l = "";
      std::string l_str_r = "";
      yat::StringUtil::split(l_str, '=', &l_str_l, &l_str_r);
      l_str = l_str_r;
    }
    else
    {
      firstIdx = l_str.find_first_of(kUFXC_ERROR_STRING);
      if (firstIdx == 0)
      {
        //!< error
        std::string l_err_str = std::string("Cannot read register: ") + name + std::string(" Returned: ") + l_str;
        throw ufxclib::Exception("READ_ERROR",
                         l_err_str.c_str(),
                         "ConfigPortInterface::readRegister");
      }
      else
      {
        //!< unknown answer !!
        std::string l_err_str = std::string("Cannot read register: ") + name + std::string(" Unknown read error: ") + l_str;
        throw ufxclib::Exception("UNKNOWN_READ_ERROR",
                     l_err_str.c_str(),
                     "ConfigPortInterface::readRegister");
      }
    }

    l_result = yat::XString<T>::to_num(l_str);
    return l_result;
  }

  //!< Write register
  //!< @param name Register name
  //!< @param value Register value
  template <class T>
  void writeRegister(std::string name, T value)
    throw (ufxclib::Exception)
  {
    yat::log_verbose("ufxclib", "ConfigPortInterface::writeRegister() entering...");
    yat::log_verbose("ufxclib", " --> register name: %s", name.c_str());

    std::string l_str = "";
    if (!m_socket)
        throw ufxclib::Exception("INTERNAL_ERROR",
                     "Cannot access socket ptr!",
                     "ConfigPortInterface::writeRegister");

    try
    {
      //!< write request on the underlying socket
      std::string l_request = name + std::string("=") + yat::XString<T>::to_string(value) + std::string("\n");
      m_socket->send(l_request);

      //!< wait then get data from peer
      m_socket->wait_input_data(kUFXC_BOX_WAIT);
      m_socket->receive(l_str);
    }
    catch (const yat::SocketException & se)
    {
      throw ufxclib::Exception("SOCKET_ERROR",
        se.text().c_str(),
        "ConfigPortInterface::writeRegister");
    }
    catch (...)
    {
      throw ufxclib::Exception("UNKNOWN_ERROR",
        "Cannot write register: unknown error!",
        "ConfigPortInterface::writeRegister");
    }

    //!< Analyze answer
    std::string::size_type firstIdx;
    firstIdx = l_str.find_first_of(kUFXC_OK_CMD_STRING);
    if (firstIdx == 0) //!< good answer
    {
      //!<ok
    }
    else
    {
      firstIdx = l_str.find_first_of(kUFXC_ERROR_STRING);
      if (firstIdx == 0)
      {
        //!< error
        std::string l_err_str = std::string("Cannot write register: ") + name + std::string(" Returned: ") + l_str;
        throw ufxclib::Exception("WRITE_ERROR",
                         l_err_str.c_str(),
                         "ConfigPortInterface::writeRegister");
      }
      else
      {
        // unknown answer !!
        std::string l_err_str = std::string("Cannot write register: ") + name + std::string(" Unknown read error: ") + l_str;
        throw ufxclib::Exception("UNKNOWN_WRITE_ERROR",
                     l_err_str.c_str(),
                     "ConfigPortInterface::writeRegister");
      }
    }
  }

  //TODO: This function is not tested
  //!< Read table
  //!<  @param name Table
  //!<  @return Table values
  template <class T>
  std::vector<T> readTable(std::string name)
    throw (ufxclib::Exception)
  {
    std::string l_str="";
    std::vector<T> l_res_list;

    if (!m_socket)
        throw ufxclib::Exception("INTERNAL_ERROR",
                     "Cannot access socket ptr!",
                     "ConfigPortInterface::readTable");
    try
    {
      //!< write request on the underlying socket
      std::string l_request = name + std::string("?\n");
      m_socket->send(l_request);

      //!< wait then get data from peer
      m_socket->wait_input_data(kUFXC_BOX_WAIT);
      m_socket->receive(l_str);
    }
    catch (const yat::SocketException & se)
    {
      throw ufxclib::Exception("SOCKET_ERROR",
        se.text().c_str(),
        "ConfigPortInterface::readTable");
    }
    catch (...)
    {
      throw ufxclib::Exception("UNKNOWN_ERROR",
        "Cannot read table: unknown error!",
        "ConfigPortInterface::readTable");
    }

    //!< Analyze answer
    std::string::size_type firstIdx;
    if (l_str[0] == '!') //!< good answer
    {
      std::vector<std::string> splited_line;
      std::vector<std::string> str_res;

      //!< seperate lines (delete '\n')
      yat::StringUtil::split(l_str, '\n', &splited_line);

      // delete last line (should be a single '.')
      splited_line.pop_back();

      //!< delete '!' at each beginning of line
      for (size_t idx = 0; idx < splited_line.size(); idx++)
        str_res.push_back(splited_line[idx].substr(1, std::string::npos));

      //!< transform string to T type
      for (size_t idx = 0; idx < str_res.size(); idx++)
        l_res_list.push_back(yat::XString<T>::to_num(str_res[idx]));
    }
    else
    {
      firstIdx = l_str.find_first_of(kUFXC_ERROR_STRING);
      if (firstIdx == 0)
      {
        //!< error
        std::string l_err_str = std::string("Cannot read table: ") + name + std::string(" Returned: ") + l_str;
        throw ufxclib::Exception("READ_ERROR",
                l_err_str.c_str(),
                "ConfigPortInterface::readTable");
      }
      else
      {
        //!< unknown answer !!
        std::string l_err_str = std::string("Cannot read table: ") + name + std::string(" Unknown read error: ") + l_str;
        throw ufxclib::Exception("UNKNOWN_READ_ERROR",
                l_err_str.c_str(),
                "ConfigPortInterface::readTable");
      }
    }

    return l_res_list;
  }

  //TODO: This function is not tested
  //!< Write table
  //!< @param name Table name
  //!< @param values Table values
  template <class T>
  void writeTable(std::string name, std::vector<T>& values)
    throw (ufxclib::Exception)
  {
    yat::log_verbose("ufxclib", "ConfigPortInterface::writeTable() entering...");
    yat::log_verbose("ufxclib", " --> table name: %s", name.c_str());

    std::string l_str = "";

    if (!m_socket)
        throw ufxclib::Exception("INTERNAL_ERROR",
                     "Cannot access socket ptr!",
                     "ConfigPortInterface::writeTable");

    try
    {
      //- write request on the underlying socket
      std::string l_request = name + std::string("<\n");
      for (size_t idx = 0; idx < values.size(); idx++)
        l_request += yat::XString<T>::to_string(values[idx]) + std::string("\n");

      l_request += std::string("\n");

      yat::log_verbose("ufxclib", " --> send command: %s", l_request.c_str());

      m_socket->send(l_request);

      //- wait then get data from peer
      m_socket->wait_input_data(kUFXC_BOX_WAIT);
      m_socket->receive(l_str);
    }
    catch (const yat::SocketException & se)
    {
      throw ufxclib::Exception("SOCKET_ERROR",
        se.text().c_str(),
        "ConfigPortInterface::writeTable");
    }
    catch (...)
    {
      throw ufxclib::Exception("UNKNOWN_ERROR",
        "Cannot write register: unknown error!",
        "ConfigPortInterface::writeTable");
    }

    // Analyze answer
    std::string::size_type firstIdx;
    firstIdx = l_str.find_first_of(kUFXC_OK_CMD_STRING);
    if (firstIdx == 0) // good answer
    {
      //ok
    }
    else
    {
      firstIdx = l_str.find_first_of(kUFXC_ERROR_STRING);
      if (firstIdx == 0)
      {
        // error
        std::string l_err_str = std::string("Cannot write table: ") + name + std::string(" Returned: ") + l_str;
        throw ufxclib::Exception("READ_ERROR",
                         l_err_str.c_str(),
                         "ConfigPortInterface::writeTable");
      }
      else
      {
        // unknown answer !!
        std::string l_err_str = std::string("Cannot write table: ") + name + std::string(" Unknown read error: ") + l_str;
        throw ufxclib::Exception("UNKNOWN_READ_ERROR",
                     l_err_str.c_str(),
                     "ConfigPortInterface::writeTable");
      }
    }
  }

  //TODO: This function is not tested
  //!< Get table length
  //!< @param name Table name
  //!< @return Table length
  unsigned int getTableLength(std::string name)
    throw (ufxclib::Exception);

  //TODO: This function is not tested
  //!< Get table max length
  //!< @param name Table name
  //!< @return Table max length
  unsigned int getTableMaxLength(std::string name)
    throw (ufxclib::Exception);

private:

  //!< Connection configuration
  T_UfxcLibCnx m_cnxConfig;

  };

} //!< namespace ufxclib

#endif //!< CONFIGPORTINTERFACE_H_

