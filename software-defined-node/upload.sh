#!/bin/bash

#upload: all
#	$(ESP_TOOL) $(UPLOAD_VERB) -cd $(UPLOAD_RESET) -cb $(UPLOAD_SPEED) -cp $(UPLOAD_PORT) -ca 0x00000 -cf $(MAIN_EXE)
#
#ota: all
#	$(OTA_TOOL) -i $(ESP_ADDR) -p $(ESP_PORT) -a $(ESP_PWD) -f $(MAIN_EXE)

set -e

TOOL_ROOT="/home/trilader/.arduino15/packages/esp8266"
ESP_TOOL="$TOOL_ROOT/tools/esptool/0.4.8/esptool"
OTA_TOOL="$TOOL_ROOT/hardware/esp8266/2.3.0-rc2/tools/espota.py"

function usage()
{
    echo "Usage: $0 serial <PORT> [BAUD]"
    echo "       $0 ota <NAME> [PASS] [-p PORT]"
    exit 1
}

if [ "$#" -lt 1 ]; then
    usage
fi

if [ "$1" == "serial" ]; then
    shift
    PORT="$1"
    BAUD="${2:-115200}"
    $ESP_TOOL -cd nodemcu -cp $PORT -cb $BAUD -ca 0x0000 -cf build/app.bin
elif [ "$1" == "ota" ]; then
    shift
    ARGS=""
    [ -n "$2" ] && ARGS="$ARGS -a $2"
    if [ "$3" == "-p" ]; then
        ARGS="$ARGS -p $4"
    else
        ARGS="$ARGS -p 8266"
    fi
    python $OTA_TOOL -d -i $(avahi-resolve-host-name -4 "${1}.local" | cut -f2) $ARGS -f build/app.bin
else 
    usage
fi
