#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-or-later
#
#    Copyright (C) 2021  The Home Control Authors
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
    $ESP_TOOL -cd nodemcu -cp "$PORT" -cb "$BAUD" -ca 0x0000 -cf "$FILE"
elif [ "$1" == "ota" ]; then
    shift
    HOSTNAME="$1"

    if [[ -f "upload_aliases.json" ]]; then
        MAYBE_HOSTNAME=$(python3 -c '
import json, sys
with open("upload_aliases.json", "r") as f:
    d = json.load(f)
    print(d.get(sys.argv[1], "null"))
' "$HOSTNAME")
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
    IP=$(python3 -c '
import ipaddress, socket, sys
try:
    print(str(ipaddress.ip_address(sys.argv[1])))
except ValueError:
    try:
        print(socket.gethostbyname(sys.argv[1]))
    except socket.gaierror:
        print(sys.argv[1])
' "$HOSTNAME")
    # shellcheck disable=SC2086
    echo python3 "$OTA_TOOL" -d -i "$IP" $ARGS -f "$FILE"
    python3 "$OTA_TOOL" -d -i "$IP" $ARGS -f "$FILE"
else
    usage
fi
