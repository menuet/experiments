
import os, conans

class ExperimentsConan(conans.ConanFile):
    name = "experiments"
    version = "1.0"
    license = "<Put the package license here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of Toto here>"
    settings = "os", "compiler", "build_type", "arch"
    options = {"use_boost_filesystem": [True, False]}
    default_options = "use_boost_filesystem=False"
    generators = "cmake"

    def requirements(self):
        self.requires("catch2/2.1.0@bincrafters/stable")
        self.requires("fmt/4.1.0@bincrafters/stable")
        if self.settings.os == "Windows":
            self.requires("sfml/2.4.2@bincrafters/testing")
        self.requires("boost_optional/1.66.0@bincrafters/stable")
        if self.options.use_boost_filesystem:
            self.requires("boost_filesystem/1.66.0@bincrafters/testing")

    def configure(self):
        pass

    def build(self):
        cmake = conans.CMake(self)
        cmake.definitions["CMAKE_CONFIGURATION_TYPES"] = str(self.settings.build_type)
        cmake.definitions["CMAKE_VERBOSE_MAKEFILE"] = "TRUE"
        cmake.definitions["EXP_USE_BOOST_FILESYSTEM"] = str(self.options.use_boost_filesystem).upper()
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.dll", "bin", "bin")
