/*! 
@file epServerConf.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 19, 2012
@brief Server Configuration Defines
@version 1.0

@section LICENSE

The MIT License (MIT)

Copyright (c) 2012-2013 Woong Gyu La <juhgiyo@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

@section DESCRIPTION

An Interface for Server Configuration Defines.

*/
#ifndef __EP_SERVER_CONF_H__
#define __EP_SERVER_CONF_H__

#include "epServerEngine.h"
namespace epse{

	/*!
	@def DEFAULT_HOSTNAME
	@brief default hostname

	Macro for the default hostname.
	*/
	#define DEFAULT_HOSTNAME "localhost"

	/*!
	@def DEFAULT_PORT
	@brief default port

	Macro for the default port.
	*/
	#define DEFAULT_PORT "80808"


	/*!
	@def CONNECTION_LIMIT_INFINITE
	@brief No limit for the connection count

	Macro for No limit for the connection count.
	*/
	#define CONNECTION_LIMIT_INFINITE 0

	/*!
	@def PROCESSOR_LIMIT_INFINITE
	@brief No limit for the processor count

	Macro for No limit for the processor count.
	*/
	#define PROCESSOR_LIMIT_INFINITE 0

	/// Receive Status
	typedef enum _receiveStatus{
		/// Success
		RECEIVE_STATUS_SUCCESS=0,
		/// Time-out
		RECEIVE_STATUS_FAIL_TIME_OUT,
		/// Not connected
		RECEIVE_STATUS_FAIL_NOT_CONNECTED,
		/// Connection closing
		RECEIVE_STATUS_FAIL_CONNECTION_CLOSING,
		/// Socket error
		RECEIVE_STATUS_FAIL_SOCKET_ERROR,
		/// Receive failed
		RECEIVE_STATUS_FAIL_RECEIVE_FAILED,
		/// Not supported
		RECEIVE_STATUS_FAIL_NOT_SUPPORTED,
		
	}ReceiveStatus;

	/// Send Status
	typedef enum _sendStatus{
		/// Success
		SEND_STATUS_SUCCESS=0,
		/// Time-out
		SEND_STATUS_FAIL_TIME_OUT,
		/// Socket error
		SEND_STATUS_FAIL_SOCKET_ERROR,
		/// Send failed
		SEND_STATUS_FAIL_SEND_FAILED,
		/// Not connected
		SEND_STATUS_FAIL_NOT_CONNECTED,

	}SendStatus;
	
}
#endif //__EP_SERVER_CONF_H__