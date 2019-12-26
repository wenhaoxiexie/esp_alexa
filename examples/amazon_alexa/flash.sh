#make flash
if [ $1 == ""]
then
	echo "please input a cert!"
else

python $IDF_PATH/components/esptool_py/esptool/esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0x10000  $1

#make monitor

fi
