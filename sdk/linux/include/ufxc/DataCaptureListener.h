/**
 *  \file DataCaptureListener.h
 *  \brief header file of DataCaptureListener class
 *  \author Ayoub GHAMMOURI
 *  \version 0.1
 *  \date November 29 2018
 *  Created on: July 30 2018
 */

#ifndef UFXCLIB_DATACAPTURELISTENER_H_
#define UFXCLIB_DATACAPTURELISTENER_H_

#include <yat/network/ClientSocket.h>
#include <yat/threading/Thread.h>
#include <deque>
#include <mutex>
#include "ufxc/UFXCLibTypesAndConsts.h"

/**
 * \namespace ufxclib
 */
namespace ufxclib
{

/**
 * \class DataCaptureListener
 * \brief Udp Listener class.
 */
class DataCaptureListener: public yat::Thread
{
public:

    /**
    * \fn DataCaptureListener()
    * \brief default constructor
    * \param none
	* \return none
    */
    DataCaptureListener();

    /**
    * \fn void exit()
    * \brief Function call the join
    * \param none
	* \return void
    */
    void exit();

    /**
    * \fn void start()
    * \brief start receiving data from one SFP via UDP Connection.
    * In this function, we start an UDP listener (in new thread) to read data from SFP and puts the received frames in 2 buffers(std::deque<yat::Socket::Data>).    *
    * \param none
	* \return void
    */
    void start();

    /**
    * \fn void stop()
    * \brief stop receiving data from SFP.
    * \param none
	* \return void
    */
    void stop();

    /**
    * \fn void init(std::size_t first_buffer_size_frames, yat::ClientSocket * sock)
    * \brief initialize config before receiving data from SFP.
    * \param first_buffer_size_frames: each udp listener start to puts data in the first buffer if the size of data is inferior to first_buffer_size_frames.
    * Else the listener puts data in the second buffer.
    * \param sock: UDP socket to connect to the DAQ
	* \return void
    */
    void init(std::size_t first_buffer_size_frames, yat::ClientSocket * sock);

    /**
    * \fn void read_data_from_SFP(yat::Socket::Data images_buffer_frames[], std::size_t & frames_number)
    * \brief This function is started in a new thread for read the images data.
    * So in this function, we read the two buffers filled in the UDP listener.
    * \param images_buffer_frames[] : All frames read from the two buffers are merged in images_buffer_frames[].
    * \param frames_number : It is an out put variable to return the frames number read from the two buffers.
	* \return void
    */
    void read_data_from_SFP(yat::Socket::Data images_buffer_frames[], std::size_t & frames_number);

    /**
    * \fn  bool get_m_reading_buffer_one_is_finished()
    * \brief check if the read_data_from_SFP terminated to reading the first buffer.
    * \param none
	* \return true if the read_data_from_SFP terminated to reading the first buffer. Else false.
    */
    bool get_m_reading_buffer_one_is_finished()
    {
        return m_reading_buffer_one_is_finished;
    }

    /**
    * \fn  bool get_m_reading_buffer_two_is_finished()
    * \brief check if the read_data_from_SFP terminated to reading the second buffer.
    * \param none
	* \return true if the read_data_from_SFP terminated to reading the second buffer. Else false.
    */
    bool get_m_reading_buffer_two_is_finished()
    {
        return m_reading_buffer_two_is_finished;
    }

    /**
    * \fn  void set_timeout_ms(yat::uint32 timeout_ms)
    * \brief set the udp listener timeout. This time is used in the wait_input_data function.
    * \param timeout_ms : timeout in millisecond
	* \return void
    */
    void set_timeout_ms(yat::uint32 timeout_ms);

protected:

    /**
    * \fn virtual yat::Thread::IOArg run_undetached(yat::Thread::IOArg ioa)
    * \brief the thread entry point - called by yat::Thread::start_undetached. It is the udp Listener to read images from DAQ.
    * \param ioa
	* \return none
    */
    virtual yat::Thread::IOArg run_undetached(yat::Thread::IOArg ioa);

    /**
    * \fn virtual ~DataCaptureListener()
    * \brief destructor
    * \param none
	* \return none
    */
    virtual ~DataCaptureListener();

private:
    bool m_go_on;
    yat::uint32 m_udp_tmo_ms;
    yat::ClientSocket * m_sock;

    std::deque<yat::Socket::Data> m_buffer_frames_one;
    std::deque<yat::Socket::Data> m_buffer_frames_two;

    bool m_buffer_frames_one_full, m_buffer_frames_twoFull;

    std::size_t m_buffer_one_size_frames;

    std::mutex m_mutex_read_buffer_one, m_mutex_read_buffer_two;

    /// the  buffer is full
    bool m_buffer_one_is_full, m_buffer_two_is_full;

    /// the reading of the buffer is finished
    bool m_reading_buffer_one_is_finished;
    bool m_reading_buffer_two_is_finished;

    yat::Socket::Data m_buffer_config_frames[CONFIG_FRAMES_NUMBER];
};

} /// namespace ufxclib

#endif /// UFXCLIB_DATACAPTURELISTENER_H_
