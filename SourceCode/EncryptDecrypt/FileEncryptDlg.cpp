#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "FileEncryptDlg.h"
#include "EncryptDecryptDlg.h"
#include "afxdialogex.h"
#include <thread>
#include <utility>



IMPLEMENT_DYNAMIC(FileEncryptDlg, CDialogEx)

FileEncryptDlg::FileEncryptDlg(CWnd* pParent /*=NULL*/)
    : BaseDialog(IDD_DIALOG_ENCRYPT, pParent)
{
    m_SimpleCrypt = std::make_shared<SimpleEncrypt>();
}

FileEncryptDlg::~FileEncryptDlg()
{
}

BOOL FileEncryptDlg::ShowChildWindowMiddle()
{
    m_CheckDeleteFile.SetCheck(BST_UNCHECKED);
    m_CheckEncryptFileName.SetCheck(BST_CHECKED);

    m_EditInputPassword.SetWindowTextW(L"");
    m_EditRepeatPassword.SetWindowTextW(L"");

    return BaseDialog::ShowChildWindowMiddle();
}

std::wstring FileEncryptDlg::GetSourcePath() const
{
    CString SourcePath;
    m_EditSourcePath.GetWindowTextW(SourcePath);
    return SourcePath.GetString();
}

std::wstring FileEncryptDlg::GetTargetPath() const
{
    CString TargetPath;
    m_EditTargetPath.GetWindowTextW(TargetPath);

    wchar_t Path[MAX_PATH] = { 0 };
    wmemcpy(Path, TargetPath.GetString(), TargetPath.GetLength());

    PathAddBackslash(Path);
    TargetPath = Path;

    return TargetPath.GetString();
}

void FileEncryptDlg::Stop()
{
    m_SimpleCrypt->SetIsForceStop(true);
    for (std::vector<std::shared_ptr<SimpleEncrypt>>::size_type index = 0; index < m_vecSimpleEncrypt.size(); index++)
    {
        m_vecSimpleEncrypt[index]->SetIsForceStop(true);
    }
}

void FileEncryptDlg::ProcessWork(void* Parent)
{
    CEncryptDecryptDlg* ParentDlg = (CEncryptDecryptDlg*)Parent;
    const std::wstring& TargetPath = GetTargetPath();

    std::vector<std::wstring> FileNameVector;
    ParentDlg->GetFiles(FileNameVector);

    //是否有文件
    if (FileNameVector.size() == 0)
    {
        MessageBox(_T("No file to encrypt!"), _T("Error Tip"), MB_OK);
        return;
    }

    //单线程处理
    const int THREAD_COUNT = GetThreadCount(static_cast<int>(FileNameVector.size()));
    {
        if (THREAD_COUNT <= 1 || FileNameVector.size() == 1)
        {
            ParentDlg->UpdateResultList(1, FileNameVector[0], PT_PROCEING, std::wstring());
            bool ProcessResult = m_SimpleCrypt->Encrypt(FileNameVector[0], TargetPath);
            ParentDlg->UpdateResultList(1, FileNameVector[0], ProcessResult ? PT_SUCCEEDED : PT_FAILED, m_SimpleCrypt->GetErrorMessage());
            return;
        }
    }

    //多线程处理
    {
        //删除旧对象
        m_vecThread.clear();
        m_vecSimpleEncrypt.clear();

        //把任务压入队列
        for (std::vector<std::wstring>::size_type index = 0; index < FileNameVector.size(); index++)
        {
            m_TaskQueue.push(TaskNode(FileNameVector[index], static_cast<int>(index + 1)));
        }

        //创建多线程
        for (int index = 0; index < THREAD_COUNT; index++)
        {
            m_vecSimpleEncrypt.push_back(std::make_shared<SimpleEncrypt>());
            std::thread newThread(&FileEncryptDlg::MultiThreadWork, this, Parent, index, std::ref(TargetPath)); //特别注意，参数是引用！！！
            m_vecThread.push_back(std::move(newThread));
        }

        //等待结束
        for (std::vector<std::thread>::size_type index = 0; index < m_vecThread.size(); index++)
        {
            m_vecThread[index].join();
        }
    }
}

bool FileEncryptDlg::Validate()
{
    if (!theApp.Validate(m_EditSourcePath))
    {
        return false;
    }

    CString TargetPath;
    m_EditTargetPath.GetWindowTextW(TargetPath);
    if (!TargetPath.IsEmpty())
    {
        if (!PathIsDirectory(TargetPath.GetString()) && SHCreateDirectoryEx(NULL, TargetPath.GetString(), NULL) != ERROR_SUCCESS)
        {
            MessageBox(_T("Target path can not created!"), _T("Error Tip"), MB_OK);
            return false;
        }
    }

    return true;
}

bool FileEncryptDlg::SetOption()
{
    CString InputPassword;
    m_EditInputPassword.GetWindowTextW(InputPassword);

    CString RepeatPassword;
    m_EditRepeatPassword.GetWindowText(RepeatPassword);

    if (InputPassword != RepeatPassword)
    {
        MessageBox(_T("The passwords entered twice are different!"), _T("Error Tip"), MB_OK);
        return false;
    }

    m_InputPassword = InputPassword.GetString();
    m_SimpleCrypt->SetPassword(m_InputPassword);
    m_SimpleCrypt->SetIsEncryptFileName(m_CheckEncryptFileName.GetState() == BST_CHECKED);
    m_SimpleCrypt->SetIsDeleteOriginalFile(m_CheckDeleteFile.GetState() == BST_CHECKED);
    m_SimpleCrypt->SetIsForceStop(false);
    return true;
}

void FileEncryptDlg::MultiThreadWork(void* Parent, int ThreadIndex, const std::wstring& TargetPath)
{
    TaskNode Task;
    CEncryptDecryptDlg* ParentDlg = (CEncryptDecryptDlg*)Parent;
    std::shared_ptr<SimpleEncrypt> LocalSimpleEncrypt = m_vecSimpleEncrypt[ThreadIndex];
    LocalSimpleEncrypt->SetIsDeleteOriginalFile(m_SimpleCrypt->IsDeleteOriginalFile());
    LocalSimpleEncrypt->SetIsEncryptFileName(m_SimpleCrypt->IsEncryptFileName());
    LocalSimpleEncrypt->SetPassword(m_InputPassword);
    LocalSimpleEncrypt->SetIsForceStop(false);

    while (!LocalSimpleEncrypt->IsForceStop())
    {
        //取出元素
        {
            std::lock_guard<std::mutex> locker(m_TaskQueueMutex);
            if (m_TaskQueue.empty())
            {
                break;
            }

            Task = m_TaskQueue.front();
            m_TaskQueue.pop();
        }

        //更新界面
        {
            std::lock_guard<std::mutex> locker(m_ResultMutex);
            ParentDlg->UpdateResultList(Task.m_FileIndex, Task.m_FileName, Task.m_ProcessType, Task.m_Error);
        }

        //处理业务
        {
            bool ProcessResult = LocalSimpleEncrypt->Encrypt(Task.m_FileName, TargetPath);
            Task.m_ProcessType = ProcessResult ? PT_SUCCEEDED : PT_FAILED;
            Task.m_Error = LocalSimpleEncrypt->GetErrorMessage();
        }

        //更新界面
        {
            std::lock_guard<std::mutex> locker(m_ResultMutex);
            ParentDlg->UpdateResultList(Task.m_FileIndex, Task.m_FileName, Task.m_ProcessType, Task.m_Error);
        }
    }
}

void FileEncryptDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_SOURCE_PATH, m_EditSourcePath);
    DDX_Control(pDX, IDC_EDIT_TARGET_PATH, m_EditTargetPath);
    DDX_Control(pDX, IDC_EDIT_INPUT_PASSWORD, m_EditInputPassword);
    DDX_Control(pDX, IDC_EDIT_REPEAT_PASSWORD, m_EditRepeatPassword);
    DDX_Control(pDX, IDC_CHECK_ENCRYPT_FILE_NAME, m_CheckEncryptFileName);
    DDX_Control(pDX, IDC_CHECK_DELETE_FILE, m_CheckDeleteFile);
}

BEGIN_MESSAGE_MAP(FileEncryptDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_SOURCE_PATH, &FileEncryptDlg::OnBnClickedButtonSourcePath)
    ON_BN_CLICKED(IDC_BUTTON_TARGET_PATH, &FileEncryptDlg::OnBnClickedButtonTargetPath)
    ON_BN_CLICKED(IDOK, &FileEncryptDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &FileEncryptDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

// FileEncrypt message handlers

void FileEncryptDlg::OnBnClickedButtonSourcePath()
{
    const std::wstring& SelectPath = theApp.GetSelectPath();
    if (SelectPath.empty())
    {
        //Add log
        return;
    }

    m_EditSourcePath.SetWindowTextW(SelectPath.c_str());
}

void FileEncryptDlg::OnBnClickedButtonTargetPath()
{
    const std::wstring SelectPath = theApp.GetSelectPath();
    m_EditTargetPath.SetWindowTextW(SelectPath.c_str());
}

void FileEncryptDlg::OnBnClickedCancel()
{
    CEncryptDecryptDlg* ParentDlg = (CEncryptDecryptDlg*)GetParent();
    ParentDlg->ResetOperationType();
    CDialogEx::OnCancel();
}

void FileEncryptDlg::OnBnClickedOk()
{
    if (Validate() && SetOption())
    {
        CEncryptDecryptDlg* ParentDlg = (CEncryptDecryptDlg*)GetParent();
        ParentDlg->SetOperationType(OT_ENCRYPT);
        ParentDlg->Start();
        CDialogEx::OnOK();
    }
}
