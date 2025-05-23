@echo off
setlocal

REM Set directories
set SRC_DIR=zlib-1.3.1
set OUT_DIR=build

REM Create output directory if it doesn't exist
if not exist %OUT_DIR% (
    mkdir %OUT_DIR%
)

REM List of source files
set FILES=adler32 compress crc32 deflate trees zutil inflate infback inftrees inffast uncompr

REM Compile each file
for %%F in (%FILES%) do (
    echo Compiling %%F.c ...
    clang -O3 -c %SRC_DIR%\%%F.c -I%SRC_DIR% -o %OUT_DIR%\%%F.obj
)

echo Done.
