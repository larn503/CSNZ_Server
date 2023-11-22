#pragma once

#include "IExtendedSocket.h"
#include "main.h"
#include "Buffer.h"

#define MAX_SEQUENCE 255
#define MAX_RECEIVE_LEN 15000

#define PACKET_MAX_SIZE 0x10000
#define PACKET_HEADER_SIZE 4

struct GuestData_s
{
	bool isGuest;

	class CChannelServer* currentServer;
	class CChannel* currentChannel;

	int launcherVersion;
};

class CSendPacket;
class CReceivePacket;
struct WOLFSSL_EVP_CIPHER_CTX;

class CExtendedSocket : public IExtendedSocket
{
public:
	CExtendedSocket(unsigned int id);
	~CExtendedSocket();

	bool SetupCrypt();
	void SetIP(const std::string& addr) { m_IP = addr; }
	void SetHWID(const std::vector<unsigned char>& hwid) { m_HWID = hwid; }
	std::string GetIP() { return m_IP; }
	std::vector<unsigned char> GetHWID() { return m_HWID; }
	void SetCryptInput(bool val) { m_bCryptInput = val; }
	void SetCryptOutput(bool val) { m_bCryptOutput = val; }
	unsigned char* GetCryptKey() { return m_pCryptKey; }
	unsigned char* GetCryptIV() { return m_pCryptIV; }
	int GetSeq();
	int LoggerGetSeq();
	void ResetSeq();
	CReceivePacket* Read();
	int Send(std::vector<unsigned char>& buffer);
	int Send(CSendPacket* msg, bool forceSend = false);

	int GetID();
	void SetSocket(SOCKET socket);
	SOCKET GetSocket();
	void SetMsg(CReceivePacket* msg);
	CReceivePacket* GetMsg();
	int GetReadResult();
	int GetBytesReceived();
	int GetBytesSent();
	std::vector<CSendPacket*> GetPacketsToSend();
	GuestData_s& GetGuestData();

private:
	int m_nID;
	SOCKET m_Socket;
	int m_nSequence;
	int m_nBytesReceived;
	int m_nBytesSent;

	GuestData_s m_GuestData;

	CReceivePacket* m_pMsg;
	int m_nPacketToReceiveFullSize;
	int m_nPacketReceivedSize;
	int m_nReadResult;
	int m_nNextExpectedSeq; // TODO: we need it?

	std::string m_IP;
	std::vector<unsigned char> m_HWID;
	std::vector<CSendPacket*> m_SendPackets;

	// crypt things
	WOLFSSL_EVP_CIPHER_CTX* m_pDecEVPCTX;
	WOLFSSL_EVP_CIPHER_CTX* m_pEncEVPCTX;
	bool m_bCryptInput;
	bool m_bCryptOutput;
	unsigned char m_pCryptKey[64];
	unsigned char m_pCryptIV[64];
};
