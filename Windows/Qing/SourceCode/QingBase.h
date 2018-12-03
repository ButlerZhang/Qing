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
#define QING_NAMESPACE_BEGIN namespace Qing {           //Tool Class
#endif

#ifndef QING_NAMESPACE_END
#define QING_NAMESPACE_END   }
#endif



//namespace indication
#ifndef QING_EXAMPLE_BEGIN
#define QING_EXAMPLE_BEGIN namespace QingExample {    //Example
#endif

#ifndef QING_EXAMPLE_END
#define QING_EXAMPLE_END   }
#endif
