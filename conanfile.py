import os
from conans import ConanFile, tools

class NpmMasMas(ConanFile):
    name = "design-patterns-cpp14"
    version = "1.0.23"
    license = "Attribution 4.0 International"
    url = "https://github.com/makiolo/design-patterns-cpp14"
    description = "This fast event system allows calls between two interfaces decoupled (sync or async)"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": True}
    generators = "cmake"

    def configure(self):
        self.options["boost"].shared = True

    def requirements(self):
        self.requires('boost/1.70.0@conan/stable')
        self.requires('gtest/1.8.1@bincrafters/stable')

    def source(self):
        self.run("git clone {}".format(self.url))

    def build(self):
        self.run("cd {} && npm run setup && npm run compile && npm test".format(self.name))

    def package(self):
        self.copy("{}/include/*.h".format(self.name), dst=os.path.join('include', self.name), keep_path=False)
        self.copy("{}/bin/{}/*.lib".format(self.name, self.settings.build_type), dst="lib", keep_path=False)
        self.copy("{}/bin/{}/*.dll".format(self.name, self.settings.build_type), dst="bin", keep_path=False)
        self.copy("{}/bin/{}/*.so".format(self.name, self.settings.build_type), dst="lib", keep_path=False)
        self.copy("{}/bin/{}/*.dylib".format(self.name, self.settings.build_type), dst="lib", keep_path=False)
        self.copy("{}/bin/{}/*.a".format(self.name, self.settings.build_type), dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = [lib for lib in tools.collect_libs(self)]
        self.cpp_info.includedirs.append('include')

