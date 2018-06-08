#! /usr/bin/env python
import os, commands, sys, argparse, re

try:
   import rucio
except ImportError:
   print 'No RUCIO setup is found please setup RUCIO using "localSetupRucioClients"'
   sys.exit(1)
    


if os.getenv("RUCIO_ACCOUNT") == None:
    print "No RUCIO ACCOUNT is available.. please define a rucio Account"
    exit(1)



def CheckRemainingProxyTime():
    RemainingTime  = 0
    try:
        RemainingTime = int(commands.getoutput("voms-proxy-info --timeleft").strip().split('\n') [-1])
    except ValueError:
        print "Verkackt"
        pass
    if not RemainingTime > 0:
        print "No valid VOMS-PROXY, creating one..."
        os.system("voms-proxy-init --voms atlas")
        CheckRemainingProxyTime()
    return RemainingTime

while CheckRemainingProxyTime() == 0:
    print "INFO: Renewing proxy"

RUCIO_ACC = os.getenv("RUCIO_ACCOUNT")
RSE = ''
if os.getenv("RUCIO_RSE"):
    RSE = os.getenv("RUCIO_RSE")

def ListDisk(RSE="MPPMU_LOCALGROUPDISK"):
    print "Read content of " + RSE
    OnDisk = commands.getoutput("rucio list-datasets-rse " + RSE)
    MyDataSets = []
    for Candidates in OnDisk.split():
        MyDataSets.append(Candidates)    
    return MyDataSets

def ListUserRequests(RSE="MPPMU_LOCALGROUPDISK"):
    print "List requests of user %s at %s" % (RUCIO_ACC, RSE)
    OnDisk = commands.getoutput("rucio list-rules --account %s" % RUCIO_ACC)
    MyDataSets = []
    for Candidates in OnDisk.split('\n'):
        if not RSE in Candidates:
            continue
        for item in Candidates.split():
            if ':' in item:
                if len(re.findall(r'(\d{2}):(\d{2}):(\d{2})', item)) == 0:
                    MyDataSets.append(item)    
    return MyDataSets

if __name__ == '__main__':
    
    OutDir = os.getcwd()
    
    parser = argparse.ArgumentParser(description='This script lists datasets located at a RSE location. Futher patterns to find or exclude can be specified.', prog='ListGroupDisk', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-p', '-P', '--pattern', help='specify a pattern which is part of dataset name', nargs='+', default=[])
    parser.add_argument('-e', '-E', '--exclude', help='specify a pattern which must not be part of dataset name', nargs='+', default=[])
    parser.add_argument('-o', '-O', '--OutDir', help='specify output directory (default: %s)' % OutDir, default=OutDir)
    parser.add_argument('-r', '-R', '--RSE', help='specify a RSE (default: %s)' % RSE, default=RSE)
    parser.add_argument('--MyRequests', help='list datasets which you requested yourself', action='store_true', default=False)
    parser.add_argument('--dry', help='dry run - do not store anything, just print on the screen', action='store_true', default=False)
    RunOptions = parser.parse_args()

    Today = commands.getoutput("date --iso")
    Patterns = RunOptions.pattern
    OutDir = RunOptions.OutDir
    RSE = RunOptions.RSE
    
    Prefix = ''
    if RunOptions.MyRequests:
        Prefix = 'MyRequestTo_'
        DS = ListUserRequests(RSE)
    else:
        DS = ListDisk(RSE)
        
    if len(DS) == 0:
        print "Disk is empty"
        exit(0)
    if os.path.exists(OutDir) == False:
        print "mkdir -p " + OutDir
        os.system("mkdir -p " + OutDir)
    FileList = "%s%s_%s" % (Prefix, RSE, Today)
    if len(Patterns) > 0:
        FileList += "_%s" % ('_'.join(Patterns))
    if len(RunOptions.exclude) > 0:
        FileList += "_exl_%s" % ('_'.join(RunOptions.exclude))
    FileList += '.txt'
    Write = []
    for d in sorted(DS):
        allPatternsFound = True
        for Pattern in Patterns:
            if not Pattern in d:
                allPatternsFound = False
        for Pattern in RunOptions.exclude:
            if Pattern in d:
                allPatternsFound = False
        if allPatternsFound:
            if d.find("_tid") > 0:
                d = d [ 0 :  d.find("_tid") ]
            DigitsToRemove = re.findall(r'.(\d{9})', d)
            if len(DigitsToRemove) != 0 and d.find('.' + str(DigitsToRemove[-1])) > 30:
                d = d [ 0 :  d.find('.' + str(DigitsToRemove[-1])) ]
            IsInWrite = False
            for w in Write:
                if d in w:
                    IsInWrite = True
                    break
            if IsInWrite == True:
                continue                
            print d
            Write.append(d)
    if len(Write) == 0:
        print "No datasets containing given pattern(s) found!"
        exit(0)

    if RunOptions.dry:
       print "Done."
       exit(0)

    List = open (OutDir + "/" + FileList , "w")
    for w in Write:
        List.write(w + "\n")
    print 'Datasets written to file %s' % List.name
