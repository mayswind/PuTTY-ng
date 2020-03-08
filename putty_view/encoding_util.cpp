#include "encoding_util.h"

void copy_to_tchar_array(char* source_str, TCHAR* dest_str)
{
	USES_CONVERSION;
	_tcscpy(dest_str, A2T(source_str));
}

string wstring_to_string(wstring source_str)
{
	USES_CONVERSION;
	return W2A(source_str.c_str());
}

string wstring_to_string(wstring source_str, int codepage)
{
	USES_CONVERSION;
	if (codepage < 65536) {
		return W2A_CP(source_str.c_str(), codepage);
	}
	else {
		return W2A(source_str.c_str());
	}
}

wstring string_to_wstring(string source_str)
{
	USES_CONVERSION;
	return A2W(source_str.c_str());
}

wstring string_to_wstring(string source_str, int codepage)
{
	USES_CONVERSION;
	if (codepage < 65536) {
		return A2W_CP(source_str.c_str(), codepage);
	}
	else {
		return A2W(source_str.c_str());
	}
}

string from_codepage(string source_str, int codepage)
{
	if (codepage >= 65536)
	{
		return source_str;
	}

	USES_CONVERSION;
	std::wstring temp_str = A2W_CP(source_str.c_str(), codepage);
	return W2A(temp_str.c_str());
}

string to_codepage(string source_str, int codepage)
{
	if (codepage >= 65536)
	{
		return source_str;
	}

	USES_CONVERSION;
	std::wstring temp_str = A2W(source_str.c_str());
	return W2A_CP(temp_str.c_str(), codepage);
}