#pragma once

#include "receivepacket.h"

class CPacketIn_UDP
{
public:
	CPacketIn_UDP(Buffer& buf);
	void Parse();

	int m_nSignature;
	int m_nUserID;
	int m_nPortID;
	string m_IpAddress;
	int m_nPort;

private:
	Buffer buffer;
};
