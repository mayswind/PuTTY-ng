#include "encoding_util.h"

void copy_to_tchar_array(char* source_str, TCHAR* dest_str)
{
	USES_CONVERSION;
	_tcscpy(dest_str, A2T(source_str));
}

string wstring_to_string(wstring source_str)
{
	return wstring_to_string(source_str, _AtlGetConversionACP());
}

string wstring_to_string(wstring source_str, int codepage)
{
	int length = WideCharToMultiByte(codepage, 0, source_str.c_str(), -1, NULL, 0, NULL, NULL);
	LPSTR ret = new char[length + 1];
	ZeroMemory(ret, sizeof(char)*(length + 1));

	if (0 == ::WideCharToMultiByte(codepage, 0, source_str.c_str(), -1, ret, length, NULL, NULL))
	{
		assert(FALSE);
		delete[] ret;
		ret = NULL;
		return NULL;
	}

	CAutoPtr<char> c_auto_ptr;
	c_auto_ptr.Attach(ret);

	CAutoPtrArray<char> c_auto_ptr_array;
	c_auto_ptr_array.Add(c_auto_ptr);

	return ret;
}

wstring string_to_wstring(string source_str)
{
	return string_to_wstring(source_str, _AtlGetConversionACP());
}

wstring string_to_wstring(string source_str, int codepage)
{
	int length = MultiByteToWideChar(codepage, 0, source_str.c_str(), -1, NULL, 0);
	LPWSTR ret = new wchar_t[length + 1];
	ZeroMemory(ret, sizeof(wchar_t)*(length + 1));

	if (0 == MultiByteToWideChar(codepage, 0, source_str.c_str(), -1, ret, length))
	{
		assert(FALSE);
		delete[] ret;
		ret = NULL;
		return NULL;
	}

	CAutoPtr<wchar_t> c_auto_ptr;
	c_auto_ptr.Attach(ret);

	CAutoPtrArray<wchar_t> c_auto_ptr_array;
	c_auto_ptr_array.Add(c_auto_ptr);

	return ret;
}

string from_codepage(string source_str, int codepage)
{
	std::wstring temp_str = string_to_wstring(source_str.c_str(), codepage);
	return wstring_to_string(temp_str.c_str());
}

string to_codepage(string source_str, int codepage)
{
	std::wstring temp_str = string_to_wstring(source_str.c_str());
	return wstring_to_string(temp_str.c_str(), codepage);
}