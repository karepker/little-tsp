#! /usr/bin/python3

import argparse
import os
import random 
import re

FILE_PREFIX = "generated-case-"
FILE_SUFFIX = ".txt"

script_dir = os.path.abspath(os.path.dirname(__file__))
project_dir = os.path.abspath(os.path.join(script_dir, os.pardir))
project_binary = os.path.join(project_dir, 'littletsp')

def get_case_num(case_filename):
    """
    Takes a filename and returns the case number

    Args:
        case_filename (string): The filename of the case for which to find the
        number
    
    Returns:
        Integer number of case
    """
    regex_string = ''.join([r'.*', FILE_PREFIX, r'(?P<case_num>\d+)', 
        FILE_SUFFIX])
    case_num_match = re.search(regex_string, case_filename)
    return int(case_num_match.group('case_num'))

def remove_old_cases():
    """
    Removes old generated cases
    """
    generated_case_regex = re.compile(''.join([FILE_PREFIX, r'\d+', 
        FILE_SUFFIX]))
    test_dir = os.path.join(project_dir, 'test')
    for test_case in os.listdir(test_dir):
        if re.match(generated_case_regex, test_case):
            os.remove(os.path.join(test_dir, test_case))

def find_starting_prefix():
    """
    Determines the high numbered test case present in the form 
    '<FILE_PREFIX>\d+<FILE_SUFFIX>'

    Returns:
        Int: The number one larger than the highest numbered test case
    """
    # find starting prefix of file
    start_case = 0
    while os.path.exists(os.path.join(script_dir, ''.join([FILE_PREFIX, 
        str(start_case), FILE_SUFFIX]))):
        start_case += 1
    return start_case

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
    case_file_name = os.path.join(script_dir, ''.join([FILE_PREFIX, 
        str("%02d" % case_num), FILE_SUFFIX]))
    with open(case_file_name, mode = 'w') as case_file:
        num_points = random.randint(num_points[0], num_points[1])

        # write the header
        case_file.write(str(map_size) + '\n')
        case_file.write(str(num_points) + '\n')

        # write the points
        for i in range(0, num_points):
            point1 = random.randint(0, map_size)
            point2 = random.randint(0, map_size)
            case_file.write(''.join([str(point1), " ", str(point2), '\n']))

    return case_file_name

def write_cases(num_cases, num_points, map_size):
    """
    Writes randomly generated test cases to file

    Args:
        num_cases (int): The number of cases to write
        num_points (int): A range over which to uniformly select number of
            points to write at random
        map_size (int): The largest x- or y- coordinate possible

    Yields:
        The name of the case most recently written
    """
    start_case = find_starting_prefix()
    for case_num in range(start_case, start_case + num_cases):
        yield write_case(case_num, num_points, map_size)
