#!/usr/bin/env python3
#! *-* coding: utf-8 *-*

import subprocess
import re
import sys

if __name__=="__main__":
    addr2line="/home/trilader/code/esp8266-sdk/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-addr2line"
    elf="/home/trilader/code/esp8266-git/software-defined-node/build/app.elf"
    
    regex = re.compile("(40[0-2][0-9a-f]{5}\\b)")
    
    addrs=[]
    for line in sys.stdin:
        matches = regex.findall(line)
        for match in matches:
            addrs.append(match)
    
    subprocess.run([addr2line,"-aipfC", "-e", elf, *addrs])