#ifndef __WIN7BEEP_H__
#define __WIN7BEEP_H__

#include <ntddk.h>

#define DEVICE_SYM_LINK_NAME L"\\DosDevices\\win7beep"
#define CTL_DEVICE_NAME L"\\Device\\win7beep"
#define CTL_DEVICE_GLOBAL_PATH L"\\\\.\\win7beep"
#define POOL_TAG (ULONG) "peeb7niw"


typedef struct _GLOBALS{
	PDEVICE_OBJECT pCtrlDevObj;
}GLOBALS, *PGLOBALS;


DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;

DRIVER_DISPATCH DispatchCreate;

DRIVER_DISPATCH DispatchClose;

DRIVER_DISPATCH DispatchCleanup;

DRIVER_DISPATCH DispatchIoctl;

#endif