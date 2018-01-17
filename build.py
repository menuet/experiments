#!/usr/bin/env python

import subprocess, os, platform, argparse

def print_step(title):
    print()
    print("**************************************************")
    print("**** {}".format(title))

def configure(config):
    print_step("Configuring")
    if not hasattr(config, "build_type"):
        config.build_type = "Debug"
    config.this_dir = os.path.dirname(os.path.realpath(__file__))
    config.platform_name = platform.system()
    config.build_dir = os.path.join(config.this_dir, "build", config.platform_name, config.build_type)

    print("Build type: {}".format(config.build_type))
    print("Use boost filesystem: {}".format(config.use_boost_filesystem))
    print("This dir: {}".format(config.this_dir))
    print("Platform name: {}".format(config.platform_name))
    print("Build dir: {}".format(config.build_dir))
    subprocess.check_call(["conan", "--version"])
    subprocess.check_call(["cmake", "--version"])
    subprocess.call(["conan", "remote", "add", "bincrafters", "https://api.bintray.com/conan/bincrafters/public-conan"])
    subprocess.call(["conan", "remote", "add", "pmenuet", "https://api.bintray.com/conan/pmenuet/public-conan"])

def conan_install(config):
    print_step("Installing Third-parties")
    if not os.path.exists(config.build_dir):
        os.makedirs(config.build_dir)
    os.chdir(config.build_dir)
    subprocess.check_call(["conan", "install", "--build=missing", "-s", "build_type={}".format(config.build_type), "-o", "use_boost_filesystem={}".format(config.use_boost_filesystem), config.this_dir])

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
    parser.add_argument("--build_type", help="set the build type", type=str, choices=["Debug", "Release"], default="Debug")
    parser.add_argument("--use_boost_filesystem", help="use boost filesystem instead of std filesystem", action="store_true")
    config = parser.parse_args()
    configure(config)
    if config.conan_install:
        conan_install(config)
    if config.conan_build:
        conan_build(config)
    if config.cmake_test:
        cmake_test(config)
