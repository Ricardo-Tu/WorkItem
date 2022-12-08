#pragma once

#ifdef __cplusplus
extern"C"
{
#endif // __cplusplus

#include <ntifs.h>

#include <ntddk.h>

#include <windef.h>

#include <intrin.h>

#include <wdmsec.h>



#ifdef __cplusplus
}
#endif // __cplusplus

#include "irp.h"

#define	IOCTL_911 (ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN,0x912,METHOD_BUFFERED,FILE_READ_DATA)
