#!/usr/bin/env python

import subprocess, os, platform, argparse

def print_step(title):
    print()
    print("**************************************************")
    print("**** {}".format(title))

def get_default_os():
    return platform.system()

def get_default_arch():
    return "x86_64"

def get_default_build_type():
    return "Debug"

def configure(config):
    print_step("Configuring")
    config.this_dir = os.path.dirname(os.path.realpath(__file__))
    config.build_dir = os.path.join(config.this_dir, "build", "{}-{}-{}".format(config.os, config.arch, config.build_type))

    print("Os: {}".format(config.os))
    print("Arch: {}".format(config.arch))
    print("Build type: {}".format(config.build_type))
    print("Use boost filesystem: {}".format(config.use_boost_filesystem))
    print("This dir: {}".format(config.this_dir))
    print("Build dir: {}".format(config.build_dir))
    subprocess.check_call(["conan", "--version"])
    subprocess.check_call(["cmake", "--version"])
    subprocess.call(["conan", "remote", "add", "bincrafters", "https://api.bintray.com/conan/bincrafters/public-conan"])

def conan_install(config):
    print_step("Installing Third-parties")
    if not os.path.exists(config.build_dir):
        os.makedirs(config.build_dir)
    os.chdir(config.build_dir)
    subprocess.check_call(["conan", "install", "--build=missing", "-s", "os={}".format(config.os), "-s", "arch={}".format(config.arch), "-s", "build_type={}".format(config.build_type), "-o", "use_boost_filesystem={}".format(config.use_boost_filesystem), config.this_dir])

def conan_build(config):
    print_step("Building Project")
    os.chdir(config.build_dir)
    subprocess.check_call(["conan", "build", config.this_dir])

def cmake_test(config):
    print_step("Testing Project")
    os.chdir(config.build_dir)
    subprocess.check_call(["ctest"])

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--conan_install", help="install the third-parties", action="store_true")
    parser.add_argument("--conan_build", help="build the project", action="store_true")
    parser.add_argument("--cmake_test", help="test the project", action="store_true")
    parser.add_argument("--os", help="set the os", type=str, choices=["Linux", "Windows"], default=get_default_os())
    parser.add_argument("--arch", help="set the arch", type=str, choices=["x86", "x86_64"], default=get_default_arch())
    parser.add_argument("--build_type", help="set the build type", type=str, choices=["Debug", "Release"], default=get_default_build_type())
    parser.add_argument("--use_boost_filesystem", help="use boost filesystem instead of std filesystem", action="store_true", default=True)
    config = parser.parse_args()
    configure(config)
    if config.conan_install:
        conan_install(config)
    if config.conan_build:
        conan_build(config)
    if config.cmake_test:
        cmake_test(config)
