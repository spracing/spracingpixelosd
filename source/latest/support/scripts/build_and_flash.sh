#!/usr/bin/env bash

# Author: Dominic Clifton

set -e

DIR="${BASH_SOURCE%/*}"
if [[ ! -d "$DIR" ]]; then DIR="$PWD"; fi
. support/scripts/config.sh
. support/scripts/utils.sh

TARGET=$1
OPTS=$2
DEBUG=N

usage() {
    MESSAGE=$1
    
    echo "<TARGET> [BUILD]"
    error "$MESSAGE"
}

if [ "$TARGET" = "" ]; then
    usage "Missing target"
    exit
fi

if [[ "$OPTS" == *"BUILD"* ]]; then
    BUILD=Y
else
    BUILD=N
fi

if  [[ "$TARGET" == *"SPRACINGH7RF"* ]]; then
    FLASH_RANGE=0x900de000:0x8000
fi

if  [[ "$TARGET" == *"SPRACINGH7CINE"* ]]; then
    FLASH_RANGE=0x97CA0000:0x8000
fi

if  [[ "$TARGET" == *"SPRACINGH7EF"* ]]; then
    FLASH_RANGE=0x900de000:0x8000
fi

if  [[ "$TARGET" == *"SPRACINGH7CL"* ]]; then
    FLASH_RANGE=0x900de000:0x8000
fi

MAKE_OPTS="-j"

if [ "$DEBUG" = "Y" ]; then
    MAKE_OPTS="$MAKE_OPTS DEBUG=1 DEVELOPER_BUILD=YES" 
fi

if [ "$BUILD" = "Y" ]; then
    make $MAKE_OPTS TARGET=${TARGET} clean
    make $MAKE_OPTS TARGET=${TARGET}
fi

BUILD_DIR=./build/

if [ "$DEBUG" = "Y" ]; then
    BIN=${BUILD_DIR}${TARGET}_DEVELOPER_DEBUG.bin
    VERIFY_BIN=${BUILD_DIR}${TARGET}_DEVELOPER_DEBUG.verify.bin
else
    BIN=${BUILD_DIR}${TARGET}_PRODUCTION_RELEASE.bin
    VERIFY_BIN=${BUILD_DIR}${TARGET}_PRODUCTION_RELEASE.verify.bin
fi

echo "*** $VERIFY_BIN"
if [ -f "$VERIFY_BIN" ]; then
    rm $VERIFY_BIN
fi


$DFU_UTIL -i 0 --alt 0 -s $FLASH_RANGE -D $BIN 
$DFU_UTIL -i 0 --alt 0 -s $FLASH_RANGE -U $VERIFY_BIN

diff -sb $BIN $VERIFY_BIN

sha1sum $BIN
