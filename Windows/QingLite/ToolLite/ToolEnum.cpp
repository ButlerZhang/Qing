#pragma once
#include "stdafx.h"
#include "ToolEnum.h"



const std::wstring GetToolName(WindowsToolEnum ToolEnum)
{
    switch (ToolEnum)
    {
    case WTE_ENCRYPTION:            return L"Encryption";
    case WTE_BOOT_START_UP:         return L"Boot Start Up";
    default:                        return L"Unknow";
    }
}

const std::wstring GetSectionName(WindowsToolEnum ToolEnum)
{
    switch (ToolEnum)
    {
    case WTE_ENCRYPTION:            return L"Encryption";
    case WTE_BOOT_START_UP:         return L"BootStartUp";
    default:                        return L"Unknow";
    }
}
