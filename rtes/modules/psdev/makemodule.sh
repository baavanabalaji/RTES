#!/bin/bash/
cd ../../../
make M=rtes/modules/psdev/
cd -
adb push psdev.ko /data
adb shell
