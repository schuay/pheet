/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <getopt.h>
#include <stdio.h>

#include "lib/Graph/Generator/Generator.h"

using namespace graph;

static void
usage(void)
{
	fprintf(stderr,
	        "gengraph [-n <# nodes>] [-m <# edges>] [-s <seed>] [-w <weight limit>] [-d <degree>]\n");
	exit(EXIT_FAILURE);
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
			m = strtol(optarg, NULL, 10);
			break;
		case 'n':
			n = strtol(optarg, NULL, 10);
			break;
		case 's':
			s = strtol(optarg, NULL, 10);
			break;
		case 'd':
			d = strtol(optarg, NULL, 10);
			break;
		case 'w':
			w = strtol(optarg, NULL, 10);
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
