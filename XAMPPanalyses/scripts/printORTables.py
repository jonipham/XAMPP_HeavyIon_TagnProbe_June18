#! /usr/bin/env python
from XAMPPbase.printCutFlow import *

def getFinalCutFlow(RunOptions,CutFlowHistoItem):
    CutFlow = {}
    
    Nbins = CutFlowHistoItem[0].GetNbinsX()
    for ibin in range(1, Nbins + 1):
        thecontent = CutFlowHistoItem[0].GetBinContent(ibin)
        theerror = CutFlowHistoItem[0].GetBinError(ibin)
        thelabel = CutFlowHistoItem[0].GetXaxis().GetBinLabel(ibin)
        if thecontent == 0:
            break
        if RunOptions.weighted:
            CutFlow[ibin] = [thelabel,thecontent * CutFlowHistoItem[2] / CutFlowHistoItem[1],
                             math.sqrt(theerror**2.*CutFlowHistoItem[2]**2./CutFlowHistoItem[1]**2.+
                                       thecontent**2.*CutFlowHistoItem[2]**2./CutFlowHistoItem[1]**3.),
                                       100.*thecontent / CutFlowHistoItem[1],
                                       100.*math.sqrt(theerror**2./CutFlowHistoItem[1]**2.+thecontent**2./CutFlowHistoItem[1]**3.)]
        else:
            CutFlow[ibin] = [thelabel,thecontent,theerror,100.*thecontent/CutFlowHistoItem[1],
                             100.*math.sqrt(theerror**2./CutFlowHistoItem[1]**2.+thecontent**2./CutFlowHistoItem[1]**3.)]
            
    return CutFlow

# this function prints a table of one complete CutFlow
def printTable(CutFlows,label,doMoreDigits=False):
    
    for chan,values in CutFlows.iteritems():
        print '\\begin{table}[h!]'
        print '\\caption{%s, DSID: %s}'%(label,chan)
        print '\\label{my-label}'
        print '\\begin{tabular}{l|r|r}'
        print 'Cut & Yield & CutEffiency \\\\ \\hline'
        for ibin in sorted(values.iterkeys()):
#            if cut=="Initial":
            numbers = values[ibin]
            if doMoreDigits:
                print '%s & %.6f $\\pm$ %.6f & %.3f $\\pm$ %.3f \\\\'%(numbers[0].replace(">=","$\\geq$").replace("<=","$\\leq$").replace(">","$>$").replace("<","$<$").replace("_","\_"),numbers[1],numbers[2],numbers[3],numbers[4])
            else:
                print '%s & %.2f $\\pm$ %.2f & %.2f $\\pm$ %.2f \\\\'%(numbers[0].replace(">=","$\\geq$").replace("<=","$\\leq$").replace(">","$>$").replace("<","$<$").replace("_","\_"),numbers[1],numbers[2],numbers[3],numbers[4])
        print '\\hline'
        print '\\end{tabular}'
        print '\\end{table}'
    
# this function prints a table of one cut for all overlap removals and samples
def printComparisonTable(ORcomp,CutNumber=1,sumBG=False,doMoreDigits=False,doRaw=False):
    NSamples = len(ORcomp.items())
    if NSamples < 1:
        print 'No samples to print, exiting...'
        sys.exit(1)
    NOverlaps = len(ORcomp.itervalues().next().items())
    if NOverlaps < 1:
        print 'No overlap removals to print, exiting...'
        sys.exit(1)
    else:
        randomCutFlow = ORcomp.itervalues().next().itervalues().next().itervalues().next()
        for ibin in sorted(randomCutFlow.iterkeys()):
            if ibin == CutNumber:
                CutName = randomCutFlow[ibin][0]
    
    if sumBG:
        NColumns = NOverlaps+1
    else:
        NColumns = 2*NOverlaps+1
        
    print '\\begin{table}[h!]'
    if sumBG:
        print '\\caption{Comparison of yields at $\\gg$%s$\\ll$ cut using different overlaps removals. Thereby \\textit{noBjet} means OR.Bjet: false, \\textit{noJVT} OR.ApplyJVT: false, \\textit{noGhost} OR.DoMuonJetGhostAssociation: false, \\textit{noCalo} OR.RemoveCaloMuons: false.}'%(CutName.replace(">=","$\\geq$").replace("<=","$\\leq$").replace(">","$>$").replace("<","$<$").replace("_","\_"))
    else:
        print '\\caption{Comparison of yields and cut efficiency of $\\gg$%s$\\ll$ cut using different overlaps removals. The cut efficiency is defined as the percentage of the yield with respect to the number of events in the preskimmed dataset. Thereby \\textit{noBjet} means OR.Bjet: false, \\textit{noJVT} OR.ApplyJVT: false, \\textit{noGhost} OR.DoMuonJetGhostAssociation: false, \\textit{noCalo} OR.RemoveCaloMuons: false.}'%(CutName.replace(">=","$\\geq$").replace("<=","$\\leq$").replace(">","$>$").replace("<","$<$").replace("_","\_"))
    print '\\label{my-label}'
    print '\\begin{tabular}{l',
    for i in range(2,NColumns+1):
        print '|r',
    print '}'
    print ' & Overlap Removal',
    for i in range(3,NColumns+1):
        print ' & ',
    print ' \\\\ '
    for OR in sorted(ORcomp.itervalues().next().iterkeys(), reverse=True):
        if sumBG:
            print ' & %s '%getORString(OR),
        else:
            print ' & %s & '%getORString(OR),
    print ' \\\\ \\hline'
    if sumBG:
        print 'Sample ',
    else:
        print 'Sample/DSID ',
    for i in range(2,NColumns+1):
        if sumBG:
            if doRaw:
                print ' & Yields [raw] ',
            else:
                print ' & Yields [1pb$^{-1}$] ',
        else:
            if i % 2 == 0:
                if doRaw:
                    print ' & Yields [raw] ',
                else:
                    print ' & Yields [1pb$^{-1}$] ',
            else:
                print ' & Efficiency [\\%] ',
    print ' \\\\ \\hline'
      
    summedBGYields = {} # for calculating S/B
    for smp in sorted(ORcomp.iterkeys()):
        print '%s '%smp,
        DSYields={}
        summedBGYields[smp] = {}
        for OR in sorted(ORcomp[smp].iterkeys(), reverse=True):
            if sumBG:
                content = 0.
                error = 0.
                for mc_channel,values in ORcomp[smp][OR].iteritems():
                    for ibin in sorted(values.iterkeys()):
                        if ibin == CutNumber:
                            content+=values[ibin][1]
                            error+=values[ibin][2]*values[ibin][2]
                if doMoreDigits:
                    print ' & %.6f $\\pm$ %.6f '%(content,math.sqrt(error)),
                else:
                    if doRaw:
                        print ' & %.0f $\\pm$ %.2f '%(content,math.sqrt(error)),
                    else:
                        print ' & %.2f $\\pm$ %.2f '%(content,math.sqrt(error)),
                summedBGYields[smp][OR] = [content,math.sqrt(error)]
            else:
                for mc_channel,values in ORcomp[smp][OR].iteritems():
                    for ibin in sorted(values.iterkeys()):
                        if ibin == CutNumber:
                            try:
                                DSYields[mc_channel]
                            except Exception, e:
                                DSYields[mc_channel]={}
                            DSYields[mc_channel][OR]=[values[ibin][1],values[ibin][2],values[ibin][3],values[ibin][4]]
        if sumBG:
            print '\\\\'
        else:
            for i in range(2,NColumns+1):
                print ' & ',
            print '\\\\'
            for DS in sorted(DSYields.iterkeys()):
                print '%s '%DS,
                for DSOR in sorted(DSYields[DS].iterkeys(), reverse=True):
                    yields=DSYields[DS][DSOR]
                    if doMoreDigits:
                        print ' & %.6f $\\pm$ %.6f & %.2f $\\pm$ %.2f '%(yields[0],yields[1],yields[2],yields[3]),
                    else:
                        if doRaw:
                            print ' & %.0f $\\pm$ %.2f & %.2f $\\pm$ %.2f '%(yields[0],yields[1],yields[2],yields[3]),
                        else:
                            print ' & %.2f $\\pm$ %.2f & %.2f $\\pm$ %.2f '%(yields[0],yields[1],yields[2],yields[3]),
                print '\\\\'
    print '\\hline'
    if sumBG:
        TotalBGForOR = {}
        for OR in sorted(ORcomp[smp].iterkeys(), reverse=True):
            TotalBGContent = 0.
            TotalBGError = 0.
            for smp in sorted(ORcomp.iterkeys()):
                if not "Signal" in smp:
                    TotalBGContent += summedBGYields[smp][OR][0]
                    TotalBGError += summedBGYields[smp][OR][1]*summedBGYields[smp][OR][1]
            TotalBGForOR[OR] = [TotalBGContent,math.sqrt(TotalBGError)]
        for smp in sorted(ORcomp.iterkeys()):
            if "Signal" in smp:
                print '$S/B$ %s [\\%%]'%smp,
                for OR in sorted(ORcomp[smp].iterkeys(), reverse=True):
                    print ' & %.2f $\\pm$ %.3f '%(100.*summedBGYields[smp][OR][0]/TotalBGForOR[OR][0],
                                                  100.*math.sqrt((summedBGYields[smp][OR][1]/TotalBGForOR[OR][0])**2.)+
                                                  (summedBGYields[smp][OR][0]*TotalBGForOR[OR][1])**2./(TotalBGForOR[OR][0])**4.),
                print '\\\\ \\hline'
                    
    print '\\end{tabular}'
    print '\\end{table}'
    
def getORString(OR):
    if OR == "Stop1L":
        return OR
    elif OR == "Nominal":
        return OR
    elif OR == "OR.Bjet: false":
        return "noBjet"
    elif OR == "OR.ApplyJVT: false":
        return "noJVT"
    elif OR == "OR.DoMuonJetGhostAssociation: false":
        return "noGhost"
    elif OR == "OR.RemoveCaloMuons: false":
        return "noCalo"

def getCutflowDict(File,RunOptions,DSID=0,label=""):
    
    FinalCutFlowHistos = {}
    CutFlowHistos = {}
    
    TheFile = ROOT.TFile.Open(File)
    
    MetaData = getMetaData(TheFile.Get("MetaData"))
    
    if RunOptions.weighted:
        if max(MetaData.iterkeys()) == -1:
            print 'ERROR: Cannot print weighted CutFlow for Data, exiting...'
            sys.exit(1)
        for mc_channel in MetaData.iterkeys():
            if mc_channel == -1:
                continue
            HistoName = "Histos_%s_Nominal/InfoHistograms/%s_CutFlow_weighted" % (RunOptions.analysis,mc_channel)
            CutFlowHistos[mc_channel] = [getCutFlow(TheFile,HistoName),MetaData[mc_channel][1],MetaData[mc_channel][2]]

    elif RunOptions.weightedNoXSec:
        if max(MetaData.iterkeys()) == -1:
            print 'ERROR: Cannot print weighted CutFlow for Data, exiting...'
            sys.exit(1)
        for mc_channel in MetaData.iterkeys():
            if mc_channel == -1:
                continue
            HistoName = "Histos_%s_Nominal/InfoHistograms/%s_CutFlow_weighted" % (RunOptions.analysis,mc_channel)
            CutFlowHistos[mc_channel] = [getCutFlow(TheFile,HistoName),MetaData[mc_channel][1]]

    else:
        for mc_channel in MetaData.iterkeys():
            if mc_channel == -1:
                HistoName = "Histos_%s_Nominal/InfoHistograms/CutFlow" % (RunOptions.analysis)
            else:
                HistoName = "Histos_%s_Nominal/InfoHistograms/%s_CutFlow" % (RunOptions.analysis,mc_channel)
            CutFlowHistos[mc_channel] = [getCutFlow(TheFile,HistoName),MetaData[mc_channel][0]]


    
    for mc_channel in CutFlowHistos.iterkeys():
        if DSID!=0:
            if mc_channel == DSID:
                FinalCutFlowHistos[mc_channel] = getFinalCutFlow(RunOptions,CutFlowHistos[mc_channel])
        else:
            FinalCutFlowHistos[mc_channel] = getFinalCutFlow(RunOptions,CutFlowHistos[mc_channel])


    return FinalCutFlowHistos
       
#     doMoreDigits=False
#     if RunOptions.weighted:
#         doMoreDigits=True
#     printTable(FinalCutFlowHistos,label,doMoreDigits=doMoreDigits)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog='printORTables')
    parser.add_argument('-a', '--analysis', help='choose CutFlow to be printed', default='MyCutFlow', required=True)
    parser.add_argument('--weighted', help='use for weighted cutflow comparisons', action='store_true', default=False)
    parser.add_argument('--weightedNoXSec', help='use for weighted cutflow comparisons', action='store_true', default=False)

    RunOptions = parser.parse_args()

    # Set batch mode and atlas style
    ROOT.gROOT.SetBatch(True)

    doMoreDigits=RunOptions.weighted
    doRaw=not RunOptions.weighted and not RunOptions.weightedNoXSec
    
    ORcomp = {}
    
    ORs600_300 = {}
    ORs800_1 = {}
    ORsttbar = {}
    ORsWjets = {}
    ORsZjets = {}
    
    currentOR = "Stop1L"
    ORs600_300[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/signal_nominal_new/signal_nominal_new.root",RunOptions,DSID=387168,label="Signal 600\_300 in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORs800_1[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/signal_nominal_new/signal_nominal_new.root",RunOptions,DSID=387198,label="Signal 800\_1 in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsttbar[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-29/ttbar_stop1L/ttbar_stop1L.root",RunOptions,label="$\\ttbar$ in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsWjets[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/wjets_nominal_new/wjets_nominal_new.root",RunOptions,label="W+jets in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsZjets[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/zjets_nominal_new/zjets_nominal_new.root",RunOptions,label="Z+jets in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    
    currentOR = "Nominal"
    ORs600_300[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/signal_nominal_new2/signal_nominal_new2.root",RunOptions,DSID=387168,label="Signal 600\_300 in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORs800_1[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/signal_nominal_new2/signal_nominal_new2.root",RunOptions,DSID=387198,label="Signal 800\_1 in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsttbar[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-29/ttbar_nominal/ttbar_nominal.root",RunOptions,label="$\\ttbar$ in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsWjets[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/wjets_nominal_new2/wjets_nominal_new2.root",RunOptions,label="W+jets in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsZjets[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/zjets_nominal_new2/zjets_nominal_new2.root",RunOptions,label="Z+jets in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    
    currentOR = "OR.Bjet: false"
    ORs600_300[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/signal_noBjet_new/signal_noBjet_new.root",RunOptions,DSID=387168,label="Signal 600\_300 in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORs800_1[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/signal_noBjet_new/signal_noBjet_new.root",RunOptions,DSID=387198,label="Signal 800\_1 in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsttbar[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-29/ttbar_noBjet/ttbar_noBjet.root",RunOptions,label="$\\ttbar$ in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsWjets[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/wjets_noBjet_new/wjets_noBjet_new.root",RunOptions,label="W+jets in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsZjets[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/zjets_noBjet_new/zjets_noBjet_new.root",RunOptions,label="Z+jets in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    
    currentOR = "OR.ApplyJVT: false"
    ORs600_300[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/signal_noJVT_new/signal_noJVT_new.root",RunOptions,DSID=387168,label="Signal 600\_300 in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORs800_1[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/signal_noJVT_new/signal_noJVT_new.root",RunOptions,DSID=387198,label="Signal 800\_1 in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsttbar[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-29/ttbar_noJVT/ttbar_noJVT.root",RunOptions,label="$\\ttbar$ in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsWjets[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/wjets_noJVT_new/wjets_noJVT_new.root",RunOptions,label="W+jets in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsZjets[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/zjets_noJVT_new/zjets_noJVT_new.root",RunOptions,label="Z+jets in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    
    currentOR = "OR.DoMuonJetGhostAssociation: false"
    ORs600_300[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/signal_noGhost_new/signal_noGhost_new.root",RunOptions,DSID=387168,label="Signal 600\_300 in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORs800_1[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/signal_noGhost_new/signal_noGhost_new.root",RunOptions,DSID=387198,label="Signal 800\_1 in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsttbar[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-29/ttbar_noGhost/ttbar_noGhost.root",RunOptions,label="$\\ttbar$ in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsWjets[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/wjets_noGhost_new/wjets_noGhost_new.root",RunOptions,label="W+jets in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsZjets[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/zjets_noGhost_new/zjets_noGhost_new.root",RunOptions,label="Z+jets in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    
    currentOR = "OR.RemoveCaloMuons: false"
    ORs600_300[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/signal_noRemoveCalo_new/signal_noRemoveCalo_new.root",RunOptions,DSID=387168,label="Signal 600\_300 in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORs800_1[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/signal_noRemoveCalo_new/signal_noRemoveCalo_new.root",RunOptions,DSID=387198,label="Signal 800\_1 in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsttbar[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-29/ttbar_noRemoveCalo/ttbar_noRemoveCalo.root",RunOptions,label="$\\ttbar$ in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsWjets[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/wjets_noRemoveCalo_new/wjets_noRemoveCalo_new.root",RunOptions,label="W+jets in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    ORsZjets[currentOR] = getCutflowDict("/ptmp/mpp/niko/Cluster/Output/2016-03-22/zjets_noRemoveCalo_new/zjets_noRemoveCalo_new.root",RunOptions,label="Z+jets in %s with OR: %s"%(RunOptions.analysis.replace("_","\_"),currentOR))
    
    ORcomp["Signal 600\_300"] = ORs600_300
    ORcomp["Signal 800\_1"] = ORs800_1
    ORcomp["$t\\bar{t}$"] = ORsttbar
    ORcomp["W+jets"] = ORsWjets
    ORcomp["Z+jets"] = ORsZjets
    
    # for Stop0L_SR
    # CutNumber 11 = Jet2Pt
    # CutNumber 12 = Met250
    # CutNumber 13 = 6jets
    # CutNumber 17 = 2b-jets
    # CutNumber 20 = MtBmin
    # for Stop0L_SR600
    # CutNumber 13 = Jet4Pt
    # CutNumber 17 = 2b-jets
    printComparisonTable(ORcomp,CutNumber=17,sumBG=True,doMoreDigits=doMoreDigits,doRaw=doRaw)
    


    
    
