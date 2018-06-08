#!/bin/bash
if [ ! -d ${ROOTCOREBIN} ];then
        echo "No ROOTCORE enviroment found... please setup ROOTCORE... Recommended SUSY, >=2.4.23"
        exit
fi
if [ ! $# -eq 0 ];then
    echo "Please provide the name of the git branch you want to check out"
fi
cd ${ROOTCOREBIN}/../

git clone ssh://git@gitlab.cern.ch:7999/atlas-mpp-xampp/XAMPPbase.git -b ${1}
if [ $? -eq 0 ]; then
    rc clean
    rc build
else
 echo "The branch ${1} does not seem to exit"
fi

