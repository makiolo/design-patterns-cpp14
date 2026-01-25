# Usando design-patterns-cpp14 Directamente desde GitHub

No necesitas subir a Conan Center. Hay varias formas de usar este paquete directamente desde GitHub.

## Opción 1: Desde Git (Recomendado)

Los consumidores pueden usar tu paquete directamente desde GitHub sin que tengas que subirlo a ningún servidor.

### En conanfile.txt:
```ini
[requires]
design-patterns-cpp14/1.0.24@makiolo/stable

[generators]
CMakeDeps
CMakeToolchain
```

### En conanfile.py:
```python
from conan import ConanFile

class MyApp(ConanFile):
    requires = "design-patterns-cpp14/1.0.24@makiolo/stable"
    generators = "CMakeDeps", "CMakeToolchain"
    
    def requirements(self):
        # Opcionalmente especificar la rama/commit
        pass
```

## Opción 2: Usar URL de Git directamente

Los consumidores pueden clonar y usar localmente:

```bash
git clone https://github.com/makiolo/design-patterns-cpp14.git
cd design-patterns-cpp14
conan create . --user=makiolo --channel=stable
```

Luego en su `conanfile.txt`:
```ini
[requires]
design-patterns-cpp14/1.0.24@makiolo/stable
```

## Opción 3: Configurar un Remote de Conan (Ideal para Equipos)

Si trabajas en equipo, configura un servidor Conan remoto. Las opciones son:

### A) **GitHub Packages** (Gratis con GitHub)
```bash
# En tu repositorio, habilitar GitHub Packages
# Luego los consumidores añaden el remote:
conan remote add github https://maven.pkg.github.com/makiolo/design-patterns-cpp14 -f

# Y luego
conan install . 
```

### B) **Artifactory** (JFrog - Free tier disponible)
- Crea cuenta en JFrog Artifactory
- Sube tu paquete
- Compartir URL del remote

### C) **GitLab / Gitea** (Si usas esos servidores)
- Tienen Conan registries integrados

## Opción 4: Referencia Git Directa (Conan 2.0+)

Para proyectos consumidores que quieren seguir cambios de Git:

```python
from conan import ConanFile
from conan.tools.scm import Version
import os

class MyProject(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    
    def requirements(self):
        # Usar directamente desde GitHub
        self.requires("design-patterns-cpp14/1.0.24@makiolo/stable")
        
    def imports(self):
        # También puedes usar el repo como editable
        pass
```

## Opción 5: Editable Packages (Para Desarrollo Local)

Si trabajas localmente con este proyecto:

```bash
# En tu proyecto consumidor
conan editable add . design-patterns-cpp14/1.0.24@makiolo/stable

# Ahora cualquier cambio se refleja inmediatamente
conan install .
```

## ¿Cuál es la Mejor Opción?

| Opción | Caso de Uso | Ventajas | Desventajas |
|--------|------------|----------|------------|
| **Git Clone Local** | Desarrollo personal | Simple, sin servidor | Manual para cada dev |
| **GitHub Packages** | Equipo en GitHub | Integrado con GitHub, gratis | Requiere auth token |
| **Artifactory** | Empresa/múltiples repos | Potente, caché | Requiere servidor |
| **Editable** | Desarrollo activo | Cambios inmediatos | Solo local |

## Recomendación para Ti

Como proyecto open-source sin dependencias:

1. **Corto plazo**: Los usuarios simplemente hacen:
   ```bash
   git clone https://github.com/makiolo/design-patterns-cpp14.git
   cd design-patterns-cpp14
   conan create . --user=makiolo --channel=stable
   ```

2. **Mediano plazo**: Si tienes cuenta en JFrog, usa Artifactory Free tier
   ```bash
   conan remote add artifactory https://artifactory.jfrog.io/artifactory/api/conan/...
   conan upload design-patterns-cpp14/* -r artifactory
   ```

3. **Largo plazo**: Si quieres publicar en Conan Center (opcional)

## Resumen: Lo que el Usuario Final Hace

**Sin servidor:**
```bash
git clone https://github.com/makiolo/design-patterns-cpp14.git && cd design-patterns-cpp14
conan create . --user=makiolo --channel=stable
# Listo, el paquete está en su cache local
```

**En su proyecto:**
```ini
[requires]
design-patterns-cpp14/1.0.24@makiolo/stable
```

¡Así de simple! **No necesitas subir a ningún lado.**
