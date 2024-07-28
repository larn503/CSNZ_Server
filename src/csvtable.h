#pragma once

#include "rapidcsv.h"

class CCSVTable : public rapidcsv::Document
{
public:
	CCSVTable(const std::string& pPath,
		const rapidcsv::LabelParams& pLabelParams = rapidcsv::LabelParams(),
		const rapidcsv::SeparatorParams& pSeparatorParams = rapidcsv::SeparatorParams(),
		const rapidcsv::ConverterParams& pConverterParams = rapidcsv::ConverterParams(),
		const rapidcsv::LineReaderParams& pLineReaderParams = rapidcsv::LineReaderParams(), bool ignoreFirstLine = false)
	{
		m_bLoadFailed = false;

		std::ifstream stream;
		stream.open(pPath, std::ios::binary);
		if (!stream.is_open())
		{
			m_bLoadFailed = true;
		}

		// read "commentary" header divided with semicolon (aka. the first line on Item.csv)
		if (ignoreFirstLine)
		{
			std::stringstream sstream;
			if (stream.is_open())
			{
				std::vector<unsigned char> vec(std::istreambuf_iterator<char>{stream}, {});
				std::vector<unsigned char>::iterator it;
				if ((it = std::find(vec.begin(), vec.end(), '\n')) != vec.end())
				{
					vec.erase(vec.begin(), vec.begin() + (it - vec.begin()) + 1);
				}

				std::copy(vec.begin(), vec.end(), std::ostream_iterator<unsigned char>(sstream));
			}

			Load(sstream, pLabelParams, pSeparatorParams, pConverterParams, pLineReaderParams);
		}
		else
		{
			Load(pPath, pLabelParams, pSeparatorParams, pConverterParams, pLineReaderParams);
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
