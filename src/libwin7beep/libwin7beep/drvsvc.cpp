
#include "stdafx.h"
#include "drvsvc.h"

#include <iostream>
#include <tchar.h>

//private method list

bool Win7BeepSvc::SetCurrentPath(){

	if (!GetCurrentDirectory(MAX_PATH, this->szPath))
	{
		printf("Cannot install service (%d)\n", GetLastError());
		return false;
	}

	_tcscat_s(this->szPath, TEXT("\\"));
	_tcscat_s(this->szPath, WIN7_BEEP_DRV_NAME);

	return true;
}

bool Win7BeepSvc::OpenSvcCtrlMgr(){

	// Get a handle to the SCM database. 
	this->schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (this->schSCManager == NULL)
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return false;
	}

	return true;
}

//publics

Win7BeepSvc::Win7BeepSvc(){

	if (!this->SetCurrentPath()){
		this->isInitialized = false;
		return;
	}

	if (!this->OpenSvcCtrlMgr()){
		this->isInitialized = false;
		return;
	}

	this->isInitialized = true;
}

Win7BeepSvc::Win7BeepSvc(bool* _isInitialized){

	if (!this->SetCurrentPath()){
		this->isInitialized = false;
		*_isInitialized = false;
		return;
	}

	if (!this->OpenSvcCtrlMgr()){
		this->isInitialized = false;
		*_isInitialized = false;
		return;
	}

	*_isInitialized = true;
	this->isInitialized = true;

}

Win7BeepSvc::~Win7BeepSvc(){

	if (schService != NULL){
		CloseServiceHandle(schService);
	}

	if (schSCManager != NULL){
		CloseServiceHandle(schSCManager);
	}
}

bool Win7BeepSvc::InstallSvc(){

	if (!this->isInitialized){
		return false;
	}

	// Create the service
	this->schService = CreateService(
		this->schSCManager,         // SCM database 
		WIN7_BEEP_DRV_SVC_NAME,		// name of service 
		WIN7_BEEP_DRV_SVC_DESC_NAME,// service name to display 
		SERVICE_ALL_ACCESS,			// desired access 
		SERVICE_KERNEL_DRIVER,		// service type 
		SERVICE_DEMAND_START,		// start type 
		SERVICE_ERROR_NORMAL,		// error control type 
		this->szPath,				// path to service's binary 
		NULL,						// no load ordering group 
		NULL,						// no tag identifier 
		NULL,						// no dependencies 
		NULL,						// LocalSystem account 
		NULL);						// no password 

	if (this->schService == NULL){
		printf("CreateService failed (%d)\n", GetLastError());
		CloseServiceHandle(this->schSCManager);
		return false;
	}
	else printf("Service installed successfully\n");

	//CloseServiceHandle(this->schService);
	//CloseServiceHandle(this->schSCManager);

	return true;
}

bool Win7BeepSvc::StartSvc(){

	SERVICE_STATUS_PROCESS ssStatus;
	DWORD dwOldCheckPoint;
	DWORD dwStartTickCount;
	DWORD dwWaitTime;
	DWORD dwBytesNeeded;

	if (!this->isInitialized){
		return false;
	}

	//Check Service Condition
	if (!QueryServiceStatusEx(
		this->schService,                     // handle to service 
		SC_STATUS_PROCESS_INFO,         // information level
		(LPBYTE)&ssStatus,             // address of structure
		sizeof(SERVICE_STATUS_PROCESS), // size of structure
		&dwBytesNeeded)){              // size needed if buffer is too small
	
		printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		CloseServiceHandle(this->schService);
		CloseServiceHandle(this->schSCManager);
		return false;
	}

	// Check if the service is already running. It would be possible 
	// to stop the service here, but for simplicity this example just returns. 
	if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING){
		printf("Cannot start the service because it is already running\n");
		CloseServiceHandle(this->schService);
		CloseServiceHandle(this->schSCManager);
		return false;
	}

	// Save the tick count and initial checkpoint.

	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING){
		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth of the wait hint but not less than 1 second  
		// and not more than 10 seconds. 

		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		// Check the status until the service is no longer stop pending. 

		if (!QueryServiceStatusEx(
			this->schService,                     // handle to service 
			SC_STATUS_PROCESS_INFO,         // information level
			(LPBYTE)&ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded)){              // size needed if buffer is too small
		
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			CloseServiceHandle(this->schService);
			CloseServiceHandle(this->schSCManager);
			return false;
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint){
			// Continue to wait and check.

			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}else{

			if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint){
				printf("Timeout waiting for service to stop\n");
				CloseServiceHandle(this->schService);
				CloseServiceHandle(this->schSCManager);
				return false;
			}
		}
	}

	// Attempt to start the service.

	if (!StartService(
		this->schService,	// handle to service 
		0,					// number of arguments 
		NULL)){				// no arguments 
	
		printf("StartService failed (%d)\n", GetLastError());
		CloseServiceHandle(this->schService);
		CloseServiceHandle(this->schSCManager);
		return false;

	}else{
		printf("Service start pending...\n");
	}

	// Check the status until the service is no longer start pending. 

	if (!QueryServiceStatusEx(
		this->schService,                // handle to service 
		SC_STATUS_PROCESS_INFO,         // info level
		(LPBYTE)&ssStatus,             // address of structure
		sizeof(SERVICE_STATUS_PROCESS), // size of structure
		&dwBytesNeeded)){              // if buffer too small
	
		printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		CloseServiceHandle(this->schService);
		CloseServiceHandle(this->schSCManager);
		return false;
	}

	// Save the tick count and initial checkpoint.

	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	while (ssStatus.dwCurrentState == SERVICE_START_PENDING){
		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth the wait hint, but no less than 1 second and no 
		// more than 10 seconds. 

		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		// Check the status again. 

		if (!QueryServiceStatusEx(
			this->schService,             // handle to service 
			SC_STATUS_PROCESS_INFO, // info level
			(LPBYTE)&ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded)){              // if buffer too small
		
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			break;
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint){
			// Continue to wait and check.

			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}else{
			if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint){
				// No progress made within the wait hint.
				break;
			}
		}
	}

	// Determine whether the service is running.

	if (ssStatus.dwCurrentState == SERVICE_RUNNING){

		printf("Service started successfully.\n");
		return true;

	}else{

		printf("Service not started. \n");
		printf("  Current State: %d\n", ssStatus.dwCurrentState);
		printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
		printf("  Check Point: %d\n", ssStatus.dwCheckPoint);
		printf("  Wait Hint: %d\n", ssStatus.dwWaitHint);
		return false;
	}

}
bool Win7BeepSvc::StopSvc(){

	SERVICE_STATUS ssStatus;
	DWORD dwBytesNeeded;

	if (!this->isInitialized){
		return false;
	}

	if (ControlService(this->schService, SERVICE_CONTROL_STOP, &ssStatus) != FALSE){

		while (ssStatus.dwCurrentState != SERVICE_STOPPED){

			Sleep(ssStatus.dwWaitHint);

			if (!QueryServiceStatusEx(
				this->schService,             // handle to service 
				SC_STATUS_PROCESS_INFO, // info level
				(LPBYTE)&ssStatus,             // address of structure
				sizeof(SERVICE_STATUS_PROCESS), // size of structure
				&dwBytesNeeded)){              // if buffer too small

				printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
				return false;
			}
		}
	}else{
		printf("ControlService failed (%d)\n", GetLastError());
		return false;
	}

	return true;
}

bool Win7BeepSvc::UninstallSvc(){
	SERVICE_STATUS ssStatus;
	DWORD dwBytesNeeded;

	if (!this->isInitialized){
		return false;
	}

	do{
		if (!QueryServiceStatusEx(
			this->schService,             // handle to service 
			SC_STATUS_PROCESS_INFO, // info level
			(LPBYTE)&ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded)){              // if buffer too small

			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			return false;
		}

		Sleep(ssStatus.dwWaitHint);

	} while (ssStatus.dwCurrentState != SERVICE_STOPPED);

	if (!DeleteService(this->schService)){
		printf("DeleteService failed (%d)\n", GetLastError());
		return false;
	}

	return true;
}




