#pragma once

#include "rapidcsv.h"

class CCSVTable : public rapidcsv::Document
{
public:
	CCSVTable(std::istream& pStream,
		const rapidcsv::LabelParams& pLabelParams = rapidcsv::LabelParams(),
		const rapidcsv::SeparatorParams& pSeparatorParams = rapidcsv::SeparatorParams(),
		const rapidcsv::ConverterParams& pConverterParams = rapidcsv::ConverterParams());

	CCSVTable(const std::string& pPath,
		const rapidcsv::LabelParams& pLabelParams = rapidcsv::LabelParams(),
		const rapidcsv::SeparatorParams& pSeparatorParams = rapidcsv::SeparatorParams(),
		const rapidcsv::ConverterParams& pConverterParams = rapidcsv::ConverterParams()) : rapidcsv::Document(pPath, pLabelParams, pSeparatorParams, pConverterParams)
	{ }

	inline CCSVTable(const std::string& pPath) : rapidcsv::Document(pPath)
	{ }

	template<class T> T GetRowValueByItemID(std::string columnName, std::string itemId)
	{
		return GetCell<T>(columnName, itemId);
	}

	bool IsRowValueExists(std::string columnName, std::string rowValue);
};
