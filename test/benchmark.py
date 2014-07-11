#! /usr/bin/env python3

"""
Benchmarking script to output times for the current build of little-tsp
"""

__author__ = "karepker@gmail.com (Kar Epker)"
__copyright__ = "2014 Kar Epker (karepker@gmail.com)"


import argparse

import utility

DEFAULT_NUM_CASES = 100
DEFAULT_NUM_POINTS = (14, 16)
DEFAULT_MAP_SIZE = 100

if __name__ == '__main__':
    PARSER = argparse.ArgumentParser(
            description='Benchmark the little-tsp binary')
    PARSER.add_argument(
            '--generate', dest='generate', action='store_true',
            help='Whether to generate new cases or not')
    PARSER.add_argument(
            '--no-generate', dest='generate', action='store_false',
            help='Whether to generate new cases or not')
    PARSER.set_defaults(generate=False)

    ARGS = PARSER.parse_args()

    # generate cases if necessary
    if ARGS.generate:
        utility.write_cases(
                DEFAULT_NUM_CASES, DEFAULT_NUM_POINTS, DEFAULT_MAP_SIZE,
                overwrite=True)

    # run the binary on all the cases, print the time it takes
    for case_filename in utility.iterate_cases():
        print(utility.benchmark_solution(utility.PROJECT_BINARY, case_filename))
