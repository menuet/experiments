#!/usr/bin/env python

import subprocess, os, platform, argparse

def print_step(title):
    print()
    print("**************************************************")
    print("**** {}".format(title))
    print("**************************************************")
    print()

def get_default_pkg_mgr():
    return "conan"

def get_default_os():
    return platform.system()

def get_default_arch():
    return "x86_64"

def get_default_build_type():
    return "Debug"

def get_default_toolchain_file():
    if platform.system() == "Windows":
        result = subprocess.check_output(["where", "vcpkg"]).decode()
        result = os.path.dirname(result)
    else:
        results = subprocess.check_output(["whereis", "vcpkg"]).decode()
        result = results.split(" ")[0]
    result = os.path.join(result, "scripts/buildsystems/vcpkg.cmake")
    return result

def get_vcpkg_triplet(config):
    vcpkg_arch = "x64" if config.arch == "x86_64" else config.arch
    vcpkg_os = config.os.lower()
    return vcpkg_arch + "-" + vcpkg_os

def get_vcpkg_thirdparties():
    return [
        "catch2",
        "fmt",
        "boost",
        "sfml",
        ]

def configure(config):
    print_step("Configuring")
    config.this_dir = os.path.dirname(os.path.realpath(__file__))
    config.build_dir = os.path.join(config.this_dir, "build", "{}-{}-{}-{}".format(config.pkg_mgr, config.os, config.arch, config.build_type))

    print("Package Manager: {}".format(config.pkg_mgr))
    print("Os: {}".format(config.os))
    print("Arch: {}".format(config.arch))
    print("Build type: {}".format(config.build_type))
    print("This dir: {}".format(config.this_dir))
    print("Build dir: {}".format(config.build_dir))
    print("Toolchain file: {}".format(config.toolchain_file))
    if config.pkg_mgr == "vcpkg":
        subprocess.check_call(["vcpkg", "version"])
    elif config.pkg_mgr == "conan":
        subprocess.check_call(["conan", "--version"])
        subprocess.call(["conan", "remote", "add", "bincrafters", "https://api.bintray.com/conan/bincrafters/public-conan"])
        # subprocess.call(["conan", "remote", "add", "conan-community", "https://api.bintray.com/conan/conan-community/conan"])
    subprocess.check_call(["cmake", "--version"])

def acquire(config):
    print_step("Acquiring Third-parties")
    if not os.path.exists(config.build_dir):
        os.makedirs(config.build_dir)
    os.chdir(config.build_dir)
    if config.pkg_mgr == "vcpkg":
        subprocess.check_call(["vcpkg", "install", "--triplet", get_vcpkg_triplet(config)] + get_vcpkg_thirdparties())
    elif config.pkg_mgr == "conan":
        subprocess.check_call(["conan", "install", "--build=missing", "-s", "os={}".format(config.os), "-s", "arch={}".format(config.arch), "-s", "build_type={}".format(config.build_type), config.this_dir])

def build(config):
    print_step("Building Project")
    os.chdir(config.build_dir)
    if config.pkg_mgr == "vcpkg":
        subprocess.check_call([
            "cmake",
            "-DVCPKG_TARGET_TRIPLET=" + get_vcpkg_triplet(config),
            "-DCMAKE_TOOLCHAIN_FILE=" + config.toolchain_file,
            "-DCMAKE_CONFIGURATION_TYPES=" + config.build_type,
            "-DCMAKE_VERBOSE_MAKEFILE=TRUE",
            "-DEXP_PKG_MGR=vcpkg",
            config.this_dir,
            ])
        subprocess.check_call([
            "cmake",
            "--build", config.build_dir,
            "--config", config.build_type,
            ])
    elif config.pkg_mgr == "conan":
        subprocess.check_call(["conan", "build", config.this_dir])

def test(config):
    print_step("Testing Project")
    os.chdir(config.build_dir)
    subprocess.check_call(["ctest"])

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--acquire", help="acquire the third-parties", action="store_true")
    parser.add_argument("--build", help="build the project", action="store_true")
    parser.add_argument("--test", help="test the project", action="store_true")
    parser.add_argument("--pkg_mgr", help="set the package manager to use", type=str, choices=["vcpkg", "conan"], default=get_default_pkg_mgr())
    parser.add_argument("--os", help="set the os", type=str, choices=["Linux", "Windows"], default=get_default_os())
    parser.add_argument("--arch", help="set the arch", type=str, choices=["x86", "x86_64"], default=get_default_arch())
    parser.add_argument("--build_type", help="set the build type", type=str, choices=["Debug", "Release"], default=get_default_build_type())
    parser.add_argument("--toolchain_file", help="provide the path to the cmake vcpkg toolchain file to use", type=str, default=get_default_toolchain_file())
    config = parser.parse_args()
    configure(config)
    if config.acquire:
        acquire(config)
    if config.build:
        build(config)
    if config.test:
        test(config)
