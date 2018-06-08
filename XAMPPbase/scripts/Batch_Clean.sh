#!/bin/bash
# some initial output
echo "###############################################################################################"
echo "					CLEAN JOB of Batch JOB ${Name}"
echo "###############################################################################################"


if [ -d ${COPYAREA} ] ;then
	echo "rm -rf ${COPYAREA}"
	rm -rf ${COPYAREA}
fi
if [ -d ${OutDirSubJobs} ]; then
	echo "rm -rf ${OutDirSubJobs}"
	rm -rf ${OutDirSubJobs}
fi
if [ -d ${OutDirSubJobs}_merge ]; then
	echo "rm -rf ${OutDirSubJobs}_merge"
	rm -rf ${OutDirSubJobs}_merge
fi

echo "###############################################################################################"
echo "					DONE"
echo "###############################################################################################"

