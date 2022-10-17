#!/bin/bash
gdb-multiarch -ex "file ./kernel8.elf" -ex "target remote 0:1234"
