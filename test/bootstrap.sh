#! /bin/bash

# Go to the correct directory
cd $(git rev-parse --show-toplevel)

# Case information
NUM_CASES=10

# Remove the old test cases, create new ones
rm test/generated-case-*.txt
./test/generate.py -p 8 10 -s 100 -n $NUM_CASES 

# run the comparisons
for ((i = 0; i < $(($NUM_CASES)); i++))
do
	# run the program under time
	time ./littletsp -m OPTTSP < test/generated-case-${i}.txt > test/optout.txt
	time ./littletsp -m NAIVETSP < test/generated-case-${i}.txt > test/naiveout.txt

	# get the lengths of the paths
	length1=$(sed -n '1p' < test/optout.txt)
	length2=$(sed -n '1p' < test/naiveout.txt)

	# make sure the lengths are the same
	if [ $length1 != $length2 ]; then
		echo Test case $i fails
		break
	fi
done
