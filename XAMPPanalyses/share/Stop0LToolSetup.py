include ("XAMPPbase/BaseToolSetup.py")

def SetupStop0LMetSelector():
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc, "SUSYMetSelector"):
        from XAMPPanalyses.XAMPPanalysesConf import XAMPP__Stop0LMetSelector
        MetSelector = CfgMgr.XAMPP__Stop0LMetSelector("SUSYMetSelector")
        MetSelector.IncludeTaus = False
        MetSelector.IncludePhotons = True
        MetSelector.DoTrackMet = True
        ToolSvc+= MetSelector
    return getattr(ToolSvc, "SUSYMetSelector" )

def SetupStop0LTruthSelector():
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc, "SUSYTruthSelector"):
        from XAMPPanalyses.XAMPPanalysesConf import XAMPP__Stop0LTruthSelector
        TruthSelector = CfgMgr.XAMPP__Stop0LTruthSelector("SUSYTruthSelector")
        TruthSelector.TruthBJetPtCut = 20000.
        TruthSelector.TruthBJetEtaCut = 2.5
        TruthSelector.TruthJetPtCut = 20000.
        TruthSelector.TruthJetEtaCut = 2.8
        ToolSvc+= TruthSelector
    return getattr(ToolSvc, "SUSYTruthSelector" )

def SetupStop0LAnalysisConfig(TreeName="Stop0L",CutFlowsToRun=[]):
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc, "AnalysisConfig"):
        if "doGamma" in globals():
            from XAMPPanalyses.XAMPPanalysesConf import XAMPP__Stop0LGammaAnalysisConfig
            AnalysisConfig = CfgMgr.XAMPP__Stop0LGammaAnalysisConfig(name = "AnalysisConfig", TreeName = TreeName)
        else:
            from XAMPPanalyses.XAMPPanalysesConf import XAMPP__Stop0LAnalysisConfig
            AnalysisConfig = CfgMgr.XAMPP__Stop0LAnalysisConfig(name = "AnalysisConfig", TreeName = TreeName)
        if len(CutFlowsToRun) > 0: AnalysisConfig.ActiveCutflows = CutFlowsToRun
        ToolSvc+= AnalysisConfig
    return getattr(ToolSvc, "AnalysisConfig" )

def SetupStop0LAnalysisHelper(TreeName="Stop0L",CutFlowsToRun=[]):
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc,"AnalysisHelper"):
        if "doGamma" in globals():
            from XAMPPanalyses.XAMPPanalysesConf import XAMPP__Stop0LGammaAnalysisHelper
            AnalysisHelper = CfgMgr.XAMPP__Stop0LGammaAnalysisHelper("AnalysisHelper")
        else:
            from XAMPPanalyses.XAMPPanalysesConf import XAMPP__Stop0LAnalysisHelper
            AnalysisHelper = CfgMgr.XAMPP__Stop0LAnalysisHelper("AnalysisHelper")
        AnalysisHelper.AnalysisConfig = SetupStop0LAnalysisConfig(TreeName=TreeName,CutFlowsToRun=CutFlowsToRun)
        if "doISR" in globals():
            AnalysisHelper.AnalysisConfig.doISR = doISR
            AnalysisHelper.doISR = doISR
        AnalysisHelper.MetSelector = SetupStop0LMetSelector()
        AnalysisHelper.doWDecays = True
        AnalysisHelper.doReclustering = True
        AnalysisHelper.CustomReclusterPt = 20000.
        if not isData():
            AnalysisHelper.TruthSelector = SetupStop0LTruthSelector()
        ToolSvc += AnalysisHelper
    return getattr(ToolSvc, "AnalysisHelper")

def SetupJVTAnalysisConfig(TreeName = "JVT", CutFlowsToRun = []):
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc,"AnalysisConfig"):
        from XAMPPanalyses.XAMPPanalysesConf import XAMPP__JVTAnalysisConfig
        Config = CfgMgr.XAMPP__JVTAnalysisConfig("AnalysisConfig",
                                                TreeName = TreeName,
                                                ActiveCutflows = CutFlowsToRun)
        ToolSvc += Config
    return getattr(ToolSvc, "AnalysisConfig")

def SetupJVTAnalysisHelper(TreeName = "JVT", CutFlowsToRun = []):
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr    
    if not hasattr(ToolSvc,"AnalysisHelper"):
        from XAMPPanalyses.XAMPPanalysesConf import XAMPP__JVTAnalysisHelper
        AnalysisHelper = CfgMgr.XAMPP__JVTAnalysisHelper("AnalysisHelper")
        AnalysisHelper.AnalysisConfig = SetupJVTAnalysisConfig(TreeName,CutFlowsToRun)
        ToolSvc += AnalysisHelper
    return getattr(ToolSvc,"AnalysisHelper")

    
