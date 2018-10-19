#include "stdafx.h"
#include "ToolEnum.h"
#include "ToolLiteProfile.h"
#include "..\..\Qing\HeaderFiles\CommonFunction.h"



static const std::wstring EncryptSelectPathKey(L"EncryptSelectPathKey");
static const std::wstring EncryptTargetPathKey(L"EncryptTargetPathKey");



ToolLiteProfile::ToolLiteProfile()
{
    LoadConfig(Qing::GetRunningPath() + L"ToolLiteConfig.ini");
}

ToolLiteProfile::~ToolLiteProfile()
{
}

bool ToolLiteProfile::SaveConfig()
{
    const std::wstring &ENCRYPTION = GetSectionName(WTE_ENCRYPTION);

    m_INIFile.WriteString(ENCRYPTION.c_str(), EncryptSelectPathKey.c_str(), m_EncryptSelectPath.c_str());
    m_INIFile.WriteString(ENCRYPTION.c_str(), EncryptTargetPathKey.c_str(), m_EncryptTargetPath.c_str());

    return true;
}

bool ToolLiteProfile::LoadConfig(const std::wstring &ConfigFile)
{
    if (!m_INIFile.Open(ConfigFile) || !m_INIFile.LoadData())
    {
        return false;
    }

    const std::wstring &ENCRYPTION = GetSectionName(WTE_ENCRYPTION);
    m_EncryptSelectPath = m_INIFile.GetString(ENCRYPTION.c_str(), EncryptSelectPathKey.c_str(), std::wstring().c_str());
    m_EncryptTargetPath = m_INIFile.GetString(ENCRYPTION.c_str(), EncryptTargetPathKey.c_str(), std::wstring().c_str());

    return true;
}
