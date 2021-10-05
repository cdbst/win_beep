#ifndef __DRVSVC_H__
#define __DRVSVC_H__

#include <windows.h>
#include <winsvc.h>


#define WIN7_BEEP_DRV_SVC_NAME			TEXT("win7beep")
#define WIN7_BEEP_DRV_NAME				TEXT("win7beep.sys")
#define WIN7_BEEP_DRV_SVC_DESC_NAME		WIN7_BEEP_DRV_SVC_NAME

class Win7BeepSvc{
	bool isInitialized;
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	TCHAR szPath[MAX_PATH];

public:
	Win7BeepSvc();
	Win7BeepSvc(bool* _isInitialized);
	~Win7BeepSvc();

	bool InstallSvc();

	bool StartSvc();
	bool StopSvc();
	bool UninstallSvc();
	

private:
	bool SetCurrentPath();
	bool OpenSvcCtrlMgr();

};

#endif