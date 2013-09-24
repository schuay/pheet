/*
 * Copyright Martin Wimmer 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#define NDEBUG 1

#include "MspBenchmarks.h"

#include <dirent.h>
#include <getopt.h>
#include <iostream>
#include <stdlib.h>
#include <stdexcept>
#include <sys/stat.h>

using namespace pheet;

inline bool is_file(char const* name)
{
	struct stat s;

	return (stat(name, &s) == 0 && (s.st_mode & S_IFREG));
}

void parse(char const* name, std::vector<std::string>& files)
{
	struct stat s;

	if (stat(name, &s) == 0) {
		if (s.st_mode & S_IFDIR) {
			//it's a directory
			DIR* dp;
			struct dirent* dirp;
			if ((dp  = opendir(name)) == NULL) {
				std::cout << "Error(" << errno << ") opening " << name << std::endl;
				return;
			}

			while ((dirp = readdir(dp)) != NULL) {
				if (is_file(dirp->d_name)) {
					files.push_back(std::string(dirp->d_name));
				}
			}
			closedir(dp);
		} else if (s.st_mode & S_IFREG) {
			//it's a file
			files.push_back(name);
		} else {
			std::cout << "\"" << name << "\" is not a file or a directory." << std::endl;
		}
	} else {
		std::cout << "Skipping file \"" << name << "\" because it does not exist." << std::endl;
	}


}

void usage()
{
	std::cout << "\n msp-bench [-n...] [--seq] [--strategy] [DIRECTORY...] [FILE...]\n\n"
	          << "Benchmark multi-objective shortest path algorithms\n"
	          << "PARAMETERS:\n"
	          << "-n\t\t number of processors to be used for benchmarking\n"
	          << "--seq\t\t benchmark the sequential algorithm."
	          << " Benchmark will be run for n=1.\n"
	          << "--strategy\t benchmark the parallel algorithm.\n\n";
	exit(1);
}

//Flags for strategies to be used
static int sequential = 0;
static int strategy = 0;

int
main(int argc, char** argv)
{
	int c, n;
	std::vector<int> nv;
	std::vector<std::string> files;

	while (1) {
		static struct option long_options[] = {

			//Options setting a flag
			{"seq",     no_argument,       &sequential,   1},
			{"strategymsp",   no_argument,       &strategy, 1},
			//Options setting a parameter
			{"number_of_cpus",  required_argument, 0, 'n'},
			{0, 0, 0, 0}
		};

		int option_index = 0;
		c = getopt_long(argc, argv, "n:", long_options, &option_index);

		if (c == -1) {
			break;
		}

		switch (c) {
		case 0:
			//Nothing to do; option set the flag
			break;

		case 'n':
			try {
				n = std::stoi(optarg);
			} catch (const std::invalid_argument& e) {
				std::cout << "Invalid argument for option -n: " << optarg << std::endl;
				usage();
			} catch (const std::out_of_range& e) {
				std::cout << "Argument for option -n out of range: " << optarg << std::endl;
				usage();
			}
			nv.push_back(n);
			break;

		case '?':
			//getopt_long handles errors
			usage();

		default:
			usage();
		}
	}

	if (optind < argc) {
		do {
			parse(argv[optind], files);
		} while (++optind < argc);
	} else {
		std::cout << "No input files given." << std::endl;
		exit(1);
	}

	if (strategy && nv.size() == 0) {
		std::cout << "Number of processors needs to be specified when --strategy is given.\n";
		exit(0);
	}

	MspBenchmarks t;
	t.run_benchmarks(sequential, strategy, nv, files);
	exit(0);
}
