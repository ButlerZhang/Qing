#pragma once

#ifndef QING_DLL
#ifdef  QING_EXPORTS
#define QING_DLL        __declspec(dllexport)
#else
#define QING_DLL        __declspec(dllimport)
#endif
#endif