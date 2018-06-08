#!/bin/bash
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
	echo "source ${ATLAS_LOCAL_ROOT_BASE}/packageSetups/atlasLocalROOTSetup.sh --skipConfirm" 
	source ${ATLAS_LOCAL_ROOT_BASE}/packageSetups/atlasLocalROOTSetup.sh --skipConfirm
fi
if [ ${SGE_TASK_ID} != "undefined" ] ;then
	MergeList=`cat ${In}_${SGE_TASK_ID}.txt` 
	for M in ${MergeList};do
		if [ "${M}" != "${M/dcap:/}" ]; then
			echo "The Dataset ${M} is stored on dCache assume. That  the file is healthy"			
		elif [ ! -f ${M} ];then
			echo "File ${M} does not exist. Merging Failed"
			echo "###############################################################################################"
			echo "					Merging failed"
			echo "###############################################################################################"
			exit 100		
		fi		
	done
    # cd ${TMP}
	cd ${OutDir}
	# hadd Merged.root ${MergeList}
	hadd Merge_${OutFile}_${SGE_TASK_ID}.root ${MergeList}
	if [ $? -eq 0 ]; then
        ls -lh
		# echo "mv ${TMP}/Merged.root ${OutDir}/Merge_${OutFile}_${SGE_TASK_ID}.root"
		# mv ${TMP}/Merged.root ${OutDir}/Merge_${OutFile}_${SGE_TASK_ID}.root
		echo "###############################################################################################"
		echo "						Merging terminated successfully"
		echo "###############################################################################################"
	
	else
		echo "###############################################################################################"
		echo "					Merging failed"
		echo "###############################################################################################"
		exit 100
	fi
else
	n_miss=0
	MergeList=""
	for i in  $(seq 1 ${nJobs});do
    echo "Look for :"${In}/Merge_${InFile}_${i}.root
    if [ ! -s ${In}/Merge_${InFile}_${i}.root ]; then
        echo "file $i is missing!!" >> ${OutDir}/MISSING.txt
		echo "file $i is missing!!"
        let "n_miss += 1"
	fi  
		MergeList="${MergeList} ${In}/Merge_${InFile}_${i}.root"
	done
	if [ $n_miss -eq 0 ];then
		if [ ${nJobs} -eq 1 ];then
			echo "Found only  one job as input move the input File"
			echo "mv ${In}/Merge_${InFile}_${i}.root ${OutDir}/${OutFile}.root"
			mv ${In}/Merge_${InFile}_${i}.root ${OutDir}/${OutFile}.root
		else
			hadd "${OutDir}/${OutFile}.root" ${MergeList}
			if [ $? -eq 0 ]; then
				echo "###############################################################################################"
				echo "						Merging terminated successfully"
				echo "###############################################################################################"	
			else
				echo "###############################################################################################"
				echo "					Merging failed"
				echo "###############################################################################################"
				exit 100
			fi
		fi
	else
		echo "###############################################################################################"
		echo "				Merging failed"
		echo "###############################################################################################"
		exit 100
	fi
fi
exit

echo "checking if all files are here and healthy..."
if [ -e ${OutDir}/${Name}.root ];then
	echo "Cleaning up the old output file in target folder"
	echo "rm ${OutDir}/${Name}.root"
	rm ${OutDir}/${Name}.root
fi
if [ -e ${OutDir}/merge.status.log ];then 
	echo "rm ${OutDir}/merge.status.log"
	rm ${OutDir}/merge.status.log
fi
n_miss=0
InFiles=""
for i in `seq 1 $nJobs`
do
    echo "Look for :"${InDir}/${Name}_${i}.root
    if [ ! -s ${InDir}/${Name}_${i}.root ]
    then
        echo "file $i is missing!!" >> ${OutDir}/MISSING.txt
		echo "file $i is missing!!"
        let "n_miss += 1"
	fi  
	InFiles="${InFiles} ${InDir}/${Name}_${i}.root"
done
if [ $n_miss -eq 0 ];then
	echo "Starting to merge the Files"
	echo "hadd \"${OutDir}/${Name}.root\" ${InFiles}"
	hadd "${OutDir}/${Name}.root" ${InFiles}
	if [ $? -eq 0 ]; then
		echo "###############################################################################################"
		echo "						Merging terminated successfully"
		echo "###############################################################################################"
	
	else
		echo "###############################################################################################"
		echo "					Merging failed"
		echo "###############################################################################################"
		exit 100
	fi
else
	
fi
