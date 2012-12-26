#! /usr/bin/python3

import argparse as argparse
import os as os 
import random as rand 

################################################################################
## CONSTANTS
################################################################################

DEFAULT_NUM_POINTS_START = 10
DEFAULT_NUM_POINTS_END = 100 

DEFAULT_SIZE = 25
DEFAULT_NUM_CASES = 1

FILE_PREFIX = "generated-case-"
FILE_SUFFIX = ".txt"

################################################################################
## PARSER
################################################################################

parser = argparse.ArgumentParser(description='Generate test cases for project 4')
parser.add_argument('-p', '--points', nargs = 2, type = int,
    default = [DEFAULT_NUM_POINTS_START, DEFAULT_NUM_POINTS_END],
    help = "Range of number of points per test case")
parser.add_argument('-s', '--size', nargs = '?', default = DEFAULT_SIZE,
    type = int, help = "Size of the map to use")
parser.add_argument('-n', '--num', nargs = '?', default = DEFAULT_NUM_CASES,
    help = "Number of test cases to generate", type = int)
parser.add_argument('-q', '--quiet', action = 'store_true')

args = parser.parse_args()

################################################################################
## SCRIPT
################################################################################

# print out some information about runtime
if not args.quiet:
    print("Writing " + str(args.num) + " case(s) with between " + 
        str(args.points[0]) + " and " + str(args.points[1]) + 
        " points on a map of size " + str(args.size))

# find starting prefix of file
start_case = 0
SCRIPT_DIR = os.path.abspath(os.path.join(__file__, os.path.pardir))
while os.path.exists(SCRIPT_DIR + os.sep + FILE_PREFIX + 
    str(start_case) + FILE_SUFFIX):
    start_case += 1

# generate cases
for write_num in range(start_case, start_case + args.num):

    # make the file and choose number of points to write
    case_file = open(SCRIPT_DIR + os.sep + FILE_PREFIX + str(write_num) + 
        FILE_SUFFIX, mode = 'w')
    num_points = rand.randint(args.points[0], args.points[1])

    # write the header
    case_file.write(str(args.size) + os.linesep)
    case_file.write(str(num_points) + os.linesep)

    # print out some status info
    if not args.quiet:
        print("Writing case " + case_file.name + " with " + 
            str(num_points) + " points ")

    # write the points
    for i in range(0, num_points):
        point1 = rand.randint(0, args.size)
        point2 = rand.randint(0, args.size)
        case_file.write(str(point1) + " " + str(point2) + os.linesep)

print("Finished writing all test cases")
