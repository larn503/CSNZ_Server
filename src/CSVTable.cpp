#include "CSVTable.h"

using namespace std;

CCSVTable::CCSVTable(istream& pStream,
	const rapidcsv::LabelParams& pLabelParams,
	const rapidcsv::SeparatorParams& pSeparatorParams,
	const rapidcsv::ConverterParams& pConverterParams) : rapidcsv::Document(pStream, pLabelParams, pSeparatorParams, pConverterParams)
{
}

bool CCSVTable::IsRowValueExists(string columnName, string rowValue)
{
	vector<string> column = GetColumn<string>(columnName);

	return (find(column.begin(), column.end(), rowValue) != column.end());
}