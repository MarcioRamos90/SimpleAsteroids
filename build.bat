@echo off

IF NOT EXIST ./build mkdir ./build
pushd build

set CommonLinkerFlags=user32.lib Gdi32.lib winmm.lib
set CommonCompilerFlags=-MTd -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4  -wd4067 -wd4701 -wd4703 -wd4459 -wd4838 -wd4267 -wd4456 -wd4091 -wd4201 -wd4100 -wd4189 -wd4505 -FC -Z7 
set CommonDynamicFlags=/DASTERIODS_DEVELOPMENT=1

call cl ../code/win32_asteroids.cpp %CommonCompilerFlags% %CommonDynamicFlags% /link user32.lib %CommonLinkerFlags%

popd