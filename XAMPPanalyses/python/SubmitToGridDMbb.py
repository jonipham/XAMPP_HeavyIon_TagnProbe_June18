import os
import commands
import subprocess
import math
import sys
import re
import argparse
from datetime import datetime

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
TODAY=commands.getoutput("date --iso")
CREATETARBALL=(os.path.isfile(WORKDIR+"TarBall.tgz")==False)
TIME = commands.getoutput("date +'%Y-%m-%d_%H:%M:%S'")
# disableAutoRetry is only used temporary for testing
#PRUN_Options= ["--workDir=\"%s\""%WORKDIR  ,"--useRootCore", "--cmtConfig=%s"%ROOTCORECONFIG, "--useNewCode" ,"--writeInputToTxt=\"IN:input.txt\"" , "--rootVer="+ROOTVER, "--disableAutoRetry" ]
PRUN_Options = ["--useRootCore" , "--cmtConfig=%s"%ROOTCORECONFIG , "--writeInputToTxt=\"IN:input.txt\"" , "--rootVer="+ROOTVER , "--disableAutoRetry"]
EXCLUDEDSITE='ANALY_BNL_LONG,ANALY_BNL_SHORT,ANALY_SLAC,ANALY_MWT2_SL6,ANALY_RAL_SL6'

if CREATETARBALL==True:
        PRUN_Options.append("--maxFileSize=107374182400")
        PRUN_Options.append("--extFile=*.root")
        PRUN_Options.append("--outTarBall=TarBall.tgz")        
else:
        PRUN_Options.append("--inTarBall=TarBall.tgz")
if PANDASYS == None:
        print "Please Setup PandaClient"
        exit(1)
if ROOTCOREBIN == None:
        print "Please Setup ROOTCORE"
        exit(1)
        
def prettyPrint(preamble,data,width=30,separator=":"):
    """Prints uniformly-formatted lines of the type "preamble : data".
    """
    preamble = preamble.ljust(width)
    print '%s%s %s'%(preamble,separator,data)

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

def ReadListFromFile (File , Campaign="mc15_13TeV"):
        List=[]
        if os.path.isfile(File) == True:
                for Line in open(File):
                        if Line[0] is not "#" and Campaign in Line:
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
        
        parser = argparse.ArgumentParser(description='This script submits the JVT analysis code to the grid. For more help type \"python SUSYFwMPI/scripts/SubmitToGrid.py -h\"',prog='runJvt')
        parser.add_argument('-i','--inputDS',help='input dataset or DS list present in \"XAMPPbase/data/SampleLists/mc15c_13TeV/\"',required=True)
        parser.add_argument('--Test',help='run in test modus, i.e. without DS replication',action='store_true',default=False)
        parser.add_argument('--DSCampaign',help='choose campaign of DS production in order to adjust the filepath to the DS list (default is \"mc15_13TeV\")',default='mc15_13TeV')
        parser.add_argument('-j','--jobName',help='specify a job name which gets included in the name of the output dataset (default is analysis name)')
        parser.add_argument('-o','--outputSuffix',help='suffix name of the output file (default is \"AnalysisOutput.root\")',default='AnalysisOutput.root')
        parser.add_argument('--destSE',help='specify a destination for replication apart from MPPMU_LOCALGROUPDISK (or \"-1\" for no replication)',default='AUSTRALIA-ATLAS_LOCALGROUPDISK')
        parser.add_argument('--outDS',help='specify a name which overwrites the naming scheme for the outDS used by this script internally (only the \"user.<username>.\" remains, the string given here gets appended and nothing more',default='')

        parser.add_argument('-a','--analysis',help='choose the analysis to run, for Custom (default) you have to setup the cutflow the others have a std cutflow that can be used',choices=['Custom','Stop0L','Stop0L_SR','Stop0L_1LCR','Stop0L_2LCR','JVT_2L','DMbb'],default='DMbb')
        parser.add_argument('--debug',help='run in debug modus',action='store_true',default=False)
        parser.add_argument('--noSyst',help='run without systematic uncertainties',action='store_true',default=False)
        parser.add_argument('--isData',action='store_true',default=False)
        parser.add_argument('--isAF2',action='store_true',default=False)
        #parser.add_argument('--is25ns',action='store_true',default=True)
        parser.add_argument('--nevents',type=int,help="number of events to process for all the datasets")
        parser.add_argument('--skipEvents',type=int,help="skip the first n events")
        parser.add_argument('--noRecluster',help='disable jet reclustering during running',action='store_true',default=False)
        parser.add_argument('--noPRW',help='disable pileup-reweighting during running',action='store_true',default=False)
        parser.add_argument('--noTruth',help='disable truth particle analysis during running',action='store_true',default=False)
        parser.add_argument('--noHistos',help='disable writing of histograms (apart from cutflows) in analysis output file',action='store_true',default=False)
        parser.add_argument('--noTrees',help='disable writing of trees in analysis output file',action='store_true',default=False)
        parser.add_argument('--noxAODs',help='disable writing of xAODs in analysis output file',action='store_true',default=True)
        parser.add_argument('--noTrackMET',help='disable track MET retrieve and usage during running',action='store_true',default=False)
        parser.add_argument('--exSite',help='blacklist some specific sites',action='store_true',default=False)

        RunOptions = parser.parse_args()
        
        print("################################################################################################################################################################")
        print("                                                                        SUSYFwMPI on the Grid")
        print("################################################################################################################################################################")
        prettyPrint('USER',USER)
        prettyPrint('RUCIO',RUCIO)
        prettyPrint('ROOTSYS',ROOTSYS)
        prettyPrint('ROOTVER',ROOTVER)
        prettyPrint('ROOTCOREBIN',ROOTCOREBIN)
        prettyPrint('ROOTCOREDIR',ROOTCOREDIR)
        prettyPrint('ROOTCORECONFIG',ROOTCORECONFIG)
        prettyPrint('WORKINGDIR',WORKDIR)
        prettyPrint('TODAY',TODAY)

        
        InDSList = RunOptions.inputDS
        
        JobName = RunOptions.analysis
        if RunOptions.jobName != None and RunOptions.jobName != "":
                JobName = RunOptions.jobName
        
        CMS = 13

        GroupDisk = True
        if RunOptions.destSE == '-1':
                GroupDisk= False
        Duplicate = True
        
        TestMode = False
        if RunOptions.Test:
                TestMode = True
        RunWithSys = True
        if RunOptions.noSyst:
                RunWithSys = False
        InputDataSets= []
        AnalysisOptions = []
        FilesPerJob=10
        nJobs=0
        OutFiles = [RunOptions.outputSuffix]
        SysList = []
        Campaign = RunOptions.DSCampaign             
        
        #Check if There is a JobName Given
        if len(JobName)==0:
                print "ERROR: Please give a JobName"
                exit(1)
           
        AnalysisOptions.append("-i input.txt")
        AnalysisOptions.append("-a %s"%RunOptions.analysis)
        if RunOptions.debug:
                AnalysisOptions.append("--debug")
        if RunOptions.noSyst:
                AnalysisOptions.append("--noSyst")
        if RunOptions.isData:
                AnalysisOptions.append("--isData")
        if RunOptions.isAF2:
                AnalysisOptions.append("--isAF2")
        #if RunOptions.is25ns:
        #        AnalysisOptions.append("--is25ns")
        if RunOptions.noRecluster:
                AnalysisOptions.append("--noRecluster")
        if RunOptions.noPRW:
                AnalysisOptions.append("--noPRW")
        if RunOptions.noTruth:
                AnalysisOptions.append("--noTruth")
        if RunOptions.noHistos:
                AnalysisOptions.append("--noHistos")
        if RunOptions.noTrees:
                AnalysisOptions.append("--noTrees")
        if RunOptions.noxAODs:
                AnalysisOptions.append("--noxAODs")
        if RunOptions.noTrackMET:
                AnalysisOptions.append("--noTrackMET")
        if RunOptions.exSite:
                PRUN_Options.append("--excludedSite=%s"%EXCLUDEDSITE)           
        if RunOptions.nevents : 
                AnalysisOptions.append("--nevents %i"%RunOptions.nevents)
        if RunOptions.skipEvents :
                AnalysisOptions.append("--skipEvents %i"%RunOptions.skipEvents)
        AnalysisOptions.append("--runModus grid")
        
        # Read in the InputDataSets
        # try to find file first at default directory:
        InDSListDir = "/coepp/cephfs/mel/laram1/XAMPP/XAMPPbase/data/SampleLists/mc15c_13TeV/"
	print InDSListDir, "is where the file should be"
        if os.path.exists(InDSListDir)==False:
                print "ERROR: Campaign is invalid. Could not find the directory containing the sample lists."
                exit(1)
        # check if inputDS was a file
        if os.path.exists(InDSListDir+InDSList):
	    print "found the file"
            InputDataSets.extend( ReadListFromFile(InDSListDir+InDSList,Campaign) )
        # if not, it should be a DS or a comma-separated list of DS
        else:
            if ',' in InDSList:
                for DS in InDSList.split(','):
                    InputDataSets.append(DS)
            else:
                InputDataSets.append(InDSList)
#         if InDSList.find(InDSListDir) > -1:
#                 print "Info: The right path is given to the Script"
#         else:
#                 InDSList = InDSListDir+InDSList
        
        if len(InputDataSets) == 0:
            prettyPrint('ERROR','No input dataset found')
            exit(1)
        
        PRUN_Options.append("--inDS=\"%s\""%ConvertListToString(InputDataSets ,","))
        
        #Sets the OutputDataSetName
        if RunOptions.outDS == '':
                if len(InputDataSets)==1:
                        DSID = InputDataSets[0].split(".")[1].split(".")[0]
                        TAG = InputDataSets[0].split(".")[-1].split("/")[0]
                        print 'submit of single DSID: %s with tag: %s'%(DSID,TAG)
                        OutDS = "user.%s.%s.%s.XAMPPbase.%s.%s"%(RUCIO,DSID,TAG,TODAY,JobName)#OutDS = "user.%s.%s.%s.SUSYFwMPI.%s.%s"%(RUCIO,DSID,TAG,TODAY,JobName)
                else:
                       OutDS = "user.%s.%s.XAMPPbase.%s"%(RUCIO,TODAY,JobName) #OutDS = "user.%s.%s.SUSYFwMPI.%s"%(RUCIO,TODAY,JobName)
        else:
                OutDS = "user.%s.%s"%(RUCIO,RunOptions.outDS)
        PRUN_Options.append("--outDS=\"%s\""%OutDS)
        
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
#         PRUN_Options.append("--mergeOutput")

        PRUN_Options.append("--outputs=\"%s\""%ConvertListToString(OutFiles ,","))
        
        print("################################################################################################################################################################")
        print("                                                                        JobOptions")        
        print("################################################################################################################################################################")
        prettyPrint('JobName',JobName)
        prettyPrint('Sqrt (s)',str(CMS))
        prettyPrint('OutputContainer',OutDS)
        prettyPrint('RunWithSystematics',BoolToString(RunWithSys))
        prettyPrint('Analysis',str(RunOptions.analysis))

        prettyPrint('CreateTarBall',BoolToString(CREATETARBALL))
        if CREATETARBALL == False:
            prettyPrint('','Using already existing tarball located at:',width=32,separator='->')
            prettyPrint('','%sTarBall.tgz'%WORKDIR,width=34,separator='')
            
        prettyPrint('FilesPerJob',str(FilesPerJob))
        if nJobs>0:
            prettyPrint('NumberOfJobs',str(nJobs))
            
        prettyPrint('InputDataSets:','',separator='')
        for ds in InputDataSets:
            prettyPrint('',ds,width=32,separator='-')
        prettyPrint('OutputFiles:','',separator='')
        for file in OutFiles:
            prettyPrint('',file,width=32,separator='-')
            
        print("################################################################################################################################################################")

        os.chdir(WORKDIR)
        AnalysisOptions.reverse()
        
#         print PRUN_Options
#         print ConvertListToString(PRUN_Options , " ")
        Command = "prun --exec=\"python XAMPPanalyses/python/runDMbb.py %s\" %s "%(ConvertListToString (AnalysisOptions, " ") , ConvertListToString(PRUN_Options , " ") )
        print '\nSubmitting using command:\n%s\n...'%Command
        os.system(Command)
        exit(0)
