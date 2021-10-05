#include <windows.h> 
#include <stdio.h>
#include <tchar.h>

typedef void(__cdecl *TEST)(void);

int main(){

	HINSTANCE hinstLib;
	TEST test;
	BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

	// Get a handle to the DLL module.

	hinstLib = LoadLibrary(TEXT("./libwin7beep.dll"));

	// If the handle is valid, try to get the function address.

	if (hinstLib != NULL)
	{
		test = (TEST)GetProcAddress(hinstLib, "hello_world");

		// If the function address is valid, call the function.

		if (NULL != test)
		{
			fRunTimeLinkSuccess = TRUE;
			(test)();
		}
		else
		{
			printf("cannot found symbol\n");
		}
		// Free the DLL module.

		fFreeResult = FreeLibrary(hinstLib);
	}
	else
	{
		printf("cannot found dll\n");
	}

	// If unable to call the DLL function, use an alternative.
	if (!fRunTimeLinkSuccess)
		printf("Message printed from executable\n");

	return 0;
}