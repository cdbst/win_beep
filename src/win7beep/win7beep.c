#include "win7beep.h"
#include "ioctl_code.h"


#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, DriverUnload)
#endif

GLOBALS globals;

NTSTATUS DriverEntry(PDRIVER_OBJECT pDrvObj, PUNICODE_STRING registryPath){
    UNICODE_STRING ctlDevName;
    UNICODE_STRING symbolicLinkName;
	NTSTATUS status;

	UNREFERENCED_PARAMETER(registryPath);		

	pDrvObj->DriverUnload = DriverUnload;
	pDrvObj->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	pDrvObj->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
	pDrvObj->MajorFunction[IRP_MJ_CLEANUP] = DispatchCleanup;
	pDrvObj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoctl;

	RtlInitUnicodeString(&ctlDevName, CTL_DEVICE_NAME);
	RtlInitUnicodeString(&symbolicLinkName, DEVICE_SYM_LINK_NAME);

	status = IoCreateDevice(pDrvObj,
	        				0,
	        				&ctlDevName,
	        				FILE_DEVICE_UNKNOWN,
	        				FILE_DEVICE_SECURE_OPEN,
	        				FALSE,
	        				&globals.pCtrlDevObj);  

	if(NT_SUCCESS(status)){

	    globals.pCtrlDevObj->Flags |= DO_BUFFERED_IO;

	    status = IoCreateSymbolicLink( &symbolicLinkName, &ctlDevName );

	    if (!NT_SUCCESS(status)){
	        IoDeleteDevice(globals.pCtrlDevObj);
	        return status;
	    }

	    globals.pCtrlDevObj->Flags &= ~DO_DEVICE_INITIALIZING;
	}

	return status;
}


VOID DriverUnload(PDRIVER_OBJECT pDrvObj){
	PAGED_CODE();
	UNREFERENCED_PARAMETER(pDrvObj);
}


NTSTATUS DispatchCreate(PDEVICE_OBJECT pDevObj, PIRP pIrp){
	UNREFERENCED_PARAMETER(pDevObj);	
	
	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	
	return STATUS_SUCCESS;
}


NTSTATUS DispatchClose(PDEVICE_OBJECT pDevObj, PIRP pIrp){
	UNREFERENCED_PARAMETER(pDevObj);
	
	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	
	return STATUS_SUCCESS;
}


NTSTATUS DispatchCleanup(PDEVICE_OBJECT pDevObj, PIRP pIrp){
	UNREFERENCED_PARAMETER(pDevObj);
	
	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	
	return STATUS_SUCCESS;
}


NTSTATUS DispatchIoctl(PDEVICE_OBJECT pDevObj, PIRP pIrp){
	NTSTATUS status;
	PIO_STACK_LOCATION pIoStackLocation;

	UNREFERENCED_PARAMETER(pDevObj);

	pIoStackLocation = IoGetCurrentIrpStackLocation(pIrp);

	switch(pIoStackLocation->Parameters.DeviceIoControl.IoControlCode){
	case IOCTL_ON_BEEP:
		WRITE_PORT_UCHAR((PUCHAR)0x61, (READ_PORT_UCHAR((PUCHAR)0x61) | (UCHAR)0x03));
		WRITE_PORT_UCHAR((PUCHAR)0x43, (UCHAR)0xb6);
		WRITE_PORT_UCHAR((PUCHAR)0x42, (UCHAR)0xd0);
		WRITE_PORT_UCHAR((PUCHAR)0x42, (UCHAR)0x11);
		
		status = STATUS_SUCCESS;
		break;
		
	case IOCTL_OFF_BEEP:
		WRITE_PORT_UCHAR((PUCHAR)0x61, (READ_PORT_UCHAR((PUCHAR)0x61) & (UCHAR)0xfc));
		status = STATUS_SUCCESS;
		break;
		
	default:
		status = STATUS_INVALID_PARAMETER;
		break;
	}
	
	return status;
}


