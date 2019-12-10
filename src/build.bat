@echo off

if not exist "../bin" mkdir "../bin"

SET COMMMON_FLAGS=-nologo -W4 -Zi -EHsc -Od

pushd "../bin"
cl %COMMMON_FLAGS% ../src/main.cpp -Femain
popd
