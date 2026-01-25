#!/bin/bash
# Quick reference for releasing

cat << 'EOF'

╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║           Publishing a New Release - Quick Reference                       ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝

🚀 QUICK RELEASE (Recommended)
═════════════════════════════════════════════════════════════════════════════

$ ./release.sh

This will:
  ✓ Ask for new version number
  ✓ Update VERSION file
  ✓ Commit changes
  ✓ Create git tag
  ✓ Push to GitHub
  ✓ Trigger GitHub Actions

═════════════════════════════════════════════════════════════════════════════

📋 MANUAL RELEASE (If you prefer to do it step by step)
═════════════════════════════════════════════════════════════════════════════

# 1. Update version
$ echo "1.0.25" > VERSION

# 2. Commit
$ git add VERSION
$ git commit -m "Bump version to 1.0.25"

# 3. Tag
$ git tag -a v1.0.25 -m "Release version 1.0.25"

# 4. Push (this triggers GitHub Actions!)
$ git push origin master
$ git push origin v1.0.25

═════════════════════════════════════════════════════════════════════════════

🔍 WHAT HAPPENS AUTOMATICALLY (GitHub Actions)
═════════════════════════════════════════════════════════════════════════════

After you push the tag (v1.0.25), GitHub automatically:

  1. Detects the new tag
     ↓
  2. Runs: .github/workflows/publish-conan.yml
     ↓
  3. Creates Conan package (version 1.0.25)
     ↓
  4. Uploads to GitHub Packages
     ↓
  5. Tests consumer can use it
     ↓
  6. Generates release notes
     ↓
  7. Available at: GitHub Packages + Releases

═════════════════════════════════════════════════════════════════════════════

📊 MONITOR YOUR RELEASE
═════════════════════════════════════════════════════════════════════════════

Action                          URL
────────────────────────────────────────────────────────────────────────────
View GitHub Actions progress    https://github.com/makiolo/design-patterns-cpp14/actions
View available packages         https://github.com/makiolo/design-patterns-cpp14/packages
View releases                   https://github.com/makiolo/design-patterns-cpp14/releases
View tags                       https://github.com/makiolo/design-patterns-cpp14/tags

═════════════════════════════════════════════════════════════════════════════

📝 VERSION FORMAT
═════════════════════════════════════════════════════════════════════════════

Use Semantic Versioning: MAJOR.MINOR.PATCH

Examples:
  1.0.0  →  Initial release
  1.0.1  →  Bug fix
  1.1.0  →  New features (backward compatible)
  2.0.0  →  Breaking changes

═════════════════════════════════════════════════════════════════════════════

⚡ FASTEST WAY (One-liner)
═════════════════════════════════════════════════════════════════════════════

$ VERSION="1.0.25" && \
  echo "$VERSION" > VERSION && \
  git add VERSION && \
  git commit -m "Bump version to $VERSION" && \
  git tag -a "v$VERSION" -m "Release version $VERSION" && \
  git push origin master && \
  git push origin "v$VERSION" && \
  echo "✓ Released v$VERSION!"

═════════════════════════════════════════════════════════════════════════════

❓ TROUBLESHOOTING
═════════════════════════════════════════════════════════════════════════════

Problem: "Tag already exists"
Solution:
  $ git tag -d v1.0.25
  $ git push origin --delete v1.0.25
  $ ./release.sh  # Try again

Problem: "GitHub Actions not running"
Check:
  • Go to: Actions tab in GitHub
  • Check for errors in publish-conan.yml workflow
  • Ensure workflow file exists: .github/workflows/publish-conan.yml

Problem: "Cannot push"
Check:
  • You have push access
  • SSH keys are configured
  • No local changes pending: git status

═════════════════════════════════════════════════════════════════════════════

📚 DOCUMENTATION
═════════════════════════════════════════════════════════════════════════════

Complete guide:          RELEASE.md
Version management:      docs/VERSION_MANAGEMENT.md
GitHub Actions setup:    docs/GITHUB_ACTIONS_SETUP.md

═════════════════════════════════════════════════════════════════════════════

✨ TL;DR

Just run:   ./release.sh

That's it! GitHub Actions handles the rest. 🚀

═════════════════════════════════════════════════════════════════════════════

EOF
