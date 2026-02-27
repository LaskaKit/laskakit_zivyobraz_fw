#!/bin/bash

DISPLAY_LIST_FILE="displays.txt"
ESPINKV3_RAW=$(awk 'NR>1 && $4=="RAW" && $5=="1" {print $3}' "$DISPLAY_LIST_FILE")
ESPINKV3_GXEPD=$(awk 'NR>1 && $4=="GXEPD" && $5=="1" {print $3}' "$DISPLAY_LIST_FILE")
ESPINKV2_RAW=$(awk 'NR>1 && $4=="RAW" && $6=="1" {print $3}' "$DISPLAY_LIST_FILE")
ESPINKV2_GXEPD=$(awk 'NR>1 && $4=="GXEPD" && $6=="1" {print $3}' "$DISPLAY_LIST_FILE")
uESPINKV1_RAW=$(awk 'NR>1 && $4=="RAW" && $7=="1" {print $3}' "$DISPLAY_LIST_FILE")
uESPINKV1_GXEPD=$(awk 'NR>1 && $4=="GXEPD" && $7=="1" {print $3}' "$DISPLAY_LIST_FILE")


function build() {
    local env="$1"
    shift
    for display in $@
    do
        echo
        echo
        echo "##################################################"
        echo "Building Zivyobraz for "$env" $display"
        echo "##################################################"
        echo
        DISPLAY_TYPE="DISPLAY_${display}" pio run -e "$env"
    done
}

build espink-v3 $ESPINKV3_RAW
build espink-v3-gxepd $ESPINKV3_GXEPD
build espink-v2 $ESPINKV2_RAW
build espink-v2-gxepd $ESPINKV2_GXEPD
build uespink-v1 $uESPINKV1_RAW
build uespink-v1-gxepd $uESPINKV1_GXEPD
