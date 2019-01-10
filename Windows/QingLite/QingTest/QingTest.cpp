// QingTest.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "..\..\Qing\SourceCode\System\SystemTime.h"
#include "..\..\Qing\SourceCode\Example\Windows\WindowsCoreProgramming.h"
#include <iostream>


int main()
{
    VOID NTAPI SimpleCallBack(
        PTP_CALLBACK_INSTANCE pInstance,    //�ص���������ֹ����
        PVOID pvContext);                   //����

    BOOL TrySubmitThreadpoolCallback(
        PTP_SIMPLE_CALLBACK pfnCallback,    //SimpleCallBack����
        PVOID pvContext,                    //�����ص�������ֵ
        PTP_CALLBACK_ENVIRON pcbe);         //���̳߳ؽ��ж��ƣ�ͨ����NULL

    PTP_WORK CreateThreadpoolWork(
        PTP_WORK_CALLBACK pfnWorkHandler,   //�ص�����
        PVOID pvContext,                    //�ص������Ĳ���
        PTP_CALLBACK_ENVIRON);              //���̳߳ؽ��ж���

    VOID CALLBACK WorkCallback(
        PTP_CALLBACK_INSTANCE Instance,     //
        PVOID Context,                      //
        PTP_WORK Work);                     //

    VOID SubmitThreadpoolWork(PTP_WORK pWork);

    VOID WaitForThreadpoolWorkCallbacks(
        PTP_WORK pWork,                     //������(CreateThreadpoolWork����)
        BOOL bCancelPendingCallbacks);      //���ӣ�Ҫ�������MSDN

    VOID CALLBACK TimeoutCallback(
        PTP_CALLBACK_INSTANCE pInstance,    //�ص���������ֹ����
        PVOID pvContext,                    //�ص�������ʵ�ʲ���
        PTP_TIMER pTimer);                  //��ʱ������

    PTP_TIMER CreateThreadpoolTimer(
        PTP_TIMER_CALLBACK pfnTimerCallback,//TimeoutCallback����
        PVOID pvContext,                    //TimeoutCallback����
        PTP_CALLBACK_ENVIRON pcbe);         //���̳߳ؽ��ж���

    VOID SetThreadpoolTimer(
        PTP_TIMER pTimer,       //CreateThreadpoolTimer�����Ķ���
        PFILETIME pftDueTime,   //��һ�ε��ûص�����Ӧ����ʲôʱ��-1��ʾ������ʼ
        DWORD msPeriod,         //�ظ����ü������λΪ΢�0��ʾ����һ��
        DWORD msWindowLength);  //���ص�������ִ��ʱ������һЩ�����

    VOID CALLBACK WaitCallback(
        PTP_CALLBACK_INSTANCE pInstance, �ص���������ֹ����
        PVOID Context,                   //�ص������Ĳ���
        PTP_WAIT wait,                   //CreateThreadpoolWait���صĶ���
        TP_WAIT_RESULT WaitResult);      //�ȴ����

    PTP_WAIT CreateThreadpoolWait(
        PTP_WAIT_CALLBACK pfnWaitCallback,  //WaitCallback����
        PVOID pvContext,                    //WaitCallback����
        PTP_CALLBACK_ENVIRON pcbe);         //���̳߳ؽ��ж���

    VOID SetThreadpoolWait(
        PTP_WAIT pWaitItem,         //CreateThreadpoolWait���صĶ���
        HANDLE hObject,             //�ں˶���
        PFILETIME pftTimeout);      //�ȴ��ں˶��󱻴������ʱ��

    VOID CALLBACK OverlappedCompletionRoutine(
        PTP_CALLBACK_INSTANCE pInstance,    //�ص���������ֹ����
        PVOID pvContext,                    //�ص���������
        PVOID pOverlapped,                  //�ص��ṹָ��
        ULONG IoResult,                     //�����Ľ��
        ULONG_PTR NumberOfBytesTranferred,  //�Ѵ�����ֽ���
        PTP_IO pIo);                        //ָ���̳߳��е�IO���ָ��

    PTP_IO CreateThreadpoolIo(
        HANDLE hDevice,                     //�ļ����豸���
        PTP_WIN32_IO_CALLBACK pfnIoCallback,//�ص�����
        PVOID pvContext,                    //�ص���������
        PTP_CALLBACK_ENVIRON pcbe);         //���̳߳ؽ��ж���

    VOID StartThreadpoolIo(PTP_IO pIo);

    VOID CancelThreadpoolIo(PTP_IO pIo);

    VOID CloseThreadpoolIo(PTP_IO pIo);

    VOID WaitForThreadpoolIoCallbacks(
        PTP_IO pio, BOOL bCancelPendingCallbacks);

    PTP_POOL CreateThreadpool(PVOID reserved); //reserved�Ǳ����ģ���NULL
    BOOL SetThreadpoolThreadMinimum(PTP_POOL pThreadPool, DWORD cthrdMin);
    BOOL SetThreadpoolThreadMaximum(PTP_POOL pThreadPool, DWORD cthrdMost);
    VOID CloseThreadpool(PTP_POOL pThreadPool);

    ::InitializeThreadpoolEnvironment

    system("pause");
    return 0;
}
