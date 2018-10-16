#pragma once
#include "QingBase.h"
#include <string>

QING_NAMESPACE_BEGIN



class QING_DLL IniFile
{
public:

    IniFile();
    ~IniFile();

    bool Open(const std::wstring &FileName);

    bool LoadData();
    bool LoadData(void *DataBuffer, int BufferSize, const std::wstring &DefaultFileName = std::wstring());

    unsigned long GetIniFileSize() const { return m_FileSize; }
    const std::wstring& GetFileName() const { return m_FileName; }

    const wchar_t *GetString(const wchar_t *Section, const wchar_t *Key, const wchar_t *DefaultValue);
    int            GetInt(const wchar_t *Section, const wchar_t *Key, int DefaultValue);
    double         GetDouble(const wchar_t *Section, const wchar_t *Key, double DefaultValue);
    bool           GetBool(const wchar_t *Section, const wchar_t *Key, bool DefaultValue);

    bool           WriteString(const wchar_t *Section, const wchar_t *Key, const wchar_t *Value) const;
    bool           WriteInt(const wchar_t *Section, const wchar_t *Key, int Value) const;
    bool           WriteDouble(const wchar_t *Section, const wchar_t *Key, double Value) const;
    bool           WriteBool(const wchar_t *Section, const wchar_t *Key, bool Value) const;

private:

    unsigned long            m_FileSize;
    std::wstring             m_FileName;
    wchar_t                 *m_FileData;
};

QING_NAMESPACE_END