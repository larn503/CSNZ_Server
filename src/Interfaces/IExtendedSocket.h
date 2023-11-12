#pragma once

#ifdef WIN32
#include "Windows.h"
#endif

#include <string>
#include <vector>

struct GuestData_s;
class CSendPacket;
class CReceivePacket;

class IExtendedSocket
{
public:
	virtual void SetIP(const std::string& addr) = 0;
	virtual void SetHWID(const std::vector<unsigned char>& hwid) = 0;
	virtual std::string GetIP() = 0;
	virtual std::vector<unsigned char> GetHWID() = 0;
	virtual int GetSeq() = 0;
	virtual int LoggerGetSeq() = 0;
	virtual void ResetSeq() = 0;
	virtual CReceivePacket* Read() = 0;
	virtual int Send(const std::vector<unsigned char>& buffer) = 0;
	virtual int Send(CSendPacket* msg, bool forceSend = false) = 0;

	virtual int GetID() = 0;
	virtual void SetSocket(SOCKET socket) = 0;
	virtual SOCKET GetSocket() = 0;
	virtual void SetMsg(CReceivePacket* msg) = 0;
	virtual CReceivePacket* GetMsg() = 0;
	virtual int GetReadResult() = 0;
	virtual int GetBytesReceived() = 0;
	virtual int GetBytesSent() = 0;
	virtual std::vector<CSendPacket*> GetPacketsToSend() = 0;
	virtual GuestData_s& GetGuestData() = 0;
};
