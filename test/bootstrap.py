#! /usr/bin/env python3

DEFAULT_NUM_CASES = 10
DEFAULT_NUM_POINTS_START = 5
DEFAULT_NUM_POINTS_END = 10
DEFAULT_SIZE = 100

NAIVE_FILENAME = '/tmp/naiveout.txt'
OPT_FILENAME = '/tmp/optout.txt'

import argparse
import subprocess

import utility

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
            description='Check correctness of the optimal TSP algorithm vs. '
            'naive TSP algorithm')
    parser.add_argument('-p', '--points', nargs=2, type=int,
        default=[DEFAULT_NUM_POINTS_START, DEFAULT_NUM_POINTS_END],
        help='Range of number of points per test case')
    parser.add_argument('-s', '--size', nargs='?', default=DEFAULT_SIZE,
        type=int, help='Size of the map to use')
    parser.add_argument('-n', '--num', nargs='?', default=DEFAULT_NUM_CASES,
            help='Number of test cases to generate', type=int)

    args = parser.parse_args()

    # remove old test cases, write new ones, and bootstrap them
    utility.write_cases(args.num, args.points, args.size, overwrite=True)
    utility.build_project()
    for case_filename in utility.iterate_cases():

        # run both naive tsp and little tsp
        with open(case_filename, 'r') as case_file, open(NAIVE_FILENAME,
                'w') as naive_file:
                naive_process = subprocess.Popen([utility.PROJECT_BINARY,
                    '-m', 'NAIVETSP'], stdin=case_file, stdout=naive_file)

        with open(case_filename, 'r') as case_file, open(
                OPT_FILENAME, 'w') as opt_file:
                opt_process = subprocess.Popen([utility.PROJECT_BINARY,
                    '-m', 'OPTTSP'], stdin=case_file, stdout=opt_file)

        # wait for the processes to finish before reading output
        opt_process.wait()
        naive_process.wait()

        with open(NAIVE_FILENAME, 'r') as naive_file:
            naive_distance = int(naive_file.readline())
        with open(OPT_FILENAME, 'r') as opt_file:
            opt_distance = int(opt_file.readline())

        if naive_distance != opt_distance:
            print('Test %s fails. Naive distance: %d. Opt distance: %d.' %
                    (case_filename, naive_distance, opt_distance))
            break
