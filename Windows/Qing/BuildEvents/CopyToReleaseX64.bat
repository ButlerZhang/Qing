@echo delete old files
del ..\Deployment\Release\x64\ClientLite\Qing64.dll
del ..\Deployment\Release\x64\QingTest\Qing64.dll
del ..\Deployment\Release\x64\ServerLite\Qing64.dll
del ..\Deployment\Release\x64\ToolLite\Qing64.dll



@echo copy files
copy ..\Deployment\Release\x64\Qing\Qing64.dll ..\Deployment\Release\x64\ClientLite\
copy ..\Deployment\Release\x64\Qing\Qing64.dll ..\Deployment\Release\x64\QingTest\
copy ..\Deployment\Release\x64\Qing\Qing64.dll ..\Deployment\Release\x64\ServerLite\
copy ..\Deployment\Release\x64\Qing\Qing64.dll ..\Deployment\Release\x64\ToolLite\