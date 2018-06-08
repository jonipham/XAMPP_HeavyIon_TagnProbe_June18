#!/bin/bash
# some initial output
echo "###############################################################################################"
echo "					BUILD JOB of SGE batch JOB ${Name}"
echo "###############################################################################################"
echo "USER"=${USER}
echo "HOME-AREA="${HOME}
echo "RUCIO_ACCOUNT"=${RUCIO_ACCOUNT}
echo "OriginalArea="${OriginalArea}
echo "AtlasProject"=${AtlasProject}  #e.g. AthAnalysisBase
echo "AtlasVersion="${AtlasVersion} #e.g. 2.4.30
echo "HostName="${HOSTNAME}
echo "COPYAREA="${COPYAREA}
echo "SGE_STDOUT_PATH="${SGE_STDOUT_PATH}
echo "SGE_STDERR_PATH="${SGE_STDERR_PATH}

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


echo "###############################################################################################"
echo "						Remove obsolete Files from previous jobs"
echo "###############################################################################################"
echo "Remove obsolete LogFiles"
if [ -d  ${CleanLogs} ];then
	Logs=`ls ${CleanLogs}`
	for L in ${Logs};do
		if [ -d  ${CleanLogs}/${L} ];then
			rm -rf ${CleanLogs}/${L}
			continue
		fi
		if [[ ${SGE_STDOUT_PATH} == *"${L}"* ]];then
			echo "		Skip ${L}"
			continue
		fi
		if [[ ${SGE_STDERR_PATH} == *"${L}"* ]];then
			echo "		Skip ${L}"
			continue
		fi		
	rm ${CleanLogs}/${L}
	done
    #Release memory
    Logs=""
fi
if [ -d ${COPYAREA} ];then
	echo "Remove the old RootCore area"
	rm -rf ${COPYAREA}
fi
if [ -d ${CleanOut} ];then
	echo "Remove also the old Output area"
	rm -rf  ${CleanOut}
fi
mkdir -p  ${CleanOut}

if [ -d ${CleanTmp} ];then
	echo "Remove also the old Temp area "${CleanTmp/%\//}
    Files=`ls ${CleanTmp}`
    for F in ${Files};do
        if [ "${F/%.root/}" != "${F}" ];then
            rm ${CleanTmp}/${F}
        fi
    done
fi
mkdir -p  ${CleanTmp}
echo "###############################################################################################"
echo "						Look for the athena area located at ${TestArea}"
echo "###############################################################################################"
echo "mkdir -p ${COPYAREA}/source"
mkdir -p ${COPYAREA}/source
echo "mkdir -p ${COPYAREA}/build"
mkdir -p ${COPYAREA}/build
echo "cd ${COPYAREA}/source"
cd ${COPYAREA}/source
echo " "
echo " "
echo " "
echo "###############################################################################################"
ls -lh ${OriginalArea}
echo "###############################################################################################"

Packages=`ls ${OriginalArea}`
for P in ${Packages};do
	if [ ! -d ${OriginalArea}/${P} ];then
		continue
	fi
    ####################################################
    #           Do not copy the Install and workarea
    ####################################################
    if [ "${P}" == "InstallArea" ];then
        continue
    fi
	if [ "${P}" == "WorkArea" ];then
        continue
    fi	
    if [ "${P}" == "TarBall.tgz" ];then
		continue
	fi
    if [ "${P}" == "CMakeFiles" ];then
        continue
    fi
    if [ "${P}" == "x86_64-slc6-gcc49-opt" ];then
        continue
    fi    
	echo "Copy ${P} from ${TestArea} to ${COPYAREA}"
	cp -r -p ${OriginalArea}/${P} ${COPYAREA}/source
done
echo "###############################################################################################"
ls -lh ${COPYAREA}/source/
echo "###############################################################################################"
echo " "
echo "###############################################################################################"
echo "						Setup the Athena release"
echo "###############################################################################################"
echo "asetup ${AtlasProject},${AtlasVersion},here"
source $AtlasSetup/scripts/asetup.sh ${AtlasProject}, ${AtlasVersion}, here


#Compile in release 21
#
if [ -z "${CMTBIN}" ];then
    echo "cd ${COPYAREA}/build"
    cd ${COPYAREA}/build
    cmake ../source/
    if [ $? -ne 0 ];then
        echo "###############################################################################################"
        echo "						BuildJob has ended with an error... Analysis jobs will be stopped"
        echo "###############################################################################################"
        echo "Will delete the current working area"
        if [ -d ${COPYAREA} ]; then
            rm -rf ${COPYAREA}
        fi
        exit 100        
    fi
    make -j2
    if [ $? -ne 0 ];then
        echo "###############################################################################################"
        echo "						BuildJob has ended with an error... Analysis jobs will be stopped"
        echo "###############################################################################################"
        echo "Will delete the current working area"
        if [ -d ${COPYAREA} ]; then
            rm -rf ${COPYAREA}
        fi
        exit 100        
    fi    
else
    #Compile in release 20.7
    echo "setupWorkArea.py"
    setupWorkArea.py
    echo "cd WorkArea/cmt"
    cd WorkArea/cmt
    echo "cmt br cmt config"
    cmt br cmt config
    if [ $? -ne 0 ];then
        echo "###############################################################################################"
        echo "						BuildJob has ended with an error... Analysis jobs will be stopped"
        echo "###############################################################################################"
        echo "Will delete the current working area"
        if [ -d ${COPYAREA} ]; then
            rm -rf ${COPYAREA}
        fi
        exit 100        
    fi 
    echo "cmt br cmt make"
    cmt br cmt make -j 1
    if [ $? -ne 0 ];then
        echo "###############################################################################################"
        echo "						BuildJob has ended with an error... Analysis jobs will be stopped"
        echo "###############################################################################################"
        echo "Will delete the current working area"
        if [ -d ${COPYAREA} ]; then
            rm -rf ${COPYAREA}
        fi
        exit 100        
    fi 
fi
echo "###############################################################################################"
echo "						BuildJob finished will now proceed with the analysis Jobs"
echo "###############################################################################################"
exit 0


