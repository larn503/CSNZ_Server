#pragma once

#include "rapidcsv.h"

class CCSVTable : public rapidcsv::Document
{
public:
	CCSVTable(const std::string& pPath,
		const rapidcsv::LabelParams& pLabelParams = rapidcsv::LabelParams(),
		const rapidcsv::SeparatorParams& pSeparatorParams = rapidcsv::SeparatorParams(),
		const rapidcsv::ConverterParams& pConverterParams = rapidcsv::ConverterParams(),
		const rapidcsv::LineReaderParams& pLineReaderParams = rapidcsv::LineReaderParams(), bool ignoreSemicolon = false)
	{
		m_bLoadFailed = false;

		std::ifstream stream;
		stream.open(pPath, std::ios::binary);
		if (!stream.is_open())
		{
			m_bLoadFailed = true;
		}

		// read "commentary" header divided with semicolon
		if (ignoreSemicolon)
		{
			std::stringstream sstream;
			if (stream.is_open())
			{
				std::vector<unsigned char> vec(std::istreambuf_iterator<char>{stream}, {});
				int semicolon = 0;
				size_t lastSemicolonPos = -1;
				for (size_t i = 0; i < vec.size(); i++)
				{
					if (vec[i] == ';')
					{
						semicolon++;
						lastSemicolonPos = i;
					}
				}

				if (semicolon)
				{
					vec.erase(vec.begin(), vec.begin() + lastSemicolonPos + 1);
				}

				std::copy(vec.begin(), vec.end(), std::ostream_iterator<unsigned char>(sstream));
			}

			Load(sstream, pLabelParams, pSeparatorParams, pConverterParams, pLineReaderParams);
		}
		else
		{
			Load(pPath);
		}
	}

	bool IsRowValueExists(const std::string& columnName, const std::string& rowValue)
	{
		std::vector<std::string> column = GetColumn<std::string>(columnName);

		return (std::find(column.begin(), column.end(), rowValue) != column.end());
	}

	bool IsLoadFailed()
	{
		return m_bLoadFailed;
	}

private:
	bool m_bLoadFailed;
};
