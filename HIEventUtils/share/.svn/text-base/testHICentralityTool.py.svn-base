#specify how many events it should process. Set it equal to -1 to process all events
theApp.EvtMax = 100

#set input files - svcMgr has a property that lets you specify the input files, as a list:
import AthenaPoolCnvSvc.ReadAthenaPool
#svcMgr.EventSelector.InputCollections = [ "/afs/cern.ch/work/q/qhu/public/data15_hi.00287334.physics_HardProbes.merge.AOD.r7874_p2580/AOD.08382621._000003.pool.root.1" ]
svcMgr.EventSelector.InputCollections = [ "/afs/cern.ch/work/q/qhu/public/data16_hip8TeV.00313259.physics_Main.recon.AOD.f774_m1736/data16_hip8TeV.00313259.physics_Main.recon.AOD.f774_m1736._lb0405._0002.1" ]

#One of the existing master sequences where one should attach all algorithms
algSeq = CfgMgr.AthSequencer("AthAlgSeq")

#In order to debug the tool, need to set OutputLevel and pass the tool to test alg
from HIEventUtils.HIEventUtilsConf import HI__HICentralityTool
CentralityTool = HI__HICentralityTool( 
    name 	= "HICentralityTool",
    RunSpecies 	= "pPb2016",
    OutputLevel = DEBUG)
ToolSvc += CentralityTool
print CentralityTool


from HIEventUtils.HIEventUtilsConf import TestCentralityCalib
alg = TestCentralityCalib(
    name 		= "centralityTestAlg",
    HICentralityTool 	= CentralityTool)

#comment this line
alg.OutputLevel = DEBUG
algSeq += alg
