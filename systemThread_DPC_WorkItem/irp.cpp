#include "common.h"


KDPC	Dpc = { 0 };

LARGE_INTEGER DueTime = { 0 };

KTIMER	Timer = { 0 };

PIO_WORKITEM	pIoWorkItem = { 0 };

PDEVICE_OBJECT	g_pDeviceObject = NULL;




VOID
MyWorkItem(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_opt_ PVOID Context
)
{

	KdPrint(("My Driver: My Work Item!\n"));

}




VOID
CustomDpc(
	_In_ struct _KDPC* Dpc,
	_In_opt_ PVOID DeferredContext,
	_In_opt_ PVOID SystemArgument1,
	_In_opt_ PVOID SystemArgument2
)
{

	// 使用IoAllocateWorkItem分配一个ioworkitem  
	pIoWorkItem = IoAllocateWorkItem(g_pDeviceObject);
	// IoInitializeWorkItem(DeviceObject,pIoWorkItem); 

	 if (pIoWorkItem)
	 {
	     //插入一个workitem， 其中TestFile就是我要写文件的函数，第四个参数也是该函数的参数
	     IoQueueWorkItem(pIoWorkItem, (PIO_WORKITEM_ROUTINE)MyWorkItem, DelayedWorkQueue, NULL);
	 }

	 //由于要定时写，因此再次设置定时器，如果不设置只写一次
	 KeSetTimer(&Timer, DueTime, Dpc);

}






_IRQL_requires_same_
_Function_class_(KSTART_ROUTINE)
VOID
MySystemThread(
	_In_ PVOID StartContext
)
{

	DueTime = RtlConvertLongToLargeInteger(-10 * 1000 * 1000);
	//初始化一个Dpc
	//这个CustomDpc是自定义的函数，运行在DISPATCH_LEVEL上,后面的参数myInfo是该函数的参数
	KeInitializeDpc(&Dpc, (PKDEFERRED_ROUTINE)CustomDpc, NULL);
	 //设置DPC定时器
	KeSetTimer(&Timer, DueTime, &Dpc);
	  //等待定时器
	KeWaitForSingleObject(&Timer, Executive, KernelMode, FALSE, NULL);

}






NTSTATUS
MyDeviceIo(
	_In_ PDEVICE_OBJECT pDeviceObject,
	_In_ PIRP	pIrp
)
{

	PIO_STACK_LOCATION  irpsp = IoGetCurrentIrpStackLocation(pIrp);

	NTSTATUS status = STATUS_SUCCESS;

	ULONG ret_len = 0;

	// 处理DeviceIoControl。
	PVOID buffer = pIrp->AssociatedIrp.SystemBuffer;

	ULONG inlen = irpsp->Parameters.DeviceIoControl.InputBufferLength;

	ULONG outlen = irpsp->Parameters.DeviceIoControl.OutputBufferLength;

	ULONG len;

	HANDLE	hThread = NULL;

	OBJECT_ATTRIBUTES obja = { 0 };

	CLIENT_ID	CID = { 0 };

	switch (irpsp->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_911:
	{
		if (*(PCHAR)buffer == 0x55)
			KdPrint(("My Driver:<%x>\n", *(PCHAR)buffer));

		g_pDeviceObject = pDeviceObject;

		InitializeObjectAttributes(&obja, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

		status = status = PsCreateSystemThread(
			     &hThread,
			     GENERIC_READ | GENERIC_WRITE,
			     &obja,
			     NULL,
			     &CID,
			     (PKSTART_ROUTINE)MySystemThread,
			     NULL
			     );

		break;
	}
	default:
		break;
	}
	


	pIrp->IoStatus.Information = ret_len;

	pIrp->IoStatus.Status = status;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return status;

}






NTSTATUS
Dispatch(
	_In_ PDEVICE_OBJECT pDeviceObject,
	_In_ PIRP	pIrp
)
{

	pIrp->IoStatus.Information = 0;

	pIrp->IoStatus.Status = STATUS_SUCCESS;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;

}



