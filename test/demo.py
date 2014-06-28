#! /usr/bin/env python3

import argparse
import os
import re
import subprocess
import time

import utility

DEFAULT_NUM_POINTS_START = 5
DEFAULT_NUM_POINTS_END = 8

DEFAULT_SIZE = 25
DEFAULT_NUM_CASES = 10

def benchmark_solution(command, case_filename):
    """
    Benchmark the command by seeing how long it takes to execute

    Args:
        command (list of strings): List of strings forming a system command
        case_filename: The filename of the case to pass input

    Returns:
        The time it took to run the command
    """
    with open(case_filename, 'r') as case_file:
        start_time = time.perf_counter()
        solution = subprocess.Popen(command, stdin=case_file, 
                stdout=subprocess.DEVNULL, cwd=utility.project_dir)
        solution.wait()
        elapsed_time = time.perf_counter() - start_time
    return elapsed_time


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

    # run the binary on all the cases
    utility.remove_old_cases()
    utility.build_project()
    header_format = "{:>5} | {:>16} | {:>16} | {:>16} | {:>16}"
    print(header_format.format('Case', 'Naive Time (s)', 'Optimal Time (s)', 
        'Difference (s)', 'Speedup '))
    print('{:->5}-+-{:->16}-+-{:->16}-+-{:->16}-+-{:->16}'.format('', '', '',
        '', ''))
    for case_filename in utility.write_cases(args.num, args.points, args.size):

        # benchmark naive and optimal solutions
        naive_time = benchmark_solution([utility.project_binary, '-m', 
            'NAIVETSP'], case_filename)
        opt_time = benchmark_solution([utility.project_binary, '-m', 
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
