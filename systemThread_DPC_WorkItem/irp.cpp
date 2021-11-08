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

	// ʹ��IoAllocateWorkItem����һ��ioworkitem  
	pIoWorkItem = IoAllocateWorkItem(g_pDeviceObject);
	// IoInitializeWorkItem(DeviceObject,pIoWorkItem); 

	 if (pIoWorkItem)
	 {
	     //����һ��workitem�� ����TestFile������Ҫд�ļ��ĺ��������ĸ�����Ҳ�Ǹú����Ĳ���
	     IoQueueWorkItem(pIoWorkItem, (PIO_WORKITEM_ROUTINE)MyWorkItem, DelayedWorkQueue, NULL);
	 }

	 //����Ҫ��ʱд������ٴ����ö�ʱ�������������ֻдһ��
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
	//��ʼ��һ��Dpc
	//���CustomDpc���Զ���ĺ�����������DISPATCH_LEVEL��,����Ĳ���myInfo�Ǹú����Ĳ���
	KeInitializeDpc(&Dpc, (PKDEFERRED_ROUTINE)CustomDpc, NULL);
	 //����DPC��ʱ��
	KeSetTimer(&Timer, DueTime, &Dpc);
	  //�ȴ���ʱ��
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

	// ����DeviceIoControl��
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



