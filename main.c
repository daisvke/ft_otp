#include <iostram>

static int	_op_parse_argv(int argc, char *argv[])
{
	if (argc < 2 || ! av[1] || !av[2])
        throw std::invalid_argument("expecting ./ircserv <port> <password>");
    checkPort(av[1]);
    checkPassword(av[2]);
}

int	main(int argc, char *argv[])
{
	_op_e_ret	_ret = 0;

	try {
		(_ret = _op_parse_argv(argc, argv))
	} catch (std::exception &e) {
		std::cout << "Invalid argument: " << e.what() << std::endl;
		return 1;
	}
	if (ret == _OP_ARG_G) _op_save_key();
	else _op_generate_key();
}
