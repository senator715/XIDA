# XIDA
XIDA Is a tool to speed up the opening of binary files in IDA by utilizing the context menu and automatically computing the correct architecture mode to use

![](https://i.imgur.com/Of1aCVP.gif)

# How to use

1. Download the latest release or compile yourself
2. Create a batch file and run the following code below
3. Locate your IDA installation
4. Done

```
@echo off

net session >nul 2>&1
if %errorLevel% neq 0 (
	echo [-] RUN AS ADMIN
	pause
	exit
)

set /p IDA_DIR=[!] IDA Directory (Example: C:\Program Files\IDA75): 

:: Ensure that ida.exe is present
if not exist "%IDA_DIR%\ida.exe" (
	echo [-] %IDA_DIR%\ida.exe missing
	pause
	exit
)

copy "%~dp0\\XIDA.exe" "%IDA_DIR%\XIDA.exe"
call "%~dp0\\XIDA.exe" -ida "%IDA_DIR%" -install 1

pause
```
