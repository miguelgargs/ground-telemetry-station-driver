#!/bin/bash

# Script de build usando Clang y Ninja

set -e # Salir si hay algún error

echo "🔧 Configurando proyecto con CMake (usando Clang)..."
cmake --preset clang

echo "🔨 Compilando con Ninja..."
ninja -C build

echo "✅ Build completado exitosamente!"
