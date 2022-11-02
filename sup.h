#pragma once

wchar_t* _strcpy_w(wchar_t* dest, const wchar_t* src);

wchar_t* _strcat_w(wchar_t* dest, const wchar_t* src);

#define _strcpy _strcpy_w
#define _strcat _strcat_w

LPVOID get_module_by_name(WCHAR* module_name);

LPVOID get_func_by_name(LPVOID module, char* func_name);

