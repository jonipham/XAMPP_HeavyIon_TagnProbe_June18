"""
Description: 
 Script to check whether samples in sample list are contained in list of PRW profiles

Usage:
 python XAMPPbase/python/checkSamplesInPRWfile.py --file-list XAMPPbase/data/SampleLists/mc15c_13TeV/SUSY2_PowHegPy8_Zmumu.txt
 python XAMPPbase/python/checkSamplesInPRWfile.py --samples-list SUSY2_PowHegPy8_Zmumu SUSY2_Sherpa221_Znunu
"""

import os, sys, commands, argparse

def isDSID(dsid):
  try:
    int(dsid)
    return True
  except ValueError:
    return False
#Get File
def GetFile(fname): 
  if not os.path.isfile(fname):
    print "Error. Please provide a file with samples. %s is invalid."%(fname)
    sys.exit(1)
  return open(fname)
  
#Get list of samples
def GetListOfSamples(Options, samplelist):
  samples = []
  for line in samplelist:
    if Options.mc_campaign in line:
      samples.append(line.rstrip("\n"))
  return samples

parser = argparse.ArgumentParser(description='Check PRW file', formatter_class=argparse.ArgumentDefaultsHelpFormatter)

parser.add_argument( '--samples-list', 
                     help='Input samples. Example: SUSY2_PowHegPy8_Zmumu SUSY2_PowHegPy8_Zee SUSY5_Higgs ...', 
                     nargs='+',
                     default=[])

parser.add_argument( '--samples-path', 
                     help='Path with samples list.', 
                     type=str,
                     default='XAMPPbase/data/SampleLists/mc15c_13TeV/' )

parser.add_argument( '--file-list', 
                     help='Input file with samples list. Example: XAMPPbase/data/SampleLists/mc15c_13TeV/SUSY2_PowHegPy8_Zmumu.txt', 
                     type=str,
                     default='' )

parser.add_argument( '--prw-file', 
                     help='Input prw file',
                     type=str,
                     default='/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/merged_prw_mc15c_latest.root' )

parser.add_argument( '--mc-campaign', 
                     help='MC campaign.',
                     type=str,
                     default='mc15_13TeV' )

parser.add_argument( '--PRWToolCheck', 
                     help='Use PileupReweightingTool for finding ignored channels',
                     action='store_true', 
                     default=False )

options = parser.parse_args()

# only import from ROOT when parser is parsed to keep the parser help working
from ROOT import TFile,TDirectory

samples    = []
# one file
if options.file_list:
  samplelist = GetFile(options.file_list) 
  samples = GetListOfSamples(options, samplelist)

#many files
if options.samples_list:
  for sample in options.samples_list:
    infile = os.path.join( options.samples_path, sample+".txt")
    samplelist = GetFile(infile)
    samples.extend(GetListOfSamples(options, samplelist))
    
#prw file
if not os.path.islink(options.prw_file) and not os.path.isfile(options.prw_file):
  print "Error. Please provide a valid/existing PRW file. File %s is not valid symbolic link."%(options.prw_file)

if not os.path.isfile(options.prw_file):
  print "Error. Please provide a valid/existing PRW file. File %s is not valid."%(options.prw_file)
  sys.exit(1)

prwfile    = TFile(options.prw_file, 'READ')

IgnoredDSIDs = []
if options.PRWToolCheck:
    Found = commands.getoutput('python FindIgnoredPRWChannels.py --PRWFile %s'%options.prw_file)
    for line in Found.split('\n'):
        if not ('Ignoring channel ' in line and ' unrepresented data ' in line): continue
        IgnoredDSIDs.append(line.split('Ignoring channel ')[1].split(', ')[0])

#Get list of PU histograms in prw file
puhists    = []
channelsPerPeriod = {}
for key in prwfile.Get("PileupReweighting").GetListOfKeys():
    histname=key.GetName()
    if 'MCPileupReweighting' == histname:
        prwtree = prwfile.Get("PileupReweighting/MCPileupReweighting")
        Nentries = prwtree.GetEntries()
        for jentries in range(0,Nentries):
            prwtree.GetEntry(jentries)
            if not prwtree.RunNumber in channelsPerPeriod.iterkeys(): channelsPerPeriod[prwtree.RunNumber] = []
            channelsPerPeriod[prwtree.RunNumber].append(prwtree.Channel)
    if "pileup_chan" in histname:
        puhists.append(histname)
    

#check whether sample in PU file list
missing = []
lowstats = []
for sample in samples:
  dsid=sample.split(".")[1]
  if not isDSID(dsid):
    raise NameError('Could not properly extract DSID from %s - Exit '%sample)
  else:
    included=False
    enoughStats=True
    for puhist in puhists:
      if dsid in puhist:
        if len(channelsPerPeriod)>0:
          for period in channelsPerPeriod.iterkeys():
            if str(period) in puhist and int(dsid) in channelsPerPeriod[period]:
              included=True
        else:
          included=True
        if options.PRWToolCheck:
          for prwskip in IgnoredDSIDs:
            if dsid == prwskip: enoughStats=False
    if included and not enoughStats: lowstats.append(sample)
    if not included:
      missing.append(sample)
#
if len(missing)>0: 
    print "\nThe following samples not included in PRW file %s:"%options.prw_file
    for i in sorted(missing):
        print i
else:
    print 'All samples included in PRW file %s'%options.prw_file
if len(lowstats)>0: 
    print "\nThe following samples are included in PRW file %s but they are ignored by the PRW tool:"%options.prw_file
    for i in sorted(lowstats):
        print i   