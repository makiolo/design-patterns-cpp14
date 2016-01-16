import os
import sys
import contextlib
import logging
from conans import tools
from conans import ConanFile, CMake

def trymkdir(directory):
    directory = str(directory)
    if not os.path.exists( directory ):
        os.makedirs( directory )

@contextlib.contextmanager
def working_directory(path):
    path = str(path)
    prev_cwd = os.getcwd()
    os.chdir(path)
    try:
        yield
    finally:
        os.chdir(prev_cwd)

class DesignPatternsDP14Conan(ConanFile):
    name = "design-patterns-dp14"
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    requires = ""
    generators = "cmake"
    default_options = ""

    def source(self):
        self.run("git clone --recursive https://github.com/makiolo/design-patterns-cpp14.git dp14")

    def build(self):
        cmake = CMake(self.settings)
        self.run('cd dp14 && cmake . %s' % (cmake.command_line))
        self.run('cd dp14 && cmake --build . %s' % cmake.build_config)

    def package(self):
        self.copy("*.h", dst="include/dp14", src="dp14")
        self.copy("*.hh", dst="include/dp14", src="dp14")
        self.copy("*.h", dst="include/dp14", src="dp14/metacommon")

    def package_info(self):
        self.cpp_info.libs = []

    def test(self):
        self.run(os.sep.join([".","bin", "test_factory_dynamic"]))
        self.run(os.sep.join([".","bin", "test_factory_static"]))
        self.run(os.sep.join([".","bin", "test_memoize_dynamic"]))
        self.run(os.sep.join([".","bin", "test_memoize_static"]))
        self.run(os.sep.join([".","bin", "test_factory_linking"]))

