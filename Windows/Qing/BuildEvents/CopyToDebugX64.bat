@echo delete old files
del ..\Deployment\Debug\x64\ClientLiteD\Qing64D.dll
del ..\Deployment\Debug\x64\QingTestD\Qing64D.dll
del ..\Deployment\Debug\x64\ServerLiteD\Qing64D.dll
del ..\Deployment\Debug\x64\ToolLiteD\Qing64D.dll



@echo copy files
copy ..\Deployment\Debug\x64\QingD\Qing64D.dll ..\Deployment\Debug\x64\ClientLiteD\
copy ..\Deployment\Debug\x64\QingD\Qing64D.dll ..\Deployment\Debug\x64\QingTestD\
copy ..\Deployment\Debug\x64\QingD\Qing64D.dll ..\Deployment\Debug\x64\ServerLiteD\
copy ..\Deployment\Debug\x64\QingD\Qing64D.dll ..\Deployment\Debug\x64\ToolLiteD\