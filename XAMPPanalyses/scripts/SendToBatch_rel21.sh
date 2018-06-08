#!/bin/bash
if [ ! -d ${ROOTCOREBIN} ] && [ ! -d ${TestArea} ]; then
	echo "No ROOTCORE or Athena environment found... please setup any of those!"
	exit
fi

if [ -z "$TestArea" ]; then
    cd ${ROOTCOREBIN}/../
else
    cd ${TestArea}
fi 

python XAMPPanalyses/python/Stop0LSubmitToBatch.py -j ttbar_rel21_test --inputDS ttbar_rel21_test --noSyst --nevents 100
