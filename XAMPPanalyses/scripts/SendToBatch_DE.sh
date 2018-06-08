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


python XAMPPanalyses/python/Stop0LTruthSubmitToBatch.py -j TRUTH1_scale3_m0p1_1p0c1_M100_v1 --inputDS TRUTH1_scale3_m0p1_1p0c1_M100 
# python XAMPPanalyses/python/Stop0LTruthSubmitToBatch.py -j TRUTH1_scale3_m0p1_1p0c1_M1000 --inputDS TRUTH1_scale3_m0p1_1p0c1_M1000 
# python XAMPPanalyses/python/Stop0LTruthSubmitToBatch.py -j TRUTH1_scale3_m0p1_1p0c1_M200 --inputDS TRUTH1_scale3_m0p1_1p0c1_M200 
# python XAMPPanalyses/python/Stop0LTruthSubmitToBatch.py -j TRUTH1_scale3_m0p1_1p0c1_M500 --inputDS TRUTH1_scale3_m0p1_1p0c1_M500 
# python XAMPPanalyses/python/Stop0LTruthSubmitToBatch.py -j TRUTH1_scale3_m0p1_1p0c1_M700 --inputDS TRUTH1_scale3_m0p1_1p0c1_M700 

# python XAMPPanalyses/python/Stop0LTruthSubmitToBatch.py -j TRUTH1_scale3_m0p1_1p0c2_M100 --inputDS TRUTH1_scale3_m0p1_1p0c2_M100 
# python XAMPPanalyses/python/Stop0LTruthSubmitToBatch.py -j TRUTH1_scale3_m0p1_1p0c2_M1000 --inputDS TRUTH1_scale3_m0p1_1p0c2_M1000 
# python XAMPPanalyses/python/Stop0LTruthSubmitToBatch.py -j TRUTH1_scale3_m0p1_1p0c2_M200 --inputDS TRUTH1_scale3_m0p1_1p0c2_M200 
# python XAMPPanalyses/python/Stop0LTruthSubmitToBatch.py -j TRUTH1_scale3_m0p1_1p0c2_M500 --inputDS TRUTH1_scale3_m0p1_1p0c2_M500 
# python XAMPPanalyses/python/Stop0LTruthSubmitToBatch.py -j TRUTH1_scale3_m0p1_1p0c2_M700 --inputDS TRUTH1_scale3_m0p1_1p0c2_M700 
