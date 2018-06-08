#!/bin/bash
# some initial output
echo "###############################################################################################"
echo "					BUILD JOB of SGE batch JOB ${Name}"
echo "###############################################################################################"
echo "USER"=${USER}
echo "HOME-AREA="${HOME}
echo "RUCIO_ACCOUNT"=${RUCIO_ACCOUNT}
echo "RC-Area="${RCArea}
echo "HostName="${HOSTNAME}
echo "COPYAREA="${COPYAREA}
echo "RCRelease="${RCRelease}
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
echo "						Look for packages in the ROOTCORE area located at ${RCArea}"
echo "###############################################################################################"
echo "mkdir -p ${COPYAREA}"
mkdir -p ${COPYAREA}
cd ${COPYAREA}
echo " "
echo " "
echo " "
echo "###############################################################################################"
ls -lh ${RCArea}
echo "###############################################################################################"

Packages=`ls ${RCArea}`
for P in ${Packages};do
	if [ ! -d ${RCArea}/${P} ];then
		continue
	fi
	# if [ ${P} == "RootCoreBin" ];then
		# continue
	# fi
	if [ ${P} == "TarBall.tgz" ];then
		continue
	fi
	echo "Copy ${P} from ${RCArea} to ${COPYAREA}"
	cp -r -p ${RCArea}/${P} ${COPYAREA}
done
echo "###############################################################################################"
ls -lh ${COPYAREA}
echo "###############################################################################################"
echo " "
echo "###############################################################################################"
echo "						Setup the ROOTCORE release"
echo "###############################################################################################"
echo "rcSetup ${RCRelease}"
rcSetup ${RCRelease}
rc find_packages
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
rc compile --single-thread
if [ $? -eq 0 ]; then
	echo "###############################################################################################"
	echo "						BuildJob finished will now proceed with the analysis Jobs"
	echo "###############################################################################################"
	exit 0

else
	echo "###############################################################################################"
	echo "						BuildJob has ended with an error... Analysis jobs will be stopped"
	echo "###############################################################################################"
	echo "Will delete the current working area"
	if [ -d ${COPYAREA} ]; then
		rm -rf ${COPYAREA}
	fi
	exit 100
fi


