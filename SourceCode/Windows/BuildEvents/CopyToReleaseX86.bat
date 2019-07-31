@echo delete old files
del ..\..\Deployment\Release\x86\ClientLite\Qing32.dll
del ..\..\Deployment\Release\x86\QingTest\Qing32.dll
del ..\..\Deployment\Release\x86\ServerLite\Qing32.dll
del ..\..\Deployment\Release\x86\EncryptDecrypt\Qing32.dll



@echo copy files
copy ..\..\Deployment\Release\x86\Qing\Qing32.dll ..\..\Deployment\Release\x86\ClientLite\
copy ..\..\Deployment\Release\x86\Qing\Qing32.dll ..\..\Deployment\Release\x86\QingTest\
copy ..\..\Deployment\Release\x86\Qing\Qing32.dll ..\..\Deployment\Release\x86\ServerLite\
copy ..\..\Deployment\Release\x86\Qing\Qing32.dll ..\..\Deployment\Release\x86\EncryptDecrypt\