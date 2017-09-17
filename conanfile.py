
import os, conans

class ExperimentsConan(conans.ConanFile):
    name = "experiments"
    version = "1.0"
    license = "<Put the package license here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of Toto here>"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"

    def requirements(self):
        if self.settings.os != "Windows":
            # self.requires("Boost/1.64.0@inexorgame/stable")
            pass
        else:
            self.requires("sfml/2.4.1@pascal/testing")

    def configure(self):
        # self.settings["sfml"].shared = True
        pass

    def build(self):
        cmake = conans.CMake(self)
        this_file_directory = os.path.dirname(os.path.realpath(__file__))
        self.run("cmake -D CMAKE_VERBOSE_MAKEFILE=ON {} {}".format(cmake.command_line, this_file_directory))
        self.run("cmake --build . {}".format(cmake.build_config))

    def imports(self):
        self.copy("*.dll", "bin", "bin")
