#! /usr/bin/env python3

"""
Demo script showing speedup vs naive approach
"""

__author__ = "karepker@gmail.com (Kar Epker)"
__copyright__ = "2014 Kar Epker (karepker@gmail.com)"


import argparse
import os
import re

import utility

DEFAULT_NUM_POINTS_START = 5
DEFAULT_NUM_POINTS_END = 8

DEFAULT_SIZE = 25
DEFAULT_NUM_CASES = 10


if __name__ == '__main__':
    # set up arguments
    parser = argparse.ArgumentParser(
            description='utility test cases in Manhattan Graph format')
    parser.add_argument('-p', '--points', nargs=2, type=int,
        default=[DEFAULT_NUM_POINTS_START, DEFAULT_NUM_POINTS_END],
        help='Range of number of points per test case')
    parser.add_argument('-s', '--size', nargs='?', default=DEFAULT_SIZE,
        type=int, help='Size of the map to use')
    parser.add_argument('-n', '--num', nargs='?', default=DEFAULT_NUM_CASES,
        help='Number of test cases to generate', type=int)

    args = parser.parse_args()

    # remove old cases, write new ones and build the project
    utility.write_cases(args.num, args.points, args.size, overwrite=True)
    utility.build_project()

    # print the header
    header_format = "{:>5} | {:>16} | {:>16} | {:>16} | {:>16}"
    print(header_format.format('Case', 'Naive Time (s)', 'Optimal Time (s)',
        'Difference (s)', 'Speedup '))
    print('{:->5}-+-{:->16}-+-{:->16}-+-{:->16}-+-{:->16}'.format('', '', '',
        '', ''))

    # run the binary on all cases
    for case_filename in utility.iterate_cases():

        # benchmark naive and optimal solutions
        naive_time = utility.benchmark_solution([utility.PROJECT_BINARY, '-m',
            'NAIVETSP'], case_filename)
        opt_time = utility.benchmark_solution([utility.PROJECT_BINARY, '-m',
            'OPTTSP'], case_filename)

        # calculate and print statistics
        diff = naive_time - opt_time
        if opt_time:
            speedup = naive_time / opt_time
        else:
            speedup = 0
        case_num = utility.get_case_num(case_filename)
        stats = '{:>5} | {:>16.4} | {:>16.4} | {:>16.4} | {:>15.4}x'.format(
                case_num, naive_time, opt_time, diff, speedup)
        print(stats)
