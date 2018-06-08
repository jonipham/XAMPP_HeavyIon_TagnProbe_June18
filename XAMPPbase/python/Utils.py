from PathResolver import *
import os
import commands
import math
import sys
import re
import argparse
import time


#######################################################################################
#                ROOTCORE enviroment variables                                        #    
#######################################################################################
USERNAME = os.getenv("USER")
ROOTCOREBIN = os.getenv("ROOTCOREBIN")
WORKAREA = os.getenv("TestArea")
ROOTSYS = os.getenv("ROOTSYS").replace("%s/root/" % os.getenv("ATLAS_LOCAL_ROOT"), "")
    
if not WORKAREA and ROOTCOREBIN:
    ROOTCOREDIR = os.getenv("ROOTCOREDIR")
    ROOTCORECONFIG = os.getenv("ROOTCORECONFIG")
    ROOTCOREOBJ = os.getenv("ROOTCOREOBJ")
    ROOTVER = ROOTSYS.replace("-" + ROOTCORECONFIG, "")
    #   Determines the setup RC release. e.g. rcSetup Base 2.3.39
    RCRELEASE = ROOTCOREDIR [  ROOTCOREDIR.find("Analysis") + len("Analysis") : ROOTCOREDIR.find("RootCore") - 1 ]
    RCRELEASE = RCRELEASE.replace("/" , " ")
    WORKAREA = ROOTCOREBIN +"/../"

#######################################################################################
#                  Athena enviroment variables                                        #    
#######################################################################################
ATLASPROJECT=os.getenv("AtlasProject")
ATLASVERSION=os.getenv("AtlasVersion")
TESTAREA=os.getenv("TestArea")
PLATFORM=os.getenv("WorkDir_PLATFORM")
#WORKDIR=os.getenv("WorkDir_DIR")

CALIBPATH = os.getenv("CALIBPATH")
PYTHONPATH = os.getenv("PYTHONPATH")

if ATLASPROJECT == None and ROOTCOREBIN == None:
    print "ERROR: please setup either RootCore or Athena"
    exit(1)
elif ATLASPROJECT and ROOTCOREBIN:
    print "ERROR: How did you manage to setup Athena and RootCore in the same shell?!?!?!"
    exit(1)



def GetKinematicCutFromConfFile(Path, CutName):
    CutValue = -1
    from PathResolver import PathResolver
    InFile = open (PathResolver.FindCalibFile(Path))
    for line in InFile:
       if line.find(CutName) > -1: return float(line.replace(CutName+":", "").strip())
    print "WARNING: Could not find the property %s in ST config file %s"%(CutName,Path)
    return CutValue


def IsArgumentDefault(Arg, Name , Parser):
    if Parser == None: return  False
    return Arg == Parser.get_default(Name)

def createStdVecStr(ROOT, list):
    vs = ROOT.std.vector(ROOT.std.string)()
    for item in list: vs.push_back(item)
    return vs
def prettyPrint(preamble, data, width=30, separator=":"):
    """Prints uniformly-formatted lines of the type "preamble : data".
    """
    preamble = preamble.ljust(width)
    print '%s%s %s' % (preamble, separator, data)
def StringToBool (Str):
    if Str.lower() == "true": return True
    return False
def BoolToString (Boolean):
    if Boolean == True: return "true"
    return "false"
    
def ReadListFromFile (File):
    List = []
    if os.path.isfile(File) == True:
        for Line in open(File):
            Line = Line.strip()
            if not Line.startswith("#") and len(Line.replace("\n", "")) > 0 and Line not in List:
                List.append(Line.replace("\n", ""))
    else: print "Could not find ListFile " + str(File)
    return List

def IsListIn (List1=[] , List2=[]):
    for L in List1:
        EntryIn = False
        if len(List2) == 0:
            print "Reference list not found"
            return False
        for C in List2:
            if C.find(L.strip()) > -1:
                EntryIn = True
                break
        if EntryIn == False:
            print C + " not found in reference list"
            return False
    return True 

def ClearFromDuplicates(In = []):
    TmpIn = []
    for I in In:
        if I in TmpIn: print "INFO: %s is already in List"%(I)
        else: TmpIn.append(I)
    return TmpIn
