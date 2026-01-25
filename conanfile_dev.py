from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake
from conan.tools.files import copy, get
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


class DesignPatternsDevConan(ConanFile):
    """Conan recipe for developing design-patterns-cpp14"""
    
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
    exports_sources = "VERSION", "include/*", "CMakeLists.txt", "tests/*"
    
    settings = "os", "compiler", "build_type", "arch"
    options = {}
    default_options = {}
    
    package_type = "header-library"
    no_copy_source = True
    
    def requirements(self):
        # No external dependencies - both this library and metacommon are header-only
        # metacommon is downloaded during source() step
        pass
    
    def build_requirements(self):
        self.test_requires("gtest/1.14.0")
    
    def layout(self):
        cmake_layout(self)
    
    def source(self):
        """Download metacommon from GitHub"""
        # Download metacommon header-only library from GitHub into a subdirectory
        get(self, "https://github.com/makiolo/metacommon/archive/refs/heads/master.zip",
            destination=os.path.join(self.source_folder, "metacommon_src"), strip_root=True)
    
    def package(self):
        # Copy this library's headers preserving directory structure
        copy(self, "*.h", src=os.path.join(self.source_folder, "include"),
             dst=os.path.join(self.package_folder, "include"), keep_path=True)
        
        # Copy metacommon headers from the downloaded source
        # The structure should be: metacommon_src/include/metacommon/
        metacommon_include = os.path.join(self.source_folder, "metacommon_src", "include")
        if os.path.exists(metacommon_include):
            # Copy everything from metacommon_src/include to package include
            # This preserves the metacommon/ subdirectory
            copy(self, "*", src=metacommon_include,
                 dst=os.path.join(self.package_folder, "include"), keep_path=True)
    
    def package_info(self):
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.set_property("cmake_find_mode", "both")
        self.cpp_info.set_property("cmake_file_name", "design-patterns-cpp14")
        self.cpp_info.set_property("cmake_target_name", "design-patterns-cpp14::design-patterns-cpp14")
