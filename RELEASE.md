# Publishing a New Release

Quick reference for publishing a new release to GitHub Packages.

## Automated Way (Recommended) 🚀

```bash
# Run the release script
chmod +x release.sh
./release.sh

# Follow the prompts:
# 1. Enter new version
# 2. Confirm
# 3. Script does everything automatically:
#    - Updates VERSION file
#    - Commits changes
#    - Creates git tag
#    - Pushes to GitHub
```

The script will:
- ✅ Update `VERSION` file
- ✅ Create git commit
- ✅ Create annotated tag
- ✅ Push to GitHub
- ✅ Trigger GitHub Actions automatically

## Manual Way (If you prefer)

```bash
# 1. Update VERSION file
echo "1.0.25" > VERSION

# 2. Commit changes
git add VERSION
git commit -m "Bump version to 1.0.25"

# 3. Create tag
git tag -a v1.0.25 -m "Release version 1.0.25"

# 4. Push to GitHub
git push origin master
git push origin v1.0.25
```

## What Happens After Push

GitHub Actions automatically:

1. **Detects** the new tag (e.g., `v1.0.25`)
2. **Creates** Conan package with that version
3. **Uploads** to GitHub Packages
4. **Tests** that consumers can use it
5. **Generates** release notes

## Monitor the Release

### Option 1: GitHub Actions
View live progress:
```
https://github.com/makiolo/design-patterns-cpp14/actions
```

### Option 2: GitHub Packages
View available versions:
```
https://github.com/makiolo/design-patterns-cpp14/packages
```

### Option 3: Releases
View releases and download:
```
https://github.com/makiolo/design-patterns-cpp14/releases
```

## Version Format

Use **Semantic Versioning**: `MAJOR.MINOR.PATCH`

Examples:
- `1.0.0` - Initial release
- `1.0.1` - Bug fix
- `1.1.0` - New features
- `2.0.0` - Breaking changes

## Troubleshooting

### "Tag already exists"
```bash
# Delete local tag
git tag -d v1.0.25

# Delete remote tag
git push origin --delete v1.0.25

# Try again
./release.sh
```

### "Cannot push - permission denied"
- Ensure you have push access to the repo
- Check SSH keys or GitHub token

### "GitHub Actions not running"
- Check if `.github/workflows/publish-conan.yml` exists
- View Actions tab for errors

## Checklist Before Release

- [ ] All tests pass locally
- [ ] Changes committed and merged to master
- [ ] VERSION file is correct
- [ ] Documentation is updated
- [ ] Ready for release

Then run:
```bash
./release.sh
```

---

**See also**: [docs/VERSION_MANAGEMENT.md](docs/VERSION_MANAGEMENT.md)
