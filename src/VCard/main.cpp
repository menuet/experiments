
#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_NO_UNPREFIXED_OPTIONS
#include <catch/doctest.h>
#include "vcard.hpp"


int main(int argc, char** argv)
{
    doctest::Context context(argc, argv);

    int unitTestsResult = context.run();

    if (context.shouldExit())
        return unitTestsResult;

    vcard::Directory directory;

    directory.loadFile(vcard::ContactFileType::Vcf, R"(C:\DEV\EXPERIMENTS\experiments\src\VCard\build\Debug\00003.vcf)");

    directory.saveFile(vcard::ContactFileType::Csv, R"(C:\DEV\EXPERIMENTS\experiments\src\VCard\build\Debug\00003.csv)");

    return 0;
}

