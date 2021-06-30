#!/bin/bash

set -e
#TODO: use esptool.py
TOOL_ROOT="$HOME/.platformio/packages/framework-arduinoespressif8266/tools"
#ESP_TOOL="$TOOL_ROOT/esptool/esptool"
ESP_TOOL="$HOME/.platformio/packages/tool-esptool/esptool"
OTA_TOOL="$TOOL_ROOT/espota.py"

function usage()
{
    echo "Usage: $0 <BOARD> serial  <PORT> [BAUD]"
    echo "       $0 <BOARD> ota <NAME> [PASS] [-p PORT]"
    exit 1
}

if [ "$#" -lt 1 ]; then
    usage
fi

FILE=".pio/build/$1/firmware.bin"
if [ ! -e "$FILE" ]; then
    usage
else
    shift
fi

echo "Uploading file: $FILE"

if [ "$1" == "serial" ]; then
    shift
    PORT="${1:-/dev/ttyUSB0}"
    BAUD="${2:-115200}"
    $ESP_TOOL -cd nodemcu -cp $PORT -cb $BAUD -ca 0x0000 -cf "$FILE"
elif [ "$1" == "ota" ]; then
    shift
    HOSTNAME="$1"

    if [[ -n "$(which jq)" && -f "upload_aliases.json" ]]; then
        MAYBE_HOSTNAME=$(jq --raw-output ".\"${HOSTNAME}\"" upload_aliases.json)
        if [[ "$MAYBE_HOSTNAME" != "null" ]]; then
            echo "$HOSTNAME is aliased to $MAYBE_HOSTNAME"
            HOSTNAME=$MAYBE_HOSTNAME
        fi
    fi

    ARGS=""
    [ -n "$2" ] && ARGS="$ARGS -a $2"
    if [ "$3" == "-p" ]; then
        ARGS="$ARGS -p $4"
    else
        ARGS="$ARGS -p 8266"
    fi
#    python $OTA_TOOL -d -i $(avahi-resolve-host-name -4 "${1}.local" | cut -f2) $ARGS -f "$FILE"
     python $OTA_TOOL -d -i $(dig +short ${HOSTNAME})  $ARGS -f "$FILE"
#    python $OTA_TOOL -d -i 10.42.0.197 $ARGS -f "$FILE"
else
    usage
fi
