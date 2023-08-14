#include "SendPacket.h"

using namespace std;

CSendPacket::CSendPacket(CExtendedSocket* socket, int packetID)
{
	m_pSocket = socket;
	m_nPacketID = packetID;
	m_nSequence = socket->GetSeq();
}

CSendPacket::~CSendPacket()
{
}

vector<unsigned char> CSendPacket::SetPacketLength()
{
	// TODO: rewrite
	vector<unsigned char> v = m_OutStream.getBuffer();

	uint16_t size = (uint16_t)v.size() - 4;
	if (size < 255)
	{
		v[2] = (uint8_t)size;
	}
	else
	{
		v[2] = (uint8_t)(size & 0xff);
		v[3] = (uint8_t)((size >> 8));
	}

	return v;
}

Buffer CSendPacket::GetData()
{
	return m_OutStream;
}

void CSendPacket::WriteInt8(int number)
{
	m_OutStream.writeInt8(number);
}

void CSendPacket::WriteInt16(int number, bool littleEndian)
{
	littleEndian ? m_OutStream.writeInt16_LE(number) : m_OutStream.writeInt16_BE(number);
}

void CSendPacket::WriteInt32(int number, bool littleEndian)
{
	littleEndian ? m_OutStream.writeInt32_LE(number) : m_OutStream.writeInt32_BE(number);
}

void CSendPacket::WriteInt64(long long number, bool littleEndian)
{
	littleEndian ? m_OutStream.writeInt64_LE(number) : m_OutStream.writeInt64_BE(number);
}

void CSendPacket::WriteUInt8(unsigned int number)
{
	m_OutStream.writeUInt8(number);
}

void CSendPacket::WriteUInt16(unsigned int number, bool littleEndian)
{
	littleEndian ? m_OutStream.writeUInt16_LE(number) : m_OutStream.writeUInt16_BE(number);
}

void CSendPacket::WriteUInt32(unsigned int number, bool littleEndian)
{
	littleEndian ? m_OutStream.writeUInt32_LE(number) : m_OutStream.writeUInt32_BE(number);
}

void CSendPacket::WriteUInt64(unsigned long long number, bool littleEndian)
{
	littleEndian ? m_OutStream.writeUInt64_LE(number) : m_OutStream.writeUInt64_BE(number);
}

void CSendPacket::WriteString(const std::string& message, bool writesize)
{
	m_OutStream.writeStr(message);
}

void CSendPacket::WriteWString(const std::wstring& message)
{
	m_OutStream.writeWStr(message);
}

void CSendPacket::WriteData(void* data, int len)
{
	m_OutStream.writeData(data, len);
}

void CSendPacket::BuildHeader()
{
	WriteUInt8('U');
	WriteUInt8(m_nSequence);
	WriteUInt16(0); // packet size
	WriteUInt8(m_nPacketID);
}

void CSendPacket::Send(const std::vector<unsigned char>& data)
{
	m_pSocket->Send(data);
}