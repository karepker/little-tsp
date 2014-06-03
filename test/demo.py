#! /usr/bin/env python

import os
import re
import subprocess

DEFAULT_NUM_POINTS_START = 10
DEFAULT_NUM_POINTS_END = 100 

DEFAULT_SIZE = 25
DEFAULT_NUM_CASES = 1

FILE_PREFIX = "generated-case-"
FILE_SUFFIX = ".txt"

def find_starting_prefix():
    """
    Determines the high numbered test case present in the form 
    '<FILE_PREFIX>\d+<FILE_SUFFIX>'

    Returns:
        Int: The number one larger than the highest numbered test case
    """
    # find starting prefix of file
    start_case = 0
    SCRIPT_DIR = os.path.abspath(os.path.join(__file__, os.path.pardir))
    while os.path.exists(os.path.join(SCRIPT_DIR, ''.join([FILE_PREFIX, 
        str(start_case), FILE_SUFFIX]))):
        start_case += 1

def write_case(start_num, num_points, map_size):
    """
    Writes randomly generated test cases to file

    Args:
        start_num (int): The number of the first test case written
        num_points (int): A range over which to uniformly select number of
            points to write at random
        map_size (int): The largest x- or y- coordinate possible

    Returns:
        Name of file written to
    """
    # make the file and choose number of points to write
    case_file_name = os.path.join(SCRIPT_DIR, ''.join([FILE_PREFIX, 
        str(write_num), FILE_SUFFIX]))
    with open(case_file_name, mode = 'w') as case_file:
        num_points = random.randomint(num_points)

        # write the header
        case_file.write(str(args.size) + '\n')
        case_file.write(str(num_points) + '\n')

        # write the points
        for i in range(0, num_points):
            point1 = random.randomint(0, args.size)
            point2 = random.randomint(0, args.size)
            case_file.write(''.join([str(point1), " ", str(point2), '\n']))

    return case_file_name


if __name__ == '__main__':
    # set up arguments
    parser = argparse.ArgumentParser(
            description='Generate test cases in Manhattan Graph format')
    parser.add_argument('-p', '--points', nargs=2, type=int,
        default=[DEFAULT_NUM_POINTS_START, DEFAULT_NUM_POINTS_END],
        help="Range of number of points per test case")
    parser.add_argument('-s', '--size', nargs='?', default=DEFAULT_SIZE,
        type=int, help="Size of the map to use")
    parser.add_argument('-n', '--num', nargs='?', default=DEFAULT_NUM_CASES,
        help="Number of test cases to generate", type=int)

    args = parser.parse_args()

    # find the directory the script is in
    project_dir = os.path.abspath(os.getcwd())

    # remove any old test cases
    generated_case_regex = re.compile('generated-case-\d+.txt')
    for test_case in os.listdir(os.path.join(project_dir, 'test')):
        if re.match(generated_case_regex, test_case):
            print(test_case)
            # os.remove(test_case)

    # generate cases
    start_case = find_starting_prefix()
    case_filenames = []
    for case_num in range(start_case, start_case + num_cases):
        case_filenames.append(write_cases(case_num, args.num, args.points, 
            args.size))

    # run the binary on all the cases
    subprocess.call('make')
    for case_filename in case_filenames:
        with open(case_filename) as case_file:
            # benchmark naive solution
            start_time = time.perf_counter()
            subprocess.call(['littletsp', '-m', 'NAIVETSP'], stdin=case_file,
                    stdout=subprocess.DEVNULL)
            naive_time = time.perf_counter() - start_time

            # benchmark optimal solution
            start_time = time.perf_counter()
            subprocess.call(['littletsp', '-m', 'NAIVETSP'], stdin=case_file,
                    stdout=subprocess.DEVNULL)
            opt_time = time.perf_counter() - start_time

            # print statistics
            diff = naive_time - opt_time
            average = (naive_time + opt_time) / 2
            if not average == 0:
                rpd = diff / average * 100
            else:
                rpd = 0
            stats = "{}: {} {} {} {}".format(case_filename, naive_time,
                    opt_time, diff, rpd)
            print(stats)
            
