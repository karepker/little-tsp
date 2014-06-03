#! /usr/bin/python3

import argparse
import os
import random 

FILE_PREFIX = "generated-case-"
FILE_SUFFIX = ".txt"

SCRIPT_DIR = os.path.abspath(os.path.dirname(__file__))


def find_starting_prefix():
    """
    Determines the high numbered test case present in the form 
    '<FILE_PREFIX>\d+<FILE_SUFFIX>'

    Returns:
        Int: The number one larger than the highest numbered test case
    """
    # find starting prefix of file
    start_case = 0
    while os.path.exists(os.path.join(SCRIPT_DIR, ''.join([FILE_PREFIX, 
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
    case_file_name = os.path.join(SCRIPT_DIR, ''.join([FILE_PREFIX, 
        str(case_num), FILE_SUFFIX]))
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

