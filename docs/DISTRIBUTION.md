# Distribution Methods - How to Use design-patterns-cpp14

## TL;DR

Users have **3 simple options** to use this library:

1. **Clone from Git** - `git clone + conan create`
2. **From GitHub Releases** - Download package from releases page
3. **Direct from Git Repo** - Conan can pull directly from git

**No server required. GitHub is your server.** 🚀

---

## The 3 Ways to Install

### Option 1: Clone + Create (Most Direct)

**What users do:**
```bash
git clone https://github.com/makiolo/design-patterns-cpp14.git
cd design-patterns-cpp14
conan create .
```

**In their project's `conanfile.txt`:**
```ini
[requires]
design-patterns-cpp14/1.0.24

[generators]
CMakeDeps
CMakeToolchain
```

✅ **Pros:** 
- No external server needed
- Works offline after clone
- Always get latest code

❌ **Cons:**
- Manual clone each time
- Requires git

---

### Option 2: GitHub Releases (Versioned Releases)

When you push a tag, GitHub Actions automatically creates a release with installation instructions.

**What users do:**
1. Visit: https://github.com/makiolo/design-patterns-cpp14/releases
2. Download the source from a release
3. Extract and use:

```bash
tar -xzf design-patterns-cpp14-1.0.24.tar.gz
cd design-patterns-cpp14-1.0.24
conan create .
```

Or directly in `conanfile.txt`:
```ini
[requires]
design-patterns-cpp14/1.0.24
```

✅ **Pros:**
- Clear version history on releases page
- Reproducible builds
- No need to clone the whole repo

❌ **Cons:**
- Manual download step
- Less convenient than package manager

---

### Option 3: Direct from Git URL (Recommended)

Conan can pull recipes directly from git repositories!

**In their `conanfile.txt`:**
```ini
[requires]
design-patterns-cpp14/1.0.24@

[generators]
CMakeDeps
CMakeToolchain
```

**Note:** The `@` at the end tells Conan to use git remote.

**Their build process:**
```bash
mkdir build && cd build
conan install .. --build=missing
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
cmake --build .
```

✅ **Pros:**
- No download needed
- Works with any Conan version
- Latest features automatically
- Simplest for users

❌ **Cons:**
- Requires git
- Can be slower than cached packages

---

## Comparison Table

| Method | Server | User Setup | Best For |
|--------|--------|-----------|----------|
| **Git Clone** | ❌ No | 3 commands | Developers wanting full control |
| **Releases** | ❌ No | Download + 1 cmd | Version pinning |
| **Git URL** | ❌ No | Add to conanfile | Most users |
| **GitHub Packages** | ⚠️ Complex | 1 setup + 1 cmd | Not recommended (complex setup) |
| **Conan Center** | ✅ Yes | 1 line | Public library standard |

---

## The Complete Workflow (Visual)

```
Your Repository on GitHub (master branch + version tags)
        ↓
  [You create release with tag v1.0.24]
        ↓
GitHub Actions automatically creates Release page
        ↓
┌─────────────────────────────────────────────┐
│  User Can Choose:                           │
│                                             │
│  A) Clone repo                              │
│     $ git clone https://...                 │
│     $ conan create .                        │
│                                             │
│  B) Download from releases page             │
│     $ tar -xzf design-patterns-cpp14.tar.gz │
│     $ conan create .                        │
│                                             │
│  C) Reference in their conanfile.txt        │
│     [requires]                              │
│     design-patterns-cpp14/1.0.24@           │
│                                             │
└─────────────────────────────────────────────┘
        ↓
    Their Cache
    (design-patterns-cpp14/1.0.24)
        ↓
    Their Project Uses It
```

---

## Recommended Setup Flow

### For You (Maintainer):

1. **Make changes** to code in `master` branch
2. **Test locally:**
   ```bash
   conan create .
   ```
3. **Update VERSION file** with new version
4. **Run release script:**
   ```bash
   ./release.sh
   ```
   This automatically:
   - Commits VERSION change
   - Creates git tag `v1.0.24`
   - Pushes to GitHub
   - GitHub Actions creates Release page

### For Users:

Users pick their preferred method (A, B, or C above) and follow those instructions.

---

## Quick Reference: Commands

### User: From Git Repository
```bash
# Clone
git clone https://github.com/makiolo/design-patterns-cpp14.git
cd design-patterns-cpp14

# Create package locally
conan create .

# Verify it's installed
conan list design-patterns-cpp14
```

### User: In Their Project

**Using `conanfile.txt`:**
```ini
[requires]
design-patterns-cpp14/1.0.24@

[generators]
CMakeDeps
CMakeToolchain
```

**Using `conanfile.py`:**
```python
from conan import ConanFile

class MyProject(ConanFile):
    requires = "design-patterns-cpp14/1.0.24@"
    generators = "CMakeDeps", "CMakeToolchain"
```

**Install dependencies:**
```bash
mkdir build && cd build
conan install .. --build=missing
```

---

## Future: Publish to Conan Center

If this library becomes popular and you want to make it even easier:

```bash
# Standard way (no @ needed)
[requires]
design-patterns-cpp14/1.0.24
```

But this is **optional**. The current setup works perfectly fine without it.

---

## Why Not GitHub Packages?

GitHub Packages is designed for Maven/npm/Docker packages, not native Conan repositories. It requires complex authentication and doesn't provide the same workflow as:

- **Conan Center** (built for Conan)
- **JFrog Artifactory** (full Conan support)
- **Git-based distribution** (simplest for open-source)

**For this project, Git distribution is optimal.**

---

## Summary

| What | Command |
|------|---------|
| **I want to publish a release** | `./release.sh` then answer prompts |
| **Users want to install** | Clone + `conan create` OR use git URL |
| **Users want specific version** | Download from `/releases` page |
| **I want to check GitHub Actions** | Go to `/actions` in repo |

**No external server needed. GitHub is your package manager.** ✅
