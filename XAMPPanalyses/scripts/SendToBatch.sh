#!/bin/bash
if [ ! -d ${ROOTCOREBIN} ];then
	echo "No ROOTCORE enviroment found... please setup ROOTCORE"
	exit
fi

cd ${ROOTCOREBIN}/../

# submit a Stop0L job

#python XAMPPanalyses/python/Stop0LSubmitToBatch.py -j TT_direct_TT_SUSY1_p2666 --inputDS TT_direct_TT_SUSY1_p2666 --isAF2 

#python XAMPPanalyses/python/Stop0LSubmitToBatch.py -j TT_mixedBT_SUSY1_p2666 -i TT_mixedBT_SUSY1_p2666 --isAF2 --doISR

#python XAMPPanalyses/python/Stop0LSubmitToBatch.py -j ttbarNew --inputDS ttbarIncl_SUSY1_p2666 --doISR
#python XAMPPanalyses/python/Stop0LSubmitToBatch.py -j singleTopNew --inputDS singleTopIncl_SUSY1_p2719 --doISR

python XAMPPanalyses/python/Stop0LSubmitToBatch.py -j radHi_ttbar --inputDS radHi_ttbar_SUSY1_p2719 --doISR --noSyst
python XAMPPanalyses/python/Stop0LSubmitToBatch.py -j radLow_ttbar --inputDS radLow_ttbar_SUSY1_p2719 --doISR --noSyst
python XAMPPanalyses/python/Stop0LSubmitToBatch.py -j ttbar_PowhegHerwigpp --inputDS ttbar_PowhegHerwigpp_SUSY1_p2812 --doISR --noSyst



# sumbit a Stop0L_Gamma job

#python XAMPPanalyses/python/Stop0LSubmitToBatch.py --exe XAMPPanalyses/python/runStop0LepGamma.py --STConfig XAMPPanalyses/SUSYTools_Stop0LGamma.conf -j TT_direct_TT_SUSY1_p2666 --inputDS TT_direct_TT_SUSY1_p2666 --isAF2 

#python XAMPPanalyses/python/Stop0LSubmitToBatch.py --exe XAMPPanalyses/python/runStop0LepGamma.py --STConfig XAMPPanalyses/SUSYTools_Stop0LGamma.conf -j ttbarGammaNew --inputDS ttbarIncl_SUSY1_p2666 -a Stop0L_1Y
#python XAMPPanalyses/python/Stop0LSubmitToBatch.py --exe XAMPPanalyses/python/runStop0LepGamma.py --STConfig XAMPPanalyses/SUSYTools_Stop0LGamma.conf -j singleTopGammaNew --inputDS singleTopIncl_SUSY1_p2719 -a Stop0L_1Y

python XAMPPanalyses/python/Stop0LSubmitToBatch.py --exe XAMPPanalyses/python/runStop0LepGamma.py --STConfig XAMPPanalyses/SUSYTools_Stop0LGamma.conf -j GammaradHi_ttbar --inputDS radHi_ttbar_SUSY1_p2719 -a Stop0L_1Y --noSyst
python XAMPPanalyses/python/Stop0LSubmitToBatch.py --exe XAMPPanalyses/python/runStop0LepGamma.py --STConfig XAMPPanalyses/SUSYTools_Stop0LGamma.conf -j GammaradLow_ttbar --inputDS radLow_ttbar_SUSY1_p2719 -a Stop0L_1Y --noSyst
python XAMPPanalyses/python/Stop0LSubmitToBatch.py --exe XAMPPanalyses/python/runStop0LepGamma.py --STConfig XAMPPanalyses/SUSYTools_Stop0LGamma.conf -j Gammattbar_PowhegHerwigpp --inputDS ttbar_PowhegHerwigpp_SUSY1_p2812 -a Stop0L_1Y --noSyst
