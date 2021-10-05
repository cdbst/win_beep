// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <iostream>

unsigned int ref_count = 0;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ref_count++;
		printf("dll - DLL_PROCESS_ATTACH %d \n", ref_count);
		break;
	case DLL_THREAD_ATTACH:
		ref_count++;
		printf("dll - DLL_THREAD_ATTACH %d \n", ref_count);
		break;
	case DLL_THREAD_DETACH:
		ref_count--;
		printf("dll - DLL_THREAD_DETACH %d \n", ref_count);
		break;
	case DLL_PROCESS_DETACH:
		ref_count--;
		printf("dll - DLL_PROCESS_DETACH %d \n", ref_count);
		break;
	}
	return TRUE;
}

