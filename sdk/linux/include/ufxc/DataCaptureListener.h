/*
 * DataCaptureListener.h
 *
 *  Created on: 7 sept. 2018
 *      Author: ghammouri
 */

#ifndef UFXCLIB_SRC_DATACAPTURELISTENER_H_
#define UFXCLIB_SRC_DATACAPTURELISTENER_H_

/********************************************//**
 *  DEPENDENCIES
 ***********************************************/
#include <yat/network/ClientSocket.h>
#include <yat/threading/Thread.h>
#include <deque>
#include "UFXC/UfxlibTypesAndConsts.h"
#include <mutex>

namespace ufxclib {

class DataCaptureListener : public yat::Thread
{
public:
	DataCaptureListener();

	//!< Function call the join
	void exit();

	//!< start receiving data from 3 SFPs
	void start();

	//!< stop receiving data from 3 SFPs
	void stop();

	//!< int config before receiving data from 3 SFPs
	void init(yat::uint32 buffSizeMax, yat::ClientSocket * sock)
		throw (ufxclib::Exception);

	//!< read the buffers if its not empty
	void readDataFromBuffers(unsigned char ImgBuffer[], unsigned int & cpt, unsigned int size) throw (ufxclib::Exception);

	//!< the reading of the buffer is finished
	bool m_oneReadinIsFinished;
	bool m_twoReadinIsFinished;

protected:
	//!< the thread entry point - called by yat::Thread::start_undetached
	virtual yat::Thread::IOArg run_undetached (yat::Thread::IOArg ioa) throw (ufxclib::Exception);

	// Destructor
	//- do not call directly! call 'exit' instead (the undelying impl. will clenup everything for you)
	virtual ~DataCaptureListener ();


private:
	bool m_go_on;
	yat::uint32 m_udp_tmo_ms;
	yat::ClientSocket * m_sock;
	std::deque<char> m_buffImgOne;
	std::deque<char> m_buffImgTwo;
	bool m_buffImgOneFull, m_buffImgTwoFull;
	yat::uint32 m_buffSizeMax;
	// mutex
	//yat::Mutex m_mutexReadBufferOne;
	//yat::Mutex m_mutexReadBufferTwo;
	std::mutex m_mutexReadBufferOne, m_mutexReadBufferTwo;
	//!< the  buffer is full
	bool m_OneIsFull, m_TwoIsFull;
};

} /* namespace ufxclib */

#endif /* UFXCLIB_SRC_DATACAPTURELISTENER_H_ */
