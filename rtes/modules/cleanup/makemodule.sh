#!/bin/bash/
cd ../../../
make M=rtes/modules/cleanup/
sleep 2
cd -
#adb push cleanup.ko /data
#adb shell
