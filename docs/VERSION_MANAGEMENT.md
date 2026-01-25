# Version Management

The project version is centralized in a single file to avoid duplication across multiple files.

## 📍 Version Location

**Version File**: `VERSION` (in project root)

Current version: `1.0.24`

## How to Update Version

### Step 1: Update VERSION File

Edit `VERSION` file and change the version number:

```bash
# Option 1: Edit manually
vim VERSION
# Change 1.0.24 to 1.0.25

# Option 2: Using sed
sed -i 's/1.0.24/1.0.25/' VERSION
```

### Step 2: Create a Git Tag

```bash
# Create annotated tag
git tag -a v1.0.25 -m "Release version 1.0.25"

# Or lightweight tag
git tag v1.0.25
```

### Step 3: Push to GitHub

```bash
git push origin v1.0.25
```

This will automatically trigger GitHub Actions to:
- Create the Conan package with the new version
- Upload to GitHub Packages
- Generate release notes

## Where Version is Used

The version is automatically read from `VERSION` file by:

- ✅ `conanfile.py` - Main package recipe
- ✅ `conanfile_dev.py` - Development recipe
- ✅ GitHub Actions - publish-conan.yml extracts from git tags
- ✅ Other references - For documentation consistency

## Version Format

Use semantic versioning: `MAJOR.MINOR.PATCH`

Examples:
- `1.0.0` - Initial release
- `1.0.1` - Patch/bugfix
- `1.1.0` - New features
- `2.0.0` - Breaking changes

## Documentation

Documentation files contain version references in examples. These are updated manually during releases:

- `docs/GITHUB_PACKAGES.md`
- `docs/GITHUB_ACTIONS_SETUP.md`
- `docs/DISTRIBUTION.md`
- `examples/consumer_project/conanfile.txt`

When releasing a new version, find and replace the old version in these files with the new one.

## Automation Script

Optional: Create a script to update version everywhere:

```bash
#!/bin/bash
# update_version.sh

OLD_VERSION="1.0.24"
NEW_VERSION="1.0.25"

# Update VERSION file
echo "$NEW_VERSION" > VERSION

# Update documentation
sed -i "s/$OLD_VERSION/$NEW_VERSION/g" docs/GITHUB_PACKAGES.md
sed -i "s/$OLD_VERSION/$NEW_VERSION/g" docs/GITHUB_ACTIONS_SETUP.md
sed -i "s/$OLD_VERSION/$NEW_VERSION/g" docs/DISTRIBUTION.md
sed -i "s/$OLD_VERSION/$NEW_VERSION/g" examples/consumer_project/conanfile.txt

# Git operations
git add VERSION docs/ examples/
git commit -m "Bump version to $NEW_VERSION"
git tag -a "v$NEW_VERSION" -m "Release version $NEW_VERSION"
git push origin master
git push origin "v$NEW_VERSION"
```

Usage:
```bash
chmod +x update_version.sh
./update_version.sh
```

## Quick Reference

| File | Read Version From |
|------|-------------------|
| `conanfile.py` | `VERSION` (automatic) |
| `conanfile_dev.py` | `VERSION` (automatic) |
| `docs/*.md` | Manual (reference in examples) |
| `examples/consumer_project/conanfile.txt` | Manual (in requires) |

---

**Note**: The `VERSION` file is the single source of truth for the package version.
