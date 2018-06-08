#! /usr/bin/env python
import ROOT
import sys
import argparse
import pprint
import math

# this function returns a dictionnary with MetaData information of the form:
# { mcChannelNumber : [initialEntries, initialWeightedEntries, xSecTimesEff] }
# for Data: mcChannelNumber = initialWeightedEntries = xSecTimesEff = -1
def getMetaData(MDtree):
    print 'Extracting MetaData information...'
    MetaData = {}
    Nentries = MDtree.GetEntries()
    for ientry in range(0, Nentries):
        MDtree.GetEntry(ientry)
        if MDtree.isData:
            runNumber = MDtree.runNumber
            if not 'Data' in MetaData.iterkeys():
                MetaData['Data'] = {}
            if not runNumber in MetaData['Data'].iterkeys():
                MetaData['Data'][runNumber] = {}
                MetaData['Data'][runNumber]['TotalEvents'] = MDtree.TotalEvents
                MetaData['Data'][runNumber]['ProcessedEvents'] = MDtree.ProcessedEvents
            else:
                MetaData['Data'][runNumber]['TotalEvents'] += MDtree.TotalEvents
                MetaData['Data'][runNumber]['ProcessedEvents'] += MDtree.ProcessedEvents
        else:
            mcChannelNumber = MDtree.mcChannelNumber
            ProcessID = MDtree.ProcessID
            if not 'MC' in MetaData.iterkeys():
                MetaData['MC'] = {}
            if not mcChannelNumber in MetaData['MC'].iterkeys():
                MetaData['MC'][mcChannelNumber] = {}
                MetaData['MC'][mcChannelNumber]['TotalEvents'] = 0.
                MetaData['MC'][mcChannelNumber]['ProcessedEvents'] = 0.
                MetaData['MC'][mcChannelNumber]['TotalSumW'] = 0.
                MetaData['MC'][mcChannelNumber]['ProcessID'] = {}
            if ProcessID != 0:
                if not ProcessID in MetaData['MC'][mcChannelNumber]['ProcessID'].iterkeys():
                    MetaData['MC'][mcChannelNumber]['ProcessID'][ProcessID] = {}
                    MetaData['MC'][mcChannelNumber]['ProcessID'][ProcessID]['xSection'] = MDtree.xSection
                    MetaData['MC'][mcChannelNumber]['ProcessID'][ProcessID]['kFactor'] = MDtree.kFactor
                    MetaData['MC'][mcChannelNumber]['ProcessID'][ProcessID]['FilterEfficiency'] = MDtree.FilterEfficiency
                    MetaData['MC'][mcChannelNumber]['ProcessID'][ProcessID]['TotalSumW'] = MDtree.TotalSumW
                else:
                    if MetaData['MC'][mcChannelNumber]['ProcessID'][ProcessID]['xSection'] != MDtree.xSection: print "xSection differs for mcChannelNumber %d and ProcessID %d"(mcChannelNumber,ProcessID)
                    if MetaData['MC'][mcChannelNumber]['ProcessID'][ProcessID]['kFactor'] != MDtree.kFactor: print "kFactor differs for mcChannelNumber %d and ProcessID %d"(mcChannelNumber,ProcessID)
                    if MetaData['MC'][mcChannelNumber]['ProcessID'][ProcessID]['FilterEfficiency'] != MDtree.FilterEfficiency: print "FilterEfficiency differs for mcChannelNumber %d and ProcessID %d"(mcChannelNumber,ProcessID)
                    MetaData['MC'][mcChannelNumber]['ProcessID'][ProcessID]['TotalSumW'] += MDtree.TotalSumW
            else:
                MetaData['MC'][mcChannelNumber]['TotalEvents'] += MDtree.TotalEvents
                MetaData['MC'][mcChannelNumber]['ProcessedEvents'] += MDtree.ProcessedEvents
                MetaData['MC'][mcChannelNumber]['TotalSumW'] += MDtree.TotalSumW
                MetaData['MC'][mcChannelNumber]['xSection'] = MDtree.xSection
                MetaData['MC'][mcChannelNumber]['kFactor'] = MDtree.kFactor
                MetaData['MC'][mcChannelNumber]['FilterEfficiency'] = MDtree.FilterEfficiency

    return MetaData

# this function returns the CutFlow histogram 'HistoName' from the file 'File'
def getCutFlow(File, HistoName):
    if not File.Get(HistoName):
        print 'ERROR: CutFlow %s was not found, did you use the right analysis name? Exiting...' % HistoName
        sys.exit(1)
    else:
        CutFlowHisto = File.Get(HistoName)
    CutFlowHisto.SetDirectory(0)
    CutFlowHisto.AddDirectory(ROOT.kFALSE)
    return CutFlowHisto

# this functions prints out the CutFlowHisto
def printCutFlow(CutFlowHisto, xSecWeight=1., doRaw=False, systematic='', lumi=1.):
    Nbins = CutFlowHisto.GetNbinsX()
    labellengths = []
    for ibin in range(1, Nbins + 1):
        labellengths.append(len(CutFlowHisto.GetXaxis().GetBinLabel(ibin)) + 2)
    prettyPrint("CutFlowHisto", CutFlowHisto.GetName(), "", width1=max(labellengths))
    prettyPrint("Systematic variation", systematic, "", width1=max(labellengths))
    separatorString  = ""
    for i in range(max(labellengths)+max(len(CutFlowHisto.GetName()),len(systematic))):
        separatorString += "#"
    print separatorString
    prettyPrint("Cut", "Yields", "", width1=max(labellengths))
    print separatorString
    width2 = 0
    for ibin in range(1, Nbins + 1):
        thecontent = CutFlowHisto.GetBinContent(ibin)
        theerror = CutFlowHisto.GetBinError(ibin)
        thelabel = CutFlowHisto.GetXaxis().GetBinLabel(ibin)
        if thecontent == 0:
            break
        if doRaw:
            content = '%.0f ' % (thecontent * xSecWeight)
            error = ' %.2f' % (theerror * xSecWeight)
        else:
            content = '%.8f ' % (thecontent * xSecWeight * lumi)
            error = ' %.8f' % (theerror * xSecWeight * lumi)
        if ibin == 1:
            width2 = len(content)
        prettyPrint(thelabel, content, error, width1=max(labellengths), width2=width2, separator2=(u'\u00B1').encode('utf-8'))
    print separatorString
    
# this function prints uniformly-formatted lines of the type 'preamble : data1 data2' (if separator1=':')
def prettyPrint(preamble, data1, data2, width1=24, width2=10, separator1="", separator2=""):
    preamble = preamble.ljust(width1)
    data1 = data1.ljust(width2)
    print(preamble + separator1 + data1 + separator2 + data2)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog='printCutFlow', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--inputFile', help='choose input ROOT file', default='AnalysisOutput.root')
    parser.add_argument('-a', '--analysis', help='choose CutFlow to be printed', default='MyCutFlow', required=True)
    parser.add_argument('--weighted', help='use for weighted cutflow comparisons', action='store_true', default=False)
    parser.add_argument('--weightedNoXSec', help='use for weighted cutflow comparisons', action='store_true', default=False)
    parser.add_argument('-l', '--lumi', help='specify a luminosity (in pb^-1) for which the weighted CutFlow shall be printed', default=1., type=float)
    parser.add_argument('-s', '--systematic', '--syst', help='specify a systematic uncertainty for which the CutFlow shall be printed', default='Nominal')

    RunOptions = parser.parse_args()

    # Set batch mode and atlas style
    ROOT.gROOT.SetBatch(True)

    TheFile = ROOT.TFile.Open(RunOptions.inputFile)
    
    MetaData = getMetaData(TheFile.Get("MetaDataTree"))
    
    if 'Data' in MetaData.iterkeys():
        print 'Having found CutFlow for Data, printing...'
        HistoName = "Histos_%s_%s/InfoHistograms/CutFlow" % (RunOptions.analysis, RunOptions.systematic)
        CutFlowHisto = getCutFlow(TheFile, HistoName)
        printCutFlow(CutFlowHisto, doRaw=True, systematic=RunOptions.systematic)
    if 'MC' in MetaData.iterkeys():
        print 'Having found CutFlow for MC, printing...'
        for mcChannelNumber in MetaData['MC'].iterkeys():
            if RunOptions.weighted:
                HistoName = "Histos_%s_%s/InfoHistograms/DSID_%s_CutFlow_weighted" % (RunOptions.analysis, RunOptions.systematic, mcChannelNumber)
                CutFlowHisto = getCutFlow(TheFile, HistoName)
                xSecWeight = 0.
                if len(MetaData['MC'][mcChannelNumber]['ProcessID']) > 0:
                    for ProcessID in MetaData['MC'][mcChannelNumber]['ProcessID'].iterkeys():
                        MDentry = MetaData['MC'][mcChannelNumber]['ProcessID'][ProcessID]
                        xSecWeight += MDentry['xSection']*MDentry['kFactor']*MDentry['FilterEfficiency']/MDentry['TotalSumW']
                else:
                    MDentry = MetaData['MC'][mcChannelNumber]
                    xSecWeight += MDentry['xSection']*MDentry['kFactor']*MDentry['FilterEfficiency']/MDentry['TotalSumW']
                print 'Printing weighted events (with xSection weight %s)'%str(xSecWeight)
                printCutFlow(CutFlowHisto, xSecWeight=xSecWeight, systematic=RunOptions.systematic, lumi=RunOptions.lumi)
            elif RunOptions.weightedNoXSec:
                HistoName = "Histos_%s_%s/InfoHistograms/DSID_%s_CutFlow_weighted" % (RunOptions.analysis, RunOptions.systematic, mcChannelNumber)
                CutFlowHisto = getCutFlow(TheFile, HistoName)
                print 'Printing weighted events (without xSection weight)'
                printCutFlow(CutFlowHisto, systematic=RunOptions.systematic)
            else:
                HistoName = "Histos_%s_%s/InfoHistograms/DSID_%s_CutFlow" % (RunOptions.analysis, RunOptions.systematic, mcChannelNumber)
                CutFlowHisto = getCutFlow(TheFile, HistoName)
                print 'Printing raw events'
                printCutFlow(CutFlowHisto, doRaw=True, systematic=RunOptions.systematic)
            
