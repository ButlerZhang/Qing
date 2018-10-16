@echo delete old files
del ..\Deployment\Debug\x86\ClientLiteD\Qing32D.dll
del ..\Deployment\Debug\x86\QingTestD\Qing32D.dll
del ..\Deployment\Debug\x86\ServerLiteD\Qing32D.dll
del ..\Deployment\Debug\x86\ToolLiteD\Qing32D.dll



@echo copy files
copy ..\Deployment\Debug\x86\QingD\Qing32D.dll ..\Deployment\Debug\x86\ClientLiteD\
copy ..\Deployment\Debug\x86\QingD\Qing32D.dll ..\Deployment\Debug\x86\QingTestD\
copy ..\Deployment\Debug\x86\QingD\Qing32D.dll ..\Deployment\Debug\x86\ServerLiteD\
copy ..\Deployment\Debug\x86\QingD\Qing32D.dll ..\Deployment\Debug\x86\ToolLiteD\