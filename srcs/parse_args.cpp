#include <iostream>
#include <getopt.h>
#include <stdexcept>
#include "FileHandler.hpp" // Assuming FileHandler is defined here

void printHelp()
{
    std::cout << "Usage: ./ft_otp [OPTIONS] <key file>\n"
              << "Options:\n"
              << "  -g, --generate     Generate and save the encrypted key\n"
              << "  -k, --key          Generate password using the provided key\n"
              << "  -v, --verbose      Enable verbose output\n"
              << "  -h, --help         Show this help message and exit\n";
}

void parseArgv(int argc, char *argv[], FileHandler *fileHandler, bool &verbose)
{
    const char *short_opts = "gkvh";
    const struct option long_opts[] = {
        {"generate", no_argument, nullptr, 'g'},
        {"key", no_argument, nullptr, 'k'},
        {"verbose", no_argument, nullptr, 'v'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0}
    };

    int opt;
    bool mode_set = false;

    while ((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1)
    {
        switch (opt)
        {
        case 'g':
            if (mode_set)
                throw std::invalid_argument("Only one mode (-g or -k) can be specified");
            fileHandler->setMode(OTP_MODE_SAVE_KEY);
            mode_set = true;
            break;
        case 'k':
            if (mode_set)
                throw std::invalid_argument("Only one mode (-g or -k) can be specified");
            fileHandler->setMode(OTP_MODE_GEN_PWD);
            mode_set = true;
            break;
        case 'v':
            verbose = true;
            break;
        case 'h':
            printHelp();
            exit(0);
        default:
            throw std::invalid_argument("Invalid arguments. Use -h or --help for usage.");
        }
    }

    if (!mode_set)
        throw std::invalid_argument("You must specify either -g (generate) or -k (key).");

    if (optind >= argc)
        throw std::invalid_argument("A key file must be provided.");

    fileHandler->setFilename(argv[optind]);
}
