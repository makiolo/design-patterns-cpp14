# Distribution without a Server

## TL;DR (La Respuesta Corta)

**No, no necesitas subir a ningún servidor.** Los usuarios simplemente hacen:

```bash
git clone https://github.com/makiolo/design-patterns-cpp14.git
cd design-patterns-cpp14
conan create . --user=makiolo --channel=stable
```

Y listo. El paquete está en su cache local de Conan. Fin.

---

## Las 4 Formas de Distribuir sin Servidor

### 1. **Git Clone + Conan Create (Más Simple)**

**Lo que hace el usuario:**
```bash
git clone https://github.com/makiolo/design-patterns-cpp14.git
cd design-patterns-cpp14
conan create . --user=makiolo --channel=stable
```

**En su proyecto (`conanfile.txt`):**
```ini
[requires]
design-patterns-cpp14/1.0.24@makiolo/stable
```

✅ Pros: Ultra simple, sin servidor, siempre última versión  
❌ Cons: Manual para cada desarrollador

---

### 2. **Script One-Liner (Para los Vagos)**

Creas un script que ellos corren una sola vez:

```bash
#!/bin/bash
git clone https://github.com/makiolo/design-patterns-cpp14.git /tmp/dp14 && \
cd /tmp/dp14 && \
conan create . --user=makiolo --channel=stable && \
rm -rf /tmp/dp14
```

O con curl:
```bash
curl https://raw.githubusercontent.com/makiolo/design-patterns-cpp14/master/install.sh | bash
```

✅ Pros: Una línea, automático  
❌ Cons: Security (ejecutar scripts desde internet)

---

### 3. **GitHub Releases + Artifact**

Cuando haces un release en GitHub, subes el conanfile.py como artifact.

**Los usuarios descargan y hacen:**
```bash
# Descargan design-patterns-cpp14-1.0.24.tar.gz desde Releases
tar -xzf design-patterns-cpp14-1.0.24.tar.gz
cd design-patterns-cpp14-1.0.24
conan create . --user=makiolo --channel=stable
```

✅ Pros: Versiones pinned, claro qué versión es  
❌ Cons: Más trabajo para ti

---

### 4. **GitHub Packages (Gratis, pero requiere GitHub Account)**

Si tus usuarios tienen cuenta de GitHub:

```bash
# Configuran el remote una sola vez
conan remote add github https://maven.pkg.github.com/makiolo/design-patterns-cpp14

# Luego simplemente
conan install . 
```

Requiere que subas el paquete:
```bash
conan create . --user=makiolo --channel=stable
conan upload design-patterns-cpp14/* -r github -c
```

✅ Pros: Rápido después de configurar, integrando con GitHub  
❌ Cons: Requiere token de GitHub, requiere que subes

---

## Comparación

| Método | Servidor | Setup Usuario | Ventajas | Desventajas |
|--------|----------|---------------|----------|------------|
| **Git Clone** | ❌ No | 3 comandos | Simple, siempre actualizado | Manual |
| **One-liner** | ❌ No | 1 comando | Ultra simple | Riesgo seguridad |
| **Releases** | ❌ No | Descargar + 2 cmd | Versiones claras | Más trabajo |
| **GitHub Pkg** | ✅ GitHub | 1 setup + conan | Profesional | Token requerido |
| **Conan Center** | ✅ Sí | 1 línea | Estándar oficial | Requiere publicar |

---

## Mi Recomendación para Ti

### Ahora (Corto Plazo):
Usa **Git Clone**. Documentación clara:

```markdown
## Installation

```bash
git clone https://github.com/makiolo/design-patterns-cpp14.git
cd design-patterns-cpp14
conan create . --user=makiolo --channel=stable
```
```

### Después (Si Crece):
Si muchos usuarios lo piden, usa **GitHub Packages**:

1. Habilita GitHub Packages en tu repo
2. Cada vez que haces release:
   ```bash
   conan create . --user=makiolo --channel=stable
   conan upload design-patterns-cpp14/* -r github -c
   ```

### Futuro (Si es Muy Popular):
Publica en **Conan Center** (pero no es obligatorio):
```bash
# Sigue el proceso de Conan Center
# Los usuarios simplemente hacen:
# [requires]
# design-patterns-cpp14/1.0.24
```

---

## Quick Start para Usuarios

**Opción A - Git (Sin configuración previa):**
```bash
git clone https://github.com/makiolo/design-patterns-cpp14.git
cd design-patterns-cpp14
conan create . --user=makiolo --channel=stable
```

**Opción B - En su proyecto (después de Opción A):**
```ini
[requires]
design-patterns-cpp14/1.0.24@makiolo/stable
```

---

## Resumen Final

**Respuesta a tu pregunta:**

> "¿No podrían depender de algo así como `makiolo/design-patterns-cpp14` y descargarlo de GitHub directamente?"

✅ **SÍ, exactamente eso es lo que hacen.** Ellos clonan tu repo, hacen `conan create`, y listo. El paquete está en su cache local bajo el nombre `design-patterns-cpp14/1.0.24@makiolo/stable`.

No necesitas subir a ningún servidor. **GitHub es tu servidor.**

---

## El Flujo Completo (Visual)

```
Tu Repositorio (GitHub)
    ↓
[Usuario 1 clona]  [Usuario 2 clona]  [Usuario 3 clona]
    ↓                    ↓                    ↓
conan create .    conan create .     conan create .
    ↓                    ↓                    ↓
Cache Local 1     Cache Local 2      Cache Local 3
(design-patterns-cpp14/1.0.24@makiolo/stable)

Cada uno lo usa en sus proyectos:
[requires]
design-patterns-cpp14/1.0.24@makiolo/stable
```

**¡Ningún servidor intermedio necesario!**

---

## Comandos de Referencia

```bash
# Usuario: Crear el paquete localmente
git clone https://github.com/makiolo/design-patterns-cpp14.git
cd design-patterns-cpp14
conan create . --user=makiolo --channel=stable

# Verificar que está creado
conan list "design-patterns-cpp14*"

# Usar en su proyecto
mkdir myproject && cd myproject
echo "[requires]
design-patterns-cpp14/1.0.24@makiolo/stable

[generators]
CMakeDeps
CMakeToolchain" > conanfile.txt

mkdir build && cd build
conan install .. --build=missing
```

---

**¡Así que la respuesta es: NO necesitas subir a ningún lado!** 🎉
