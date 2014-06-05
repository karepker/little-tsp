#! /usr/bin/env python

import argparse
import os
import re
import subprocess
import time

import utility

DEFAULT_NUM_POINTS_START = 10
DEFAULT_NUM_POINTS_END = 100 

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
        subprocess.Popen(command, stdin=case_file, stdout=subprocess.DEVNULL,
                cwd=utility.project_dir)
        elapsed_time = time.perf_counter() - start_time
    return elapsed_time


if __name__ == '__main__':
    # set up arguments
    parser = argparse.ArgumentParser(
            description='utility test cases in Manhattan Graph format')
    parser.add_argument('-p', '--points', nargs=2, type=int,
        default=[DEFAULT_NUM_POINTS_START, DEFAULT_NUM_POINTS_END],
        help="Range of number of points per test case")
    parser.add_argument('-s', '--size', nargs='?', default=DEFAULT_SIZE,
        type=int, help="Size of the map to use")
    parser.add_argument('-n', '--num', nargs='?', default=DEFAULT_NUM_CASES,
        help="Number of test cases to utility", type=int)

    args = parser.parse_args()

    # run the binary on all the cases
    subprocess.Popen('make', cwd=utility.project_dir, stdout=subprocess.DEVNULL)
    utility.remove_old_cases()
    header_format = "{:>5} | {:>16} | {:>16} | {:>16} | {:>16}"
    print(header_format.format("", "", "", "", "Relative Percent"))
    print(header_format.format("Case", "Naive Time (s)", "Optimal Time (s)", 
        "Difference (s)", "Difference(%)"))
    print("{:->5}-+-{:->16}-+-{:->16}-+-{:->16}-+-{:->16}".format("", "", "",
        "", ""))
    for case_filename in utility.write_cases(args.num, args.points, args.size):

        # benchmark naive and optimal solutions
        naive_time = benchmark_solution([utility.project_binary, '-m', 
            'NAIVETSP'], case_filename)
        opt_time = benchmark_solution([utility.project_binary, '-m', 
            'OPTTSP'], case_filename)
   
        # calculate and print statistics
        diff = naive_time - opt_time
        average = (naive_time + opt_time) / 2
        if not average == 0:
            rpd = diff / average
        else:
            rpd = 0
        case_num = utility.get_case_num(case_filename)
        stats = "{:>5} | {:>16.2} | {:>16.2} | {:>16.2} | {:>16.2%}".format(
                case_num, naive_time, opt_time, diff, rpd)
        print(stats)
