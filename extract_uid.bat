REM Required files:
REM https://raw.githubusercontent.com/fireeye/sunburst_countermeasures/main/indicator_release/Indicator_Release_NBIs.csv
REM https://raw.githubusercontent.com/bambenek/research/main/sunburst/uniq-hostnames.txt
@echo off
REM Extract the domain names
del %TEMP%\domain_names_f.txt 2>NUL
for /f "tokens=3 delims=," %%a in ('findstr CNAME Indicator_Release_NBIs.csv') do echo %%a >> %TEMP%\domain_names_f.txt
%~dp0\flames_pub.exe < %TEMP%\domain_names_f.txt > %TEMP%\flames_pub_f.txt
%~dp0\flames_pub.exe < uniq-hostnames.txt | findstr domain > %TEMP%\flames_all.txt
for /f "tokens=4 delims= " %%a in ('findstr NextString %TEMP%\flames_pub_f.txt') do findstr %%a %TEMP%\flames_all.txt
del %TEMP%\domain_names_f.txt
del %TEMP%\flames_pub_f.txt
del %TEMP%\flames_all.txt
