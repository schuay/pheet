/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <fstream>
#include <getopt.h>
#include <stdio.h>

#include "PaixaoSantos.h"

static void
usage()
{
	fprintf(stderr, "Usage: convps -i <infile> -o <outfile>\n");
	exit(EXIT_FAILURE);
}

int
main(int argc,
     char** argv)
{
	int opt;
	char const* infile = nullptr;
	char const* outfile = nullptr;

	while ((opt = getopt(argc, argv, "i:o:")) != -1) {
		switch (opt) {
		case 'i':
			infile = optarg;
			break;
		case 'o':
			outfile = optarg;
			break;
		default: /* '?' */
			usage();
		}
	}

	if (infile == nullptr || outfile == nullptr) {
		usage();
	}

	if (optind < argc) {
		usage();
	}

	std::ifstream istream(infile);
	FILE* outfptr = fopen(outfile, "w");

	graph::PaixaoSantos adapter;
	graph::Graph* g = adapter(istream);
	g->write(outfptr);
	delete g;

	fclose(outfptr);

	return 0;
}
