#pragma once

#include "..\..\Qing\HeaderFiles\IniFile.h"
#include <string>



class ToolLiteProfile
{
public:

    ToolLiteProfile();
    ~ToolLiteProfile();

    bool SaveConfig();
    bool LoadConfig(const std::wstring &ConfigFile);

public:

    std::wstring             m_EncryptSelectPath;
    std::wstring             m_EncryptTargetPath;

private:

    Qing::IniFile            m_INIFile;
    std::wstring             m_ConfigFile;
};
