#!/bin/bash
# release.sh - Automated release script for design-patterns-cpp14

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_header() {
    echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║  Design Patterns C++14 - Release Manager                  ║${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
    echo ""
}

print_error() {
    echo -e "${RED}✗ ERROR: $1${NC}"
    exit 1
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_info() {
    echo -e "${YELLOW}→ $1${NC}"
}

# Get current version
CURRENT_VERSION=$(cat VERSION)

print_header
print_info "Current version: $CURRENT_VERSION"
echo ""

# Ask for new version
read -p "Enter new version (e.g., 1.0.25): " NEW_VERSION

# Validate version format
if ! [[ $NEW_VERSION =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    print_error "Invalid version format. Use semantic versioning (e.g., 1.0.25)"
fi

# Confirm
echo ""
print_info "Release: $CURRENT_VERSION → $NEW_VERSION"
read -p "Continue? (y/N) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    print_error "Release cancelled"
fi

echo ""
print_info "Step 1: Updating VERSION file..."
echo "$NEW_VERSION" > VERSION
print_success "VERSION updated to $NEW_VERSION"

echo ""
print_info "Step 2: Committing changes..."
git add VERSION
git commit -m "Bump version to $NEW_VERSION"
print_success "Changes committed"

echo ""
print_info "Step 3: Creating git tag..."
git tag -a "v$NEW_VERSION" -m "Release version $NEW_VERSION"
print_success "Tag created: v$NEW_VERSION"

echo ""
print_info "Step 4: Pushing to GitHub..."
git push origin master
git push origin "v$NEW_VERSION"
print_success "Pushed to GitHub"

echo ""
echo -e "${GREEN}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║  Release Completed Successfully! 🎉                        ║${NC}"
echo -e "${GREEN}╚════════════════════════════════════════════════════════════╝${NC}"
echo ""
echo -e "${BLUE}What happens next:${NC}"
echo "  • GitHub Actions detects the tag: v$NEW_VERSION"
echo "  • Creates Conan package automatically"
echo "  • Uploads to GitHub Packages"
echo "  • Generates release notes"
echo ""
echo -e "${BLUE}View progress:${NC}"
echo "  https://github.com/makiolo/design-patterns-cpp14/actions"
echo ""
echo -e "${BLUE}View package:${NC}"
echo "  https://github.com/makiolo/design-patterns-cpp14/packages"
echo ""
