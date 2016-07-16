
#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_NO_UNPREFIXED_OPTIONS
#include <catch/doctest.h>
#include "vcard.hpp"
#include <boost/program_options.hpp>
#include <iostream>


int main(int argc, char** argv)
{
    namespace po = boost::program_options;

    doctest::Context context(argc, argv);

    int unitTestsResult = context.run();

    if (context.shouldExit())
        return unitTestsResult;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("source", po::value<std::string>(), "contacts source file")
        ("target", po::value<std::string>(), "contacts target file")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") || ! vm.count("source") || !vm.count("target"))
    {
        std::cout << desc << "\n";
        return 1;
    }

    const auto sourceFileName = vm["source"].as<std::string>();
    const auto targetFileName = vm["target"].as<std::string>();

    vcard::Directory directory;

    directory.loadFile(vcard::ContactFileType::Vcf, sourceFileName);

    directory.saveFile(vcard::ContactFileType::Csv, targetFileName);

    return 0;
}

