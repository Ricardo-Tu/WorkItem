#pragma once








NTSTATUS
Dispatch(
	_In_ PDEVICE_OBJECT pDeviceObject,
	_In_ PIRP	pIrp
);



NTSTATUS
MyDeviceIo(
	_In_ PDEVICE_OBJECT pDeviceObject,
	_In_ PIRP	pIrp
);




extern KTIMER	Timer;

extern PIO_WORKITEM	pIoWorkItem;





