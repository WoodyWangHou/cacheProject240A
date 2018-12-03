#!/usr/bin/env bash
FILENAME='hou_wang_a53241783.zip'
# rm $FILENAME
zip -X $FILENAME *.c *.h Makefile *.cpp
