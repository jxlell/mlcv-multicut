#!/bin/bash

mkdir -p build

SRC_DIR="zlib-1.3.1"
OUT_DIR="build"

FILES=(
    adler32
    compress
    crc32
    deflate
    trees
    zutil
    inflate
    infback
    inftrees
    inffast
    uncompr
)

for file in "${FILES[@]}"; do
    clang -O3 -c "$SRC_DIR/$file.c" -I"$SRC_DIR" -o "$OUT_DIR/$file.o"
done
