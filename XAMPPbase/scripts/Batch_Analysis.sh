#!/bin/bash
TASK_ID=$SGE_TASK_ID
if [[ -z $TASK_ID ]];then
	TASK_ID=1
fi 
# user-defined parameters
WORKDIR=${ROOTCOREBIN}"/bin/"${ROOTCORECONFIG}"/"              #run folder
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
	echo "cd ${TMPDIR}"
	cd ${TMPDIR}
	echo "source /t2/sw/setupwn_cvmfs.sh"
	source /t2/sw/setupwn_cvmfs.sh
	echo "Setting Up the ATLAS Enviroment:"
	echo "source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh" 
	source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
	echo "Setting Up ROOT:"
	echo "source ${ATLAS_LOCAL_ROOT_BASE}/packageSetups/atlasLocalROOTSetup.sh ${ROOTVER}-${ROOTCORECONFIG} --skipConfirm" 
	source ${ATLAS_LOCAL_ROOT_BASE}/packageSetups/atlasLocalROOTSetup.sh --rootVersion ${ROOTVER}-${ROOTCORECONFIG} --skipConfirm
	echo "Setup ROOTCORE:"
	#echo "export PATH=${WORKDIR}/:$PATH"
	#export PATH=${WORKDIR}/:$PATH
	echo "export LD_LIBRARY_PATH=${ROOTCOREBIN}/lib/${ROOTCORECONFIG}/:${LD_LIBRARY_PATH}"
	export LD_LIBRARY_PATH=${ROOTCOREBIN}/lib/${ROOTCORECONFIG}:${LD_LIBRARY_PATH}
	export PYTHONPATH=${ROOTCOREBIN}/python:${PYTHONPATH}
fi
if [ ! -f ${InListDir}/FileList_$TASK_ID.txt ];then
	echo "ERROR: Input FileList not found"
	exit 100
fi
cd ${TMPDIR}
echo "python ${ROOTCOREBIN}/python/${Execute}  -i ${InListDir}/FileList_$TASK_ID.txt -r dCache ${Options}"
python ${ROOTCOREBIN}/python/${Execute}  -i ${InListDir}/FileList_$TASK_ID.txt -r dCache ${Options}

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
