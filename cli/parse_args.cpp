#include <iostream>
#include <getopt.h>
#include <stdexcept>
#include "../core/FileHandler.hpp"

void printHelp()
{
    std::cout   << "Usage: ./ft_otp [OPTIONS] <key file>\n"
                << "Options:\n"
                << "  -g, --generate     Generate and save the encrypted key\n"
                << "  -k, --key          Generate password using the provided key\n"
                << "  -q, --qrcode       Generate a QR code containing the key (requires -g)\n"
                << "  -v, --verbose      Enable verbose output\n"
                << "  -h, --help         Show this help message and exit\n";
}

void parseArgv(int argc, char *argv[], FileHandler *fileHandler, bool &verbose)
{
    const char          *short_opts = "gkvhq";
    const struct option long_opts[] = {
        {"generate", no_argument, nullptr, 'g'},
        {"key", no_argument, nullptr, 'k'},
        {"qrcode", no_argument, nullptr, 'q'},
        {"verbose", no_argument, nullptr, 'v'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0}
    };
    int                 opt;
    bool                mode_set = false;
    bool                generate_mode = false;
    verbose = false;

    while ((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1)
    {
        switch (opt)
        {
        case 'g':
            if (mode_set)
                throw std::invalid_argument("Only one mode (-g or -k) can be specified");
            fileHandler->setMode(OTP_MODE_SAVE_KEY);
            mode_set = true;
            generate_mode = true;
            break;
        case 'k':
            if (mode_set)
                throw std::invalid_argument("Only one mode (-g or -k) can be specified");
            fileHandler->setMode(OTP_MODE_GEN_PWD);
            mode_set = true;
            break;
        case 'q':
            if (!generate_mode)
                throw std::invalid_argument("The -q option (QR code generation) requires -g (generate mode). Use -g along with -q.");
            fileHandler->setMode(OTP_MODE_GEN_QR);
            break;
        case 'v':
            verbose = true;
            fileHandler->setVerbose(true);
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

    /*
     * optind is an external global variable declared in the <unistd.h> header,
     * and it is used by functions like getopt and getopt_long in C to keep track
     * of the next argument to be processed. 
     * It is automatically managed by the getopt family of functions.
     */
    if (optind >= argc)
        throw std::invalid_argument("A key file must be provided.");

    fileHandler->setFilename(argv[optind]);
}
