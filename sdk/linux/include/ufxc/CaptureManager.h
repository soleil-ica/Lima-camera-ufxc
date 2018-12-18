/**
 *  \file CaptureManager.h
 *  \brief header file of CaptureManager class
 *  \author Ayoub GHAMMOURI
 *  \version 0.1
 *  \date November 29 2018
 *  Created on: August 21 2018
 */

#ifndef UFXCLIB_CAPTUREMANAGER_H_
#define UFXCLIB_CAPTUREMANAGER_H_

#include "ufxc/DaqConnection.h"
#include "ufxc/DataCaptureListener.h"

/**
 * \namespace ufxclib
 */
namespace ufxclib
{
/**
 *  \class CaptureManager
 *  \brief This class contains:
 *  -1- The creation of three UDPs Listeners to receive the UDP data.
 *  Each listener is started in a new thread.And in each thread, we fill two buffers.
 *  We first fill the first, and if we still have data and the max size of the first buffer has arrived, we start filling the second buffer.
 *  -2- The reading of the two buffers fills in the acquisition:
 *  This reading is processed in a new thread. If the first buffer is released by the mutex, we can start reading the first buffer.
 *  After that, we repeat the same thing for the second buffer. These two buffers are filled with frames.
 *  And the size of each frame is 1030 bytes (6 bytes for the header and the remains are data).
 */
class CaptureManager
{
public:

    /**
    * \fn CaptureManager(DaqConnection * daq_connection_SFP1, DaqConnection * daq_connection_SFP2, DaqConnection * daq_connection_SFP3)
    * \brief Constructor
    * \param daq_connection_SFP1 : DaqConnection object to connect to the SFP1
    * \param daq_connection_SFP2 : DaqConnection object to connect to the SFP2
    * \param daq_connection_SFP3 : DaqConnection object to connect to the SFP3
	* \return none
    */
    CaptureManager(DaqConnection * daq_connection_SFP1, DaqConnection * daq_connection_SFP2, DaqConnection * daq_connection_SFP3);

    /**
    * \fn void init_capture()
    * \brief initialize config before receiving data from 3 SFPs
    * \param none
    * \return void
    */
    void init_capture();

    /**
    * \fn virtual ~CaptureManager()
    * \brief destructor
    * \param none
	* \return none
    */
    virtual ~CaptureManager();

    /**
    * \fn void start_capture()
    * \brief start 3 Listeners UDP to receive data from 3 SFPs
    * \param none
	* \return void
    */
    void start_capture();

    /**
    * \fn void stop_capture()
    * \brief stop 3 Listeners UDP (3 SFPs)
    * \param none
	* \return void
    */
    void stop_capture();

    /**
    * \fn yat::uint32 read_data_from_SFP(yat::Socket::Data images_buffer_frames[])
    * \brief read the filled buffers in the acquisition
    * \param images_buffer_frames[] : Frames buffer. This frames are read in the acquisition
	* \return frames number
    */
    yat::uint32 read_data_from_SFP(yat::Socket::Data images_buffer_frames[]);

    /**
    * \fn void set_first_buffer_size_frames(std::size_t first_buffer_size_frames)
    * \brief For each listener UDP we have two buffers data. This function set the size of the first buffer.
    * \param first_buffer_size_frames : The listener puts the read data in the first buffer if the data size is inferior the first_buffer_size_frames else it puts data in the second buffer
	* \return void
    */
    void set_first_buffer_size_frames(std::size_t first_buffer_size_frames);

    /**
    * \fn yat::uint32 get_first_buffer_size_frames()
    * \brief For each listener UDP we have two buffers data. This function get the size of the first buffer.
    * \param none
	* \return buffer size
    */
    yat::uint32 get_first_buffer_size_frames();

    /**
    * \fn void set_timeout_ms(yat::uint32 timeout_ms)
    * \brief set time out in millisecond to read data from udp. If we don't receive data after this time, the lister exit.
    * \param timeout_ms : the time out in millisecond.
	* \return void
    */
    void set_timeout_ms(yat::uint32 timeout_ms);

private:

    DaqConnection * m_daq_connection_SFP1;
    DaqConnection * m_daq_connection_SFP2;
    DaqConnection * m_daq_connection_SFP3;

    DataCaptureListener * m_data_capture_listener_SFP1;
    DataCaptureListener * m_data_capture_listener_SFP2;
    DataCaptureListener * m_data_capture_listener_SFP3;
    yat::uint32 m_buffer_one_size_frames;
    yat::uint32 m_timeout_ms;

};

}            /// namespace ufxclib
#endif /// UFXCLIB_CAPTUREMANAGER_H_
