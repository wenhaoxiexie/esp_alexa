#!/bin/bash
ls /dev/ttyUSB*
echo $IDF_PATH
cp /home/lin/esp/alexa/esp-alexa/examples/amazon_alexa/.bashrc ~
source ~/.bashrc
cp /home/lin/esp/alexa/esp-alexa/examples/amazon_alexa/build/bootloader/*bin /mnt/hgfs/Share/fw/source/
cp /home/lin/esp/alexa/esp-alexa/examples/amazon_alexa/build/*.bin /mnt/hgfs/Share/fw/source/
