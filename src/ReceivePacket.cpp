#include "ReceivePacket.h"

using namespace std;

CReceivePacket::CReceivePacket(Buffer& buf)
{
	SetBufferAndParse(buf);
}

bool CReceivePacket::IsValid()
{
	return m_nSignature == 'U';
}

Buffer& CReceivePacket::GetData()
{
	return m_Buffer;
}

int CReceivePacket::GetID()
{
	return m_nPacketID;
}

int CReceivePacket::GetLength()
{
	return m_nLength;
}

int CReceivePacket::GetSequence()
{
	return m_nSequence;
}

int8_t CReceivePacket::ReadInt8()
{
	if (!CanReadBytes(1))
	{
		g_pConsole->Error("ReadInt8: out of buffer, packet id: %d, len: %d, offset: %d\n", m_nPacketID, m_nLength, m_Buffer.getReadOffset());
		return 0;
	}

	return m_Buffer.readInt8();
}

int16_t CReceivePacket::ReadInt16(bool bigEndian)
{
	if (!CanReadBytes(2))
	{
		g_pConsole->Error("ReadInt16: out of buffer, packet id: %d, len: %d, offset: %d\n", m_nPacketID, m_nLength, m_Buffer.getReadOffset());
		return 0;
	}

	return bigEndian ? m_Buffer.readInt16_BE() : m_Buffer.readInt16_LE();
}

int32_t CReceivePacket::ReadInt32(bool bigEndian)
{
	if (!CanReadBytes(4))
	{
		g_pConsole->Error("ReadInt32: out of buffer, packet id: %d, len: %d, offset: %d\n", m_nPacketID, m_nLength, m_Buffer.getReadOffset());
		return 0;
	}

	return bigEndian ? m_Buffer.readInt32_BE() : m_Buffer.readInt32_LE();
}

int64_t CReceivePacket::ReadInt64(bool bigEndian)
{
	if (!CanReadBytes(8))
	{
		g_pConsole->Error("ReadInt64: out of buffer, packet id: %d, len: %d, offset: %d\n", m_nPacketID, m_nLength, m_Buffer.getReadOffset());
		return 0;
	}

	return bigEndian ? m_Buffer.readInt64_BE() : m_Buffer.readInt64_LE();
}

uint8_t CReceivePacket::ReadUInt8()
{
	if (!CanReadBytes(1))
	{
		g_pConsole->Error("ReadUInt8: out of buffer, packet id: %d, len: %d, offset: %d\n", m_nPacketID, m_nLength, m_Buffer.getReadOffset());
		return 0;
	}

	return m_Buffer.readUInt8();
}

uint16_t CReceivePacket::ReadUInt16(bool bigEndian)
{
	if (!CanReadBytes(2))
	{
		g_pConsole->Error("ReadUInt16: out of buffer, packet id: %d, len: %d, offset: %d\n", m_nPacketID, m_nLength, m_Buffer.getReadOffset());
		return 0;
	}

	return bigEndian ? m_Buffer.readUInt16_BE() : m_Buffer.readUInt16_LE();
}

uint32_t CReceivePacket::ReadUInt32(bool bigEndian)
{
	if (!CanReadBytes(4))
	{
		g_pConsole->Error("ReadUInt32: out of buffer, packet id: %d, len: %d, offset: %d\n", m_nPacketID, m_nLength, m_Buffer.getReadOffset());
		return 0;
	}

	return bigEndian ? m_Buffer.readUInt32_BE() : m_Buffer.readUInt32_LE();
}

uint64_t CReceivePacket::ReadUInt64(bool bigEndian)
{
	if (!CanReadBytes(8))
	{
		g_pConsole->Error("ReadUInt64: out of buffer, packet id: %d, len: %d, offset: %d\n", m_nPacketID, m_nLength, m_Buffer.getReadOffset());
		return 0;
	}

	return bigEndian ? m_Buffer.readUInt64_BE() : m_Buffer.readUInt64_LE();
}

float CReceivePacket::ReadFloat(bool bigEndian)
{
	if (!CanReadBytes(4))
	{
		g_pConsole->Error("ReadFloat: out of buffer, packet id: %d, len: %d, offset: %d\n", m_nPacketID, m_nLength, m_Buffer.getReadOffset());
		return 0;
	}

	return bigEndian ? m_Buffer.readFloat_BE() : m_Buffer.readFloat_LE();
}

string CReceivePacket::ReadString()
{
	if (!CanReadBytes(1))
	{
		g_pConsole->Error("ReadString: out of buffer, packet id: %d, len: %d, offset: %d\n", m_nPacketID, m_nLength, m_Buffer.getReadOffset());
		return "";
	}

	string str = m_Buffer.readStr();
	return str;
}

vector<unsigned char> CReceivePacket::ReadArray(int length)
{
	if (!CanReadBytes(length))
	{
		g_pConsole->Error("ReadArray: out of buffer, packet id: %d, len: %d, offset: %d\n", m_nPacketID, m_nLength, m_Buffer.getReadOffset());
		return vector<unsigned char>();
	}

	return m_Buffer.readArr(length);
}

void CReceivePacket::ParseHeader()
{
	m_nSignature = m_Buffer.readUInt8();

	if (!IsValid())
	{
		return;
	}

	m_nSequence = m_Buffer.readUInt8();
	m_nLength = m_Buffer.readUInt16_LE();

	// kostyl'
	if (m_Buffer.getBuffer().size() > 4)
	{
		m_nPacketID = m_Buffer.readUInt8();

		g_pConsole->Debug("ReceivePacket::Parse() sequence: %d, length: %d, id: %d\n", m_nSequence, m_nLength, m_nPacketID);
	}
}

void CReceivePacket::SetBufferAndParse(Buffer& buf)
{
	m_Buffer = buf;
	ParseHeader();
}

bool CReceivePacket::CanReadBytes(int len)
{
	return (m_nLength + 4) >= m_Buffer.getReadOffset() + len;
}