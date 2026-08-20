#!/bin/bash
# Author: miguelgargs
# This file is the build script
# that calls cmake and ninja to build
# the project.


echo "[i] Configuring cmake..."
cmake --preset clang
if [ $? -ne 0 ]
then
    echo "[X] Error setting cmake preset. Check your CMake installation."
    exit 1

echo "[i] Building..."
ninja -C build
if [ $? -ne 0 ]
then
    echo "[X] Error building project with ninja. Check your ninja installation or your project definition."
    exit 1

echo "[✅] Project successfully built!"
