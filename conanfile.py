from conan import ConanFile
from conan.tools.files import copy, get
from conan.tools.cmake import cmake_layout
import os


def _read_version():
    """Read version from VERSION file for local development"""
    try:
        version_file = os.path.join(os.path.dirname(__file__), "VERSION")
        with open(version_file, "r") as f:
            return f.read().strip()
    except FileNotFoundError:
        # Fallback version if VERSION file is not available
        return "1.0.24"


class DesignPatternsCpp14Conan(ConanFile):
    """Conan package for design-patterns-cpp14 - A header-only C++14 design patterns library"""
    
    name = "design-patterns-cpp14"
    # Version is hardcoded for reliability; VERSION file is used in local development only
    version = "1.0.24"
    license = "CC-BY-4.0"
    author = "Ricardo Marmolejo García"
    url = "https://github.com/makiolo/design-patterns-cpp14"
    homepage = "https://github.com/makiolo/design-patterns-cpp14"
    description = "Header-only library with common design patterns implemented in C++11/14"
    topics = ("design-patterns", "header-only", "c++14", "factory", "memoize")
    
    # Export the VERSION file with the recipe
    exports_sources = "VERSION", "include/*", "CMakeLists.txt"
    
    # Settings (not needed for header-only, but kept for completeness)
    settings = "os", "compiler", "build_type", "arch"
    
    # No options for header-only library
    options = {}
    default_options = {}
    
    # This is a header-only library
    package_type = "header-library"
    no_copy_source = True
    
    def requirements(self):
        """Dependencies required by consumers of this package"""
        # No external dependencies - both this library and metacommon are header-only
        # metacommon is downloaded during source() step
        pass
    
    def build_requirements(self):
        """Dependencies needed only for building and testing this package"""
        self.test_requires("gtest/1.14.0")
    
    def layout(self):
        """Define the layout of the source and build directories"""
        cmake_layout(self)
    
    def source(self):
        """Download metacommon from GitHub"""
        # Download metacommon header-only library from GitHub
        # It's a separate repository but header-only, so we include it in the package
        get(self, "https://github.com/makiolo/metacommon/archive/refs/heads/master.zip",
            destination=self.source_folder, strip_root=True)
    
    def build(self):
        """Build step (not needed for header-only libraries)"""
        # Header-only library - no build step needed
        pass
    
    def package(self):
        """Copy header files to package"""
        # Copy this library's headers
        copy(self, "*.h", src=os.path.join(self.source_folder, "include"),
             dst=os.path.join(self.package_folder, "include"), keep_path=True)
        # Copy metacommon headers (they are also in include/ after extraction)
        copy(self, "**/*.h", src=os.path.join(self.source_folder, "include"),
             dst=os.path.join(self.package_folder, "include"), keep_path=True)
    
    def package_info(self):
        """Define what consumers of this package need to know"""
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
        self.cpp_info.set_property("cmake_find_mode", "both")
        
        # Make headers available
        self.cpp_info.includedirs = ["include"]
        
        # Require C++14 minimum
        self.cpp_info.set_property("cmake_minimum_version", "3.5")
        self.cpp_info.set_property("cmake_target_name", "design-patterns-cpp14::design-patterns-cpp14")
        
        # For CMake integration
        self.cpp_info.set_property("cmake_file_name", "design-patterns-cpp14")
        self.cpp_info.set_property("cmake_target_name", "design-patterns-cpp14::design-patterns-cpp14")

