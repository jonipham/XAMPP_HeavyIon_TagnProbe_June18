#!/bin/bash
TASK_ID=$SGE_TASK_ID
if [[ -z $TASK_ID ]];then
	TASK_ID=1
fi 
# some initial output
echo "###############################################################################################"
echo "					 Enviroment variables"
echo "###############################################################################################"
export
echo "###############################################################################################"
echo " "
if [ -z ${ATLAS_LOCAL_ROOT_BASE} ];then
	echo "###############################################################################################"
	echo "					Setting up the enviroment"
	echo "###############################################################################################"
	echo "source /t2/sw/setupwn_cvmfs.sh"
	source /t2/sw/setupwn_cvmfs.sh
	echo "Setting Up the ATLAS Enviroment:"
	echo "source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh" 
	source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
	echo "Setup athena:"
	echo "cd ${OriginalArea}"
	cd ${OriginalArea}
	echo "asetup ${OriginalProject},${OriginalPatch},here"
	asetup ${OriginalProject},${OriginalPatch},here
	#Check whether we're in release 21
	if [ -z "${CMTBIN}" ];then
		echo "source ${OriginalArea}/../build/${LCG_PLATFORM}/setup.sh"
		source ${OriginalArea}/../build/${LCG_PLATFORM}/setup.sh
	fi
fi
if [ ! -f ${InListDir}/FileList_$TASK_ID.txt ];then
	echo "ERROR: Input FileList not found"
	exit 100
fi
echo "cd ${TMPDIR}"
cd ${TMPDIR}
if [ "${AtlasProject/Ath}" = "$AtlasProject" ] ; then
    echo "python ${WORKDIR}/python/${Execute}  -i ${InListDir}/FileList_$TASK_ID.txt -r dCache ${Options}"
    python ${WORKDIR}/python/${Execute}  -i ${InListDir}/FileList_$TASK_ID.txt -r dCache ${Options}
else
    echo "execute jobOptions..."
	echo "athena ${Execute} -c \"inputFile='${InListDir}/FileList_$TASK_ID.txt';${Options}\""
	athena ${Execute} -c "inputFile='${InListDir}/FileList_$TASK_ID.txt';${Options}"
fi


if [ $? -eq 0 ]; then
	echo "###############################################################################################"
	echo "						Analysis job terminated successfully"
	echo "###############################################################################################"
	ls -lh
	echo "mv ${TMPDIR}/AnalysisOutput.root ${OutDir}/${Name}_${TASK_ID}.root"
	mv ${TMPDIR}/AnalysisOutput.root ${OutDir}/${Name}_${TASK_ID}.root
	
else
	echo "###############################################################################################"
	echo "					Analysis job has experienced an error"
	echo "###############################################################################################"
	exit 100
fi
