#! /bin/bash

# Go to the correct directory
cd $(git rev-parse --show-toplevel)

# Case information
NUM_CASES=10
POINTS_START=7
POINTS_END=10

# Remove any old test cases, create new ones
rm test/generated-case-*.txt
echo -n "Generating ${NUM_CASES} test cases with between ${POINTS_START} and ${POINTS_END} points ... "
./test/generate.py -q -p $POINTS_START $POINTS_END -s 100 -n $NUM_CASES 
echo "done"

TIMEFORMAT=%U
echo -e "Case Naive Opt Diff RPD"

# run the comparisons
for ((i = 0; i < $(($NUM_CASES)); i++)); do

	# run the program under time
	OPT_TIME=$( time (./littletsp -m OPTTSP < test/generated-case-${i}.txt > /dev/null) 2>&1)
	NAIVE_TIME=$( time (./littletsp -m NAIVETSP < test/generated-case-${i}.txt > /dev/null) 2>&1)

	# do time math
	DIFF=$(echo "${NAIVE_TIME} - ${OPT_TIME}" | bc | awk '{printf "%1.3f", $0}')
	AV=$(echo "scale=3;(${NAIVE_TIME} + ${OPT_TIME})/2" | bc)

	# check whether average is zero before calculating RPD
	if [[ $(echo "${AV} == 0" | bc) -eq 0 ]]; then
		RPD=$(echo "(${DIFF} / ${AV}) * 100" | bc)
	else
		RPD=$(echo "(0/1) * 100" | bc | awk '{printf "%3.0f", $0}')
	fi

	CASE_NUM=$(echo $i | awk '{printf "%4.1i", $0}')

	# output quantities
	echo "${CASE_NUM} ${NAIVE_TIME}s ${OPT_TIME}s ${DIFF}s ${RPD}%"
done
echo "Finished running tests"
