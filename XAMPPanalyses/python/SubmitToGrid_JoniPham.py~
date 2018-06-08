#! /usr/bin/env python
from XAMPPbase.runSUSYAnalysis import *

import os
import commands
import subprocess
import math
import sys
import re
import argparse
from datetime import datetime
import time

USER = os.getenv("USER")
RUCIO = os.getenv("RUCIO_ACCOUNT")
ROOTSYS= os.getenv("ROOTSYS").replace("%s/root/"%os.getenv("ATLAS_LOCAL_ROOT"),"")
ROOTCOREBIN = os.getenv("ROOTCOREBIN")
ROOTCOREDIR = os.getenv("ROOTCOREDIR")
ROOTRELEASE = ROOTCOREBIN.replace("/cvmfs/atlas.cern.ch/repo/sw/ASG/","").replace("RootCore","")
ROOTCORECONFIG = os.getenv("ROOTCORECONFIG")
ROOTVER = ROOTSYS.replace("-"+ROOTCORECONFIG,"")
PANDASYS = os.getenv("PANDA_SYS")
WORKDIR = "%s/../"%(ROOTCOREBIN)
#TODAY=commands.getoutput("date --iso")
TODAY= str(time.strftime("%d-%m-%Y"))
CREATETARBALL=(os.path.isfile(WORKDIR+"TarBall.tgz")==False)
#CREATETARBALL=True
TIME = commands.getoutput("date +'%Y-%m-%d_%H:%M:%S'")
# disableAutoRetry is only used temporary for testing
#PRUN_Options= ["--workDir=\"%s\""%WORKDIR  ,"--useRootCore", "--cmtConfig=%s"%ROOTCORECONFIG, "--useNewCode" ,"--writeInputToTxt=\"IN:input.txt\"" , "--rootVer="+ROOTVER, "--disableAutoRetry" ]
PRUN_Options = ["--useRootCore" , "--cmtConfig=%s"%ROOTCORECONFIG , "--writeInputToTxt=\"IN:input.txt\"" , "--rootVer="+ROOTVER , "--disableAutoRetry"]
#EXCLUDEDSITE='ANALY_IN2P3-CC,ANALY_INFN-T1'
#EXCLUDEDSITE='ANALY_BNL_LONG,ANALY_BNL_SHORT,ANALY_SLAC,ANALY_MWT2_SL6,ANALY_RAL_SL6,ANALY_TRIUMF,ANALY_LPSC,ANALY_MWT2_SL6,ANALY_SiGNET_DIRECT,ANALY_TOKYO,ANALY_CERN_SLC6,ANALY_CERN_SHORT,ANALY_IN2P3-CC,ANALY_CONNECT,ANALY_CONNECT_SHORT,ANALY_PIC_SL6,ANALY_AGLT2_SL6'
EXCLUDEDSITE='ANALY_UNIBE-LHEP-UBELIX'
RUNONSITE='AUSTRALIA-ATLAS_LOCALGROUPDISK,AUSTRALIA-ATLAS_SCRATCHDISK'
#EXCLUDEDSITE='ANALY_INFN-T1'

if CREATETARBALL==True:
#        PRUN_Options.append("--maxFileSize=107374182400")
        PRUN_Options.append("--extFile=*.root")
        PRUN_Options.append("--outTarBall=TarBall.tgz")    
#        PRUN_Options.append("--excludeFile='*.svn*'")       
        PRUN_Options.append(" --workDir='%s'" % (WORKDIR))
    
else:
        PRUN_Options.append("--inTarBall=TarBall.tgz")
if PANDASYS == None:
        print "Please Setup PandaClient"
        exit(1)
if ROOTCOREBIN == None:
        print "Please Setup ROOTCORE"
        exit(1)

def AddToAnalysisOption (Name, Opt):
        AnalysisOptions.append("--%s"%Name)
        AnalysisOptions.append(Opt)
        return StringToBool(Opt)
def StringToBool (Str):
        if Str.lower() == "true":
                return True
        return False
def BoolToString (Boolean):
        if Boolean==True:
                return "true"
        return "false"
def CheckForOpt (Option):
        for Opt in Option.split():
                if "GetTrees" in Opt:
                        return True
                if "GetHistos" in Opt:
                        return True
                if "SystematicSet" in Opt:
                        return True
        return False
def CreateSystematicList (Options):
        Command= "%s/bin/%s/4LGetSystematics"%(ROOTCOREBIN,ROOTCORECONFIG) 
        for Opt in Options:
                Command="%s %s"%(Command,Opt)
        return subprocess.call(Command,shell=True)

#def ReadListFromFile (File , Campaign="mc15c_13TeV"):
def ReadListFromFile (File , Campaign):
        List=[]
        if os.path.isfile(File) == True:
                for Line in open(File):
			print "Print content of the text file: ", Line
			#print "Print Line[0]: ", Line[0]
#                        if Line[0] is not "#" and Campaign in Line:
#                               List.append(Line.replace("\n","") )
#				#print "Print List: ", List
#                        if Line[0] is not "#" and "mc15_13TeV" in Line:
#                                List.append(Line.replace("\n","") )
#				#print "Print List: ", List

########## There is an inconsistency between the Campaign name (mc15c_13TeV) and the first part of the data set. That's why I changed the script like this: 
                        if Line[0] is not "#":
				if Campaign is "mc15c_13TeV" and "mc15_13TeV"  in Line:
                                	List.append(Line.replace("\n","") )
				else:
					if Campaign in Line  or "data16_hip8TeV" in Line:
                                		List.append(Line.replace("\n","") )
				

        else:
		print "Could not find ListFile "+str(File)
        return List
def ConvertListToString(List , Sep=","):
        Str=""
        for E in List:
                Str=E+Sep+Str
        return Str[0 : len(Str) -1 ]
if __name__=='__main__':
        
        tstart = datetime.now()
        print str(tstart) + ' : Job started'
        
#        parser = argparse.ArgumentParser(description='This script submits the stop-0L analysis code to the grid. For more help type \"python SUSYFwMPI/scripts/SubmitToGridFra.py -h\"',prog='runStop0Lep')
        parser = argparse.ArgumentParser(description='This script submits the stop-0L analysis code to the grid. For more help type \"python SUSYFwMPI/scripts/SubmitToGridFra.py -h\"',prog='runJVT')
#        parser.add_argument('-i','--inputDS',help='input dataset or DS list present in \"XAMPPbase/data/mc15c_13TeV/SampleLists/\"',required=True)
        parser.add_argument('-i','--inputDS',help='input dataset or DS list present in \"data/XAMPPbase/SampleLists/mc15c_13TeV/\"',required=True)
        parser.add_argument('--DSCampaign',help='choose campaign of MC production in order to adjust the filepath to the DS list (default is \"mc15c_13TeV\")',default='mc15c_13TeV')
        parser.add_argument('-j','--jobName',help='specify a job name which gets included in the name of the output dataset (default is analysis name)')
        parser.add_argument('-o','--outputSuffix',help='suffix name of the output file (default is \"AnalysisOutput.root\")',default='AnalysisOutput.root')
        parser.add_argument('-a','--analysis',help='choose the analysis to run, for Custom (default) you have to setup the cutflow the others have a std cutflow that can be used',choices=['HeavyIon','Custom','Stop0L','Stop0L_SR','Stop0L_1LCR','Stop0L_2LCR','JVT_2L','JVT_SR'],default='HeavyIon')
        parser.add_argument('--destSE',help='specify a destination for replication apart from MPPMU_LOCALGROUPDISK (or \"-1\" for no replication)',default='MPPMU_LOCALGROUPDISK')
        parser.add_argument('--debug',help='run in debug modus',action='store_true',default=False)
        parser.add_argument('--noSyst',help='run without systematic uncertainties',action='store_true',default=False)
        parser.add_argument('--noBtag', help='If you switch this option on then no Btag weights will be calculated', action='store_true', default=False)
        parser.add_argument('--noSF', help='This option disables the SFs entirely', action='store_true', default=False)
        parser.add_argument('--isData',action='store_true',default=False)
        parser.add_argument('--isAF2',action='store_true',default=False)
	parser.add_argument('--isMC15a', action='store_true', default=False)
        parser.add_argument('--nevents',type=int,help="number of events to process for all the datasets")
        parser.add_argument('--skipEvents',type=int,help="skip the first n events")
        parser.add_argument('--noRecluster',help='disable jet reclustering during running',action='store_true',default=False)
        parser.add_argument('--noPRW',help='disable pileup-reweighting during running',action='store_true',default=False)
        parser.add_argument('--Test',help='run in test modus, i.e. without DS replication',action='store_true',default=False)
        parser.add_argument('--exSite',help='blacklist some specific sites',action='store_true',default=False)
        parser.add_argument('--mergeOutput',help='no merging outputs',action='store_false',default=True)
        parser.add_argument('--RunOnSite',help='send job to a site, if it is AUTO, jobs will go to the site which holds the most number of input files',action='store_true',default=False)
        #parser.add_argument('--noTrackMET',help='disable track MET retrieve and usage during running',action='store_true',default=False)
        parser.add_argument('--IsPrimAOD', help='the input file-type is the primary xAOD', action='store_true', default=False)
        parser.add_argument('--nFiles', help='use an limited number of files in the input dataset', action='store_true', default=False)
        #parser.add_argument('--PRWfiles' , help='List of the PRW input files', nargs='+' , default=[ 'dev/SUSYTools/merged_prw_mc15c_July27_afterFix.root'])
        parser.add_argument('--PRWfiles' , help='List of the PRW input files', nargs='+' , default=[ 'dev/SUSYTools/merged_prw_mc15c_latest.root'])
        RunOptions = parser.parse_args()
        
        print("################################################################################################################################################################")
        print("                                                                        SUSYFwMPI on the Grid")
        print("################################################################################################################################################################")
        print ("USER:                        "+USER)
        print ("RUCIO:                        "+RUCIO)
        print ("ROOTSYS:                "+ROOTSYS)
        print ("ROOTVER:                "+ROOTVER)
        print ("ROOTCOREBIN:                "+ROOTCOREBIN)
        print ("ROOTCOREDIR:                "+ROOTCOREDIR)
        
        print ("ROOTCORECONFIG:                "+ROOTCORECONFIG)
        print ("WORKINGDIR:                "+WORKDIR)
        print ("TODAY:                        "+TODAY)        
        
        
        InDSList = RunOptions.inputDS
        
        JobName = RunOptions.analysis
        if RunOptions.jobName != None and RunOptions.jobName != "":
                JobName = RunOptions.jobName
        
        CMS = 13

        GroupDisk = False
        if RunOptions.destSE == '-1':
                GroupDisk= False
        Duplicate = False
        
        TestMode = False
        if RunOptions.Test:
                TestMode = True
        RunWithSys = True
        if RunOptions.noSyst:
                RunWithSys = False
        
        InputDataSets= []
        AnalysisOptions = []
        FilesPerJob=1
        nJobs=0
        OutFiles = [RunOptions.outputSuffix]
        SysList = []
        NumOfSys=0
        #Executable="runStop0Lep"        
        Campaign = RunOptions.DSCampaign             
        
        #Check if There is a JobName Given
        if len(JobName)==0:
                print "ERROR: Please give a JobName"
                exit(1)
        
        #Parse the CMS energy of the analysis --> needed for HistoManager / Crosssection
        #AnalysisOptions.append("--AnalysisRelease ")
        #AnalysisOptions.append(ROOTRELEASE)
        #AnalysisOptions.append("-s")        
        #AnalysisOptions.append(str(CMS))        
        AnalysisOptions.append("-i input.txt")
        AnalysisOptions.append("-a %s"%RunOptions.analysis)
        if RunOptions.debug:
                AnalysisOptions.append("--debug")
        if RunOptions.noSyst:
                AnalysisOptions.append("--noSyst")
        if RunOptions.noSF:
                AnalysisOptions.append("--noSF")
        if RunOptions.noBtag:
                AnalysisOptions.append("--noBtag")
        if RunOptions.isData:
                AnalysisOptions.append("--isData")
        if (RunOptions.isAF2):
        # or (re.search('.*\.e[0-9]+_a[0-9]+_.*', RunOptions.inputDS)):
                AnalysisOptions.append("--isAF2")
        if RunOptions.noRecluster:
                AnalysisOptions.append("--noRecluster")
        if RunOptions.noPRW:
                AnalysisOptions.append("--noPRW")
                
        if RunOptions.noPRW == False and len(RunOptions.PRWfiles) > 0:
                AnalysisOptions.append("--PRWfiles " + ConvertListToString(RunOptions.PRWfiles, " "))
       # if RunOptions.noTrackMET:
         #       AnalysisOptions.append("--noTrackMET")
        if RunOptions.IsPrimAOD:
                AnalysisOptions.append("--IsPrimAOD")
                
        if RunOptions.nevents : 
                AnalysisOptions.append("--nevents %i"%RunOptions.nevents)
        if RunOptions.skipEvents :
                AnalysisOptions.append("--skipEvents %i"%RunOptions.skipEvents)
        AnalysisOptions.append("--runModus grid")
        
        #Read in the InputDataSets
        InDSListDir = "%s/data/XAMPPbase/SampleLists/%s/"%(ROOTCOREBIN, Campaign)
        if os.path.exists(InDSListDir)==False:
                print "ERROR: Campaign is invalid. Could not find the SampleListDir"
                exit(1)
        if InDSList.find(InDSListDir) > -1:
                print "Info: The right path is given to the Script"
        else:
                InDSList = InDSListDir+InDSList
        InputDataSets.extend( ReadListFromFile(InDSList,Campaign) )
	content = ReadListFromFile(InDSList,Campaign) 
	print "InDSListDir: ", InDSListDir
	print "InDSList: ", InDSList
	print "Campaign: ", Campaign
	print "ReadListFromFile: ", content
	print "InputDataSets: ", InputDataSets
        if len(InputDataSets) ==0:
                print "ERROR: No Input DataSet found"
                exit(1)

        if len(InputDataSets) >1 and CREATETARBALL==True:
                print "ERROR: You are creating the tarball, submit only one sample"
                exit(1)
        
        #Additional Options parsing to the prun Command
        if TestMode== True:
                GroupDisk = False
                Duplicate = False
                FilesPerJob=1
                nJobs=1
                PRUN_Options.append("--disableAutoRetry")
        if GroupDisk==True:
                PRUN_Options.append("--destSE=%s"%RunOptions.destSE)
        if Duplicate== True:
                PRUN_Options.append("--allowTaskDuplication")
        if nJobs > 0:
                PRUN_Options.append("--nJobs=%d "%nJobs)
                PRUN_Options.append("--nFiles=%d "%nJobs)                
        PRUN_Options.append("--nFilesPerJob=%i"%FilesPerJob)
        if RunOptions.exSite:
                PRUN_Options.append("--excludedSite=%s"%EXCLUDEDSITE)
        if RunOptions.RunOnSite:
                PRUN_Options.append("--site=%s"%RUNONSITE)
        #if RunOptions.noMergeOutput:
        	#PRUN_Options.append("--mergeOutput=False")
                

        

        PRUN_Options.append("--outputs=\"%s\""%ConvertListToString(OutFiles ,","))
        print("################################################################################################################################################################")
        print("                                                                        JobOptions")        
        print("################################################################################################################################################################")
        print "JobName:                "+JobName        
        print "Sqrt (s):                "+str(CMS)
        print "RunWithSystematics:         "+BoolToString(RunWithSys)
        print "Analysis:                "+str(RunOptions.analysis)
        if RunWithSys==True:
                print "Run with systematics"
        
        print "CreateTarBall:                "+BoolToString(CREATETARBALL)
        print "FilesPerJob:                "+str(FilesPerJob)        
        if nJobs>0:
                print "NumberOfJobs:                "+str(nJobs)        
        for fi in OutFiles:
                print ("                        "+fi)
        print("################################################################################################################################################################")
        if CREATETARBALL == False:
            print 'Using already existing tarball located at %sTarBall.tgz'%WORKDIR
        os.chdir(WORKDIR)
        AnalysisOptions.reverse()
        #Sets the OutputDataSetName
        print 'submitting %s datasets'%( len(InputDataSets))
        for dataset in InputDataSets:
                DSID = dataset.split(".")[1].split(".")[0]
                TAG = dataset.split(".")[-1].split("/")[0]
                print 'DSID: %s with tag: %s'%(DSID,TAG)
                OutDS = "user.%s.%s.%s.SUSYFwMPI.%s.%s"%(RUCIO,DSID,TAG,TODAY,JobName)


                print "InputContainer:         "+dataset
                print "OutputContainer:        "+OutDS
                Command = "prun --exec=\"python XAMPPanalyses/python/runHeavyIon.py %s\" --inDS %s --outDS %s %s"%(ConvertListToString (AnalysisOptions, " ") , dataset, OutDS, ConvertListToString(PRUN_Options , " "))
                print '\nSubmitting using command:\n%s\n...'%Command
                os.system(Command)
        exit(0)
