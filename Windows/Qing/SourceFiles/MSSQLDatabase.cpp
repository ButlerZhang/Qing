#include "..\HeaderFiles\MSSQLDatabase.h"

QING_NAMESPACE_BEGIN



MSSQLDataSet::MSSQLDataSet()
{
    m_RecordsetPtr = NULL;
}

MSSQLDataSet::~MSSQLDataSet()
{
    Close();
}

bool MSSQLDataSet::Open(_ConnectionPtr ConnectionPtr, const wchar_t * QueryStr)
{
    if (m_RecordsetPtr != NULL)
    {
        Close();
    }

    HRESULT HrValue = m_RecordsetPtr.CreateInstance("ADODB.Recordset");
    if (FAILED(HrValue))
    {
        m_RecordsetPtr = NULL;
        return false;
    }

    m_RecordsetPtr->CursorLocation = adUseClient;
    HrValue = m_RecordsetPtr->Open(_variant_t(QueryStr), (_variant_t)((IDispatch*)ConnectionPtr), adOpenDynamic, adLockReadOnly, adCmdText);//adLockOptimistic
    if (FAILED(HrValue))
    {
        return false;
    }

    if (!m_RecordsetPtr->adoEOF)
    {
        m_RecordsetPtr->MoveFirst();
    }

    return true;
}

void MSSQLDataSet::Close()
{
    if (m_RecordsetPtr != NULL)
    {
        m_RecordsetPtr->Close();
        m_RecordsetPtr.Release();
        m_RecordsetPtr = 0;
    }
}

bool MSSQLDataSet::MoveNext()
{
    if (m_RecordsetPtr != NULL)
    {
        HRESULT HrValue = m_RecordsetPtr->MoveNext();
        if (SUCCEEDED(HrValue))
        {
            return !(m_RecordsetPtr->adoEOF);
        }
    }

    return false;
}

bool MSSQLDataSet::GetValue(const std::wstring & FieldName, std::wstring &Data) const
{
    if (m_RecordsetPtr == NULL)
    {
        return false;
    }

    _variant_t Value = m_RecordsetPtr->Fields->GetItem(_variant_t(FieldName.c_str()))->Value;
    Data = (wchar_t *)((_bstr_t)Value);
    return true;
}



MSSQLDatabase::MSSQLDatabase() : Database()
{
    ::CoInitialize(0);
    m_ConnectionObject = 0;
}

MSSQLDatabase::~MSSQLDatabase()
{
    Disconnect();
    ::CoUninitialize();
}

bool MSSQLDatabase::Connect(const wchar_t * Host, const wchar_t * User, const wchar_t * Passwd, const wchar_t * DB, unsigned int Port, const wchar_t * CharSet, int TimeoutDays)
{
    Disconnect();

    HRESULT HrValue = m_ConnectionObject.CreateInstance(__uuidof(adoConnection));
    if (FAILED(HrValue))
    {
        m_ConnectionObject = 0;
        return false;
    }

    wchar_t ConnectionStr[_MAX_DIR];
    _snwprintf_s(ConnectionStr, _MAX_DIR - 1, L"Provider=SQLOLEDB.1;Persist Security Info=True;User ID=%s;Password=%s;Initial Catalog=%s;Data Source=%s", User, Passwd, DB, Host);
    HrValue = m_ConnectionObject->Open(ConnectionStr, "", "", adModeUnknown);
    if (FAILED(HrValue))
    {
        return false;
    }


    m_Isconnected = true;

    m_ConnectionInfo.m_DBName = DB;
    m_ConnectionInfo.m_DBHost = Host;
    m_ConnectionInfo.m_DBUser = User;
    m_ConnectionInfo.m_DBPassword = Passwd;
    m_ConnectionInfo.m_TimeoutDays = TimeoutDays;
    if (CharSet != 0)
    {
        m_ConnectionInfo.m_DBCharset = CharSet;
    }

    m_ConnectionInfo.m_DBPort = Port;
    return true;
}

void MSSQLDatabase::Disconnect()
{
    if (m_ConnectionObject != 0 && m_Isconnected)
    {
        m_ConnectionObject->Close();
        m_ConnectionObject.Release();
        m_Isconnected = false;
    }
}

bool MSSQLDatabase::Isconnected()
{
    return m_Isconnected;
}

bool MSSQLDatabase::Reconnect()
{
    if (m_ConnectionInfo.IsValid())
    {
        Disconnect();
        return Connect(m_ConnectionInfo.m_DBHost.c_str(),
            m_ConnectionInfo.m_DBUser.c_str(),
            m_ConnectionInfo.m_DBPassword.c_str(),
            m_ConnectionInfo.m_DBName.c_str(),
            m_ConnectionInfo.m_DBPort,
            m_ConnectionInfo.m_DBCharset.c_str(),
            m_ConnectionInfo.m_TimeoutDays);
    }

    return false;
}

bool MSSQLDatabase::ExecuteQuery(const wchar_t * QueryStr, DatabaseDataSet * MyDataSet)
{
    if (!m_Isconnected || QueryStr == 0)
    {
        return false;
    }

    MSSQLDataSet *DataSet = (MSSQLDataSet*)MyDataSet;
    if (MyDataSet == NULL)
    {
        m_ConnectionObject->Execute(_bstr_t(QueryStr), NULL, adCmdText | adExecuteNoRecords);
        return true;
    }
    else
    {
        return DataSet->Open(m_ConnectionObject, QueryStr);
    }
}

QING_NAMESPACE_END