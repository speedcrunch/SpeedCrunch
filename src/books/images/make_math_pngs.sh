#!/bin/bash

SIZE="150"
PICS=(
    "cube"
    "sphere"
    "cone"
    "cylinder"
    "circle"
    "right_triangle"
    "sector"
)

for PIC in "${PICS[@]}"; do
    inkscape --without-gui --file="$PIC.svgz" --export-png="$PIC.png" \
        --export-width=$SIZE --export-height=$SIZE
done
