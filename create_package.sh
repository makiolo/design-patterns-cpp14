#!/bin/bash
# Script para crear y probar el paquete Conan

set -e

echo "=== Design Patterns C++14 - Conan Package Builder ==="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_step() {
    echo -e "${YELLOW}>>> $1${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

# Check if conan is installed
if ! command -v conan &> /dev/null; then
    print_error "Conan is not installed. Please install it first: pip install conan"
    exit 1
fi

print_step "Checking Conan version..."
conan --version

print_step "Creating Conan package..."
conan create . --build=missing

print_success "Package created successfully!"

print_step "Listing created packages..."
conan list "design-patterns-cpp14*"

print_success "All done! You can now use this package in other projects."
echo ""
echo "Usage in other projects:"
echo "  [requires]"
echo "  design-patterns-cpp14/1.0.24"
echo ""
