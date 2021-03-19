#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <exception>
#include <fstream>
#include <memory>

struct Options {
	std::string filename;
	int thread_count;
};

using CommandList = std::vector<std::string>;

static Options create_options(int argc, char** argv)
{
	static const std::array<std::string, 2> valid_options = { "-f", "-t" };
	std::vector<std::pair<std::string, std::string>> args;
	args.reserve(argc - 1);
	for (int i = 1; i < argc; i += 2) {

		std::pair<std::string, std::string> opt;

		opt.first = argv[i];

		if (std::find(valid_options.begin(), valid_options.end(), opt.first) == valid_options.end())
			throw std::runtime_error(opt.first + " is not a valid argument");

		if ((i + 1) >= argc)
			throw std::runtime_error(opt.first + " is missing a value");

		opt.second = argv[i + 1];
		args.push_back(opt);
	}

	Options options = { "", 0 };
	
	for (const auto& arg : args) {
		if (arg.first == "-f") {
			options.filename = arg.second;
		} else if (arg.first  == "-t") {
			options.thread_count = std::stoi(arg.second.c_str());

		}
	}

	if (options.filename == "") {
		throw std::runtime_error("No command list file given");
	}

	if (options.thread_count <= 0) {
		throw std::runtime_error("thread count not valid");
	}


	return options;
}


static CommandList create_command_list(const std::string& filename)
{
	std::fstream f(filename, std::ios::in);
	std::string aux;
	CommandList cmdlist;
	while (getline(f, aux)) {
		cmdlist.push_back(aux);
	}
	return cmdlist;
}

static void run_command_list(int thread_count_i, const CommandList& cmdlist)
{

	const auto execfn = [&](const auto start_index, const auto count) {
		const auto start_itr = cmdlist.begin() + start_index;
		const auto end_itr = start_itr + count;
		for (auto itr = start_itr; itr != end_itr; ++itr) {
			system((*itr).c_str());
		}
	};

	const auto list_size = cmdlist.size();
	auto thread_count = static_cast<decltype(list_size)>(thread_count_i);
	if (thread_count > list_size) {
		std::cout << "adjusting thread count"
			<<" ("<<thread_count<<") "
			<<"to number of commands"
			<<" ("<<list_size<<") "
			<< std::endl;
		thread_count = list_size;
	}

	std::unique_ptr<std::thread[]> threads = std::make_unique<std::thread[]>(thread_count);

	const auto division = list_size / thread_count;
	const auto leftover = list_size % thread_count;
	auto i = decltype(division){0};
	for (; i < thread_count - 1; ++i) {
		threads[i] = std::thread(execfn, i * division, division);
	}
	threads[i] = std::thread(execfn, (i * division), division + leftover); 

	for (i = 0; i < thread_count; ++i)
		threads[i].join();
}


int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cout << "Usage: " 
			<< argv[0] 
			<< " -f <command-list-file> -t <number of threads>" 
			<< std::endl;
		return EXIT_SUCCESS;
	}

	const Options options = create_options(argc, argv);
	const CommandList cmdlist = create_command_list(options.filename);
	run_command_list(options.thread_count, cmdlist);

	return EXIT_SUCCESS;
}







