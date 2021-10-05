// libwin7beep.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <iostream>
#include "drvsvc.h"

Win7BeepSvc* beepSvc;

extern "C" __declspec(dllexport) void hello_world(){
	printf("hello world\n");
}

extern "C" __declspec(dllexport) bool Init(){
	bool status;

	beepSvc = new Win7BeepSvc(&status);

	if (!status){
		return false;
	}

	if (!beepSvc->InstallSvc()){
		return false;
	}

	if (!beepSvc->StartSvc()){
		return false;
	}
}

