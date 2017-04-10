from conans import ConanFile, CMake

class DesignPatternsConan(ConanFile):
    name = "design-patterns-cpp14"
    version = "1.0.0"
    url = "https://github.com/makiolo/design-patterns-cpp14.git"
    license = "MIT"
    settings = "os", "compiler", "build_type", "arch"
    options = {"build_shared_libs":   [True, False],
               "build_dp14_tests":    [True, False],
               "build_dp14_coverage": [True, False]}
    default_options = "build_shared_libs=False", "build_dp14_tests=False", "build_dp14_coverage=False"
    requires = ()
    generators = "cmake"
    exports = ["*"]

    def configure(self):
        if self.options.build_dp14_coverage:
            self.options.build_dp14_tests=True

    def requirements(self):
        if self.options.build_dp14_tests:
            self.requires.add("gtest/1.7.0@lasote/stable", private=False)
            self.options["gtest"].shared = False
        else:
            if "gtest" in self.requires:
                del self.requires["gtest"]

    def build(self):
        cmake = CMake(self.settings)
        build_shared_libs = "-DBUILD_SHARED_LIBS=ON" if self.options.build_shared_libs else "-DBUILD_SHARED_LIBS=OFF"
        build_dp14_tests = "-DBUILD_LUNA_TESTS=ON" if self.options.build_dp14_tests else "-DBUILD_DP14_TESTS=OFF"
        build_dp14_coverage = "-DBUILD_LUNA_COVERAGE=ON" if self.options.build_dp14_coverage else "-DBUILD_DP14_COVERAGE=OFF"
        self.run('cmake %s %s %s "%s" %s' % (build_shared_libs, build_dp14_tests, build_dp14_coverage, self.conanfile_directory, cmake.command_line))
        self.run('cmake --build . %s' % cmake.build_config)

    def package(self):
        # self.copy("*.h", dst="include/dp14", src="dp14")
        # self.copy("*.lib", dst="lib", src="lib")
        # self.copy("*.a", dst="lib", src="lib")
        self.copy("*.h", dst="include/dp14", src="dp14")
        self.copy("*.hh", dst="include/dp14", src="dp14")
        self.copy("*.h", dst="include/dp14", src="dp14/metacommon")

    def package_info(self):
        # self.cpp_info.libs = ["dp14"]
        self.cpp_info.libs = []
