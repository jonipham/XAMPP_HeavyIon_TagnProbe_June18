#!/bin/bash
# source this before your first grid submit in this shell
if [ -d "${ROOTCOREBIN}" ];then
	cd ${ROOTOCREBIN}/../
elif [ -d "${TestArea}" ];then
	cd ${TestArea}
fi
localSetupPandaClient
#localSetupPyAMI
localSetupRucioClients
voms-proxy-init -voms atlas

# now proceed with 'python XAMPPbase/python/SubmitToGrid.py -i <name of DS list> -a <name of analysis>'
