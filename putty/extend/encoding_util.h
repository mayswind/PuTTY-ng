#include <string>
#include <atlcoll.h>
#include <atlconv.h>
#include <atlstr.h>
#include <putty.h>

using namespace std;

string wstring_to_string(wstring source_str);
string wstring_to_string(wstring source_str, int codepage);
wstring string_to_wstring(string source_str);
wstring string_to_wstring(string source_str, int codepage);
string from_codepage(string source_str, int codepage);
string to_codepage(string source_str, int codepage);