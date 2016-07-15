from conans import ConanFile, CMake

class VCardConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "Boost/1.60.0@lasote/stable"
    generators = "cmake"
    options = {"build": [True, False]}
    default_options = "build=True", "Boost:header_only=False", "Boost:shared=False", "zlib:shared=True"

    def build(self):
        cmake = CMake(self.settings)
        self.run('cmake "%s" %s' % (self.conanfile_directory, cmake.command_line))
        if self.build:
            self.run('cmake --build . %s' % cmake.build_config)

