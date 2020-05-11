@echo off

@setlocal

set start=%time%

set cl_flags=/Gm- /W1 /MP /MTd /Z7 /Od /EHsc /nologo
set link_flags=/out:game.exe /incremental:no /debug:full /nologo
set src_files=w:\all_files.cpp
REM set src_files=W:\*.cpp
set lib_files=w:\lib\sdl2.lib w:\lib\sdl2_image.lib w:\lib\glew32sd.lib user32.lib gdi32.lib opengl32.lib winmm.lib shell32.lib

pushd w:\build
cl %cl_flags% %src_files% %lib_files% /link %link_flags% 
popd

set end=%time%
set options="tokens=1-4 delims=:.,"
for /f %options% %%a in ("%start%") do set start_h=%%a&set /a start_m=100%%b %% 100&set /a start_s=100%%c %% 100&set /a start_ms=100%%d %% 100
for /f %options% %%a in ("%end%") do set end_h=%%a&set /a end_m=100%%b %% 100&set /a end_s=100%%c %% 100&set /a end_ms=100%%d %% 100

set /a hours=%end_h%-%start_h%
set /a mins=%end_m%-%start_m%
set /a secs=%end_s%-%start_s%
set /a ms=%end_ms%-%start_ms%
if %ms% lss 0 set /a secs = %secs% - 1 & set /a ms = 100%ms%
if %secs% lss 0 set /a mins = %mins% - 1 & set /a secs = 60%secs%
if %mins% lss 0 set /a hours = %hours% - 1 & set /a mins = 60%mins%
if %hours% lss 0 set /a hours = 24%hours%
if 1%ms% lss 100 set ms=0%ms%

set /a totalsecs = %hours%*3600 + %mins%*60 + %secs%
echo build_debug completed in: %totalsecs%.%ms%s