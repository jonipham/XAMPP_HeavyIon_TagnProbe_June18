#! /usr/bin/env python
from __future__ import print_function

from XAMPPbase.runSUSYAnalysis import *
#from XAMPPbase.ListGroupDisk import *
#from XAMPPbase.SubmitToBatch import *

import PathResolver
import ROOT
import commands
import subprocess
import math
import sys
from pprint import pprint
PANDASYS = os.getenv("PANDA_SYS")
prettyPrint('PANDASYS', PANDASYS)
USER = os.getenv("USER")
prettyPrint('USER',USER)
RUCIO = os.getenv("RUCIO_ACCOUNT")
prettyPrint('RUCIO',RUCIO)
ROOTSYS = os.getenv("ROOTSYS").replace("%s/root/"%os.getenv("ATLAS_LOCAL_ROOT"),"")
prettyPrint('ROOTSYS',ROOTSYS)
ROOTCOREBIN = os.getenv("ROOTCOREBIN")
prettyPrint('ROOTCOREBIN',ROOTCOREBIN)
ROOTCOREDIR = os.getenv("ROOTCOREDIR")
prettyPrint('ROOTCOREDIR',ROOTCOREDIR)
ROOTRELEASE = ROOTCOREBIN.replace("/cvmfs/atlas.cern.ch/repo/sw/ASG/","").replace("RootCore","")
ROOTCORECONFIG = os.getenv("ROOTCORECONFIG")
prettyPrint('ROOTCORECONFIG',ROOTCORECONFIG)
ROOTVER = ROOTSYS.replace("-"+ROOTCORECONFIG,"")
if PANDASYS == None:
    print('Please setup Panda using "localSetupPandaClient"') 
    exit(1)


TODAY = commands.getoutput("date --iso")
CREATETARBALL = (os.path.isfile(WORKAREA + "/TarBall.tgz") == False)
TIME = commands.getoutput("date +'%Y-%m-%d_%H:%M:%S'")
PRUN_Options = ["--useRootCore" , "--cmtConfig=%s" % ROOTCORECONFIG , "--writeInputToTxt=\"IN:input.txt\"" , "--rootVer=" + ROOTVER]


TarList = []
if CREATETARBALL:
    TarList = ["--extFile=*.root", "--outTarBall=TarBall.tgz", "--excludeFile='*.svn*','*.git*','*.pyc','\*.*~','\*.tex','\*.tmp','\*.pdf','\*.png','\*.log','\*.dat','\*.core','\*README*'"]
    if ROOTCOREBIN: TarList.append(" --workDir='%s'" % (WORKAREA))
else: TarList = ["--inTarBall=TarBall.tgz"]
   
PRUN_Options.extend(TarList)

# add additional arguments to the argument parser of XAMPPbase.runSUSYAnalysis
def setupGridSubmitArgParser():
    parser = argparse.ArgumentParser(description='This script submits the analysis code to the grid. For more help type \"python XAMPPbase/scripts/SubmitToGrid.py -h\"', prog='SubmitToGrid', conflict_handler='resolve', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser = setupSUSYArgParser(parser)
   # RSE = '-1' if not os.getenv("RUCIO_RSE") else os.getenv("RUCIO_RSE")
    if ROOTCOREBIN: parser.add_argument('--exe', help='executable/run script to use for submitting (give package name and scipt name, e.g. XAMPPbase/python/runSUSYAnalysis.py)', default='XAMPPbase/python/runSUSYAnalysis.py')
    parser.add_argument('--Test', help='run in test modus, i.e. without DS replication', action='store_true', default=False)
    parser.add_argument('--DSCampaign', help='choose campaign of DS production in order to adjust the filepath to the DS list (default is \"mc15c_13TeV\")', default='mc15c_13TeV')
    parser.add_argument('--destSE', help='specify a destination for replication apart from to %s (or \"-1\" for no replication)' % RSE, default='%s' % RSE)
    parser.add_argument('--DuplicateTask', help='You can create another job with the same output dataset...', action='store_true', default=False)
    parser.add_argument('--FilesPerJob', help='You can specify the number of files per job... If you have data then it would be may be better to give a larger number... Default: 5', type=int, default=5)
    parser.add_argument('--nJobs', help='Specifiy the maximum number of jobs you want to run ', type=int, default=0)

    parser.add_argument('--outDS', help='specify a name which overwrites the naming scheme for the outDS used by this script internally (only the \"user.<username>.\" remains, the string given here gets appended and nothing more', required =True)
    parser.add_argument('-i', '--inputDS', help='input dataset or DS list present in \"XAMPPbase/data/<DSCampaign>/SampleLists/\"', required=True)
    parser.add_argument('--noAmiCheck', help='disables the check of the existence of the input datasets.', action="store_true", default=False)
    return parser

def DoesDSExists(DS):
    if os.system("rucio list-dids %s --filter type=CONTAINER --short"%(DS.strip().replace("/",""))) != 0:
        return False  
    return True

def isPrimaryAOD(DataSets):
    for DS in DataSets:
        if "merge.AOD." in DS or "recon.AOD" in DS: return true
    return False

def SubmitJobs(RunOptions, AnalysisOptions):
   
    os.chdir(WORKAREA)
    
    InDSList = RunOptions.inputDS        
       
    GroupDisk = RunOptions.destSE != '-1'
    Duplicate = RunOptions.DuplicateTask
        
    InputDataSets = []
    FilesPerJob = RunOptions.FilesPerJob
    nJobs = RunOptions.nJobs
    OutFiles = [RunOptions.outFile]
    
    # Read in the InputDataSets
    # try to find file first at default directory:
    InDSListDir = ROOT.PathResolver.FindCalibDirectory("XAMPPbase/SampleLists/"+RunOptions.DSCampaign+"/")
       
    if  not os.path.exists(InDSListDir):
        print('ERROR: Campaign %s is invalid. Could not find the directory containing the sample lists.'%Campaign) 
        exit(1)
   
   # check if inputDS was a file
    if os.path.exists(InDSListDir + InDSList + ".txt"): InDSList += ".txt"
    
    
    #################################################
    # Assemble the dataets list
    #################################################
    
    #The DS List is in the InDSListDir directory
    if os.path.exists(InDSListDir + InDSList): InputDataSets.extend(ReadListFromFile(InDSListDir + InDSList))
    #The path is directly given
    elif os.path.exists(InDSList):InputDataSets.extend(ReadListFromFile(InDSList))
    # if not, it should be a DS or a comma-separated list of DS
    elif ',' in InDSList: InputDataSets.extendd(InDSList.split(","))
    #Assume there is only one DS given to the script
    else: InputDataSets.append(InDSList)
     
    ###############################################
    #   No dataset could be extracted from the list
    ###############################################
    if len(InputDataSets) == 0:
        prettyPrint('ERROR', 'No input dataset found')
        exit(1)
    
    if RunOptions.noAmiCheck: print('WARNIG: The check of the existence of the inputDS is disabled, which should be fine.') 
    else:
        sys.stdout.write('Checking in rucio if dataset(s) is exist(s)..')
        sys.stdout.flush()
        DSCandidates = InputDataSets
        InputDataSets = []
        FoundAll = True
        for D in DSCandidates:
            sys.stdout.write('.')
            sys.stdout.flush()
            D = D.replace("/", "").strip() 
            if not DoesDSExists(D):
                print('ERROR: The input DS %s is not known!'%D)
                FoundAll = False
            else: InputDataSets.append(D)
        if FoundAll: print(' Successful!')
    
    
    if len(InputDataSets) == 0:
        print('ERROR: There are no valid input DS')
        exit(1)
    ######################################
    #   Automated AOD property only for ROOTCORE needed
    ######################################
    if  ROOTCOREBIN and (RunOptions.IsPrimAOD or isPrimaryAOD(InputDataSets)): AnalysisOptions.append("--IsPrimAOD")
    
    
    PRUN_Options.append("--inDS=\"%s\"" % ",".join(InputDataSets))
      
    # Sets the OutputDataSetName
    
    OutDS = "user.%s.%s" % (RUCIO_ACC, RunOptions.outDS) if not "group" in RUCIO_ACC else "%s.%s"%(RUCIO_ACC,RunOptions.outDS)
   
    PRUN_Options.append('--outDS=\"%s\"' % OutDS.replace('\r', ''))
    PRUN_Options.append("--skipScout")
    PRUN_Options.append("--express")
    PRUN_Options.append("--useShortLivedReplicas")
    
    # Additional Options parsing to the prun Command
    if RunOptions.Test == True:
        GroupDisk = False
        Duplicate = True
        FilesPerJob = 1
        nJobs = 1
        PRUN_Options.append("--nFiles=1")
        PRUN_Options.append("--disableAutoRetry")
    if GroupDisk: PRUN_Options.append("--destSE=%s" % RunOptions.destSE)
    if Duplicate: PRUN_Options.append("--allowTaskDuplication")
    if nJobs > 0: PRUN_Options.append("--nJobs=%d " % nJobs)
 
    PRUN_Options.append("--nFilesPerJob=%i" % FilesPerJob)
    
    
    ###########################################
    #  Configurations based on ROOTCORE
    ############################################
    if ROOTCOREBIN:
        AnalysisOptions.append("-i input.txt")
        AnalysisOptions.append("--runModus grid")
        if not RunOptions.noxAODs: PRUN_Options.append("--mergeOutput")
        PRUN_Options.append("--outputs=\"%s\"" % ",".join(OutFiles))
        # in case someone uses: 'XAMPPbase/runSUSYAnalysis.py'
        if os.path.isfile(RunOptions.exe): print ("Use executable "+RunOptions.exe)
        elif os.path.isfile(RunOptions.exe.replace("/", "/python/")):  RunOptions.exe = RunOptions.exe.replace("/", "/python/")
        else: 
            print('ERROR: Executable %s could not be found, exiting...'%RunOptions.exe)
            sys.exit(1)
   
    print("################################################################################################################################################################")
    print("                                                                        XAMPP on the grid")
    print("################################################################################################################################################################")
    prettyPrint('USER', USERNAME)
    prettyPrint('RUCIO', RUCIO_ACC)
    
    prettyPrint('WORKINGDIR', WORKAREA)
    prettyPrint('TODAY', TODAY)   
   
    if ROOTCOREBIN:
        prettyPrint('ROOTSYS', ROOTSYS)
        prettyPrint('ROOTCOREBIN', ROOTCOREBIN)
        prettyPrint('ROOTCOREDIR', ROOTCOREDIR)
        prettyPrint('ROOTVER', ROOTVER)
        prettyPrint('ROOTCORECONFIG', ROOTCORECONFIG)
    else:
        prettyPrint("ATLASPROJECT", ATLASPROJECT)
        prettyPrint("ATLASVERSION",ATLASVERSION)
        prettyPrint("TESTAREA", TESTAREA)
    print("################################################################################################################################################################")
    print("                                                                        JobOptions")        
    print("################################################################################################################################################################")
    prettyPrint('InputDataSets:', '', separator='')
    for ds in InputDataSets: prettyPrint('', ds, width=32, separator='-')
    prettyPrint('FilesPerJob', str(FilesPerJob))
    if nJobs > 0: prettyPrint('NumberOfJobs', str(nJobs))
    
    prettyPrint('OutputContainer', OutDS)
    prettyPrint('CreateTarBall', BoolToString(CREATETARBALL))
    if CREATETARBALL == False:
        prettyPrint('', 'Using already existing tarball located at:', width=32, separator='->')
        prettyPrint('', '%s/TarBall.tgz' % WORKAREA, width=34, separator='')
    
    if ROOTCOREBIN: prettyPrint('Executable', RunOptions.exe)
    else: prettyPrint('JobOptions', RunOptions.jobOptions)
    prettyPrint('RunWithSystematics', BoolToString(RunOptions.noSyst == False))
            
    print("################################################################################################################################################################")

    AnalysisOptions.reverse()
    Command = ""
    if ROOTCOREBIN:
        Command = "prun --exec=\"python %s %s\" %s " % (
                                RunOptions.exe, 
                                " ".join(AnalysisOptions), 
                                " ".join(PRUN_Options))
    else:
        Command = "pathena %s %s %s"%(
            " ".join(PRUN_Options),
            AssembleConfigArgument(AnalysisOptions),
            RunOptions.jobOptions)
    
    print('\nSubmitting using command:\n%s\n...'%Command)
    os.system(Command)    
    
if __name__ == '__main__':       
    Parser = setupGridSubmitArgParser()
    RunOptions = setupGridSubmitArgParser().parse_args()    
    AnalysisOptions = AssembleRemoteRunCmd(RunOptions,Parser)   
    SubmitJobs(RunOptions, AnalysisOptions)   

   
