#!/bin/bash
# Quick Start Guide - Conan Integration

cat << 'EOF'

╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║         🚀 Design Patterns C++14 - Conan Integration Complete 🚀            ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝

📋 QUICK START COMMANDS
═════════════════════════════════════════════════════════════════════════════

🔧 PARA DESARROLLADORES (Este Proyecto)
─────────────────────────────────────────────────────────────────────────────

1️⃣  Crear el paquete Conan:
    $ ./create_package.sh
    
    O manualmente:
    $ conan create . --build=missing

2️⃣  Construir y probar:
    $ ./build.sh

3️⃣  Ver el paquete creado:
    $ conan list "design-patterns*"

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

👥 PARA CONSUMIDORES (Otros Proyectos)
─────────────────────────────────────────────────────────────────────────────

1️⃣  Opción A - Usar conanfile.txt (Recomendado para proyectos simples):

    En tu proyecto, crea: conanfile.txt
    ───────────────────────────────────────────────────────────────
    [requires]
    design-patterns-cpp14/1.0.24
    boost/1.86.0

    [generators]
    CMakeDeps
    CMakeToolchain
    ───────────────────────────────────────────────────────────────

2️⃣  Opción B - Usar conanfile.py (Para proyectos complejos):

    En tu proyecto, crea: conanfile.py
    ───────────────────────────────────────────────────────────────
    from conan import ConanFile

    class MyApp(ConanFile):
        requires = "design-patterns-cpp14/1.0.24"
        generators = "CMakeDeps", "CMakeToolchain"
    ───────────────────────────────────────────────────────────────

3️⃣  Instalar dependencias:
    $ mkdir build && cd build
    $ conan install .. --build=missing

4️⃣  Construir con CMake:
    $ cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
    $ cmake --build .

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

📝 INTEGRACIÓN EN CMakeLists.txt
─────────────────────────────────────────────────────────────────────────────

cmake_minimum_required(VERSION 3.15)
project(MyProject CXX)

set(CMAKE_CXX_STANDARD 14)

# Encontrar el paquete
find_package(design-patterns-cpp14 REQUIRED CONFIG)

# Crear executable
add_executable(my_app main.cpp)

# Linkar con el target
target_link_libraries(my_app design-patterns-cpp14::design-patterns-cpp14)

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

📚 DOCUMENTACIÓN
─────────────────────────────────────────────────────────────────────────────

📄 CONAN.md
   └─ Guía completa de uso y configuración

📄 CONAN_IMPLEMENTATION.md
   └─ Detalles técnicos de la implementación

📄 SETUP_COMPLETE.md
   └─ Resumen final y próximos pasos

📄 CAMBIOS_REALIZADOS.md
   └─ Lista de todos los cambios realizados

📁 examples/
   └─ Ejemplos prácticos de uso

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

🔍 COMANDOS ÚTILES
─────────────────────────────────────────────────────────────────────────────

# Ver información del paquete
$ conan inspect .

# Crear paquete con perfil específico
$ conan create . -pr=release

# Instalar solo dependencias (sin crear paquete)
$ conan install . --build=missing

# Limpiar paquetes locales
$ conan remove "design-patterns-cpp14*"

# Ver dependencias transitivas
$ conan graph info . --format=html

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

✨ CARACTERÍSTICAS
─────────────────────────────────────────────────────────────────────────────

✅ Header-Only Library     - Sin binarios compilados
✅ Transitive Dependencies - Boost incluido automáticamente
✅ Conan 2.0 Compatible   - API moderna
✅ CMake Integration      - find_package() automático
✅ Cross-Platform         - Linux, Windows, macOS
✅ C++14 Required         - Compilador moderno

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

🎯 FLUJO TÍPICO
─────────────────────────────────────────────────────────────────────────────

DESARROLLADOR (Este Proyecto)          CONSUMIDOR (Otro Proyecto)
    │                                          │
    ├─ ./create_package.sh                    ├─ mkdir proyecto
    │  (crea paquete Conan)                   ├─ cd proyecto
    │                                         ├─ echo "[requires]
    ├─ conan list "..."                       │   design-patterns-cpp14/1.0.24
    │  (verifica creación)                    │   [generators]
    │                                         │   CMakeDeps
    └─ Paquete listo en Conan                 │   CMakeToolchain" > conanfile.txt
                                             ├─ mkdir build && cd build
                                             ├─ conan install .. --build=missing
                                             ├─ cmake .. -DCMAKE_TOOLCHAIN_FILE=...
                                             └─ cmake --build .

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

🔗 RECURSOS
─────────────────────────────────────────────────────────────────────────────

Conan Official:        https://docs.conan.io/
Header-Only Packages:  https://docs.conan.io/2/tutorials/package_binary_model/header_only.html
CMakeDeps Generator:   https://docs.conan.io/2/reference/conanfile/tools/cmake/cmakedeps.html

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

💬 PREGUNTAS FRECUENTES
─────────────────────────────────────────────────────────────────────────────

P: ¿Necesito tener Boost instalado?
R: No, Conan lo instala automáticamente.

P: ¿Puedo usar con Visual Studio?
R: Sí, genera archivos compatibles con MSVC.

P: ¿Puedo publicar en Conan Center?
R: Sí, sigue el proceso de Conan Center.

P: ¿Soporta Windows/macOS?
R: Sí, está optimizado para múltiples plataformas.

P: ¿Qué versión mínima de C++?
R: C++14 es el mínimo requerido.

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

✨ ¡Ahora tienes Conan totalmente integrado! ✨

EOF
