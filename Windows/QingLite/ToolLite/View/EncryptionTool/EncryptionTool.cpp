#include "stdafx.h"
#include "afxdialogex.h"
#include "EncryptionTool.h"
#include "..\..\WindowsTool.h"
#include "..\..\..\..\Qing\HeaderFiles\CommonFunction.h"
#include "..\..\..\..\Qing\HeaderFiles\FileManager.h"

#include <shlobj.h>



IMPLEMENT_DYNAMIC(EncryptionTool, CDialogEx)

EncryptionTool::EncryptionTool(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_DIALOG_ENCRYPTION, pParent)
{
}

EncryptionTool::~EncryptionTool()
{
}

void EncryptionTool::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_ENCRYPTION_PATH, m_EditSourcePath);
    DDX_Control(pDX, IDC_EDIT_ENCRYPTION_PATH2, m_EditTargetPath);
}

BEGIN_MESSAGE_MAP(EncryptionTool, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON3, &EncryptionTool::OnBnClickedButtonSelectSourcePath)
    ON_BN_CLICKED(IDC_BUTTON4, &EncryptionTool::OnBnClickedButtonSelectTargetPath)
    ON_BN_CLICKED(IDC_BUTTON1, &EncryptionTool::OnBnClickedButtonEncrypt)
    ON_BN_CLICKED(IDC_BUTTON2, &EncryptionTool::OnBnClickedButtonDecrypt)
END_MESSAGE_MAP()

void EncryptionTool::OnBnClickedButtonSelectSourcePath()
{
    std::wstring SelectPath = GetSelectPath();
    if (SelectPath.empty())
    {
        //Add log
        return;
    }

    DWORD Result = ::GetFileAttributes(SelectPath.c_str());
    if (INVALID_FILE_ATTRIBUTES == Result)
    {
        //Add log
        return;
    }

    m_EditSourcePath.SetWindowTextW(SelectPath.c_str());

    if (Result & FILE_ATTRIBUTE_DIRECTORY)
    {
        if (SelectPath[SelectPath.size()-1] == L'\\')
        {
            SelectPath += L"EncryptionTool\\";
        }
        else
        {
            SelectPath += L"\\EncryptionTool\\";
        }
    }
    else
    {
        std::wstring::size_type Index = SelectPath.find_last_of(L"\\");
        if (Index != std::wstring::npos)
        {
            SelectPath.replace(Index, SelectPath.size(), L"\\EncryptionTool\\");
        }
    }

    m_EditTargetPath.SetWindowTextW(SelectPath.c_str());
}

void EncryptionTool::OnBnClickedButtonSelectTargetPath()
{
    const std::wstring SelectPath = GetSelectPath();
    m_EditTargetPath.SetWindowTextW(SelectPath.c_str());
}

std::wstring EncryptionTool::GetSelectPath() const
{
    wchar_t ResultBuffer[MAX_PATH];
    wchar_t SelectPathBuffer[MAX_PATH];

    memset(ResultBuffer, 0, sizeof(ResultBuffer));
    memset(SelectPathBuffer, 0, sizeof(SelectPathBuffer));

    BROWSEINFO BrowseInfo;
    BrowseInfo.hwndOwner = m_hWnd;                                  //����������
    BrowseInfo.pidlRoot = NULL;                                     //��������Ŀ¼
    BrowseInfo.pszDisplayName = SelectPathBuffer;                   //����ѡ���·���Ļ�����
    BrowseInfo.lpszTitle = _T("Current Path");                      //�����Ĵ��ڵ�������ʾ
    BrowseInfo.ulFlags = BIF_BROWSEINCLUDEFILES;                    //���Ի�ȡĿ¼���ļ�
    BrowseInfo.lpfn = NULL;                                         //�ص�����
    BrowseInfo.lParam = 0;                                          //���ص������Ĳ���ָ��
    BrowseInfo.iImage = 0;                                          //��ѡ��Ŀ¼��ص�ͼ��
    ITEMIDLIST* pidl = ::SHBrowseForFolder(&BrowseInfo);            //��ʾ��������

    if (::SHGetPathFromIDList(pidl, ResultBuffer))                  //��ITEMIDLIST�еõ�Ŀ¼��������·��
    {
        return std::wstring(ResultBuffer);
    }

    return std::wstring();
}

void EncryptionTool::OnBnClickedButtonEncrypt()
{
    CString SourcePath;
    m_EditSourcePath.GetWindowTextW(SourcePath);
    if (SourcePath.GetLength() <= 0)
    {
        MessageBox(NULL, _T("Source path is empty!"), MB_OK);
        return;
    }

    CString TargetPath;
    m_EditTargetPath.GetWindowTextW(TargetPath);
    if (TargetPath.GetLength() <= 0)
    {
        MessageBox(NULL, _T("Target path is empty!"), MB_OK);
        return;
    }

    Qing::FileManager MyFileManager;
    if (!MyFileManager.IsDirectory((Qing::WStringToString(TargetPath.GetString())).c_str()))
    {
        MessageBox(NULL, _T("Target path is not directory!"), MB_OK);
        return;
    }

    long TotalSize = 0, MaxSize = 0;
    std::vector<std::string> FileNameVector;
    const std::string &SourcePathString = Qing::WStringToString(SourcePath.GetString());
    //MyFileManager.GetFileNameNonRecursion(SourcePathString, FileNameVector, TotalSize, MaxSize);
}

void EncryptionTool::OnBnClickedButtonDecrypt()
{
}
