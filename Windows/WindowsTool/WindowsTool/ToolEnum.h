#pragma once



enum WindowsToolEnum
{
    WTE_ENCRYPTION = 0,
    WTE_BOOT_START_UP,

    WTE_COUNT,
};



const std::wstring GetToolName(WindowsToolEnum ToolEnum)
{
    switch (ToolEnum)
    {
    case WTE_ENCRYPTION:            return L"Encryption";
    case WTE_BOOT_START_UP:         return L"Boot Start Up";
    default:                        return L"Unknow";
    }
}