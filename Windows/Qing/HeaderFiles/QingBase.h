#pragma once

//warning C4251: STL template class needs to have dll-interface to be used by clients of class User-define class
#pragma warning(disable:4251)



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



//namespace indication
#ifndef QING_TEMPLATE_BEGIN
#define QING_TEMPLATE_BEGIN namespace QingTemplate {
#endif

#ifndef QING_TEMPLATE_END
#define QING_TEMPLATE_END   }
#endif