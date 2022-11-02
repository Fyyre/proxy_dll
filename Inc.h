#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <intrin.h>
#include <emmintrin.h>


#ifndef _NTDEF_
typedef _Return_type_success_(return >= 0) LONG NTSTATUS;
typedef NTSTATUS* PNTSTATUS;
#endif

