#include "Inc.h"

#include "sup.h"

#ifndef TO_LOWERCASE
#define TO_LOWERCASE(out, c1) (out = (c1 <= 'Z' && c1 >= 'A') ? c1 = (c1 - 'A') + 'a': c1)
#endif

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _XEB_LDR_DATA
{
	ULONG Length;
	BOOLEAN Initialized;
	HANDLE SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID EntryInProgress;
} XEB_LDR_DATA, * PXEB_LDR_DATA;

typedef struct _XDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	void* BaseAddress;
	void* EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	SHORT LoadCount;
	SHORT TlsIndex;
	HANDLE SectionHandle;
	ULONG CheckSum;
	ULONG TimeDateStamp;
} XDR_DATA_TABLE_ENTRY, * PXDR_DATA_TABLE_ENTRY;
typedef struct _XEB
{
	BOOLEAN InheritedAddressSpace;
	BOOLEAN ReadImageFileExecOptions;
	BOOLEAN BeingDebugged;
	BOOLEAN SpareBool;
	HANDLE Mutant;
	PVOID ImageBaseAddress;
	PXEB_LDR_DATA Ldr;

} XEB, * PXEB;

wchar_t* _strcpy_w(wchar_t* dest, const wchar_t* src)
{
	wchar_t* p;

	if ((dest == 0) || (src == 0))
		return dest;

	if (dest == src)
		return dest;

	p = dest;
	while (*src != 0) {
		*p = *src;
		p++;
		src++;
	}

	*p = 0;
	return dest;
}

wchar_t* _strcat_w(wchar_t* dest, const wchar_t* src)
{
	if ((dest == 0) || (src == 0))
		return dest;

	while (*dest != 0)
		dest++;

	while (*src != 0) {
		*dest = *src;
		dest++;
		src++;
	}

	*dest = 0;
	return dest;
}

LPVOID get_module_by_name(WCHAR* module_name)
{
	PXEB peb = NULL;
#if defined(_WIN64)
	peb = (PXEB)__readgsqword(0x60);
#else
	peb = (PXEB)__readfsdword(0x30);
#endif
	PXEB_LDR_DATA ldr = peb->Ldr;
	LIST_ENTRY list = ldr->InLoadOrderModuleList;
	PXDR_DATA_TABLE_ENTRY Flink = *((PXDR_DATA_TABLE_ENTRY*)(&list));
	PXDR_DATA_TABLE_ENTRY curr_module = Flink;
	while (curr_module != NULL && curr_module->BaseAddress != NULL) {
		if (curr_module->BaseDllName.Buffer == NULL) continue;
		WCHAR* curr_name = curr_module->BaseDllName.Buffer;
		size_t i = 0;
		for (i = 0; module_name[i] != 0 && curr_name[i] != 0; i++) {
			WCHAR c1, c2;
			TO_LOWERCASE(c1, module_name[i]);
			TO_LOWERCASE(c2, curr_name[i]);
			if (c1 != c2) break;
		}
		if (module_name[i] == 0 && curr_name[i] == 0) {
			//found
			return curr_module->BaseAddress;
		}
		// not found, try next:
		curr_module = (PXDR_DATA_TABLE_ENTRY)curr_module->InLoadOrderModuleList.Flink;
	}
	return NULL;
}


LPVOID get_func_by_name(LPVOID module, char* func_name)
{
	IMAGE_DOS_HEADER* idh = (IMAGE_DOS_HEADER*)module;
	if (idh->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return nullptr;
	}

	IMAGE_NT_HEADERS* nt_headers = (IMAGE_NT_HEADERS*)((BYTE*)module + idh->e_lfanew);
	IMAGE_DATA_DIRECTORY* exportsDir = &(nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
	if (!exportsDir->VirtualAddress)
	{
		return nullptr;
	}

	DWORD expAddr = exportsDir->VirtualAddress;
	IMAGE_EXPORT_DIRECTORY* exp = (IMAGE_EXPORT_DIRECTORY*)(expAddr + (ULONG_PTR)module);
	SIZE_T namesCount = exp->NumberOfNames;

	DWORD funcsListRVA = exp->AddressOfFunctions;
	DWORD funcNamesListRVA = exp->AddressOfNames;
	DWORD namesOrdsListRVA = exp->AddressOfNameOrdinals;

	//go through names:
	for (SIZE_T i = 0; i < namesCount; i++)
	{
		DWORD* nameRVA = (DWORD*)(funcNamesListRVA + (BYTE*)module + i * sizeof(DWORD));
		WORD* nameIndex = (WORD*)(namesOrdsListRVA + (BYTE*)module + i * sizeof(WORD));
		DWORD* funcRVA = (DWORD*)(funcsListRVA + (BYTE*)module + (*nameIndex) * sizeof(DWORD));

		LPSTR curr_name = (LPSTR)(*nameRVA + (BYTE*)module);
		size_t k;
		for (k = 0; func_name[k] != 0 && curr_name[k] != 0; k++)
		{
			if (func_name[k] != curr_name[k])
				break;
		}
		if (func_name[k] == 0 && curr_name[k] == 0)
		{
			//found
			return (BYTE*)module + (*funcRVA);
		}
	}
	return nullptr;
}




