import os
import commands
import math
import sys
import re
import argparse
import time
from XAMPPbase.runSUSYAnalysis import *
from XAMPPbase.Utils import *

BASEFOLDER = os.getenv("SGE_BASEFOLDER")
if BASEFOLDER == None:
    print "INFO: Could not find the enviroment variable 'SGE_BASEFOLDER'. This variable defines the directory of your output & logs. "
    BASEFOLDER = "/ptmp/mpp/%s/Cluster/" % (USERNAME)
    print "INFO: Will set its default value to " + BASEFOLDER

MYEMAIL = ""
if os.getenv("JOB_MAIL") is None: MYEMAIL = USERNAME + "@rzg.mpg.de" #   where to send job status emails
else: MYEMAIL = os.getenv("JOB_MAIL")

def CreateDirectory(Path, CleanUpOld = True):
    if len(Path) == 0 or Path == os.getenv("HOME"):
        prettyPrint("Could not create", Path)
        return False
    if os.path.exists(Path) and CleanUpOld:
        print "INFO: Found old copy of the folder "+Path
        print "INFO: Will delete it."
        os.system("rm -rf "+Path)
    if not os.path.exists(Path):
        print "INFO: Create directory "+Path
    os.system ("mkdir -p "+Path)
    return os.path.exists(Path)

def GetHoldString(HoldJobs, JobName):
    Hold = ""
    if len(HoldJobs)>0:
        prettyPrint("Hold "+JobName+" until","")
    for H in HoldJobs:
        prettyPrint('', H, width=32, separator='*')
        Hold+=" -hold_jid "+H
    return Hold


def CopyExecutingScript(Script,TmpDir,  Source= WORKAREA):
    OldLocation = "%s/%s"%(Source,Script)
    if not os.path.exists(OldLocation):
        print "ERROR: Could not find script "+OldLocation
        exit(1)    
    NewLocation = "%s/%s"%(TmpDir,Script)
    
    DestDir = NewLocation[0 : NewLocation.find(NewLocation.split("/")[-1])]
    if not CreateDirectory(DestDir, False):
        print "ERROR: Could not create "+DestDir
        exit(1)
    
    if not os.path.exists(NewLocation):
        os.system("cp %s %s"%(OldLocation,NewLocation))
    return NewLocation  

def WriteList (Files , OutLocation):
    Out = open (OutLocation, "w")
    if Out == None:
        print 'ERROR: Could not create the File List: ' + OutLocation
        return False
    for F in Files:
        Out.write(F + "\n")
    Out.close()
    return True
def CreateIteratedList(Location, FileName , number):
    Items = []
    for a in range(1, number + 1):
        I = "%s/%s_%i.root" % (Location, FileName, a)
        Items.append(I)    
    return Items

def SplitFiles(FilesToMerge, Nfiles):    
    if Nfiles < 2: Nfiles = 2    
    SplittedFiles = []
    curList = []
    curIdx = 1
    for file in FilesToMerge:
        if curIdx == len(FilesToMerge):
            curList.append(file)
            SplittedFiles.append(curList)
        elif len(curList) < Nfiles: curList.append(file)
        elif len(curList) == Nfiles:
            SplittedFiles.append(curList)
            curList = []
            curList.append(file)
        curIdx += 1   
    return SplittedFiles

def SubmitMergeJobs(RunOptions, Files, Nfiles=50, AddString="" , HoldJobs=[], OutFile="", SubJob=""):
     if Nfiles < 2: Nfiles = 2
     if len(OutFile)==0: OutFile = RunOptions.jobName
     if len(AddString) ==0 and len(SubJob)>0: AddString+=SubJob     
     Today = time.strftime("%Y-%m-%d")    
     MyBatchLogs = "%s/Batch_job_logs/%s/%s/"%(RunOptions.BaseFolder, Today, RunOptions.jobName) 
     MyOutDir = "%s/Output/%s/%s/" % (RunOptions.BaseFolder, Today, RunOptions.jobName)
     MyTmpDir = "%s/Batch_tmp/%s/%s/" % (RunOptions.BaseFolder, Today, RunOptions.jobName)    
     Split = SplitFiles(Files , Nfiles)
     OutListDir = MyTmpDir
     os.system("mkdir -p " + OutListDir)    
     for a in range(1, len(Split) + 1):
         OutList = OutListDir + "MergeList" + AddString + "_%d" % a + ".txt"
         WriteList(Split[a - 1], OutList)
     
     Hold = GetHoldString(HoldJobs, RunOptions.jobName + AddString + "_SplitMerge")
     Cmd = "qsub -o " + str(MyBatchLogs) + " -m a -j y -l h_vmem=2000M -l h_rt=04:00:00 " + Hold + " -t 1-" + str(len(Split)) + " -N '" + RunOptions.jobName + AddString + "_SplitMerge' -M " + MYEMAIL + " -v In='" + OutListDir + "MergeList" + AddString + "' -v nJobs='" + str(len(Split)) + "' -v OutDir='" + MyTmpDir + "' -v OutFile='" + RunOptions.jobName + AddString + "'  -cwd " +CopyExecutingScript("XAMPPbase/scripts/Batch_Merge.sh", MyTmpDir)
     os.system(Cmd)
     if len(Split) > Nfiles:
         print "INFO: The number of parallel merge jobs is larger than the fixed limit of %d files per Job. Return new iteration" % (Nfiles)
         HoldJobs.append(RunOptions.jobName + AddString + "_SplitMerge")
         NewList = CreateIteratedList(MyTmpDir, "Merge_" + RunOptions.jobName + AddString, len(Split))         
         CleanCom = "qsub -o " + str(MyBatchLogs) + " -j y -l h_vmem=200M  -l h_rt=00:30:00 -l h_core=0 -N SplitClean_" + RunOptions.jobName + AddString + " -hold_jid '" + RunOptions.jobName + AddString + "_Itr_SplitMerge' -v Files='" + MyTmpDir + "/Merge_" + RunOptions.jobName + AddString + "' -v nJobs='" + str(len(Split)) + "' -cwd " + CopyExecutingScript("XAMPPbase/scripts/Batch_Clean.sh", MyTmpDir)
         AddString = AddString + "_Itr"         
         SubmitMergeJobs(RunOptions, NewList , Nfiles / 2, AddString, HoldJobs,OutFile,SubJob)   
         os.system(CleanCom)
     else:
        Cmd = "qsub -o " + str(MyBatchLogs) + " -m e -m a -j y -l h_vmem=2000M -l h_rt=04:00:00 " + Hold + " -hold_jid '" + RunOptions.jobName + AddString + "_SplitMerge' -N '" + RunOptions.jobName +SubJob+ "_merge' -M " + MYEMAIL + " -v In='" + MyTmpDir + "' -v nJobs='" + str(len(Split)) + "' -v OutDir='" + MyOutDir + "' -v OutFile='" +OutFile + "' -v InFile='" + RunOptions.jobName + AddString + "'  -cwd "  +CopyExecutingScript("XAMPPbase/scripts/Batch_Merge.sh", MyTmpDir)
        os.system(Cmd)
         
def setupBatchSubmitArgParser():
    parser = argparse.ArgumentParser(description='This script submits the analysis code to the grid. For more help type \"python XAMPPbase/scripts/SubmitToBatch.py -h\"', prog='SubmitToBatch', conflict_handler='resolve', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser = setupSUSYArgParser(parser)
    parser.add_argument('-j', '--jobName', help='Specify the JobName', required=True)
    parser.add_argument('--DSCampaign', help='choose campaign of DS production in order to adjust the filepath to the DS list (default is \"mc15c_13TeV\")', default='mc15c_13TeV')
    parser.add_argument('--inputDS', help='input dataset or DS list present in \"'  'XAMPPbase/data/<Campaign>/SampleLists/\"', default=[], nargs="+")
    parser.add_argument('--HoldJob', help='Specfiy job names which shouldbe finished before your job is starting. BuildJobs are not affected by this option', default=[], nargs="+")
    parser.add_argument('--HoldBuildJob', help='Specfiy job names which shouldbe finished before your job is starting. BuildJobs are not affected by this option', default=[], nargs="+")
    parser.add_argument('--BuildTime', help='Changes the RunTime of the BuildJob: default 00:59:59 ', default='00:59:59')
    
    parser.add_argument('--Mail', help='E-mail address to which the cluster notification mails are sent to default: ' + MYEMAIL, action='store_true', default=MYEMAIL)
    
    parser.add_argument('--RSE', help='RSE storage element for files located via dcache. Default: MPPMU_LOCALGROUPDISK ', action='store_true', default='MPPMU_LOCALGROUPDISK')
    parser.add_argument('--BaseFolder', help='Changes the BaseFolder where the OutputFiles are saved. Default value: %s' % (BASEFOLDER) , default=BASEFOLDER)
    parser.add_argument('--RunTime', help='Changes the RunTime of the Jobs: default 19:59:59 ', default='19:59:59')
    
    parser.add_argument('--EventsPerJob', help='Changes the Events per Batch job. Default: 10000 ', default=10000, type=int)
    parser.add_argument('--ResetJobSplit', help='This option resets the splitting of the batch jobs', action='store_true', default=False)
    if ROOTCOREBIN or "Ath" not in ATLASPROJECT: parser.add_argument('--exe', help="Which python script should be executed", default="XAMPPbase/runSUSYAnalysis.py") 
    parser.add_argument('--FilesPerMergeJob', help='Number of files per merge', default=50, type=int)
    parser.add_argument('--vmem', help='Virtual memory reserved for each analysis  jobs', default=3500 , type=int)
    parser.add_argument('--Build_vmem', help='Changes the virtual memory needed by the build job', type=int, default=8000)
    return parser

def PrepareInput(RunOptions, In):
    #Check where the input files could actually be stored. If they are in GroupDiskLists they are interpreted as a list of root files
    #If they are in SampleLists they are interpreted as a list of datasets containing the root files
    if ROOTCOREBIN: DCacheDir = "%s/data/XAMPPbase/GroupDiskLists/%s/" % (ROOTCOREBIN, RunOptions.RSE)
    else: DCacheDir = "%s/XAMPPbase/data/GroupDiskLists/%s/" % (TESTAREA, RunOptions.RSE)
    if os.path.exists(DCacheDir) == False:
        print "ERROR: No FileLists found for RSE: " + RunOptions.RSE
        exit(1)
    if ROOTCOREBIN: InDSListDir = "%s/data/XAMPPbase/SampleLists/%s/" % (ROOTCOREBIN, RunOptions.DSCampaign)
    else: InDSListDir = "%s/XAMPPbase/data/SampleLists/%s/" % (TESTAREA, RunOptions.DSCampaign)
    if os.path.exists(InDSListDir) == False:
        print "ERROR: Campaign is invalid. Could not find the directory containing the sample lists."
        exit(1)
    
    ROOTFiles = []
    InputDataSets = []
    ListName = ""    
    if os.path.isdir(In):
        print "INFO: The given input %s is a directory will read out all containing .root files."%(In)
        ListName="Dir_"+In.replace("/","_")
        DirContent = os.listdir(In)        
        for File in DirContent:
            if (not File.endswith(".root") and not File.endswith(".root.1")):
                print "INFO: Skip file "+File
            else: ROOTFiles.append(In+File)
    elif os.path.exists(InDSListDir+In+".txt") or os.path.exists(InDSListDir+In+".conf") or os.path.exists(InDSListDir+In):
        print "INFO: The given input %s is a list of datasets. Read out all the datasets and look up the files in %s."%(In, RunOptions.RSE)
        ListName = In.replace(".txt","").replace(".conf","")
        if os.path.exists(InDSListDir+In+".txt"): InputDataSets = ReadListFromFile(InDSListDir+In+".txt")
        elif os.path.exists(InDSListDir+In+".conf"): InputDataSets = ReadListFromFile(InDSListDir+In+".conf")
        else: InputDataSets = ReadListFromFile(InDSListDir+In)
    elif os.path.isfile(In): InputDataSets = ReadListFromFile(In)
    else:
       print "INFO: Assume that %s is a list of datasets"%(In)
       for DS in In.split(","): InputDataSets.append(DS.strip("\n").strip("/"))
       if len (InputDataSets) > 0: ListName = "Direct_"+InputDataSets[0]
    
    if len (InputDataSets) > 0:
        print "INFO: Look up file lists of the %d in %s"%(len(InputDataSets), DCacheDir)
        for DS in InputDataSets:
            DS.replace(" ", "")
            ifilelist = DCacheDir+DS.replace("/",".txt") if DS.endswith('/') else DCacheDir+DS+".txt"

            if not os.path.exists(ifilelist):
                print "WARNING: Could not find input '%s' for dataset '%s'"%(ifilelist,DS)
            else: ROOTFiles.extend(ReadListFromFile(DCacheDir+DS.replace("/","")+".txt"))
    
    
    JobFileListDir = RunOptions.BaseFolder + "/.Configs/" + RunOptions.DSCampaign + "_" + ListName + "/"
    if not len (ROOTFiles):
        print "No Files to analyze"
        return JobFileListDir, 0
    
    JobFileListDir = RunOptions.BaseFolder + "/.Configs/" + RunOptions.DSCampaign + "_" + ListName + "/"
    JobFileList = JobFileListDir + "Files.txt"        
    
    ListUpToDate = True
    if not os.path.exists(JobFileList):
        ListUpToDate = False
    else:
        StoredList = ReadListFromFile(JobFileList)
        if IsListIn (StoredList , ROOTFiles) == False or IsListIn (ROOTFiles, StoredList) == False:
            print "Info the input FileList has been updated since last time"
            ListUpToDate = False
    
    if ListUpToDate == False or RunOptions.ResetJobSplit == True:
        CreateDirectory(JobFileListDir)
        WriteList(ROOTFiles,JobFileList)
    SplitDir = JobFileListDir + "/" + str(RunOptions.EventsPerJob) + "/"
    CreateDirectory(SplitDir,False)
    if len(os.listdir(SplitDir)) == 0:
        if len(ROOTFiles) > 5:
            if ROOTCOREBIN: os.system ("CreateBatchJobSplit -I %s -O %s -EpJ %i" % (JobFileList, SplitDir, RunOptions.EventsPerJob))
            else: os.system ("CreateBatchJobSplit.exe -I %s -O %s -EpJ %i" % (JobFileList, SplitDir, RunOptions.EventsPerJob))
        else: WriteList(ROOTFiles, "%s/FileList_1.txt"%(SplitDir))
    return SplitDir, len (os.listdir(SplitDir))

def SubmitBuildJob(RunOptions):
    Today = time.strftime("%Y-%m-%d")    
    MyBatchLogs = "%s/Batch_job_logs/%s/%s/"%(RunOptions.BaseFolder, Today, RunOptions.jobName) 
    MyOutDir = "%s/Output/%s/%s/" % (RunOptions.BaseFolder, Today, RunOptions.jobName)
    MyTmpDir = "%s/Batch_tmp/%s/%s/" % (RunOptions.BaseFolder, Today, RunOptions.jobName)
    ReleaseCopy = "%s/BuildJob/%s/%s/" % (RunOptions.BaseFolder, Today, RunOptions.jobName)
    if not CreateDirectory(MyBatchLogs,False):
        print "ERROR: Could not Create the LogDirectory: "+MyBatchLogs
        return False    
    print "INFO: Submit the BuildJob"
    if len (RunOptions.HoldBuildJob) > 0:
        prettyPrint("Hold BuildJob until:" , "")
        for J in RunOptions.HoldBuildJob:
            prettyPrint('', J, width=32, separator='*')           
    
    if ROOTCOREBIN:
        BuildCom = "qsub -o %s -m a -M %s %s -j y -l h_vmem=%iM -l h_rt=%s -l h_core=0 -N Build_%s  -v COPYAREA='%s'  -v CleanOut='%s' -v CleanLogs='%s' -v CleanTmp='%s'  -v RcArea='%s/../' -v RCRelease='%s' -v ROOTCORE_NCPUS=4 -cwd %s"%(
                       MyBatchLogs,
                       MYEMAIL,
                       GetHoldString(RunOptions.HoldBuildJob, "BuildJob"),
                       RunOptions.Build_vmem,
                       RunOptions.BuildTime,
                       RunOptions.jobName,
                       ReleaseCopy,
                       MyOutDir,
                       MyBatchLogs,
                       MyTmpDir,
                       ROOTCOREBIN,                     
                       RCRELEASE,
                       CopyExecutingScript("XAMPPbase/scripts/Batch_Build.sh", MyTmpDir, ROOTCOREBIN+"/../")
                    ) 
                
    else:
        BuildCom="qsub -o %s -m a -M %s %s -j y -l h_vmem=%iM -l h_rt=%s -l h_core=0 -N Build_%s  -v COPYAREA='%s'  -v CleanOut='%s' -v CleanLogs='%s' -v CleanTmp='%s'  -v OriginalArea='%s' -v AtlasProject='%s' -v AtlasVersion='%s' -cwd %s"%(
                       MyBatchLogs,
                       MYEMAIL,
                       GetHoldString(RunOptions.HoldBuildJob, "BuildJob"),
                       RunOptions.Build_vmem,
                       RunOptions.BuildTime,
                       RunOptions.jobName,
                       ReleaseCopy,
                       MyOutDir,
                       MyBatchLogs,
                       MyTmpDir,
                       TESTAREA,
                       ATLASPROJECT,
                       ATLASVERSION,                
                       CopyExecutingScript("XAMPPbase/scripts/Batch_AthenaBuild.sh", MyTmpDir, TESTAREA)
                    ) 
        
    RunOptions.HoldJob.append("Build_"+RunOptions.jobName)
    os.system(BuildCom)
    return True


def SubmitCleanJob(RunOptions, HoldJobs):
    Today = time.strftime("%Y-%m-%d")    
    MyBatchLogs = "%s/Batch_job_logs/%s/%s/"%(RunOptions.BaseFolder, Today, RunOptions.jobName) 
    MyTmpDir = "%s/Batch_tmp/%s/%s/" % (RunOptions.BaseFolder, Today, RunOptions.jobName)
    ReleaseCopy = "%s/BuildJob/%s/%s/" % (RunOptions.BaseFolder, Today,  RunOptions.jobName)
    
    CleanCom = "qsub -o  %s -j y -l h_vmem=200M  -l h_rt=00:30:00 -l h_core=0 -N Clean_%s  %s -v COPYAREA='%s' -v OutDirSubJobs='%s' -cwd %s"%(
            MyBatchLogs,
            RunOptions.jobName,
            GetHoldString(HoldJobs, "Clean_"+RunOptions.jobName), 
            ReleaseCopy,
            MyTmpDir, 
            (ROOTCOREBIN + "/../" if ROOTCOREBIN else TESTAREA) + "/XAMPPbase/scripts/Batch_Clean.sh")
    os.system(CleanCom)
    
def SubmitAnalysisJob(RunOptions, AnaOptions, Input):
    Today = time.strftime("%Y-%m-%d")  
    MyBatchLogs = "%s/Batch_job_logs/%s/%s/"%(RunOptions.BaseFolder, Today, RunOptions.jobName) 
    MyOutDir = "%s/Output/%s/%s/" % (RunOptions.BaseFolder, Today, RunOptions.jobName)
    MyTmpDir = "%s/Batch_tmp/%s/%s/" % (RunOptions.BaseFolder, Today, RunOptions.jobName)
    ReleaseCopy = "%s/BuildJob/%s/%s/" % (RunOptions.BaseFolder, Today, RunOptions.jobName)
    
    Executable = RunOptions.exe.replace("/python/", "/") if hasattr(RunOptions, "exe") else RunOptions.jobOptions.replace("share/","")
  
    if ROOTCOREBIN: EnviromentVariables = " -v ROOTVER=%s -v ROOTCOREBIN=%s -v ROOTCOREDIR=%s -v ROOTCORECONFIG=%s -v CALIBPATH=%s -v ROOTCOREOBJ=%s -v Execute='%s' " % (ROOTVER, ROOTCOREBIN , ROOTCOREDIR , ROOTCORECONFIG , CALIBPATH , ROOTCOREOBJ, Executable)   
    else: EnviromentVariables = " -v OriginalArea='%s/source/' -v Execute='%s' -v OriginalProject='%s' -v OriginalPatch='%s' " % (ReleaseCopy, Executable, ATLASPROJECT, ATLASVERSION)   
    
    print "INFO: Prepare job configuration for "+Input
    SplitDir, NumberOfJobs = PrepareInput(RunOptions,Input)   
    if NumberOfJobs == 0:
       print "ERROR: No job to submit"
       return False
    SubJName = Input if len(RunOptions.inputDS) > 1 else ""
    OutFile = Input if len(RunOptions.inputDS) > 1 else RunOptions.jobName
   
    if ROOTCOREBIN: executingScript = "XAMPPbase/scripts/Batch_Analysis.sh"
    else: executingScript = "XAMPPbase/scripts/Batch_AthenaAnalysis.sh"
    
    Options = " ".join(AnaOptions) if ROOTCOREBIN or "Ath" not in ATLASPROJECT else ";".join(AnaOptions)
    
    EnviromentVariables += GetHoldString(RunOptions.HoldJob, RunOptions.jobName+SubJName) + " "+" -v Name='"+RunOptions.jobName+SubJName+"'"
    SubCom = "qsub -o " + str(MyBatchLogs) + " -j y -l h_vmem=" + str(RunOptions.vmem) + "M  -l h_rt=" + RunOptions.RunTime + " -l h_core=0 -N '" + RunOptions.jobName +SubJName+ "' -t 1-" + str(NumberOfJobs) + " -M " + MYEMAIL + " -m a " + EnviromentVariables + " -v InListDir='" + SplitDir + "' -v OutDir=" + MyTmpDir + " -v Options=\"" + Options + "\" -cwd " + CopyExecutingScript(executingScript, MyTmpDir)
    os.system(SubCom)
    OutFiles = CreateIteratedList(MyTmpDir, RunOptions.jobName+SubJName, NumberOfJobs)
    OutName = RunOptions.jobName if len(RunOptions.inputDS) == 1 else SubJName
    SubmitMergeJobs(RunOptions, OutFiles , MyBatchLogs , MyTmpDir, MyOutDir, Nfiles=RunOptions.FilesPerMergeJob, HoldJobs=[RunOptions.jobName+SubJName], OutFile=OutName,SubJob=SubJName)
    return True
def SubmitJob(RunOptions , AnaOptions):
    print "#####################################################################################################"
    print "                        XAMPPbase SubmitToBatch "
    print "#####################################################################################################"
    RunOptions.inputDS = ClearFromDuplicates(RunOptions.inputDS)
    if len (RunOptions.inputDS) == 0:
        print "ERROR: Please give at least one --inputDS"
        exit(1)
    
    Executable = RunOptions.exe.replace("/python/", "/") if hasattr(RunOptions, "exe") else RunOptions.jobOptions.replace("share/","")
    prettyPrint("JobName", RunOptions.jobName)
    prettyPrint("Executable", Executable)
    prettyPrint("AnalysisOptions:","")
    for o in AnaOptions: prettyPrint("", o)
   
    prettyPrint("Memory", str(RunOptions.vmem) + " MB")
    prettyPrint("EventsPerJob", str(RunOptions.EventsPerJob))
    prettyPrint("RunTime", RunOptions.RunTime)
    
    
    
    
    ###################################################
    # Define the Working folders for the job
    ##################################################
    Today = time.strftime("%Y-%m-%d")    
    MyBatchLogs = "%s/Batch_job_logs/%s/%s/"%(RunOptions.BaseFolder, Today, RunOptions.jobName) 
    MyOutDir = "%s/Output/%s/%s/" % (RunOptions.BaseFolder, Today, RunOptions.jobName)
    MyTmpDir = "%s/Batch_tmp/%s/%s/" % (RunOptions.BaseFolder, Today, RunOptions.jobName)
    ReleaseCopy = "%s/BuildJob/%s/%s/" % (RunOptions.BaseFolder, Today,  RunOptions.jobName)
    
    prettyPrint("LogDir", MyBatchLogs)   
    prettyPrint("TmpDir", MyTmpDir)
    prettyPrint("OutDir", MyOutDir)

    #Setup the BuildJob
    if not SubmitBuildJob(RunOptions):
        print "ERROR: Failed to setup the Job"
        exit(1)
     
    
    HoldJob = []
    HoldJob.extend(RunOptions.HoldJob)
    for In in RunOptions.inputDS:
       if SubmitAnalysisJob(RunOptions,AnaOptions,In):
              HoldJob.append(RunOptions.jobName+(In if len(RunOptions.inputDS) > 1 else "")+"_merge")
    
    SubmitCleanJob(RunOptions,HoldJob)
    
         
    
if __name__ == '__main__':   
    Parser = setupBatchSubmitArgParser()
    RunOptions = Parser.parse_args()   
    AnaOpt = AssembleRemoteRunCmd(RunOptions,Parser)
    SubmitJob (RunOptions, AnaOpt)
        
    
    
   
    
   
    
   
    
