@echo off

pushd %~dp0
cd %~dp0

mkdir %~dp0\..\out

mkdir %~dp0\..\out\Debug
xcopy %~dp0\Debug\lib %~dp0\..\out\Debug /y
xcopy %~dp0\Debug\*.pdb %~dp0\..\out\Debug\. /y

mkdir %~dp0\..\out\Release
xcopy %~dp0\Release\lib %~dp0\..\out\Release /y
xcopy %~dp0\Release\*.pdb %~dp0\..\out\Release\. /y

popd
pause