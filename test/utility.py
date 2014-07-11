#! /usr/bin/env python3

"""
Utility methods for testing little-tsp
"""

__author__ = "karepker@gmail.com (Kar Epker)"
__copyright__ = "2014 Kar Epker (karepker@gmail.com)"


import os
import random
import re
import subprocess
import time

BUILD_DIR_NAME = "build"
TEST_DIR_NAME = "test"
FILE_PREFIX = "generated-case-"
FILE_SUFFIX = ".txt"

SCRIPT_DIR = os.path.abspath(os.path.dirname(__file__))
PROJECT_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, os.pardir))
BUILD_DIR = os.path.abspath(os.path.join(PROJECT_DIR, BUILD_DIR_NAME))
PROJECT_BINARY = os.path.join(BUILD_DIR, 'src', 'littletsp')

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
        solution = subprocess.Popen(
                command, stdin=case_file, stdout=subprocess.DEVNULL)
        solution.wait()
        elapsed_time = time.perf_counter() - start_time
    return elapsed_time


def build_project():
    """
    Builds the cmake project
    """
    make_process = subprocess.Popen(
            ['make'], stdout=subprocess.DEVNULL, cwd=BUILD_DIR)
    make_process.wait()


def create_case_name(
        case_num, file_prefix=FILE_PREFIX, file_suffix=FILE_SUFFIX):
    """
    Creates the filename for a case

    Args:
        case_num (int): The number of the case
        file_prefix (string): The prefix to use for the file (default:
            FILE_PREFIX)
        file_suffix (string): The suffix to use for the file (default:
            FILE_SUFFIX)

    Returns:
        A string for the case
    """
    return os.path.join(
            SCRIPT_DIR, ''.join([file_prefix, str("%03d" % case_num),
                                 file_suffix]))


def get_case_num(
        case_filename, file_prefix=FILE_PREFIX, file_suffix=FILE_SUFFIX):
    """
    Takes a filename and returns the case number

    Args:
        case_filename (string): The filename from which to extract the case
            number
        file_prefix (string): The file prefix with which to search for cases
            (default FILE_PREFIX)
        file_suffix (string): The file suffix with which to search for cases
            (default FILE_SUFFIX)

    Returns:
        Integer number of case
    """
    regex_string = ''.join(
            [r'.*', file_prefix, r'(?P<case_num>\d+)', file_suffix])
    case_num_match = re.search(regex_string, case_filename)
    return int(case_num_match.group('case_num'))


def remove_old_cases(file_prefix=FILE_PREFIX, file_suffix=FILE_SUFFIX):
    """
    Removes old generated cases

    Args:
        file_prefix (string): The file prefix with which to search for cases
            (default FILE_PREFIX)
        file_suffix (string): The file suffix with which to search for cases
            (default FILE_SUFFIX)
    """
    for test_case in iterate_cases(
            file_prefix=file_prefix, file_suffix=file_suffix):
        os.remove(os.path.join(SCRIPT_DIR, test_case))


def find_starting_prefix():
    """
    Determines the high numbered test case present in the form
    '<FILE_PREFIX><number><FILE_SUFFIX>'

    Returns:
        Int: The number one larger than the highest numbered test case
    """
    # find starting prefix of file
    case_names = list(iterate_cases())
    if not case_names:
        return 0
    return get_case_num(case_names[-1]) + 1


def iterate_cases(file_prefix=FILE_PREFIX, file_suffix=FILE_SUFFIX):
    """
    Iterates over existing cases

    Args:
        file_prefix (string): The file prefix with which to search for cases
            (default FILE_PREFIX)
        file_suffix (string): The file suffix with which to search for cases
            (default FILE_SUFFIX)

    Yields:
        Cases starting at the lowest case number
    """
    current_case_num = 0
    # check if the file exists, yield it if it does
    while True:
        case_name = create_case_name(
                current_case_num, file_prefix=file_prefix,
                file_suffix=file_suffix)
        try:
            with open(case_name, 'r'):
                yield case_name
                current_case_num += 1
        except IOError:
            return


def write_case(case_num, num_points, map_size):
    """
    Writes randomly generated test cases to file

    Args:
        case_num (int): The number of the case to write
        num_points (int): A range over which to uniformly select number of
            points to write at random
        map_size (int): The largest x- or y- coordinate possible

    Returns:
        Name of file written to
    """
    # make the file and choose number of points to write
    case_file_name = create_case_name(case_num)
    with open(case_file_name, 'w') as case_file:
        num_points = random.randint(num_points[0], num_points[1])

        # write the header
        case_file.write(str(map_size) + '\n')
        case_file.write(str(num_points) + '\n')

        # write the points
        for _ in range(0, num_points):
            point1 = random.randint(0, map_size)
            point2 = random.randint(0, map_size)
            case_file.write(''.join([str(point1), " ", str(point2), '\n']))

    return case_file_name


def write_cases(num_cases, num_points, map_size, overwrite=False):
    """
    Writes randomly generated test cases to file

    Args:
        num_cases (int): The number of cases to write
        num_points (int): A range over which to uniformly select number of
            points to write at random
        map_size (int): The largest x- or y- coordinate possible
        overwrite (bool): Whether to overwrite previously written cases
            (default False)
    """
    start_case = 0 if overwrite else find_starting_prefix()
    for case_num in range(start_case, start_case + num_cases):
        write_case(case_num, num_points, map_size)
