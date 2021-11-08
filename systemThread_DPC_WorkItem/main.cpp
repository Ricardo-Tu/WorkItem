#include "common.h"	

NTSTATUS CreateDevice(_In_ PDRIVER_OBJECT pDriverObject);



VOID	UnloadMyDriver(_In_ PDRIVER_OBJECT pDriverObject)
{

	UNICODE_STRING	symlinkname = { 0 };

	RtlInitUnicodeString(&symlinkname, L"\\??\\MyLinkName");

	if (pDriverObject->DeviceObject)
	{
		KeCancelTimer(&Timer);
		
		IoFreeWorkItem(pIoWorkItem);

		IoDeleteDevice(pDriverObject->DeviceObject);

		IoDeleteSymbolicLink(&symlinkname);

	}

	KdPrint(("My Driver:Unload...\n"));

}



EXTERN_C
NTSTATUS
DriverEntry(
	_In_	PDRIVER_OBJECT pDriverObject,
	_In_	PUNICODE_STRING	pRegistryPath
)
{

	NTSTATUS	status = STATUS_SUCCESS;

	for (ULONG i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		pDriverObject->MajorFunction[i] = Dispatch;
	}

	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MyDeviceIo;


	pDriverObject->DriverUnload = UnloadMyDriver;

	CreateDevice(pDriverObject);

	KeInitializeTimer(&Timer);


	return STATUS_SUCCESS;

}



NTSTATUS CreateDevice(_In_ PDRIVER_OBJECT pDriverObject)
{

	NTSTATUS status = STATUS_SUCCESS;

	UNICODE_STRING devicename = { 0 };

	UNICODE_STRING symlinkname = { 0 };

	PDEVICE_OBJECT pDeviceObject = NULL;

	RtlInitUnicodeString(&devicename,L"\\Device\\MyDevice");

	RtlInitUnicodeString(&symlinkname, L"\\??\\MyLinkName");

	// 生成一个控制设备对象。
	status = IoCreateDevice(
		pDriverObject,
		0, 
		&devicename,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE, 
		&pDeviceObject);

	if (!NT_SUCCESS(status))
		return status;

	// 生成符号链接.
	IoDeleteSymbolicLink(&symlinkname);

	status = IoCreateSymbolicLink(&symlinkname, &devicename);

	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDeviceObject);

		return status;
	}

	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	pDeviceObject->Flags |= DO_BUFFERED_IO;

	KdPrint(("My Driver:Create Device Success!\n"));

	return status;

}




























