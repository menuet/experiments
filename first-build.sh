#!/bin/sh

echo "**************************************"
echo "************ Displaying tools versions"
cmake --version
conan --version

echo "**************************************************"
echo "************ Exporting Third-parties Conan Recipes"
conan export --path src/conan/sfml sfml/2.4.1@pascal/testing
conan export --path src/conan/fmt fmt/3.0.1@pascal/testing

echo "*********************************"
echo "************ Jumping to build/lin"
if [ ! -d "build/lin" ]; then
    if [ ! -d "build" ]; then
        mkdir "build"
    fi
    mkdir "build/lin"
fi
cd "build/lin"
pwd

echo "***********************************"
echo "************ Building Third-parties"
conan install --file ../../conanfile.py --build=missing -s build_type=Debug

echo "*********************************"
echo "************ Building experiments"
conan build --file conanfile.py ../..

