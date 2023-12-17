#include "stdafx.h"
#include "EncryptDecrypt.h"
#include "FileDecryptDlg.h"
#include "EncryptDecryptDlg.h"
#include "afxdialogex.h"



IMPLEMENT_DYNAMIC(FileDecryptDlg, CDialogEx)

FileDecryptDlg::FileDecryptDlg(CWnd* pParent /*=NULL*/)
    : BaseDialog(IDD_DIALOG_DECRYPT, pParent)
{
    m_SimpleCrypt = std::make_shared<SimpleEncrypt>();
}

FileDecryptDlg::~FileDecryptDlg()
{
}

BOOL FileDecryptDlg::ShowChildWindowMiddle()
{
    m_EditInputPassword.SetWindowTextW(L"");
    return BaseDialog::ShowChildWindowMiddle();
}

std::wstring FileDecryptDlg::GetSourcePath() const
{
    CString SourcePath;
    m_EditSourcePath.GetWindowTextW(SourcePath);
    return SourcePath.GetString();
}

void FileDecryptDlg::Stop()
{
    m_SimpleCrypt->SetIsForceStop(true);
    for (std::vector<std::shared_ptr<SimpleEncrypt>>::size_type index = 0; index < m_vecSimpleEncrypt.size(); index++)
    {
        m_vecSimpleEncrypt[index]->SetIsForceStop(true);
    }
}

void FileDecryptDlg::ProcessWork(void* Parent)
{
    CEncryptDecryptDlg* ParentDlg = (CEncryptDecryptDlg*)Parent;
    const std::wstring& TargetPath = GetTargetPath();

    std::vector<std::wstring> FileNameVector;
    ParentDlg->GetFiles(FileNameVector);

    //是否有文件
    if (FileNameVector.size() == 0)
    {
        MessageBox(_T("No file to decrypt!"), _T("Error Tip"), MB_OK);
        return;
    }

    //单线程处理
    const int THREAD_COUNT = GetThreadCount(static_cast<int>(FileNameVector.size()));
    {
        if (THREAD_COUNT <= 1 || FileNameVector.size() == 1)
        {
            ParentDlg->UpdateResultList(1, FileNameVector[0], PT_PROCEING, std::wstring());
            bool ProcessResult = m_SimpleCrypt->Decrypt(FileNameVector[0], TargetPath);
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
            std::thread newThread(&FileDecryptDlg::MultiThreadWork, this, Parent, index, std::ref(TargetPath)); //特别注意，参数是引用！！！
            m_vecThread.push_back(std::move(newThread));
        }

        //等待结束
        for (std::vector<std::thread>::size_type index = 0; index < m_vecThread.size(); index++)
        {
            m_vecThread[index].join();
        }
    }
}

bool FileDecryptDlg::Validate()
{
    return theApp.Validate(m_EditSourcePath);
}

bool FileDecryptDlg::SetOption()
{
    CEncryptDecryptDlg* ParentDlg = (CEncryptDecryptDlg*)GetParent();
    m_SimpleCrypt->SetIsForceStop(false);
    m_SimpleCrypt->SetIsDeleteOriginalFile(true);

    CString InputPassword;
    m_EditInputPassword.GetWindowTextW(InputPassword);

    m_InputPassword = InputPassword.GetString();
    m_SimpleCrypt->SetPassword(InputPassword.GetString());
    return true;
}

void FileDecryptDlg::MultiThreadWork(void* Parent, int ThreadIndex, const std::wstring& TargetPath)
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
            bool ProcessResult = LocalSimpleEncrypt->Decrypt(Task.m_FileName, TargetPath);
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

void FileDecryptDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_SOURCE_PATH, m_EditSourcePath);
    DDX_Control(pDX, IDC_EDIT_INPUT_PASSWORD, m_EditInputPassword);
}

BEGIN_MESSAGE_MAP(FileDecryptDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &FileDecryptDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &FileDecryptDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON1, &FileDecryptDlg::OnBnClickedButtonSelectSourcePath)
END_MESSAGE_MAP()


// FileDecryptDlg message handlers
void FileDecryptDlg::OnBnClickedButtonSelectSourcePath()
{
    const std::wstring& SelectPath = theApp.GetSelectPath();
    if (SelectPath.empty())
    {
        //Add log
        return;
    }

    m_EditSourcePath.SetWindowTextW(SelectPath.c_str());
}

void FileDecryptDlg::OnBnClickedCancel()
{
    CEncryptDecryptDlg* ParentDlg = (CEncryptDecryptDlg*)GetParent();
    ParentDlg->ResetOperationType();
    CDialogEx::OnCancel();
}

void FileDecryptDlg::OnBnClickedOk()
{
    if (Validate() && SetOption())
    {
        CEncryptDecryptDlg* ParentDlg = (CEncryptDecryptDlg*)GetParent();
        ParentDlg->SetOperationType(OT_DECRYPT);
        ParentDlg->Start();
        CDialogEx::OnOK();
    }
}
