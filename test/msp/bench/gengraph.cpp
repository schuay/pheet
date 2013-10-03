/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <getopt.h>
#include <stdio.h>

#include "lib/Graph/Generator/Generator.h"
#include "Util.h"

using namespace graph;

static void
usage(void)
{
	fprintf(stderr,
	        "gengraph [-n <# nodes>] [-m <# edges>] [-s <seed>] [-w <weight limit>] [-d <degree>]\n");
	exit(EXIT_FAILURE);
}

static int
parse_int(char const* str)
{
	int val;
	if (!msp::Util::parse_int(str, val)) {
		usage();
	}
	return val;

}

int
main(int argc,
     char** argv)
{
	int n = 10;
	int m = 15;
	int s = 42;
	int w = 10000;
	int d = 3;

	int opt;
	while ((opt = getopt(argc, argv, "n:m:s:w:d:")) != -1) {
		switch (opt) {
		case 'm':
			m = parse_int(optarg);
			break;
		case 'n':
			n = parse_int(optarg);
			break;
		case 's':
			s = parse_int(optarg);
			break;
		case 'd':
			d = parse_int(optarg);
			break;
		case 'w':
			w = parse_int(optarg);
			break;
		default:
			usage();
		}
	}

	Graph* g = Generator::directed("test graph", n, m, true, d, w, s);

	g->write(stdout);

	delete g;

	return 0;
}
