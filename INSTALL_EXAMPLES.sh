#!/bin/bash
# Ejemplo: Cómo un usuario instala design-patterns-cpp14 desde GitHub

cat << 'EOF'

╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║      Instalando design-patterns-cpp14 Directamente desde GitHub             ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝

📦 OPCIÓN 1: Instalación Rápida (Recomendada)
═════════════════════════════════════════════════════════════════════════════

$ git clone https://github.com/makiolo/design-patterns-cpp14.git
$ cd design-patterns-cpp14
$ conan create . --user=makiolo --channel=stable

✅ El paquete está en tu cache local de Conan
✅ Puedes usarlo en otros proyectos

───────────────────────────────────────────────────────────────────────────────

Ahora en tu proyecto:

$ mkdir my_project && cd my_project

Crea: conanfile.txt
──────────────────
[requires]
design-patterns-cpp14/1.0.24@makiolo/stable

[generators]
CMakeDeps
CMakeToolchain

───────────────────────────────────────────────────────────────────────────────

$ mkdir build && cd build
$ conan install ..
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
$ cmake --build .

═════════════════════════════════════════════════════════════════════════════

📦 OPCIÓN 2: Sin clonar (Script One-liner)
═════════════════════════════════════════════════════════════════════════════

# Simplemente clona, crea el paquete e instala en tu proyecto

#!/bin/bash
set -e

REPO_URL="https://github.com/makiolo/design-patterns-cpp14.git"
TEMP_DIR=$(mktemp -d)

echo "Clonando $REPO_URL..."
git clone "$REPO_URL" "$TEMP_DIR"

echo "Creando paquete Conan..."
cd "$TEMP_DIR"
conan create . --user=makiolo --channel=stable

rm -rf "$TEMP_DIR"
echo "✅ Paquete creado. Ya puedes usarlo en tu proyecto."

═════════════════════════════════════════════════════════════════════════════

📦 OPCIÓN 3: Usando en Conanfile.py (Más Control)
═════════════════════════════════════════════════════════════════════════════

from conan import ConanFile
from conan.tools.cmake import cmake_layout

class MyProjectConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "design-patterns-cpp14/1.0.24@makiolo/stable"
    generators = "CMakeDeps", "CMakeToolchain"
    
    def layout(self):
        cmake_layout(self)

$ conan install . --build=missing
$ cmake -B build -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
$ cmake --build build

═════════════════════════════════════════════════════════════════════════════

📦 OPCIÓN 4: Editable (Para Desarrollo Conjunto)
═════════════════════════════════════════════════════════════════════════════

Si estás modificando design-patterns-cpp14 y quieres que otro proyecto
vea los cambios inmediatamente sin hacer conan create cada vez:

# En el directorio de design-patterns-cpp14
$ conan editable add . design-patterns-cpp14/1.0.24@makiolo/stable

# En tu proyecto consumidor
$ conan install .
# Todos los cambios se reflejan inmediatamente

# Cuando termines
$ conan editable remove design-patterns-cpp14/1.0.24@makiolo/stable

═════════════════════════════════════════════════════════════════════════════

✨ ¡No necesitas subir a ningún servidor!

Solo comparte:
  https://github.com/makiolo/design-patterns-cpp14

Y otros harán:
  git clone https://github.com/makiolo/design-patterns-cpp14.git
  cd design-patterns-cpp14
  conan create . --user=makiolo --channel=stable

═════════════════════════════════════════════════════════════════════════════

EOF
