/*! 
@file epClientInterfaces.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Client Interfaces
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

An Interface for Client Interfaces.

*/
#ifndef __EP_CLIENT_INTERFACES_H__
#define __EP_CLIENT_INTERFACES_H__

#include "epServerEngine.h"
#include <winsock2.h>
#include "epPacket.h"
#include "epBaseServerObject.h"

namespace epse{
	class ClientCallbackInterface;

	
	/*! 
	@struct ClientOps epClientInterfaces.h
	@brief A class for Client Options.
	*/
	struct EP_SERVER_ENGINE ClientOps{
		/// Callback Object
		ClientCallbackInterface *callBackObj;
		/// Hostname
		const TCHAR *hostName;
		/// Port
		const TCHAR *port;
		/*!
		The flag for asynchronous receive.
		@remark For Asynchronous Client Use Only!
		*/
		bool isAsynchronousReceive;
		/// Wait time in millisecond for client threads
		unsigned int waitTimeMilliSec;
		/*!
		The maximum possible number of packet processor
		@remark If isAsynchronousReceive is false then this value is ignored!
		@remark For Asynchronous Client Use Only!
		*/
		unsigned int maximumProcessorCount;

		/*!
		The number of worker thread.
		@remark For IOCP Use Only!
		*/
		unsigned int workerThreadCount;

		/*!
		Default Constructor

		Initializes the Client Options
		*/
		ClientOps()
		{
			callBackObj=NULL;
			hostName=_T(DEFAULT_HOSTNAME);
			port=_T(DEFAULT_PORT);
			isAsynchronousReceive=true;
			waitTimeMilliSec=WAITTIME_INIFINITE;
			maximumProcessorCount=PROCESSOR_LIMIT_INFINITE;
			workerThreadCount=0;
		}

		static ClientOps defaultClientOps;
	};

	/*! 
	@class ClientInterface epClientInterfaces.h
	@brief A class for Client Interface.
	*/
	class EP_SERVER_ENGINE ClientInterface{
	public:

		/*!
		Set the hostname for the server.
		@remark Cannot be changed while connected to server
		@param[in] hostName The hostname to set.
		*/
		void SetHostName(const TCHAR * hostName);

		/*!
		Set the port for the server.
		@remark Cannot be changed while connected to server
		@param[in] port The port to set.
		*/
		void SetPort(const TCHAR * port);

		/*!
		Get the hostname of server
		@return the hostname in string
		*/
		epl::EpTString GetHostName() const;

		/*!
		Get the port number of server
		@return the port number in string
		*/
		epl::EpTString GetPort() const;

		/*!
		Set the Maximum Processor Count for the Socket.
		@param[in] maxProcessorCount The Maximum Processor Count to set.
		@remark 0 means there is no limit
		@remark For Asynchronous Client Use Only!
		@remark If IsAsynchronousReceive is false then this value is ignored!
		*/
		virtual void SetMaximumProcessorCount(unsigned int maxProcessorCount){}

		/*!
		Get the Maximum Processor Count of the Socket
		@return The Maximum Processor Count
		@remark 0 means there is no limit
		@remark For Asynchronous Client Use Only!
		@remark If IsAsynchronousReceive is false then this value is ignored!
		*/
		virtual unsigned int GetMaximumProcessorCount() const{return 0;}

		/*!
		Set the wait time for the thread termination
		@param[in] milliSec the time for waiting in millisecond
		*/
		virtual void SetWaitTime(unsigned int milliSec)=0;
		
		/*!
		Get the wait time for the parser thread termination
		@return the current time for waiting in millisecond
		*/
		virtual unsigned int GetWaitTime() const=0;

		/*!
		Connect to the server
		@param[in] ops the client options
		@return true if successfully connected otherwise false
		@remark if argument is NULL then previously setting value is used
		*/
		virtual bool Connect(const ClientOps &ops=ClientOps::defaultClientOps)=0;

		/*!
		Disconnect from the server
		*/
		virtual void Disconnect()=0;

		/*!
		Check if the connection is alive
		@return true if the connection is alive otherwise false
		*/
		virtual bool IsConnectionAlive() const=0;

		/*!
		Send the packet to the client
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@param[in] sendStatus the status of send.
		@return sent byte size
		@remark return -1 if error occurred
		*/
		virtual int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE,SendStatus *sendStatus=NULL)=0;

		/*!
		Receive the packet from the client
		@param[in] waitTimeInMilliSec wait time for receiving the packet in millisecond
		@param[out] retStatus the pointer to ReceiveStatus enumerator to get receive status.
		@return received packet
		@remark the caller must call ReleaseObj() for Packet to avoid the memory leak.
		@remark For Synchronous Client Use Only!
		*/
		virtual Packet *Receive(unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE,ReceiveStatus *retStatus=NULL)
		{
			if(retStatus)
				*retStatus=RECEIVE_STATUS_FAIL_NOT_SUPPORTED;
			return NULL;
		}
	
		/*!
		Set the Callback Object for the server.
		@param[in] callBackObj The Callback Object to set.
		*/
		virtual void SetCallbackObject(ClientCallbackInterface *callBackObj)=0;

		/*!
		Get the Callback Object of server
		@return the current Callback Object
		*/
		virtual ClientCallbackInterface *GetCallbackObject()=0;

	
		/*!
		Get the asynchronous receive flag for the Socket.
		@return The flag whether to receive asynchronously.
		@remark for Asynchronous Client Use Only!
		*/
		virtual bool GetIsAsynchronousReceive() const
		{
			return false;
		}

		/*!
		Set the asynchronous receive flag for the Socket.
		@param[in] isASynchronousReceive The flag whether to receive asynchronously.
		@remark for Asynchronous Client Use Only!
		*/
		virtual void SetIsAsynchronousReceive(bool isASynchronousReceive)
		{
			return;
		}

		/*!
		Get the maximum packet byte size
		@return the maximum packet byte size
		@remark for UDP Client Use Only!
		*/
		virtual unsigned int GetMaxPacketByteSize() const{return 0;}

		

	};

	/*! 
	@class ClientCallbackInterface epClientInterfaces.h
	@brief A class for Client Callback Interface.
	*/
	class EP_SERVER_ENGINE ClientCallbackInterface{
	public:
		/*!
		Received the packet from the server.
		@param[in] client the client which received the packet
		@param[in] receivedPacket the received packet
		@param[in] status the status of receive
		@remark for Asynchronous Client Use Only!
		*/
		virtual void OnReceived(ClientInterface *client,const Packet*receivedPacket,ReceiveStatus status)=0;

		
		/*!
		Sent the packet from the client.
		@param[in] client the client which sent the packet
		@param[in] status the status of Send
		@remark for IOCP Server Use Only!
		*/
		virtual void OnSent(ClientInterface *client,SendStatus status){}

		/*!
		The client is disconnected.
		@param[in] client the client, disconnected.
		*/
		virtual void OnDisconnect(ClientInterface *client){}
	};
}

#endif //__EP_CLIENT_INTERFACES_H__