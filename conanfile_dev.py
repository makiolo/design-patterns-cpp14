from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake
from conan.tools.files import copy, get, download
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
    version = "1.0.25"
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
    
    def export(self):
        """Export phase - copy CMakeLists for metacommon download"""
        pass
    
    def requirements(self):
        # No external dependencies - both this library and metacommon are header-only
        # metacommon is downloaded during source() step
        pass
    
    def build_requirements(self):
        self.test_requires("gtest/1.14.0")
    
    def layout(self):
        cmake_layout(self)
    
    def source(self):
        """Source step - download metacommon headers"""
        # Create metacommon directory in source
        metacommon_dir = os.path.join(self.source_folder, "include", "metacommon")
        os.makedirs(metacommon_dir, exist_ok=True)
        
        # Download common.h from metacommon
        common_h_path = os.path.join(metacommon_dir, "common.h")
        if not os.path.exists(common_h_path):
            self.output.info("Downloading metacommon/common.h...")
            download(self, 
                    "https://raw.githubusercontent.com/makiolo/metacommon/master/common.h",
                    common_h_path)
    
    def package(self):
        # Copy this library's headers
        copy(self, "*.h", src=os.path.join(self.source_folder, "include"),
             dst=os.path.join(self.package_folder, "include"), keep_path=True)
        
        # Copy metacommon headers if they exist
        metacommon_src = os.path.join(self.source_folder, "include", "metacommon")
        if os.path.exists(metacommon_src):
            copy(self, "*.h", src=metacommon_src,
                 dst=os.path.join(self.package_folder, "include", "metacommon"), keep_path=False)
    
    def package_info(self):
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.set_property("cmake_find_mode", "both")
        self.cpp_info.set_property("cmake_file_name", "design-patterns-cpp14")
        self.cpp_info.set_property("cmake_target_name", "design-patterns-cpp14::design-patterns-cpp14")
