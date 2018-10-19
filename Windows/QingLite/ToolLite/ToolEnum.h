#pragma once

#include <string>



enum WindowsToolEnum
{
    WTE_ENCRYPTION = 0,
    WTE_BOOT_START_UP,

    WTE_COUNT,
};

const std::wstring GetToolName(WindowsToolEnum ToolEnum);
const std::wstring GetSectionName(WindowsToolEnum ToolEnum);
