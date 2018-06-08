#!/bin/bash
if [ ! -d ${ROOTCOREBIN} ];then
	echo "No ROOTCORE enviroment found... please setup ROOTCORE"
	ex#it
fi

cd ${ROOTCOREBIN}/../
Exec="XAMPPanalyses/runStop0Lep.py"

# compare several OR of SUSYTools for signal+ttbar+(the most contributing)Z/W+jets
# therefore use several different SUSYTools config files

# ttbar
python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j ttbar_stop1L -i ttbar_SUSY1_p2540 --noSyst
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j ttbar_noBjet -i ttbar_SUSY1_p2540 --noSyst  --STConfig XAMPPanalyses/SUSYTools_Stop0L_noBjet.conf
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j ttbar_noGhost -i ttbar_SUSY1_p2540 --noSyst  --STConfig XAMPPanalyses/SUSYTools_Stop0L_noGhost.conf
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j ttbar_noJVT -i ttbar_SUSY1_p2540 --noSyst  --STConfig XAMPPanalyses/SUSYTools_Stop0L_noJVT.conf
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j ttbar_noRemoveCalo -i ttbar_SUSY1_p2540 --noSyst  --STConfig XAMPPanalyses/SUSYTools_Stop0L_noRemoveCalo.conf

# signal
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j signal_nominal_new2 -i signalTN1_SUSY1_p2540 --noSyst --isMC15a
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j signal_noBjet_new -i signalTN1_SUSY1_p2540 --noSyst --isMC15a --STConfig XAMPPanalyses/SUSYTools_Stop0L_noBjet.conf
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j signal_noGhost_new -i signalTN1_SUSY1_p2540 --noSyst --isMC15a --STConfig XAMPPanalyses/SUSYTools_Stop0L_noGhost.conf
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j signal_noJVT_new -i signalTN1_SUSY1_p2540 --noSyst --isMC15a --STConfig XAMPPanalyses/SUSYTools_Stop0L_noJVT.conf
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j signal_noRemoveCalo_new -i signalTN1_SUSY1_p2540 --noSyst --isMC15a --STConfig XAMPPanalyses/SUSYTools_Stop0L_noRemoveCalo.conf
#
## Wjets
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j wjets_nominal_new2 -i Wjets_SUSY1_p2540 --noSyst 
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j wjets_noBjet_new -i Wjets_SUSY1_p2540 --noSyst  --STConfig XAMPPanalyses/SUSYTools_Stop0L_noBjet.conf
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j wjets_noGhost_new -i Wjets_SUSY1_p2540 --noSyst  --STConfig XAMPPanalyses/SUSYTools_Stop0L_noGhost.conf
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j wjets_noJVT_new -i Wjets_SUSY1_p2540 --noSyst  --STConfig XAMPPanalyses/SUSYTools_Stop0L_noJVT.conf
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j wjets_noRemoveCalo_new -i Wjets_SUSY1_p2540 --noSyst  --STConfig XAMPPanalyses/SUSYTools_Stop0L_noRemoveCalo.conf
#
## Zjets
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j zjets_nominal_new2 -i Zjets_SUSY1_p2540 --noSyst 
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j zjets_noBjet_new -i Zjets_SUSY1_p2540 --noSyst  --STConfig XAMPPanalyses/SUSYTools_Stop0L_noBjet.conf
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j zjets_noGhost_new -i Zjets_SUSY1_p2540 --noSyst  --STConfig XAMPPanalyses/SUSYTools_Stop0L_noGhost.conf
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j zjets_noJVT_new -i Zjets_SUSY1_p2540 --noSyst  --STConfig XAMPPanalyses/SUSYTools_Stop0L_noJVT.conf
#python XAMPPbase/python/SubmitToBatch.py --exe ${Exec} -j zjets_noRemoveCalo_new -i Zjets_SUSY1_p2540 --noSyst  --STConfig XAMPPanalyses/SUSYTools_Stop0L_noRemoveCalo.conf
