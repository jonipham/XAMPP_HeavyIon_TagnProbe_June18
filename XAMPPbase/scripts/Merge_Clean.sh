#!/bin/bash
# some initial output
echo "###############################################################################################"
echo "					Clean previous input from previos iteration "
echo "###############################################################################################"
for i in  $(seq 1 ${nJobs});do
    echo "rm ${Files}_${i}.root"
        rm ${Files}_${i}.root
done
echo "###############################################################################################"
echo "					DONE"
echo "###############################################################################################"

