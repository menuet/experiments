
import os, conans

class ExperimentsConan(conans.ConanFile):
    name = "experiments"
    version = "1.0"
    license = "<Put the package license here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of Toto here>"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package"

    def requirements(self):
        self.requires("Catch2/2.9.2@catchorg/stable")
        self.requires("fmt/5.3.0@bincrafters/stable")
        self.requires("boost/1.70.0@conan/stable")
        self.requires("bzip2/1.0.8@conan/stable")
        self.requires("jsonformoderncpp/3.6.1@vthiery/stable")
        # self.requires("sfml/2.5.0@bincrafters/stable")

    def configure(self):
        # self.options["sfml"].window = True
        # self.options["sfml"].graphics = True
        # self.options["sfml"].network = True
        # self.options["sfml"].audio = True
        pass

    def imports(self):
        self.copy("*.dll", "bin", "bin")
