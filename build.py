#!/usr/bin/env python

import subprocess, os, platform, argparse

def print_step(title):
    print()
    print("**************************************************")
    print("**** {}".format(title))
    print("**************************************************")
    print()

def log_command(command):
    command_line = " ".join('"{}"'.format(arg) for arg in command)
    print("Executing: {}".format(command_line))

def log_check_call(command, shell=False):
    log_command(command)
    subprocess.check_call(command, shell=shell)

def log_call(command):
    log_command(command)
    subprocess.call(command)

def log_check_output(command):
    log_command(command)
    return subprocess.check_output(command)

def get_latest_vs_install_path():
    program_files_x86 = os.environ["ProgramFiles(x86)"]
    # print("program_files_x86=" + program_files_x86)
    visual_studio_vswhere_path = os.path.join(program_files_x86, "Microsoft Visual Studio/Installer/vswhere.exe")
    # print("visual_studio_vswhere_path=" + visual_studio_vswhere_path)
    latest_vs_install_path = log_check_output([visual_studio_vswhere_path, "-latest", "-products", "*", "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-property", "installationPath"]).decode().strip()
    # print("visual_studio_install_path=" + latest_vs_install_path)
    return latest_vs_install_path

def get_latest_vc_path():
    latest_vs_install_path = get_latest_vs_install_path()
    default_vc_version_file_path = os.path.join(latest_vs_install_path, "VC/Auxiliary/Build/Microsoft.VCToolsVersion.default.txt")
    # print("default_vc_version_file_path=" + default_vc_version_file_path)
    with open(default_vc_version_file_path, "r") as file:
        default_vc_version = file.read().strip()
    latest_vc_path = os.path.join(latest_vs_install_path, "VC/Tools/MSVC", default_vc_version, "bin/HostX64/x64/cl.exe").replace("\\", "/")
    # print("latest_vc_path=" + latest_vc_path)
    return latest_vc_path

def get_latest_vcvars64_path():
    latest_vs_install_path = get_latest_vs_install_path()
    latest_vsvars64_path = os.path.join(latest_vs_install_path, "VC/Auxiliary/Build/vcvars64.bat")
    # print("latest_vsvars64_path=" + latest_vsvars64_path)
    return latest_vsvars64_path

def update_env_with_vcvars64(config):
    latest_vsvars64_path = get_latest_vcvars64_path()
    vcvars_env_file_path = os.path.join(config.build_dir, "vcvars_env.txt")
    # print("vcvars_env_file_path=" + vcvars_env_file_path)
    log_check_call([latest_vsvars64_path, "&&", "set", ">", vcvars_env_file_path], shell=True)
    with open(vcvars_env_file_path, "r") as file:
        lines = file.readlines()
    for line in lines:
        stripped_line = line.strip()
        equal_pos = stripped_line.find("=")
        if equal_pos > 0:
            env_var_name = stripped_line[:equal_pos]
            env_var_value = stripped_line[equal_pos+1:]
            # print("ENV: {} = {}".format(env_var_name, env_var_value))
            os.environ[env_var_name] = env_var_value

def get_default_pkg_mgr():
    return "conan"

def get_default_os():
    return platform.system()

def get_default_arch():
    return "x86_64"

def get_default_build_type():
    return "Debug"

def get_cmake_toolchain_settings(config):
    if config.toolchain_file != "":
        return [
            "-DCMAKE_TOOLCHAIN_FILE=" + config.toolchain_file,
            ]
    if config.os != "Windows" or config.generator == "ninja":
        return []
    return [
        "-A", "x64" if config.arch == "x86_64" else "Win32",
    ]

def get_default_toolchain_file(config):
    if config.pkg_mgr != "vcpkg":
        return ""
    try:
        if platform.system() == "Windows":
            result = log_check_output(["where", "vcpkg"]).decode()
            result = os.path.dirname(result)
        else:
            results = log_check_output(["whereis", "vcpkg"]).decode()
            result = results.split(" ")[-1].strip()
            result = os.path.dirname(result)
    except:
        print("Warning: Could not infer vcpkg path - will default to '.'")
        result = "."
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
        "nlohmann-json",
        ]

def get_cmake_configure_command(config):
    command = [
        "cmake",
        "-DCMAKE_VERBOSE_MAKEFILE=TRUE",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE",
        "-DCMAKE_CONFIGURATION_TYPES=" + config.build_type,
        "-DCMAKE_BUILD_TYPE=" + config.build_type,
        "-DEXP_PKG_MGR=" + config.pkg_mgr,
        ]
    command += get_cmake_toolchain_settings(config)
    if config.pkg_mgr == "vcpkg":
        command += [
            "-DVCPKG_TARGET_TRIPLET=" + get_vcpkg_triplet(config),
            ]
    if config.generator == "ninja":
        if config.os == "Windows":
            latest_vc_path = get_latest_vc_path()
            command += [
                "-DCMAKE_CXX_COMPILER=" + latest_vc_path,
                "-DCMAKE_C_COMPILER=" + latest_vc_path,
                ]
        command += [
            "-G", "Ninja",
            ]
    command += [
        config.this_dir,
        ]
    return command

def get_cmake_build_command(config):
    command = [
        "cmake",
        "--build", config.build_dir,
        "--config", config.build_type,
        ]
    return command

def get_package_manager_command(config):
    if config.pkg_mgr == "vcpkg":
        return [
            "vcpkg",
            "install",
            "--triplet", get_vcpkg_triplet(config)
            ] + get_vcpkg_thirdparties()
    if config.pkg_mgr == "conan":
        return [
            "conan",
            "install",
            "--build=missing",
            "-s", "os={}".format(config.os),
            "-s", "arch={}".format(config.arch),
            "-s", "build_type={}".format(config.build_type),
            config.this_dir
            ]
    return []

def initialize(config):
    print_step("Initializing")
    config.this_dir = os.path.dirname(os.path.realpath(__file__))
    config.build_dir = os.path.join(config.this_dir, "build", "py-{}-{}-{}-{}-{}".format(config.pkg_mgr, config.os, config.arch, config.build_type, config.generator))
    config.toolchain_file = get_default_toolchain_file(config)

    print("Package Manager: {}".format(config.pkg_mgr))
    print("Os: {}".format(config.os))
    print("Arch: {}".format(config.arch))
    print("Build type: {}".format(config.build_type))
    print("This dir: {}".format(config.this_dir))
    print("Build dir: {}".format(config.build_dir))
    print("Generator: {}".format(config.generator))
    print("Toolchain file: {}".format(config.toolchain_file))
    if config.pkg_mgr == "vcpkg":
        log_check_call(["vcpkg", "version"])
    elif config.pkg_mgr == "conan":
        log_check_call(["conan", "--version"])
        log_call(["conan", "remote", "add", "bincrafters", "https://api.bintray.com/conan/bincrafters/public-conan"])
        # log_call(["conan", "remote", "add", "conan-community", "https://api.bintray.com/conan/conan-community/conan"])
    log_check_call(["cmake", "--version"])

    if not os.path.exists(config.build_dir):
        os.makedirs(config.build_dir)
    os.chdir(config.build_dir)

    if config.generator == "ninja" and config.os == "Windows":
        update_env_with_vcvars64(config)

def acquire(config):
    print_step("Acquiring Third-parties")
    package_manager_command = get_package_manager_command(config)
    log_check_call(package_manager_command)

def configure(config):
    print_step("Configuring Project")
    cmake_configure_command = get_cmake_configure_command(config)
    log_check_call(cmake_configure_command)

def build(config):
    print_step("Building Project")
    cmake_build_command = get_cmake_build_command(config)
    log_check_call(cmake_build_command)

def test(config):
    print_step("Testing Project")
    cmake_test_command = ["ctest", "-C", config.build_type]
    log_check_call(cmake_test_command)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--acquire", help="acquire the third-parties", action="store_true")
    parser.add_argument("--configure", help="configure cmake", action="store_true")
    parser.add_argument("--build", help="build with cmake", action="store_true")
    parser.add_argument("--test", help="test with ctest", action="store_true")
    parser.add_argument("--pkg_mgr", help="set the package manager to use", type=str, choices=["vcpkg", "conan"], default=get_default_pkg_mgr())
    parser.add_argument("--os", help="set the os", type=str, choices=["Linux", "Windows"], default=get_default_os())
    parser.add_argument("--arch", help="set the arch", type=str, choices=["x86", "x86_64"], default=get_default_arch())
    parser.add_argument("--build_type", help="set the build type", type=str, choices=["Debug", "Release"], default=get_default_build_type())
    parser.add_argument("--generator", help="set the generator to use", type=str, choices=["default", "ninja"], default="default")
    config = parser.parse_args()
    initialize(config)
    if config.acquire:
        acquire(config)
    if config.configure:
        configure(config)
    if config.build:
        build(config)
    if config.test:
        test(config)
