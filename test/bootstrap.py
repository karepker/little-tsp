#! /usr/bin/env python3

NUM_BOOTSTRAPS = 10
NAIVE_FILENAME = '/tmp/naiveout.txt'
OPT_FILENAME = '/tmp/optout.txt'

import subprocess

import generate

if __name__ == '__main__':

    # bootstrap on an arbitrary number of test cases
    start_case = generate.find_starting_prefix()
    for case_num in range(start_case, start_case + NUM_BOOTSTRAPS):
        case_filename = generate.write_case(case_num, (7, 9), 100)

        # run both naive tsp and little tsp
        with open(case_filename, 'r') as case_file, open(NAIVE_FILENAME, 
                'w') as naive_file:
                subprocess.call(['../littletsp', '-m', 'NAIVETSP'], 
                        stdin=case_file, stdout=naive_file)
        with open(case_filename, 'r') as case_file, open(
                OPT_FILENAME, 'w') as opt_file:
                subprocess.call(['../littletsp', '-m', 'OPTTSP'], 
                        stdin=case_file, stdout=opt_file)


        with open(NAIVE_FILENAME, 'r') as naive_file:
            naive_distance = int(naive_file.readline())
        with open(OPT_FILENAME, 'r') as opt_file:
            opt_distance = int(opt_file.readline())

        if naive_distance != opt_distance:
            print("Test %d fails. Naive distance: %d. Opt distance: %d." %
                    case_num, naive_distance, opt_distance)
            break
