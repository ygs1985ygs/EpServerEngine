/*! 
BaseUdpServer for the EpServerEngine

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
*/
#include "epBaseUdpServer.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

BaseUdpServer::BaseUdpServer(epl::LockPolicy lockPolicyType): BaseServer(lockPolicyType)
{
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_sendLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_sendLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_sendLock=EP_NEW epl::NoLock();
		break;
	default:
		m_sendLock=NULL;
		break;
	}
	m_maxPacketSize=0;
}

BaseUdpServer::BaseUdpServer(const BaseUdpServer& b):BaseServer(b)
{
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_sendLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_sendLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_sendLock=EP_NEW epl::NoLock();
		break;
	default:
		m_sendLock=NULL;
		break;
	}

	LockObj lock(b.m_baseServerLock);
	m_maxPacketSize=b.m_maxPacketSize;
}
BaseUdpServer::~BaseUdpServer()
{
}
BaseUdpServer & BaseUdpServer::operator=(const BaseUdpServer&b)
{
	if(this!=&b)
	{

		BaseServer::operator =(b);

		switch(m_lockPolicy)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_sendLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_sendLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_sendLock=EP_NEW epl::NoLock();
			break;
		default:
			m_sendLock=NULL;
			break;
		}

		LockObj lock(b.m_baseServerLock);
		m_maxPacketSize=b.m_maxPacketSize;
	}
	return *this;
}

void BaseUdpServer::resetServer()
{
	BaseServer::resetServer();

	if(m_sendLock)
		EP_DELETE m_sendLock;
	m_sendLock=NULL;
}


unsigned int BaseUdpServer::GetMaxPacketByteSize() const
{
	return m_maxPacketSize;
}

int BaseUdpServer::send(const Packet &packet,const sockaddr &clientSockAddr, unsigned int waitTimeInMilliSec,SendStatus *sendStatus)
{

	epl::LockObj lock(m_sendLock);

	// select routine
	TIMEVAL	timeOutVal;
	fd_set	fdSet;
	int		retfdNum = 0;

	FD_ZERO(&fdSet);
	FD_SET(m_listenSocket, &fdSet);
	if(waitTimeInMilliSec!=WAITTIME_INIFINITE)
	{
		// socket select time out setting
		timeOutVal.tv_sec = (long)(waitTimeInMilliSec/1000); // Convert to seconds
		timeOutVal.tv_usec = (long)(waitTimeInMilliSec%1000)*1000; // Convert remainders to micro-seconds
		// socket select
		// socket read select
		retfdNum = select(0, NULL, &fdSet, NULL, &timeOutVal);
	}
	else
	{
		retfdNum = select(0, NULL, &fdSet, NULL, NULL);
	}
	if (retfdNum == SOCKET_ERROR)	// select failed
	{
		if(sendStatus)
			*sendStatus=SEND_STATUS_FAIL_SOCKET_ERROR;
		return retfdNum;
	}
	else if (retfdNum == 0)		    // select time-out
	{
		if(sendStatus)
			*sendStatus=SEND_STATUS_FAIL_TIME_OUT;
		return retfdNum;
	}

	// send routine

	int sentLength=0;
	int writeLength=0;
	const char *packetData=packet.GetPacket();
	int length=packet.GetPacketByteSize();
	EP_ASSERT(length<=m_maxPacketSize);

	int sockAddrSize=sizeof(sockaddr);
	while(length>0)
	{
		sentLength=sendto(m_listenSocket,packetData,length,0,&clientSockAddr,sizeof(sockaddr));
		writeLength+=sentLength;
		if(sentLength<=0)
		{
			if(sendStatus)
				*sendStatus=SEND_STATUS_FAIL_SEND_FAILED;
			return sentLength;
		}
		length-=sentLength;
		packetData+=sentLength;
	}
	if(sendStatus)
		*sendStatus=SEND_STATUS_SUCCESS;
	return sentLength;
}

bool BaseUdpServer::socketCompare(sockaddr const & clientSocket, const BaseServerObject*obj )
{
	SocketInterface *workerObj=(SocketInterface*)const_cast<BaseServerObject*>(obj);
	if(clientSocket.sa_family==workerObj->GetSockAddr().sa_family)
	{
		if(System::Memcmp((void*)clientSocket.sa_data,(void*)workerObj->GetSockAddr().sa_data,sizeof(clientSocket.sa_data))==0)
			return true;
	}
	return false;
}


bool BaseUdpServer::StartServer(const ServerOps &ops)
{
	epl::LockObj lock(m_baseServerLock);
	if(IsServerStarted())
		return true;


	if(ops.callBackObj)
		m_callBackObj=ops.callBackObj;
	EP_ASSERT(m_callBackObj);

	if(ops.port)
	{
		setPort(ops.port);
	}

	if(!m_port.length())
	{
		m_port=DEFAULT_PORT;
	}

	SetWaitTime(ops.waitTimeMilliSec);
	m_maxConnectionCount=ops.maximumConnectionCount;

	WSADATA wsaData;
	int iResult;

	m_listenSocket= INVALID_SOCKET;
	m_maxPacketSize=0;

	m_result = NULL;


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {

		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) WSAStartup failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		return false;
	}

	/// internal use variable2
	struct addrinfo iHints;
	ZeroMemory(&iHints, sizeof(iHints));
	iHints.ai_family = AF_INET;
	iHints.ai_socktype = SOCK_DGRAM;
	iHints.ai_protocol = IPPROTO_UDP;
	iHints.ai_flags = AI_PASSIVE;


	// Resolve the server address and port
	iResult = getaddrinfo(NULL, m_port.c_str(), &iHints, &m_result);
	if ( iResult != 0 ) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) getaddrinfo failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		WSACleanup();
		return false;
	}

	// Create a SOCKET for connecting to server
	m_listenSocket = socket(m_result->ai_family, m_result->ai_socktype, m_result->ai_protocol);
	if (m_listenSocket == INVALID_SOCKET) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) socket failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		cleanUpServer();
		return false;
	}

	// set SO_REUSEADDR for setsockopt function to reuse the port immediately as soon as the service exits.
	int sockoptval = 1;
	setsockopt(m_listenSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&sockoptval), sizeof(int));

	// Setup the TCP listening socket
	iResult = bind( m_listenSocket, m_result->ai_addr, static_cast<int>(m_result->ai_addrlen));
	if (iResult == SOCKET_ERROR) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) bind failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		cleanUpServer();
		return false;
	}

	int nTmp = sizeof(int);
	getsockopt(m_listenSocket, SOL_SOCKET,SO_MAX_MSG_SIZE, (char *)&m_maxPacketSize,&nTmp);

	// Create thread 1.
	if(Start())
	{
		return true;
	}
	cleanUpServer();
	return false;


}

void BaseUdpServer::StopServer()
{
	epl::LockObj lock(m_baseServerLock);
	if(!IsServerStarted())
	{
		return;
	}
	if(m_listenSocket!=INVALID_SOCKET)
	{
		int iResult;
		iResult = shutdown(m_listenSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) shutdown failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		}
		closesocket(m_listenSocket);
		m_listenSocket=INVALID_SOCKET;
	}
	TerminateAfter(m_waitTime);
	shutdownAllClient();
	cleanUpServer();
}


void BaseUdpServer::cleanUpServer()
{
	BaseServer::cleanUpServer();
	m_maxPacketSize=0;
}

