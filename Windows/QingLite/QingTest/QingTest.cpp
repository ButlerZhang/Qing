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
        PTP_CALLBACK_INSTANCE pInstance,    //回调函数的终止操作
        PVOID pvContext);                   //参数

    BOOL TrySubmitThreadpoolCallback(
        PTP_SIMPLE_CALLBACK pfnCallback,    //SimpleCallBack函数
        PVOID pvContext,                    //传给回调函数的值
        PTP_CALLBACK_ENVIRON pcbe);         //对线程池进行定制，通常传NULL

    PTP_WORK CreateThreadpoolWork(
        PTP_WORK_CALLBACK pfnWorkHandler,   //回调函数
        PVOID pvContext,                    //回调函数的参数
        PTP_CALLBACK_ENVIRON);              //对线程池进行定制

    VOID CALLBACK WorkCallback(
        PTP_CALLBACK_INSTANCE Instance,     //
        PVOID Context,                      //
        PTP_WORK Work);                     //

    VOID SubmitThreadpoolWork(PTP_WORK pWork);

    VOID WaitForThreadpoolWorkCallbacks(
        PTP_WORK pWork,                     //工作项(CreateThreadpoolWork创建)
        BOOL bCancelPendingCallbacks);      //复杂，要具体参阅MSDN

    VOID CALLBACK TimeoutCallback(
        PTP_CALLBACK_INSTANCE pInstance,    //回调函数的终止操作
        PVOID pvContext,                    //回调函数的实际参数
        PTP_TIMER pTimer);                  //计时器对象

    PTP_TIMER CreateThreadpoolTimer(
        PTP_TIMER_CALLBACK pfnTimerCallback,//TimeoutCallback函数
        PVOID pvContext,                    //TimeoutCallback参数
        PTP_CALLBACK_ENVIRON pcbe);         //对线程池进行定制

    VOID SetThreadpoolTimer(
        PTP_TIMER pTimer,       //CreateThreadpoolTimer创建的对象
        PFILETIME pftDueTime,   //第一次调用回调函数应该是什么时候，-1表示立即开始
        DWORD msPeriod,         //重复调用间隔，单位为微妙，0表示触发一次
        DWORD msWindowLength);  //给回调函数的执行时间增加一些随机性

    VOID CALLBACK WaitCallback(
        PTP_CALLBACK_INSTANCE pInstance, 回调函数的终止操作
        PVOID Context,                   //回调函数的参数
        PTP_WAIT wait,                   //CreateThreadpoolWait返回的对象
        TP_WAIT_RESULT WaitResult);      //等待结果

    PTP_WAIT CreateThreadpoolWait(
        PTP_WAIT_CALLBACK pfnWaitCallback,  //WaitCallback函数
        PVOID pvContext,                    //WaitCallback参数
        PTP_CALLBACK_ENVIRON pcbe);         //对线程池进行定制

    VOID SetThreadpoolWait(
        PTP_WAIT pWaitItem,         //CreateThreadpoolWait返回的对象
        HANDLE hObject,             //内核对象
        PFILETIME pftTimeout);      //等待内核对象被触发的最长时间

    VOID CALLBACK OverlappedCompletionRoutine(
        PTP_CALLBACK_INSTANCE pInstance,    //回调函数的终止操作
        PVOID pvContext,                    //回调函数参数
        PVOID pOverlapped,                  //重叠结构指针
        ULONG IoResult,                     //操作的结果
        ULONG_PTR NumberOfBytesTranferred,  //已传输的字节数
        PTP_IO pIo);                        //指向线程池中的IO项的指针

    PTP_IO CreateThreadpoolIo(
        HANDLE hDevice,                     //文件或设备句柄
        PTP_WIN32_IO_CALLBACK pfnIoCallback,//回调函数
        PVOID pvContext,                    //回调函数参数
        PTP_CALLBACK_ENVIRON pcbe);         //对线程池进行定制

    VOID StartThreadpoolIo(PTP_IO pIo);

    VOID CancelThreadpoolIo(PTP_IO pIo);

    VOID CloseThreadpoolIo(PTP_IO pIo);

    VOID WaitForThreadpoolIoCallbacks(
        PTP_IO pio, BOOL bCancelPendingCallbacks);

    PTP_POOL CreateThreadpool(PVOID reserved); //reserved是保留的，传NULL
    BOOL SetThreadpoolThreadMinimum(PTP_POOL pThreadPool, DWORD cthrdMin);
    BOOL SetThreadpoolThreadMaximum(PTP_POOL pThreadPool, DWORD cthrdMost);
    VOID CloseThreadpool(PTP_POOL pThreadPool);

    ::InitializeThreadpoolEnvironment

    system("pause");
    return 0;
}
