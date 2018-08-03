#pragma once

#ifndef QING_DLL
#ifdef  QING_EXPORTS
#define QING_DLL        __declspec(dllexport)
#else
#define QING_DLL        __declspec(dllimport)
#endif
#endif



//namespace indication
#ifndef QING_NAMESPACE_BEGIN
#define QING_NAMESPACE_BEGIN namespace Qing {
#endif

#ifndef QING_NAMESPACE_END
#define QING_NAMESPACE_END   }
#endif