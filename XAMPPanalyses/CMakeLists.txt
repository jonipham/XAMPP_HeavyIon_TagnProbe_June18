# Declare the package name:
atlas_subdir( XAMPPanalyses )

# Extra dependencies, based on the environment:
set( extra_dep )
if( XAOD_STANDALONE )
   set( extra_dep Control/xAODRootAccess )
else()
   set( extra_dep Control/AthenaBaseComps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODCore
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODJet
   Event/xAOD/xAODMissingET
   Event/xAOD/xAODTrigMissingET
   Event/xAOD/xAODMuon
   Event/xAOD/xAODTau
   Event/xAOD/xAODTracking
   Event/xAOD/xAODTruth
   Event/xAOD/xAODParticleEvent 
   Event/xAOD/xAODLuminosity 
   PhysicsAnalysis/AnalysisCommon/AssociationUtils
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/AnalysisCommon/PMGTools
   PhysicsAnalysis/MCTruthClassifier
   PhysicsAnalysis/SUSYPhys/SUSYTools 
   Reconstruction/Jet/JetJvtEfficiency
   Reconstruction/Jet/JetSubStructureUtils
   Trigger/TrigAnalysis/TrigDecisionTool
   PRIVATE
   XAMPPbase   
   AsgExternal/Asg_Test
   Control/xAODRootAccess
   Control/AthContainers
   DataQuality/GoodRunsLists
   Event/EventPrimitives
   Event/FourMomUtils
   Event/xAOD/xAODBTagging
   Event/xAOD/xAODBase
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODCutFlow
   Event/xAOD/xAODPrimitives
   PhysicsAnalysis/AnalysisCommon/IsolationSelection
   PhysicsAnalysis/AnalysisCommon/PileupReweighting
   PhysicsAnalysis/ElectronPhotonID/ElectronEfficiencyCorrection
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonFourMomentumCorrection
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonShowerShapeFudgeTool
   PhysicsAnalysis/ElectronPhotonID/IsolationCorrections
   PhysicsAnalysis/ElectronPhotonID/PhotonEfficiencyCorrection
   PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
   PhysicsAnalysis/JetMissingEtID/JetSelectorTools
   PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/xAODBTaggingEfficiency
   PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonEfficiencyCorrections
   PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonMomentumCorrections
   PhysicsAnalysis/MuonID/MuonSelectorTools
   PhysicsAnalysis/TauID/TauAnalysisTools
   PhysicsAnalysis/HeavyIonPhys/HIEventUtils
   Reconstruction/Jet/JetCPInterfaces
   Reconstruction/Jet/JetCalibTools
   Reconstruction/Jet/JetInterface
   Reconstruction/Jet/JetResolution
   Reconstruction/Jet/JetUncertainties
   Reconstruction/Jet/JetMomentTools
   Reconstruction/MET/METInterface
   Reconstruction/MET/METUtilities
   Reconstruction/tauRecTools
   Tools/PathResolver
   Trigger/TrigAnalysis/TriggerMatchingTool
   Trigger/TrigConfiguration/TrigConfInterfaces
   Trigger/TrigConfiguration/TrigConfxAOD
   ${extra_deps} )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree RIO Hist Physics )

# Libraries in the package:
atlas_add_library( XAMPPanalysesLib
   XAMPPanalyses/*.h Root/*.cxx
   PUBLIC_HEADERS XAMPPanalyses
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools xAODCore xAODEgamma xAODEventInfo xAODRootAccess
   xAODJet xAODMissingET xAODMuon xAODTau xAODTracking xAODTruth xAODParticleEvent xAODLuminosity SUSYToolsLib xAODCutFlow
   XAMPPbaseLib 
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${BOOST_LIBRARIES}
   AssociationUtilsLib PATInterfaces TrigDecisionToolLib PMGToolsLib
   MCTruthClassifierLib JetJvtEfficiencyLib JetSubStructureUtils
   AthContainers EventPrimitives FourMomUtils xAODBTagging xAODBase
   xAODPrimitives IsolationSelectionLib PileupReweightingLib
   ElectronEfficiencyCorrectionLib ElectronPhotonFourMomentumCorrectionLib
   ElectronPhotonSelectorToolsLib ElectronPhotonShowerShapeFudgeToolLib
   IsolationCorrectionsLib PhotonEfficiencyCorrectionLib JetSelectorToolsLib
   xAODBTaggingEfficiencyLib MuonEfficiencyCorrectionsLib
   MuonMomentumCorrectionsLib MuonSelectorToolsLib TauAnalysisToolsLib
   JetCPInterfaces JetCalibToolsLib JetInterface JetResolutionLib
   JetUncertaintiesLib JetMomentToolsLib METInterface METUtilitiesLib
   PathResolver TriggerMatchingToolLib TrigConfInterfaces TrigConfxAODLib
   xAODTrigMissingET xAODMetaData GoodRunsListsLib
   PRIVATE_LINK_LIBRARIES xAODTrigger PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( XAMPPanalyses
      src/*.h src/*.cxx src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel GoodRunsListsLib
      AthenaKernel AthAnalysisBaseCompsLib xAODEventInfo xAODMuon xAODPrimitives xAODCutFlow
      xAODJet xAODBTagging xAODEgamma xAODMissingET xAODTracking xAODTau xAODParticleEvent
      TauAnalysisToolsLib xAODCore AthContainers AsgTools xAODBase xAODCutFlow xAODRootAccess
      xAODMetaData PATInterfaces PathResolver SUSYToolsLib XAMPPbaseLib  XAMPPanalysesLib )
endif()

atlas_add_dictionary( XAMPPanalysesDict
   XAMPPanalyses/XAMPPanalysesDict.h
   XAMPPanalyses/selection.xml
   LINK_LIBRARIES XAMPPanalysesLib )


# Install files from the package:
atlas_install_data( data/* )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

