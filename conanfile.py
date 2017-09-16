
import conans

class ExperimentsConan(conans.ConanFile):
    name = "experiments"
    version = "1.0"
    license = "<Put the package license here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of Toto here>"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    requires = ""

    def build(self):
        cmake = conans.CMake(self)
        self.run("cmake -D CMAKE_VERBOSE_MAKEFILE=ON {}/.. {}".format(self.source_folder, cmake.command_line))
        self.run("cmake --build . {}".format(cmake.build_config))
