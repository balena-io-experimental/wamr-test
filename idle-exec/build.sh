#!/bin/bash

CURR_DIR=$PWD
WAMR_DIR=${PWD}/../..
OUT_DIR=${PWD}/out
AS_DIR=$PWD/../idle

cd ${AS_DIR}
npm run asbuild:optimized
wamrc  -o ../idle-exec/idle.aot build/optimized.wasm
cd -


rm -rf ${OUT_DIR}
mkdir ${OUT_DIR}

xxd -i idle.aot src/idle.h

echo "#####################build project"
cd ${CURR_DIR}
mkdir -p cmake_build
cd cmake_build
cmake .. 
make
if [ $? != 0 ];then
    echo "BUILD_FAIL exit as $?\n"
    exit 2
fi

cp -a idle ${OUT_DIR}

echo -e "\n"

echo "#####################copy wasm app"


# cp -a ../extern.wasm ${OUT_DIR}

echo "####################done"
