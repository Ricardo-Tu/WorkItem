#include <Windows.h>

#include <stdio.h>	

#include <tchar.h>

#include <ioapiset.h>

#define	IOCTL_911 (ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN,0x912,METHOD_BUFFERED,FILE_READ_DATA)

#define	SYMLINK_NAME  L"\\\\.\\MyLinkName"

int _tmain(void)
{

	HANDLE	hDevice = NULL;

	hDevice = CreateFile(SYMLINK_NAME, GENERIC_WRITE | GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Open Device Error!\n");

		system("pause");

		return 0;
	}

	DWORD	dwIn = 0x55;

	DWORD	dwOut = 0;

	DWORD	ret = 0;

	if (!DeviceIoControl(hDevice, IOCTL_911, &dwIn, sizeof(DWORD), &dwOut, sizeof(DWORD), &ret, NULL));
	{
		printf("Error Code: <%d> \n", GetLastError());

		system("pause");

		return 0;
	}

	printf("<%d>OK!\r\n",dwOut);

	return 0;

}














