/*
 * ArgsParser.h
 *
 *  Created on: May 21, 2014
 *      Author: asdf
 */

#ifndef ARGSPARSER_H_
#define ARGSPARSER_H_

#include <iostream>
#include <string.h>
#include <stdlib.h>
using namespace std;

// return index if exists, or return 0
int option_exists(int argc, char **argv, const char *option) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], option) == 0)
			return i;
	}
	return 0;
}

char* option_value(int argc, char **argv, const char *option, const char* long_name_option, const char *default_value) {
	int index = option_exists(argc, argv, option);
	//char *ret = default_value;
	char *ret = const_cast<char *>(default_value);
	if (index && argc > index + 1) {
		ret = argv[index + 1];
	} else {
		if (long_name_option) {
			index = option_exists(argc, argv, long_name_option);
			if (index && argc > index + 1) {
				ret = argv[index + 1];
			}
		}
	}
	return ret;
}

int option_value(int argc, char **argv, const char *option, const char *long_name_option, int default_value) {
	int index = option_exists(argc, argv, option);
	int ret = default_value;
	if (index && argc > index + 1) {
		ret = atoi(argv[index + 1]);
	} else {
		if (long_name_option) {
			index = option_exists(argc, argv, long_name_option);
			if (index && argc > index + 1) {
				ret = atoi(argv[index + 1]);
			}
		}
	}
	return ret;
}

#endif /* ARGSPARSER_H_ */
