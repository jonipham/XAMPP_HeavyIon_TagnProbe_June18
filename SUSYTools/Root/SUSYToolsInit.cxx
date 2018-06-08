/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SUSYTools/SUSYObjDef_xAOD.h"

using namespace ST;

#include "JetCalibTools/JetCalibrationTool.h"
#include "JetSelectorTools/JetCleaningTool.h"
#include "JetResolution/JERTool.h"
#include "JetResolution/JERSmearingTool.h"
#include "JetUncertainties/JetUncertaintiesTool.h"
#include "JetMomentTools/JetVertexTaggerTool.h"
#include "JetJvtEfficiency/JetJvtEfficiency.h"
#include "JetMomentTools/JetForwardJvtTool.h"
//
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"
#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"
#include "MuonSelectorTools/MuonSelectionTool.h"
#include "MuonEfficiencyCorrections/MuonTriggerScaleFactors.h"
//
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"
#include "ElectronEfficiencyCorrection/ElectronChargeEfficiencyCorrectionTool.h"
#include "ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonShowerShapeFudgeTool.h"
#include "PhotonEfficiencyCorrection/AsgPhotonEfficiencyCorrectionTool.h"
#include "ElectronPhotonSelectorTools/EGammaAmbiguityTool.h"
#include "ElectronPhotonSelectorTools/AsgElectronChargeIDSelectorTool.h"
//
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/TauTruthMatchingTool.h" 
#include "TauAnalysisTools/TauOverlappingElectronLLHDecorator.h"
#include "tauRecTools/TauWPDecorator.h"
//
#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"
//
#include "METUtilities/METMaker.h"
#include "METUtilities/METSystematicsTool.h"
// xAOD trigger
#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TriggerMatchingTool/MatchingTool.h"
//
#include "IsolationCorrections/IsolationCorrectionTool.h"
#include "IsolationSelection/IsolationSelectionTool.h"
#include "IsolationSelection/IsolationCloseByCorrectionTool.h"
//
#include "PileupReweighting/PileupReweightingTool.h"
#include "PathResolver/PathResolver.h"
//
#include "AssociationUtils/OverlapRemovalInit.h"
#include "AssociationUtils/OverlapRemovalTool.h"
#include "AssociationUtils/EleJetOverlapTool.h"
#include "AssociationUtils/MuJetOverlapTool.h"
//
#include "PMGTools/PMGSherpa22VJetsWeightTool.h"

//#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

#define SET_DUAL_TOOL( TOOLHANDLE, TOOLTYPE, TOOLNAME )                \
  ASG_SET_ANA_TOOL_TYPE(TOOLHANDLE, TOOLTYPE);                        \
  TOOLHANDLE.setName(TOOLNAME);                                                

#define CONFIG_EG_EFF_TOOL( TOOLHANDLE, TOOLNAME, CORRFILE )                \
  if( !TOOLHANDLE.isUserConfigured() ) {                                \
    SET_DUAL_TOOL(TOOLHANDLE, AsgElectronEfficiencyCorrectionTool, TOOLNAME); \
    std::vector< std::string > corrFileNameList = {CORRFILE}; \
    ATH_CHECK( TOOLHANDLE.setProperty("CorrectionFileNameList", corrFileNameList) ); \
    if(!isData())                                                        \
      ATH_CHECK (TOOLHANDLE.setProperty("ForceDataType", (int) data_type) ); \
    ATH_CHECK( TOOLHANDLE.setProperty("CorrelationModel", m_EG_corrModel) ); \
    ATH_CHECK( TOOLHANDLE.initialize() );                                \
  }  

#define CONFIG_EG_EFF_TOOL_KEY( TOOLHANDLE, TOOLNAME, KEYNAME, KEY )        \
  if( !TOOLHANDLE.isUserConfigured() ) {                                \
    SET_DUAL_TOOL(TOOLHANDLE, AsgElectronEfficiencyCorrectionTool, TOOLNAME); \
    std::cout << "Will now set key \"" << KEYNAME << "\" to value \"" << KEY << "\" when configuring an AsgElectronEfficiencyCorrectionTool" << std::endl; \
    ATH_CHECK( TOOLHANDLE.setProperty(KEYNAME, KEY) );                  \
    if(!isData())                                                        \
      ATH_CHECK (TOOLHANDLE.setProperty("ForceDataType", (int) data_type) ); \
    ATH_CHECK( TOOLHANDLE.setProperty("CorrelationModel", m_EG_corrModel) ); \
    ATH_CHECK( TOOLHANDLE.initialize() );                                \
  }  

#define CONFIG_TAU_TRIGEFF_TOOL(TOOLHANDLE, INDEX, TRIGGER, TAUID )         \
  if (!TOOLHANDLE.isUserConfigured() && tau_trig_support.size() > INDEX && !isData()) { \
    toolName = "TauTrigEffTool_" + std::to_string(TAUID) + "_" + TRIGGER; \
    SET_DUAL_TOOL(TOOLHANDLE, TauAnalysisTools::TauEfficiencyCorrectionsTool, toolName); \
    ATH_CHECK(TOOLHANDLE.setProperty("EfficiencyCorrectionTypes", std::vector<int>({TauAnalysisTools::SFTriggerHadTau}) )); \
    ATH_CHECK(TOOLHANDLE.setProperty("TriggerName", TRIGGER));                \
    ATH_CHECK(TOOLHANDLE.setProperty("IDLevel", TAUID ));                \
    ATH_CHECK(TOOLHANDLE.setProperty("PileupReweightingTool", m_prwTool.getHandle() )); \
    ATH_CHECK(TOOLHANDLE.retrieve());                                        \
  } 

StatusCode SUSYObjDef_xAOD::SUSYToolsInit()
{
  if (m_dataSource < 0) {
    ATH_MSG_FATAL("Data source incorrectly configured!!");
    ATH_MSG_FATAL("You must set the DataSource property to Data, FullSim or AtlfastII !!");
    ATH_MSG_FATAL("Expect segfaults if you're not checking status codes, which you should be !!");
    return StatusCode::FAILURE;
  }

  if (m_subtool_init) {
    ATH_MSG_INFO("SUSYTools subtools already created. Ignoring this call.");
    ATH_MSG_INFO("Note: No longer necessary to explicitly call SUSYToolsInit. Will avoid creating tools again.");
    return StatusCode::SUCCESS;
  }

	//SET_DUAL_TOOL(m_InDetTrackSelectionTool, InDet::InDetTrackSelectionTool, "TrackSelection" );
	//ATH_CHECK( m_InDetTrackSelectionTool.setProperty("CutLevel", "LoosePrimary" ); // set tool to apply the pre-defined "Loose-Primary" cuts        
	//ATH_CHECK( m_InDetTrackSelectionTool.setProperty("maxZ0SinTheta", 50.) ); // additional cuts can be set on top of/overwriting the pre-defined ones
	//ATH_CHECK( m_InDetTrackSelectionTool.setProperty("maxD0overSigmaD0", 1.5) );
	//ATH_CHECK( m_InDetTrackSelectionTool.initialize() );

  // /////////////////////////////////////////////////////////////////////////////////////////
  // Initialise PileupReweighting Tool

  if (!m_prwTool.isUserConfigured()) {
    std::cout << "Will now init the PRW tool" << std::endl;
    std::vector<std::string> file_conf;
    for (UInt_t i = 0; i < m_prwConfFiles.size(); i++) {
      file_conf.push_back(PathResolverFindCalibFile(m_prwConfFiles.at(i)));
    }

    std::vector<std::string> file_ilumi;
    for (UInt_t i = 0; i < m_prwLcalcFiles.size(); i++) {
      ATH_MSG_INFO("Adding ilumicalc file: " << m_prwLcalcFiles.at(i));
      file_ilumi.push_back(PathResolverFindCalibFile(m_prwLcalcFiles.at(i)));
    }

    SET_DUAL_TOOL(m_prwTool, CP::PileupReweightingTool, "PrwTool");
    ATH_CHECK( m_prwTool.setProperty("ConfigFiles", file_conf) );
    ATH_CHECK( m_prwTool.setProperty("LumiCalcFiles", file_ilumi) );
    ATH_CHECK( m_prwTool.setProperty("DataScaleFactor",     1. / 1.09) );
    ATH_CHECK( m_prwTool.setProperty("DataScaleFactorUP",   1.) );
    ATH_CHECK( m_prwTool.setProperty("DataScaleFactorDOWN", 1. / 1.18) );
    ATH_CHECK( m_prwTool.setProperty("OutputLevel", MSG::WARNING) );
    ATH_CHECK( m_prwTool.retrieve() );
  } else {
    std::cout << "Using user-configured PRW tool" << std::endl;
  }
  
  std::string toolName; // to be used for tool init below, keeping explicit string constants a minimum /CO
 
  

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise jet calibration tool

  std::string jetname("AntiKt4" + xAOD::JetInput::typeName(xAOD::JetInput::Type(m_jetInputType)));
  std::string jetcoll(jetname + "Jets");

  if (!m_jetCalibTool.isUserConfigured()) {
    toolName = "JetCalibTool_" + jetname;
    SET_DUAL_TOOL( m_jetCalibTool, JetCalibrationTool, toolName ); 

    // pick the right config file for the JES tool
    std::string JES_config_file("JES_data2016_data2015_Recommendation_Dec2016.config"); //JES_MC15cRecommendation_May2016.config");
    
    if(!m_JMScalib.empty()){ //with JMS calibration (if requested)
      JES_config_file = "JES_data2016_data2015_Recommendation_Dec2016_JMS.config";
    }
    
    if (isAtlfast()) {

      if (m_jetInputType == xAOD::JetInput::EMTopo || m_jetInputType == xAOD::JetInput::LCTopo) { // only supported ones for AF-II
        JES_config_file = "JES_MC15Prerecommendation_AFII_June2015.config";
      }
      else {
        ATH_MSG_ERROR("JES recommendations only exist for EMTopo jets in AF-II samples (m_jetInputType = " << m_jetInputType << ")");
        return StatusCode::FAILURE;
      }

      if(!m_JMScalib.empty()){
        ATH_MSG_ERROR("JMS calibration is not supported for AF-II samples. Please modify your settings.");
        return StatusCode::FAILURE;
        // ATH_MSG_WARNING("*** JMS calibration is not validated for AF-II samples. Please modify your settings if this is not intended ! ***");
      }

    }

    // form the string describing the calibration sequence to use
    std::string calibseq("JetArea_Residual_Origin_EtaJES_GSC");
    if(!m_JMScalib.empty()){
      calibseq += "_JMS";
    }
    if (isData()) { 
      calibseq += "_Insitu";
    }
    
    // finally, PFlow jets need special care
    if (m_jetInputType == xAOD::JetInput::EMPFlow) {
      //Following : https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/ApplyJetCalibration2016#Calibration_of_PFlow_jets_in_20
      //Note! : There is no origin correction explicitly included in the PFlow JES
      //
      
      JES_config_file = "JES_MC15cRecommendation_PFlow_Aug2016.config";
      calibseq = "JetArea_Residual_EtaJES_GSC";

      if(!m_JMScalib.empty()){
        ATH_MSG_ERROR("JMS calibration is not supported for EMPFlow jets. Please modify your settings.");
        return StatusCode::FAILURE;
      }
    }
    
    //check isData parameter (no in-situ calibration for LCTopo yet) //MT : revise when it becomes available!
    //    bool data_par = (( m_jetInputType == xAOD::JetInput::LCTopo) ? false : isData());
    bool data_par = isData(); 
    
    // now instantiate the tool
    ATH_CHECK( m_jetCalibTool.setProperty("JetCollection", jetname) );
    ATH_CHECK( m_jetCalibTool.setProperty("ConfigFile", JES_config_file) );
    ATH_CHECK( m_jetCalibTool.setProperty("CalibSequence", calibseq) );
    ATH_CHECK( m_jetCalibTool.setProperty("IsData", data_par) );
    ATH_CHECK( m_jetCalibTool.retrieve() );
    
  }
  

  //same for fat groomed jets
  std::string fatjetcoll(m_fatJets); 
  if (fatjetcoll.size()>3) fatjetcoll = fatjetcoll.substr(0,fatjetcoll.size()-4); //remove (new) suffix
  if (!m_jetFatCalibTool.isUserConfigured() && ""!=m_fatJets) {
    toolName = "JetFatCalibTool_" + jetname;
    SET_DUAL_TOOL(m_jetFatCalibTool, JetCalibrationTool, "JetCalibrationTool/"+toolName);

    // pick the right config file for the JES tool
    //std::string JES_config_file("JES_MC15recommendation_FatJet_June2015.config");   //Supported as a standard jet definition, but less performant at high pT<\sub>
    std::string JES_config_file("JES_MC15recommendation_FatJet_Nov2016_QCDCombinationUncorrelatedWeights.config"); //Supported/recommended if you are performing an analysis intending to tag W/Z/H/top jets 

    // form the string describing the calibration sequence to use
    std::string calibseq("EtaJES_JMS");
    
    // now instantiate the tool
    ATH_CHECK( m_jetFatCalibTool.setProperty("JetCollection", fatjetcoll) );
    ATH_CHECK( m_jetFatCalibTool.setProperty("ConfigFile", JES_config_file) );
    ATH_CHECK( m_jetFatCalibTool.setProperty("CalibSequence", calibseq) );
    ATH_CHECK( m_jetFatCalibTool.setProperty("IsData", isData()) );
    ATH_CHECK( m_jetFatCalibTool.retrieve() );

  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise Boson taggers     
  m_WTaggerTool = new JetSubStructureUtils::BosonTag(m_WtagWP, "smooth", PathResolverFindCalibFile("JetSubStructureUtils/config_13TeV_Wtagging_MC15_Prerecommendations_20150809.dat"), false, false);
  m_ZTaggerTool = new JetSubStructureUtils::BosonTag(m_ZtagWP, "smooth", PathResolverFindCalibFile("JetSubStructureUtils/config_13TeV_Ztagging_MC15_Prerecommendations_20150809.dat"), false, false);

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise jet resolution tools

  if ( !m_jerTool.isUserConfigured() ) {
    ATH_MSG_INFO("Set up JER tool...");

    toolName = "JERTool_" + jetcoll;
    SET_DUAL_TOOL(m_jerTool, JERTool, toolName);

    ATH_CHECK( m_jerTool.setProperty("PlotFileName", "JetResolution/Prerec2015_xCalib_2012JER_ReducedTo9NP_Plots_v2.root") );
    //ATH_CHECK( asg::setProperty(m_jerTool, "CollectionName", jetcoll) ); //AntiKt4EMTopoJets (the default) is the only collection supported at the moment! //MT
    ATH_CHECK( m_jerTool.retrieve() );
  }

  if ( !m_jerSmearingTool.isUserConfigured() ) {
    ATH_MSG_INFO("Set up JERSmearing tool...");
    
    toolName = "JERSmearingTool_" + jetcoll;
    SET_DUAL_TOOL(m_jerSmearingTool, JERSmearingTool, toolName);

    ATH_CHECK( m_jerSmearingTool.setProperty("ApplyNominalSmearing", false) );
    ATH_CHECK( m_jerSmearingTool.setProperty("JERTool", m_jerTool.getHandle() ) );
    ATH_CHECK( m_jerSmearingTool.setProperty("isMC", !isData()) );
    ATH_CHECK( m_jerSmearingTool.setProperty("SystematicMode", "Simple") );
    ATH_CHECK( m_jerSmearingTool.retrieve() );

  }


  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise jet uncertainty tool
  ATH_MSG_INFO("Set up Jet Uncertainty tool...");

  if (!m_jetUncertaintiesTool.isUserConfigured()) {
    std::string jetdef("AntiKt4" + xAOD::JetInput::typeName(xAOD::JetInput::Type(m_jetInputType)));
    // Until uncertainties provided for other collections
    if(jetdef != "AntiKt4EMTopo"){
      ATH_MSG_WARNING("  *** HACK *** Treating " << jetdef << " jets as EMTopo -- use at your own risk!");
      jetdef = "AntiKt4EMTopo";
    }
    toolName = "JetUncertaintiesTool_" + jetdef;

    std::string configfile;
    if (m_JMScalib.empty()){
      configfile = "JES_2016/Moriond2017/JES2016_21NP.config";
      switch (m_jesNPset) {
        //  For testing reduced JES NPs
      case 1:  configfile = "JES_2016/Moriond2017/JES2016_SR_Scenario1.config"; break;
      case 2:  configfile = "JES_2016/Moriond2017/JES2016_SR_Scenario2.config"; break;
      case 3:  configfile = "JES_2016/Moriond2017/JES2016_SR_Scenario3.config"; break;
      case 4:  configfile = "JES_2016/Moriond2017/JES2016_SR_Scenario4.config"; break;
      default: break;
      }
    } else {
      // Make sure we got a sensible value for the configuration
      if (m_JMScalib!="Frozen" && m_JMScalib!="Extrap"){
        ATH_MSG_ERROR("JMS calibration uncertainty must be either Frozen or Extrap.  " << m_JMScalib << " is not a supported value. Please modify your settings.");
        return StatusCode::FAILURE;
      }
      // Calibration uncertainties with small-R jet mass calibration
      configfile = "JES_2016/Moriond2017/JES2016_21NP_JMS"+m_JMScalib+".config";
      switch (m_jesNPset) {
        //  For testing reduced JES NPs
      case 1:  configfile = "JES_2016/Moriond2017/JES2016_SR_Scenario1_JMS"+m_JMScalib+".config"; break;
      case 2:  configfile = "JES_2016/Moriond2017/JES2016_SR_Scenario2_JMS"+m_JMScalib+".config"; break;
      case 3:  configfile = "JES_2016/Moriond2017/JES2016_SR_Scenario3_JMS"+m_JMScalib+".config"; break;
      case 4:  configfile = "JES_2016/Moriond2017/JES2016_SR_Scenario4_JMS"+m_JMScalib+".config"; break;
      default: break;
      }
    }
    SET_DUAL_TOOL(m_jetUncertaintiesTool,JetUncertaintiesTool,"JetUncertaintiesTool/"+toolName);

    ATH_CHECK( m_jetUncertaintiesTool.setProperty("JetDefinition", jetdef) );
    ATH_CHECK( m_jetUncertaintiesTool.setProperty("MCType", isAtlfast() ? "AFII" : "MC15") );    
    ATH_CHECK( m_jetUncertaintiesTool.setProperty("ConfigFile", configfile) );
    ATH_CHECK( m_jetUncertaintiesTool.retrieve() );

    // CP::SystematicSet defaultSet;
    // CP::SystematicCode ret = m_jetUncertaintiesTool->applySystematicVariation(defaultSet);
    // if (ret != CP::SystematicCode::Ok) {
    //   ATH_MSG_ERROR("Cannot configure JetUncertaintiesTool for default systematic setting.");
    //   return StatusCode::FAILURE;
    // }
  }

  // Initialise jet uncertainty tool for fat jets
  if (!m_fatjetUncertaintiesTool.isUserConfigured() && ""!=m_fatJets && ""!=m_fatJetUncConfig) {
    toolName = "JetUncertaintiesTool_" + m_fatJets;
    SET_DUAL_TOOL(m_fatjetUncertaintiesTool, JetUncertaintiesTool, "JetUncertaintiesTool/"+toolName);

    ATH_CHECK( m_fatjetUncertaintiesTool.setProperty("JetDefinition", fatjetcoll) );
    ATH_CHECK( m_fatjetUncertaintiesTool.setProperty("MCType", "MC15c") );
    
    //Configs from here: https://svnweb.cern.ch/trac/atlasoff/browser/Reconstruction/Jet/JetUncertainties/trunk/share/UJ_2016/Moriond2017/
    std::string configfile("UJ_2016/Moriond2017/"+m_fatJetUncConfig);
    ATH_CHECK( m_fatjetUncertaintiesTool.setProperty("ConfigFile", configfile) );
    
    //Restrict variables to be shifted if (required)
    if( m_fatJetUncVars != "default" ){
      std::vector<std::string> shift_vars = {};

      std::string temp(m_fatJetUncVars);
      do {
        auto pos = temp.find(",");
        shift_vars.push_back(temp.substr(0, pos));
        if (pos == std::string::npos)
          temp = "";
        else
          temp = temp.substr(pos + 1);
        
      }
      while (!temp.empty() );

      ATH_CHECK( m_fatjetUncertaintiesTool.setProperty("VariablesToShift", shift_vars) );      
    }

    ATH_CHECK( m_fatjetUncertaintiesTool.retrieve() );
    // CP::SystematicSet defaultSet;
    // CP::SystematicCode ret = m_fatjetUncertaintiesTool->applySystematicVariation(defaultSet);
    // if (ret != CP::SystematicCode::Ok) {
    //   ATH_MSG_ERROR("Cannot configure (Fat)JetUncertaintiesTool for default systematic setting.");
    //   return StatusCode::FAILURE;
    // }
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise jet cleaning tools

  if (m_badJetCut!="" && !m_jetCleaningTool.isUserConfigured()) {
    toolName = "JetCleaningTool";
    SET_DUAL_TOOL(m_jetCleaningTool, JetCleaningTool, toolName);
    ATH_CHECK( m_jetCleaningTool.setProperty("CutLevel", m_badJetCut) );
    ATH_CHECK( m_jetCleaningTool.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise InDetTrackSelectionTool 

  if (m_InDetTrackSelection!="" && !m_InDetTrackSelectionTool.isUserConfigured()) {
    toolName = "TrackSelection";
    SET_DUAL_TOOL(m_InDetTrackSelectionTool, InDet::InDetTrackSelectionTool, toolName);
    ATH_CHECK( m_InDetTrackSelectionTool.setProperty("CutLevel", m_InDetTrackSelection) );
    ATH_CHECK( m_InDetTrackSelectionTool.retrieve() );
  }



  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise JVT tool

  if (!m_jetJvtUpdateTool.isUserConfigured()) {
    toolName = "JetVertexTaggerTool";
    SET_DUAL_TOOL(m_jetJvtUpdateTool, JetVertexTaggerTool, "JetVertexTaggerTool/"+toolName);
    ATH_CHECK( m_jetJvtUpdateTool.setProperty("JVTFileName", "JetMomentTools/JVTlikelihood_20140805.root") );
    ATH_CHECK( m_jetJvtUpdateTool.retrieve() );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise jet JVT efficiency tool

  m_applyJVTCut = m_JVT_WP!="";
  if (!m_jetJvtEfficiencyTool.isUserConfigured() && m_applyJVTCut) {
    toolName = "JVTEfficiencyTool";
    SET_DUAL_TOOL(m_jetJvtEfficiencyTool, CP::JetJvtEfficiency, "CP::JetJvtEfficiency/"+toolName);
    ATH_CHECK( m_jetJvtEfficiencyTool.setProperty("WorkingPoint",m_JVT_WP) );
    // Set the decoration to the name we used to use
    ATH_CHECK( m_jetJvtEfficiencyTool.setProperty("ScaleFactorDecorationName","jvtscalefact") );
    // Set the jvt moment (the one we update!)
    //    ATH_CHECK( m_jetJvtEfficiencyTool.setProperty("JetJvtMomentName", "Jvt") ); //default!

    if (m_jetInputType == xAOD::JetInput::EMTopo){
      ATH_CHECK( m_jetJvtEfficiencyTool.setProperty("SFFile","JetJvtEfficiency/Moriond2017/JvtSFFile_EM.root") );
    } else if (m_jetInputType == xAOD::JetInput::LCTopo){
      ATH_CHECK( m_jetJvtEfficiencyTool.setProperty("SFFile","JetJvtEfficiency/Moriond2017/JvtSFFile_LC.root") );
    } else if (m_jetInputType == xAOD::JetInput::EMPFlow){
      ATH_CHECK( m_jetJvtEfficiencyTool.setProperty("SFFile","JetJvtEfficiency/Moriond2017/JvtSFFile_EMPFlow.root") );
    } else {
      ATH_MSG_ERROR("Cannot configure JVT uncertainties for unsupported jet input type (neither EM nor LC)"); 
      return StatusCode::FAILURE;
    }

    ATH_CHECK( m_jetJvtEfficiencyTool.retrieve() );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise FwdJVT tool

  if (!m_jetFwdJvtTool.isUserConfigured()) {
    SET_DUAL_TOOL(m_jetFwdJvtTool, JetForwardJvtTool, "JetForwardJvtTool");
    ATH_CHECK( m_jetFwdJvtTool.setProperty("OutputDec", "passFJvt") ); //Output decoration
    ATH_CHECK( m_jetFwdJvtTool.setProperty("UseTightOP", m_fwdjetTightOp) ); //Tight or Loose
    ATH_CHECK( m_jetFwdJvtTool.setProperty("EtaThresh", m_fwdjetEtaMin) );   //Eta dividing central from forward jets
    ATH_CHECK( m_jetFwdJvtTool.setProperty("ForwardMaxPt", m_fwdjetPtMax) ); //Max Pt to define fwdJets for JVT
    ATH_CHECK( m_jetFwdJvtTool.retrieve() );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise muon calibration tool

  if (!m_muonCalibrationAndSmearingTool.isUserConfigured()) {
    toolName = "MuonCalibrationAndSmearingTool";
    SET_DUAL_TOOL(m_muonCalibrationAndSmearingTool, CP::MuonCalibrationAndSmearingTool, toolName);
    ATH_CHECK( m_muonCalibrationAndSmearingTool.setProperty("StatComb", true) );
    ATH_CHECK( m_muonCalibrationAndSmearingTool.retrieve() );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise muon selection tool

  std::string muQualBaseline = "";
  switch (m_muIdBaseline) {
  case (int)xAOD::Muon::Quality(xAOD::Muon::VeryLoose):  muQualBaseline = "VeryLoose";
    ATH_MSG_WARNING("No muon scale factors are available for VeryLoose working point.");
    break;
  case (int)xAOD::Muon::Quality(xAOD::Muon::Loose):  muQualBaseline = "Loose";  break;
  case (int)xAOD::Muon::Quality(xAOD::Muon::Medium): muQualBaseline = "Medium"; break;
  case (int)xAOD::Muon::Quality(xAOD::Muon::Tight):  muQualBaseline = "Tight";  break;
  case 4:  muQualBaseline = "HighPt";  break;
  default:
    ATH_MSG_ERROR("Invalid muon working point provided: " << m_muIdBaseline << ". Cannot initialise!");
    return StatusCode::FAILURE;
    break;
  }

  if (!m_muonSelectionToolBaseline.isUserConfigured()) {
    toolName = "MuonSelectionTool_Baseline_" + muQualBaseline;
    SET_DUAL_TOOL(m_muonSelectionToolBaseline, CP::MuonSelectionTool, toolName);
    if (m_muBaselineEta<m_muEta){  // Test for inconsistent configuration
      ATH_MSG_ERROR( "Requested a baseline eta cut for muons (" << m_muBaselineEta << 
                     ") that is tighter than the signal cut (" << m_muEta << ").  Please check your config." );
      return StatusCode::FAILURE;
    }
    ATH_CHECK( m_muonSelectionToolBaseline.setProperty( "MaxEta", m_muBaselineEta) );
    //      ATH_CHECK( m_muonSelectionToolBaseline.setProperty( "MuQuality", int(m_muIdBaseline) ) );
    ATH_CHECK( m_muonSelectionToolBaseline.setProperty( "MuQuality", m_muIdBaseline ) );
    ATH_CHECK( m_muonSelectionToolBaseline.setProperty( "TrtCutOff", m_muNoTRT ));
    ATH_CHECK( m_muonSelectionToolBaseline.retrieve() );
  }

  std::string muQual = "";
  switch (m_muId) {
  case (int)xAOD::Muon::Quality(xAOD::Muon::VeryLoose):  muQual = "VeryLoose";
    ATH_MSG_WARNING("No muon scale factors are available for VeryLoose working point.");
    break;
  case (int)xAOD::Muon::Quality(xAOD::Muon::Loose):  muQual = "Loose";  break;
  case (int)xAOD::Muon::Quality(xAOD::Muon::Medium): muQual = "Medium"; break;
  case (int)xAOD::Muon::Quality(xAOD::Muon::Tight):  muQual = "Tight";  break;
  case 4:  muQual = "HighPt";  break;
  default:
    ATH_MSG_ERROR("Invalid muon working point provided: " << m_muId << ". Cannot initialise!");
    return StatusCode::FAILURE;
    break;
  }

  if (!m_muonSelectionTool.isUserConfigured()) {
    toolName = "MuonSelectionTool_" + muQual;
    SET_DUAL_TOOL(m_muonSelectionTool, CP::MuonSelectionTool, toolName);
    ATH_CHECK( m_muonSelectionTool.setProperty( "MaxEta", m_muEta) );
    //      ATH_CHECK( m_muonSelectionTool.setProperty( "MuQuality", int(m_muId) ) );
    ATH_CHECK( m_muonSelectionTool.setProperty( "MuQuality", m_muId ) );
    ATH_CHECK( m_muonSelectionTool.setProperty( "TrtCutOff", m_muNoTRT ));
    ATH_CHECK( m_muonSelectionTool.retrieve() );
  }

  if (!m_muonSelectionHighPtTool.isUserConfigured()) { //Fixed to HighPt WP
    toolName = "MuonSelectionHighPtTool_" + muQual; 
    SET_DUAL_TOOL(m_muonSelectionHighPtTool, CP::MuonSelectionTool, toolName);
    ATH_CHECK( m_muonSelectionHighPtTool.setProperty( "MaxEta", m_muEta) );
    ATH_CHECK( m_muonSelectionHighPtTool.setProperty( "MuQuality", 4 ) );
    ATH_CHECK( m_muonSelectionHighPtTool.setProperty( "TrtCutOff", m_muNoTRT ));
    ATH_CHECK( m_muonSelectionHighPtTool.retrieve() );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise muon efficiency tools

  if (!m_muonEfficiencySFTool.isUserConfigured() && m_muId != xAOD::Muon::VeryLoose) {
    toolName = "MuonEfficiencyScaleFactors_" + muQual;
    SET_DUAL_TOOL(m_muonEfficiencySFTool, CP::MuonEfficiencyScaleFactors, toolName);
    ATH_CHECK( m_muonEfficiencySFTool.setProperty("WorkingPoint", muQual) );
    if(m_muNoTRT) //use config with disabled TRT quality cut
      ATH_CHECK( m_muonEfficiencySFTool.setProperty("CalibrationRelease", "170410_Moriond_noTRT") );
    
    ATH_CHECK( m_muonEfficiencySFTool.retrieve() );
  }

  if (!m_muonEfficiencyBMHighPtSFTool.isUserConfigured()){
    toolName = "MuonEfficiencyScaleFactorsBMHighPt_" + muQual;
    SET_DUAL_TOOL(m_muonEfficiencyBMHighPtSFTool, CP::MuonEfficiencyScaleFactors, toolName);
    ATH_CHECK( m_muonEfficiencyBMHighPtSFTool.setProperty("WorkingPoint", "BadMuonVeto_HighPt") );
    if(m_muNoTRT) //use config with disabled TRT quality cut
      ATH_CHECK( m_muonEfficiencyBMHighPtSFTool.setProperty("CalibrationRelease", "170410_Moriond_noTRT") );
    
    ATH_CHECK( m_muonEfficiencyBMHighPtSFTool.retrieve() );
  }

  if (m_doTTVAsf && m_mud0sig<0 && m_muz0<0){
    ATH_MSG_WARNING("Requested TTVA SFs without d0sig and z0 cuts. Disabling scale factors as they will not make sense.");
    m_doTTVAsf=false;
  }

  if (!m_muonTTVAEfficiencySFTool.isUserConfigured()) {
    toolName = "MuonTTVAEfficiencyScaleFactors";
    SET_DUAL_TOOL(m_muonTTVAEfficiencySFTool, CP::MuonEfficiencyScaleFactors, toolName);
    ATH_CHECK( m_muonTTVAEfficiencySFTool.setProperty("WorkingPoint", "TTVA") );
    // Recommendation of the muon experts: use noTRT calibration, as the noTRT and standard
    //  calibrations are the same, but the noTRT calibration includes low-pT SFs
    ATH_CHECK( m_muonTTVAEfficiencySFTool.setProperty("CalibrationRelease", "170410_Moriond_noTRT") );
    ATH_CHECK( m_muonTTVAEfficiencySFTool.retrieve() );
  }
  
  
  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise muon isolation tool
  
  if (!m_muonIsolationSFTool.isUserConfigured()) { // so far only one supported WP
    toolName = "MuonIsolationScaleFactors_" + m_muIso_WP;
    SET_DUAL_TOOL(m_muonIsolationSFTool, CP::MuonEfficiencyScaleFactors, toolName);
    ATH_CHECK( m_muonIsolationSFTool.setProperty("WorkingPoint", m_muIso_WP + "Iso") );
    ATH_CHECK( m_muonIsolationSFTool.retrieve() );
  }
  
  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise muon trigger scale factor tools

  if (!m_muonTriggerSFTool2015.isUserConfigured()) {
    toolName = "MuonTriggerScaleFactors2015_" + muQual;
    SET_DUAL_TOOL(m_muonTriggerSFTool2015, CP::MuonTriggerScaleFactors, toolName);
    ATH_CHECK( m_muonTriggerSFTool2015.setProperty("MuonQuality", muQual));
    ATH_CHECK( m_muonTriggerSFTool2015.setProperty("Isolation", m_muIso_WP));
    ATH_CHECK( m_muonTriggerSFTool2015.setProperty("Year", "2015"));
    ATH_CHECK( m_muonTriggerSFTool2015.setProperty("AllowZeroSF", true));
    ATH_CHECK( m_muonTriggerSFTool2015.retrieve());
  }

  if (!m_muonTriggerSFTool2016.isUserConfigured()) {
    toolName = "MuonTriggerScaleFactors2016_" + muQual;
    SET_DUAL_TOOL(m_muonTriggerSFTool2016, CP::MuonTriggerScaleFactors, toolName);
    ATH_CHECK( m_muonTriggerSFTool2016.setProperty("MuonQuality", muQual));
    ATH_CHECK( m_muonTriggerSFTool2016.setProperty("Isolation", m_muIso_WP));
    //      ATH_CHECK( m_muonTriggerSFTool2016.setProperty("Year", "2016")); // not needed, 2016 is the default
    ATH_CHECK( m_muonTriggerSFTool2016.setProperty("AllowZeroSF", true));
    ATH_CHECK( m_muonTriggerSFTool2016.retrieve());
  }
  // /////////////////////////////////////////////////////////////////////////////////////////
  // Initialise electron selector tools
  //std::set<std::string> eg_id_support = {"TightLLH", "MediumLLH", "LooseAndBLayerLLH", "LooseLLH", "VeryLooseLLH"};

  // Signal Electrons
  if (!m_elecSelLikelihood.isUserConfigured()) {
    toolName = "EleSelLikelihood_" + m_eleId;
    SET_DUAL_TOOL(m_elecSelLikelihood, AsgElectronLikelihoodTool, toolName);
    
    if ( !check_isOption(m_eleId, el_id_support) ) { //check if supported
      ATH_MSG_ERROR("Invalid electron ID selected: " << m_eleId);
      return StatusCode::FAILURE;
    }
    else if (m_eleId == "VeryLooseLLH" || m_eleId == "LooseLLH") {
      ATH_MSG_WARNING(" ****************************************************************************");
      ATH_MSG_WARNING(" CAUTION: Setting " << m_eleId << " as signal electron ID");
      ATH_MSG_WARNING(" These may be used for loose electron CRs but no scale factors are provided.");
      ATH_MSG_WARNING(" ****************************************************************************");
      //        return StatusCode::FAILURE;
    }
    ATH_CHECK( m_elecSelLikelihood.setProperty("WorkingPoint", EG_WP(m_eleId) ));
    ATH_CHECK( m_elecSelLikelihood.retrieve() );
  }

  // Baseline Electrons
  if (!m_elecSelLikelihoodBaseline.isUserConfigured()) {
    toolName = "EleSelLikelihoodBaseline_" + m_eleIdBaseline;
    SET_DUAL_TOOL(m_elecSelLikelihoodBaseline, AsgElectronLikelihoodTool, toolName);

    if ( !check_isOption(m_eleIdBaseline, el_id_support) ) { //check if supported
      ATH_MSG_ERROR("Invalid electron ID selected: " << m_eleIdBaseline);
      return StatusCode::FAILURE;
    }
    ATH_CHECK( m_elecSelLikelihoodBaseline.setProperty("WorkingPoint", EG_WP(m_eleIdBaseline)) );
    ATH_CHECK( m_elecSelLikelihoodBaseline.retrieve() );
  }


  // /////////////////////////////////////////////////////////////////////////////////////////
  // Initialise photon selector tools

  if (!m_photonSelIsEM.isUserConfigured()) {
    toolName = "PhotonSelIsEM_" + m_photonId;
    SET_DUAL_TOOL(m_photonSelIsEM, AsgPhotonIsEMSelector, toolName);

    if (!check_isOption(m_photonId, ph_id_support)){ //check if supported
      ATH_MSG_ERROR("Invalid photon ID selected: " << m_photonId);
      return StatusCode::FAILURE;
    }

    ATH_CHECK( m_photonSelIsEM.setProperty("WorkingPoint", m_photonId+"Photon") );
    ATH_CHECK( m_photonSelIsEM.retrieve() );
  }

  if (!m_photonSelIsEMBaseline.isUserConfigured()) {
    toolName = "PhotonSelIsEMBaseline_" + m_photonIdBaseline;
    SET_DUAL_TOOL(m_photonSelIsEMBaseline, AsgPhotonIsEMSelector, toolName);

    if(!check_isOption(m_photonIdBaseline, ph_id_support)){ //check if supported
      ATH_MSG_ERROR("Invalid photon ID selected: " << m_photonIdBaseline);
      return StatusCode::FAILURE;
    }

    ATH_CHECK( m_photonSelIsEMBaseline.setProperty("WorkingPoint", m_photonIdBaseline+"Photon") );    
    ATH_CHECK( m_photonSelIsEMBaseline.retrieve() );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Initialise electron efficiency tool
  //map name hardcoded for now. Might need some maintenance/thinking //MT
  std::string egMapFile = "ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v3/map1.txt";

  PATCore::ParticleDataType::DataType data_type(PATCore::ParticleDataType::Data);
  if (!isData()) {
    if (isAtlfast()) data_type = PATCore::ParticleDataType::Fast;
    else data_type = PATCore::ParticleDataType::Full;
    ATH_MSG_DEBUG( "Setting data type to " << data_type);
  }

  toolName = "AsgElectronEfficiencyCorrectionTool_reco";
  CONFIG_EG_EFF_TOOL_KEY(m_elecEfficiencySFTool_reco, toolName, "RecoKey", "Reconstruction");

  if (m_eleId == "VeryLooseLLH" || m_eleId == "LooseLLH" || m_eleId == "Loose" || m_eleId == "Medium" || m_eleId == "Tight") {
    ATH_MSG_WARNING("Not configuring electron ID and trigger scale factors for " << m_eleId);
  } 
  else {
    
    std::string eleId = TString(m_eleId).ReplaceAll("AndBLayer", "BLayer").ReplaceAll("LLH", "").Data();

    // electron id
    toolName = "AsgElectronEfficiencyCorrectionTool_id_" + m_eleId;
    CONFIG_EG_EFF_TOOL_KEY(m_elecEfficiencySFTool_id, toolName, "IdKey", eleId);

    // electron iso
    toolName = "AsgElectronEfficiencyCorrectionTool_iso_" + m_eleId + m_eleIso_WP;
    // can't do the iso tool via the macro, it needs two properties set
    if ( !m_elecEfficiencySFTool_iso.isUserConfigured() ) {
      SET_DUAL_TOOL(m_elecEfficiencySFTool_iso, AsgElectronEfficiencyCorrectionTool, toolName);

      // New isolation recommendations are in the release - no need to update the map file by hand
      //ATH_CHECK( m_elecEfficiencySFTool_iso.setProperty ("MapFilePath", egMapFile) );

      ATH_CHECK( m_elecEfficiencySFTool_iso.setProperty("IdKey", eleId) );
      ATH_CHECK( m_elecEfficiencySFTool_iso.setProperty("IsoKey", m_eleIso_WP) );
      if (!isData()) {
        ATH_CHECK (m_elecEfficiencySFTool_iso.setProperty("ForceDataType", (int) data_type) ); 
      }
      ATH_CHECK( m_elecEfficiencySFTool_iso.setProperty("CorrelationModel", m_EG_corrModel) ); 
      ATH_CHECK( m_elecEfficiencySFTool_iso.initialize() );
    }

    // electron ChargeID (NEW) (doesn't support keys yet . and only Medium ChIDWP )
    toolName = "AsgElectronEfficiencyCorrectionTool_chf_" + m_eleId + m_eleIso_WP + m_eleChID_WP;
    CONFIG_EG_EFF_TOOL(m_elecEfficiencySFTool_chf, toolName, "ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v1/charge_misID/efficiencySF.ChargeID.MediumLLH_d0z0_v11_isolGradient_MediumCFT.root");

    if(m_eleChID_WP.empty() || m_eleChID_WP=="None" || m_eleChID_WP=="none"){
      m_runECIS = false;
    }
    else{
      if(m_eleChID_WP != "Medium")
        ATH_MSG_WARNING("** Only MediumCFT supported for SF at the moment. We rolled back to that for now, but be aware of it! ");
      m_runECIS = true;
    }

    //-- get KEYS supported by egamma SF tools
    //map name hardcoded above for now
    std::vector<std::string> eSF_keys = getElSFkeys(egMapFile);
 
    // electron triggers - first SFs (but we need to massage the id string since all combinations are not supported)

    //single lepton
    std::string triggerEleIso("");
    if (std::find(eSF_keys.begin(), eSF_keys.end(), m_electronTriggerSFStringSingle+"_"+eleId+"_"+m_eleIso_WP) != eSF_keys.end()){
      triggerEleIso   = m_eleIso_WP;    
    }
    else{
      ATH_MSG_ERROR("***  THE ELECTRON TRIGGER SF YOU SELECTED (" << m_electronTriggerSFStringSingle << ") GOT NO SUPPORT FOR YOUR ID+ISO WPs (" << m_eleId << "+" << m_eleIso_WP << ") ***");
      return StatusCode::FAILURE;
    }
    

    //dilepton
    std::string triggerDiEleIso("");
    std::cout <<  m_electronTriggerSFStringDiLepton+"_"+eleId+"_"+m_eleIso_WP << std::endl;
    if (std::find(eSF_keys.begin(), eSF_keys.end(), m_electronTriggerSFStringDiLepton+"_"+eleId+"_"+m_eleIso_WP) != eSF_keys.end()){
      triggerDiEleIso  = TString(m_eleIso_WP).Copy().ReplaceAll("GradientLoose","Gradient").ReplaceAll("TrackOnly","").ReplaceAll("FixedCutTight","Gradient").Data();  // because they don't support things they claim to in the map    
    }
    else{
      triggerDiEleIso = TString(m_eleIso_WP).Copy().ReplaceAll("GradientLoose","Gradient").ReplaceAll("TrackOnly","").ReplaceAll("FixedCutTight","Gradient").Data();
      if (std::find(eSF_keys.begin(), eSF_keys.end(), m_electronTriggerSFStringDiLepton+"_"+eleId+"_"+triggerDiEleIso) != eSF_keys.end()){
        ATH_MSG_WARNING("  ***  THERE IS A PROBLEM WITH THE  ELECTRON TRIGGER SF YOU SELECTED:  " << m_electronTriggerSFStringDiLepton << " ***");
        ATH_MSG_WARNING("  ***     THE ELECTRON TRIGGER SF ARE NOT SUPPORTED FOR THE ISOLATION WP YOU PICKED (" << m_eleIso_WP << ") . FALLING TO 'Gradient' HERE (ONLY) -- Use at your own risk! ***");
      }
      else{
        ATH_MSG_ERROR("***  THE ELECTRON TRIGGER SF YOU SELECTED (" << m_electronTriggerSFStringDiLepton << ") GOT NO SUPPORT FOR YOUR ID+ISO WPs (" << m_eleId << "+" << m_eleIso_WP << "). The fallback options failed as well sorry! ***");
        return StatusCode::FAILURE;
      }
    }

    //mixed-leptons
    std::string triggerMixedEleIso("");
    if (std::find(eSF_keys.begin(), eSF_keys.end(), m_electronTriggerSFStringMixedLepton+"_"+eleId+"_"+m_eleIso_WP) != eSF_keys.end()){
      triggerMixedEleIso  = TString(m_eleIso_WP).Copy().ReplaceAll("GradientLoose","Gradient").ReplaceAll("TrackOnly","").ReplaceAll("FixedCutTight","Gradient").Data();  // because they don't support things they claim to in the map    
    }
    else{
      triggerMixedEleIso = TString(m_eleIso_WP).Copy().ReplaceAll("GradientLoose","Gradient").ReplaceAll("TrackOnly","").ReplaceAll("FixedCutTight","Gradient").Data();
      if (std::find(eSF_keys.begin(), eSF_keys.end(), m_electronTriggerSFStringMixedLepton+"_"+eleId+"_"+triggerMixedEleIso) != eSF_keys.end()){
        ATH_MSG_WARNING("  ***  THERE IS A PROBLEM WITH THE  ELECTRON TRIGGER SF YOU SELECTED:  " << m_electronTriggerSFStringMixedLepton << " ***");
        ATH_MSG_WARNING("  ***     THE ELECTRON TRIGGER SF ARE NOT SUPPORTED FOR THE ISOLATION WP YOU PICKED (" << m_eleIso_WP << ") . FALLING TO 'GRADIENT' HERE (ONLY) -- Use at your own risk! ***");
      }
      else{
        ATH_MSG_ERROR("***  THE ELECTRON TRIGGER SF YOU SELECTED (" << m_electronTriggerSFStringMixedLepton << ") GOT NO SUPPORT FOR YOUR ID+ISO WPs (" << m_eleId << "+" << m_eleIso_WP << "). The fallback options failed as well sorry! ***");
        return StatusCode::FAILURE;
      }
    }

    //configure the tools now!
    toolName = "AsgElectronEfficiencyCorrectionTool_trig_singleLep_" + m_eleId;
    if ( !m_elecEfficiencySFTool_trig_singleLep.isUserConfigured() ) {
      SET_DUAL_TOOL(m_elecEfficiencySFTool_trig_singleLep, AsgElectronEfficiencyCorrectionTool, toolName);
      //ATH_CHECK( m_elecEfficiencySFTool_trig_singleLep.setProperty("MapFilePath", egMapFile) );
      ATH_CHECK( m_elecEfficiencySFTool_trig_singleLep.setProperty("TriggerKey", m_electronTriggerSFStringSingle) );
      ATH_CHECK( m_elecEfficiencySFTool_trig_singleLep.setProperty("IdKey", eleId) );
      ATH_CHECK( m_elecEfficiencySFTool_trig_singleLep.setProperty("IsoKey", triggerEleIso) );   
      ATH_CHECK( m_elecEfficiencySFTool_trig_singleLep.setProperty("CorrelationModel", m_EG_corrModel) ); 
      if (!isData()) {
        ATH_CHECK( m_elecEfficiencySFTool_trig_singleLep.setProperty("ForceDataType", (int) data_type) );
      }
      ATH_CHECK( m_elecEfficiencySFTool_trig_singleLep.initialize() );   
    }
    toolName = "AsgElectronEfficiencyCorrectionTool_trig_diLep_" + m_eleId;
    if ( !m_elecEfficiencySFTool_trig_diLep.isUserConfigured() ) {
      SET_DUAL_TOOL(m_elecEfficiencySFTool_trig_diLep, AsgElectronEfficiencyCorrectionTool, toolName);
      //ATH_CHECK( m_elecEfficiencySFTool_trig_diLep.setProperty("MapFilePath", egMapFile) );
      ATH_CHECK( m_elecEfficiencySFTool_trig_diLep.setProperty("TriggerKey", m_electronTriggerSFStringDiLepton) );
      ATH_CHECK( m_elecEfficiencySFTool_trig_diLep.setProperty("IdKey", eleId) );
      ATH_CHECK( m_elecEfficiencySFTool_trig_diLep.setProperty("IsoKey", triggerDiEleIso) );
      ATH_CHECK( m_elecEfficiencySFTool_trig_diLep.setProperty("CorrelationModel", m_EG_corrModel) ); 
      if (!isData()) {
        ATH_CHECK( m_elecEfficiencySFTool_trig_diLep.setProperty("ForceDataType", (int) data_type) );
      }
      ATH_CHECK( m_elecEfficiencySFTool_trig_diLep.initialize() );
    }    
    toolName = "AsgElectronEfficiencyCorrectionTool_trig_mixLep_" + m_eleId;
    if ( !m_elecEfficiencySFTool_trig_mixLep.isUserConfigured() ) {
      SET_DUAL_TOOL(m_elecEfficiencySFTool_trig_mixLep, AsgElectronEfficiencyCorrectionTool, toolName);
      //ATH_CHECK( m_elecEfficiencySFTool_trig_mixLep.setProperty("MapFilePath", egMapFile) );
      ATH_CHECK( m_elecEfficiencySFTool_trig_mixLep.setProperty("TriggerKey", m_electronTriggerSFStringMixedLepton) );
      ATH_CHECK( m_elecEfficiencySFTool_trig_mixLep.setProperty("IdKey", eleId) );
      ATH_CHECK( m_elecEfficiencySFTool_trig_mixLep.setProperty("IsoKey", triggerMixedEleIso) );   
      ATH_CHECK( m_elecEfficiencySFTool_trig_mixLep.setProperty("CorrelationModel", m_EG_corrModel) ); 
      if (!isData()) {
        ATH_CHECK( m_elecEfficiencySFTool_trig_mixLep.setProperty("ForceDataType", (int) data_type) );
      }
      ATH_CHECK( m_elecEfficiencySFTool_trig_mixLep.initialize() );
    }    
    // then efficiencies
    toolName = "AsgElectronEfficiencyCorrectionTool_trigEff_singleLep_" + m_eleId;
    if ( !m_elecEfficiencySFTool_trigEff_singleLep.isUserConfigured() ) {
      SET_DUAL_TOOL(m_elecEfficiencySFTool_trigEff_singleLep, AsgElectronEfficiencyCorrectionTool, toolName);
      //ATH_CHECK( m_elecEfficiencySFTool_trigEff_singleLep.setProperty("MapFilePath", egMapFile) );
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_singleLep.setProperty("TriggerKey", "Eff_"+m_electronTriggerSFStringSingle) );
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_singleLep.setProperty("IdKey", eleId) );
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_singleLep.setProperty("IsoKey", triggerEleIso) );   
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_singleLep.setProperty("CorrelationModel", m_EG_corrModel) ); 
      if (!isData()) {
        ATH_CHECK( m_elecEfficiencySFTool_trigEff_singleLep.setProperty("ForceDataType", (int) data_type) );
      }
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_singleLep.initialize() );
    }
    std::cout <<  m_electronTriggerSFStringDiLepton+"_"+eleId+"_"+triggerDiEleIso << std::endl;

    toolName = "AsgElectronEfficiencyCorrectionTool_trigEff_diLep_" + m_eleId;
    if ( !m_elecEfficiencySFTool_trigEff_diLep.isUserConfigured() ) {
      SET_DUAL_TOOL(m_elecEfficiencySFTool_trigEff_diLep, AsgElectronEfficiencyCorrectionTool, toolName);
      //ATH_CHECK( m_elecEfficiencySFTool_trigEff_diLep.setProperty("MapFilePath", egMapFile) );
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_diLep.setProperty("TriggerKey", "Eff_"+m_electronTriggerSFStringDiLepton) );
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_diLep.setProperty("IdKey", eleId) );
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_diLep.setProperty("IsoKey", triggerDiEleIso) );   
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_diLep.setProperty("CorrelationModel", m_EG_corrModel) ); 
      if (!isData()) {
        ATH_CHECK( m_elecEfficiencySFTool_trigEff_diLep.setProperty("ForceDataType", (int) data_type) );
      }
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_diLep.initialize() );
    }    
    toolName = "AsgElectronEfficiencyCorrectionTool_trig_mixLep_" + m_eleId;
    if ( !m_elecEfficiencySFTool_trigEff_mixLep.isUserConfigured() ) {
      SET_DUAL_TOOL(m_elecEfficiencySFTool_trigEff_mixLep, AsgElectronEfficiencyCorrectionTool, toolName);
      //ATH_CHECK( m_elecEfficiencySFTool_trigEff_mixLep.setProperty("MapFilePath", egMapFile) );
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_mixLep.setProperty("TriggerKey", "Eff_"+m_electronTriggerSFStringMixedLepton) );
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_mixLep.setProperty("IdKey", eleId) );
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_mixLep.setProperty("IsoKey", triggerMixedEleIso) );   
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_mixLep.setProperty("CorrelationModel", m_EG_corrModel) ); 
      if (!isData()) {
        ATH_CHECK( m_elecEfficiencySFTool_trigEff_mixLep.setProperty("ForceDataType", (int) data_type) );
      }
      ATH_CHECK( m_elecEfficiencySFTool_trigEff_mixLep.initialize() );
    }
  }



  // electron charge-flip  :: NEW
  toolName = "ElectronChargeEfficiencyCorrectionTool_" + m_eleId + m_eleIso_WP + m_eleChID_WP;
  // can't do the chf tool via the macro, it needs custom input files for now (+ only one config supported)
  if ( !m_elecChargeEffCorrTool.isUserConfigured() ) {
    SET_DUAL_TOOL(m_elecChargeEffCorrTool,  CP::ElectronChargeEfficiencyCorrectionTool, toolName);
    
    std::string tmpIDWP = m_eleId.substr(0, m_eleId.size()-3);
    if (tmpIDWP != "Medium" && tmpIDWP != "Tight") {
      ATH_MSG_WARNING( "** Only MediumLLH & TightLLH ID WP are supported for ChargeID SFs at the moment. Falling back to MediumLLH, be aware! **");
      tmpIDWP = "Medium";
    }
    
    std::string tmpIsoWP = TString(m_eleIso_WP).Copy().ReplaceAll("GradientLoose","Gradient").ReplaceAll("TrackOnly","").Data();
    if (tmpIsoWP != "Gradient" && tmpIsoWP != "FixedCutTight") {
      ATH_MSG_WARNING( "** Only Gradient & FixedCutTight Iso WP are supported for ChargeID SFs at the moment. Falling back to Gradient, be aware! **");
      tmpIsoWP = "Gradient";
    }
    if (tmpIsoWP == "Gradient") tmpIsoWP="isolGradient";  // <3 egamma

    std::string tmpChID("");
    if(!m_eleChID_WP.empty()){
      ATH_MSG_WARNING( "** Only one ID+ISO+CFT configuration supported for ChargeID SFs at the moment (Medium_FixedCutTightIso_CFTMedium) . Falling back to that, be aware! **");
      tmpIDWP  = "Medium";
      tmpIsoWP = "FixedCutTightIso";
      tmpChID  = "_CFTMedium";
    }
    
    std::string inputFile("ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v1/charge_misID/ChargeCorrectionSF."+tmpIDWP+"_"+tmpIsoWP+tmpChID+".root");//only supported for now!
    
    ATH_CHECK( m_elecChargeEffCorrTool.setProperty("CorrectionFileName",inputFile) );
    
    if (!isData()) {
      ATH_CHECK (m_elecChargeEffCorrTool.setProperty("ForceDataType", (int) data_type) ); 
    }
    ATH_CHECK( m_elecChargeEffCorrTool.initialize() );
  }
  
  // /////////////////////////////////////////////////////////////////////////////////////////
  // Initialise photon efficiency tool

  if (!m_photonEfficiencySFTool.isUserConfigured() && !isData()) {
    SET_DUAL_TOOL(m_photonEfficiencySFTool, AsgPhotonEfficiencyCorrectionTool, "AsgPhotonEfficiencyCorrectionTool_" + m_photonId);
      
    if (m_photonId != "Tight" ) { ATH_MSG_WARNING( "No Photon efficiency available for " << m_photonId << ", using Tight instead..." );  }

    ATH_CHECK( m_photonEfficiencySFTool.setProperty("MapFilePath", "PhotonEfficiencyCorrection/map0.txt") );
    if(!isData())
      ATH_CHECK( m_photonEfficiencySFTool.setProperty("ForceDataType", 1) ); //force to fullsim until we get AFII recommendations
    //      ATH_CHECK( m_photonEfficiencySFTool.setProperty("ForceDataType", (int) data_type) );
    ATH_CHECK( m_photonEfficiencySFTool.retrieve() );
  }

  if (!m_photonIsolationSFTool.isUserConfigured() && !isData()) {
    SET_DUAL_TOOL(m_photonIsolationSFTool, AsgPhotonEfficiencyCorrectionTool, "AsgPhotonEfficiencyCorrectionTool_isol" + m_photonIso_WP);
    
    if (m_photonIso_WP != "FixedCutTight" && m_photonIso_WP != "FixedCutLoose" && m_photonIso_WP != "FixedCutTightCaloOnly") {
      ATH_MSG_WARNING( "No Photon efficiency available for " << m_photonIso_WP);
    }
    
    ATH_CHECK( m_photonIsolationSFTool.setProperty("MapFilePath", "PhotonEfficiencyCorrection/map0.txt"));
    ATH_CHECK( m_photonIsolationSFTool.setProperty("IsoWP", m_photonIso_WP.substr(8) ));    // Set isolation WP: Loose,Tight,TightCaloOnly (remove the 'FixedCut', because egamma is egamma...)
    
    if(!isData())
      ATH_CHECK( m_photonIsolationSFTool.setProperty("ForceDataType", 1) );
    ATH_CHECK( m_photonIsolationSFTool.retrieve() );
    
  }
  
 ///////////////////////////////////////////////////////////////////////////////////////////
 // Initialize the MC fudge tool

  if (!m_electronPhotonShowerShapeFudgeTool.isUserConfigured()) {
    SET_DUAL_TOOL(m_electronPhotonShowerShapeFudgeTool, ElectronPhotonShowerShapeFudgeTool, "ElectronPhotonShowerShapeFudgeTool");
  
    int FFset = 21; // for MC15 samples, which are based on a geometry derived from GEO-21 from 2015+2016 data
    ATH_CHECK( m_electronPhotonShowerShapeFudgeTool.setProperty("Preselection", FFset));
    //      ATH_CHECK( m_electronPhotonShowerShapeFudgeTool.setProperty("ConfigFile", "ElectronPhotonShowerShapeFudgeTool/May2016_ShiftsForMCBasedLHTuneUsing20pt7.conf"));
    ATH_CHECK( m_electronPhotonShowerShapeFudgeTool.retrieve());
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialize the EgammaAmbiguityTool

  if (!m_egammaAmbiguityTool.isUserConfigured()) {
    SET_DUAL_TOOL(m_egammaAmbiguityTool, EGammaAmbiguityTool, "EGammaAmbiguityTool");
    ATH_CHECK( m_egammaAmbiguityTool.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialize the AsgElectronChargeIDSelector

  if (!m_elecChargeIDSelectorTool.isUserConfigured()) {

    std::string eleId = TString(m_eleId).ReplaceAll("AndBLayer", "BLayer").ReplaceAll("LLH", "").Data();
    SET_DUAL_TOOL(m_elecChargeIDSelectorTool, AsgElectronChargeIDSelectorTool, "ElectronChargeIDSelectorTool_"+eleId);
 
    //default cut value for https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ElectronChargeFlipTaggerTool
    float BDTcut = -0.28087; //Medium 97%
    if( m_eleChID_WP == "Loose")  BDTcut = -0.288961; //97%
    else if( m_eleChID_WP == "Tight") BDTcut = -0.325856; //97%
    else if( m_eleChID_WP != "Medium" && !m_eleChID_WP.empty()){
      ATH_MSG_ERROR("Invalid ChargeIDSelector WP selected : " << m_eleChID_WP);
      return StatusCode::FAILURE;
    }

    ATH_CHECK( m_elecChargeIDSelectorTool.setProperty("TrainingFile", "ElectronPhotonSelectorTools/ChargeID/ECIDS_20161125for2017Moriond.root"));
    ATH_CHECK( m_elecChargeIDSelectorTool.setProperty("CutOnBDT", BDTcut));
    ATH_CHECK( m_elecChargeIDSelectorTool.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise egamma calibration tool

  if (!m_egammaCalibTool.isUserConfigured()) {
    SET_DUAL_TOOL(m_egammaCalibTool, CP::EgammaCalibrationAndSmearingTool, "EgammaCalibrationAndSmearingTool");
    ATH_MSG_DEBUG( "Initialising EgcalibTool " );
    ATH_CHECK( m_egammaCalibTool.setProperty("ESModel", "es2016data_mc15c_final") );
    //want to use _summer as soon as possible; see ATLASG-1255; used for analysis using 2015 or 2016 or 2015+2016 data (all processed with 20.7).
    ATH_CHECK( m_egammaCalibTool.setProperty("decorrelationModel", "1NP_v1") );
    if (isAtlfast()) ATH_CHECK( m_egammaCalibTool.setProperty("useAFII", 1) );
    
    ATH_CHECK( m_egammaCalibTool.retrieve() );
  }


///////////////////////////////////////////////////////////////////////////////////////////
// Initialize tau score re-decorator
  if (!m_tauWPdecorator.isUserConfigured()) {
    toolName = "TauWPDecorator";
    SET_DUAL_TOOL(m_tauWPdecorator, TauWPDecorator, toolName);
    
    if (!m_tauWPdecorator.empty()) {
      ATH_CHECK( m_tauWPdecorator.setProperty("flatteningFile1Prong", "flat1SmoothAODfix.root") );
      ATH_CHECK( m_tauWPdecorator.setProperty("flatteningFile3Prong", "flat3SmoothAODfix.root") );
      ATH_CHECK( m_tauWPdecorator.setProperty("DefineWPs", true) );
      ATH_CHECK( m_tauWPdecorator.setProperty("SigEffWPVeryLoose1P", 0.95) );
      ATH_CHECK( m_tauWPdecorator.setProperty("SigEffWPLoose1P", 0.85) );
      ATH_CHECK( m_tauWPdecorator.setProperty("SigEffWPMedium1P", 0.75) );
      ATH_CHECK( m_tauWPdecorator.setProperty("SigEffWPTight1P", 0.60) );
      ATH_CHECK( m_tauWPdecorator.setProperty("SigEffWPVeryLoose3P", 0.95) );
      ATH_CHECK( m_tauWPdecorator.setProperty("SigEffWPLoose3P", 0.75) );
      ATH_CHECK( m_tauWPdecorator.setProperty("SigEffWPMedium3P", 0.60) );
      ATH_CHECK( m_tauWPdecorator.setProperty("SigEffWPTight3P", 0.45) );
    }
    ATH_CHECK( m_tauWPdecorator.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise tau selection tools

  if (!m_tauSelTool.isUserConfigured()) {
    std::string inputfile = "";
    if (!m_tauConfigPath.empty() && (m_tauConfigPath!="default")) inputfile = m_tauConfigPath;
    else if (m_tauId == "Loose") inputfile = "SUSYTools/tau_selection_loose.conf";
    else if (m_tauId == "Medium") inputfile = "SUSYTools/tau_selection_medium.conf";
    else if (m_tauId == "Tight") inputfile = "SUSYTools/tau_selection_tight.conf";
    else {
      ATH_MSG_ERROR("Invalid tau ID selected: " << m_tauId);
      return StatusCode::FAILURE;
    }
    toolName = "TauSelectionTool_" + m_tauId;
    SET_DUAL_TOOL(m_tauSelTool, TauAnalysisTools::TauSelectionTool, toolName);
    ATH_CHECK( m_tauSelTool.setProperty("ConfigPath", inputfile) );

    if(m_tauRecalcOLR){
      ATH_CHECK( m_tauSelTool.setProperty("RecalcEleOLR", true) );     
    }
    if(m_tauNoAODFixCheck){
      ATH_CHECK( m_tauSelTool.setProperty("IgnoreAODFixCheck", true) );
    }

    ATH_CHECK( m_tauSelTool.retrieve() );
  }

  if (!m_tauSelToolBaseline.isUserConfigured()) {
    std::string inputfile = "";
    if (!m_tauConfigPathBaseline.empty() && (m_tauConfigPathBaseline!="default")) inputfile = m_tauConfigPathBaseline;
    else if (m_tauIdBaseline == "Loose") inputfile = "SUSYTools/tau_selection_loose.conf";
    else if (m_tauIdBaseline == "Medium") inputfile = "SUSYTools/tau_selection_medium.conf";
    else if (m_tauIdBaseline == "Tight") inputfile = "SUSYTools/tau_selection_tight.conf";
    else {
      ATH_MSG_ERROR("Invalid baseline tau ID selected: " << m_tauIdBaseline);
      return StatusCode::FAILURE;
    }
    toolName = "TauSelectionToolBaseline_" + m_tauIdBaseline;
    SET_DUAL_TOOL(m_tauSelToolBaseline, TauAnalysisTools::TauSelectionTool, toolName);
    ATH_CHECK( m_tauSelToolBaseline.setProperty("ConfigPath", inputfile) );

    if(m_tauRecalcOLR){
      ATH_CHECK( m_tauSelToolBaseline.setProperty("RecalcEleOLR", true) );
    }
    if(m_tauNoAODFixCheck){
      ATH_CHECK( m_tauSelToolBaseline.setProperty("IgnoreAODFixCheck", true) );
    }

    ATH_CHECK( m_tauSelToolBaseline.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise tau efficiency tool

  if (!m_tauEffTool.isUserConfigured()) {
    toolName = "TauEffTool_" + m_tauId;
    SET_DUAL_TOOL(m_tauEffTool, TauAnalysisTools::TauEfficiencyCorrectionsTool, toolName);

    if (!m_tauSelTool.empty()) {
      ATH_CHECK( m_tauEffTool.setProperty("TauSelectionTool", m_tauSelTool.getHandle()) );
    }
    ATH_CHECK( m_tauEffTool.retrieve() );
  }

  // TODO: add SF tool for baseline tau id as well? /CO

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise tau trigger efficiency tool(s)

  int iTauID = (int) TauAnalysisTools::JETIDNONEUNCONFIGURED;
  if (m_tauId == "Loose")       iTauID = (int) TauAnalysisTools::JETIDBDTLOOSE;
  else if (m_tauId == "Medium") iTauID = (int) TauAnalysisTools::JETIDBDTMEDIUM;
  else if (m_tauId == "Tight")  iTauID = (int) TauAnalysisTools::JETIDBDTTIGHT;
  else {
    ATH_MSG_ERROR("Invalid tau ID selected: " << m_tauId); // we shouldn't get here but ok...
    return StatusCode::FAILURE;
  }
  
  CONFIG_TAU_TRIGEFF_TOOL( m_tauTrigEffTool0, 0, tau_trig_support[0], iTauID);
  CONFIG_TAU_TRIGEFF_TOOL( m_tauTrigEffTool1, 1, tau_trig_support[1], iTauID);
  CONFIG_TAU_TRIGEFF_TOOL( m_tauTrigEffTool2, 2, tau_trig_support[2], iTauID);
  CONFIG_TAU_TRIGEFF_TOOL( m_tauTrigEffTool3, 3, tau_trig_support[3], iTauID);
  CONFIG_TAU_TRIGEFF_TOOL( m_tauTrigEffTool4, 4, tau_trig_support[4], iTauID);
  
  ///////////////////////////////////////////////////////////////////////////////////////////
// Initialise tau smearing tool
  
  if (!m_tauSmearingTool.isUserConfigured()) {
    toolName = "TauSmearingTool";
    SET_DUAL_TOOL(m_tauSmearingTool, TauAnalysisTools::TauSmearingTool, toolName);
    if (m_tauMVACalib) { // Apply the MVA calibration?
      ATH_CHECK( m_tauSmearingTool.setProperty("ApplyMVATES",true) );
    }
    ATH_CHECK( m_tauSmearingTool.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise tau truth matching tool
  
  if (!m_tauTruthMatch.isUserConfigured() && m_tauDoTTM) {
    toolName = "TauTruthMatch";
    SET_DUAL_TOOL(m_tauTruthMatch, TauAnalysisTools::TauTruthMatchingTool, toolName);
    ATH_CHECK( m_tauTruthMatch.setProperty("WriteTruthTaus", true) );
    ATH_CHECK( m_tauTruthMatch.retrieve() );
  }
  
///////////////////////////////////////////////////////////////////////////////////////////
// Initialise TauOverlappingElectronLLHDecorator tool

  if (!m_tauElORdecorator.isUserConfigured()) {
    toolName = "TauEleORDecorator";
    SET_DUAL_TOOL(m_tauElORdecorator, TauAnalysisTools::TauOverlappingElectronLLHDecorator, toolName);
    ATH_CHECK( m_tauElORdecorator.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise B-tagging tools
  std::string jetcollBTag = jetcoll;
  if (jetcoll == "AntiKt4EMPFlowJets") {
    ATH_MSG_WARNING("  *** HACK *** Treating EMPFlow jets as EMTopo -- use at your own risk!");
    jetcollBTag = "AntiKt4EMTopoJets";
  }
  if (jetcoll == "AntiKt4LCTopoJets") {
    ATH_MSG_WARNING("  *** HACK *** Treating LCTopoJets jets as EMTopo -- use at your own risk!");
    jetcollBTag = "AntiKt4EMTopoJets";
  }
  //if (m_btagSelTool.empty() && !m_BtagWP.empty()) {
  if (!m_btagSelTool.isUserConfigured() && !m_BtagWP.empty()) {
    if (jetcoll != "AntiKt4EMTopoJets" && jetcoll != "AntiKt4EMPFlowJets") {
      ATH_MSG_WARNING("** Only AntiKt4EMTopoJets and AntiKt3PV0TrackJets are supported with scale factors!");
      if( jetcoll == "AntiKt3PV0TrackJets" ){
        ATH_MSG_ERROR("AntiKt3PV0TrackJets not yet implemented in SUSYTools. Please inform the Background Forum Conveners if you wish to use these.");
        return StatusCode::FAILURE;
      }
    }
    toolName = "BTagSel_" + jetcollBTag + m_BtagWP;
    SET_DUAL_TOOL(m_btagSelTool, BTaggingSelectionTool, toolName);    
    ATH_CHECK( m_btagSelTool.setProperty("TaggerName",     m_BtagTagger ) );
    ATH_CHECK( m_btagSelTool.setProperty("OperatingPoint", m_BtagWP  ) );
    ATH_CHECK( m_btagSelTool.setProperty("JetAuthor",      jetcollBTag   ) );
    ATH_CHECK( m_btagSelTool.setProperty("FlvTagCutDefinitionsFileName",  m_bTaggingCalibrationFilePath) );
    ATH_CHECK( m_btagSelTool.retrieve() );
  }

  //if (m_btagSelTool_OR.empty() && !m_orBtagWP.empty()) {
  if (!m_btagSelTool_OR.isUserConfigured() && !m_orBtagWP.empty()) {
    if (jetcoll != "AntiKt4EMTopoJets" && jetcoll != "AntiKt4EMPFlowJets") {
      ATH_MSG_WARNING("** Only AntiKt4EMTopoJets and AntiKt3PV0TrackJets are supported with scale factors!");
      if( jetcoll == "AntiKt3PV0TrackJets" ){
        ATH_MSG_ERROR("AntiKt3PV0TrackJets not yet implemented in SUSYTools. Please inform the Background Forum Conveners if you wish to use these.");
        return StatusCode::FAILURE;
      }
    }

    toolName = "BTagSelOR_" + jetcollBTag + m_orBtagWP;
    SET_DUAL_TOOL(m_btagSelTool_OR, BTaggingSelectionTool, toolName);
    ATH_CHECK( m_btagSelTool_OR.setProperty("TaggerName",     m_BtagTagger  ) );
    ATH_CHECK( m_btagSelTool_OR.setProperty("OperatingPoint", m_orBtagWP  ) );
    ATH_CHECK( m_btagSelTool_OR.setProperty("JetAuthor",      jetcollBTag   ) );
    ATH_CHECK( m_btagSelTool_OR.setProperty("FlvTagCutDefinitionsFileName",  m_bTaggingCalibrationFilePath) );
    ATH_CHECK( m_btagSelTool_OR.retrieve() );
  }

  if (!m_btagEffTool.isUserConfigured() && !m_BtagWP.empty()) {
    if (jetcoll != "AntiKt4EMTopoJets") {
      ATH_MSG_WARNING("** Only AntiKt4EMTopoJets and AntiKt3PV0TrackJets are supported with scale factors!");
      if( jetcoll == "AntiKt3PV0TrackJets" ){
        ATH_MSG_ERROR("AntiKt3PV0TrackJets not yet implemented in SUSYTools. Please inform the Background Forum Conveners if you wish to use these.");
        return StatusCode::FAILURE;
      }
    }

    std::string MCshowerID = "410000";                 //PowhegPy6
    if (m_showerType == 1) MCshowerID = "410004";      //HerwigPP
    else if (m_showerType == 2) MCshowerID = "410500"; //PowhegPy8
    else if (m_showerType == 3) MCshowerID = "410021"; //Sherpa 2.1.1
    else if (m_showerType == 4) MCshowerID = "410187"; //Sherpa 2.2.0

    toolName = "BTagSF_" + jetcollBTag;
    SET_DUAL_TOOL(m_btagEffTool, BTaggingEfficiencyTool, toolName);        
    ATH_CHECK( m_btagEffTool.setProperty("TaggerName",     m_BtagTagger ) );
    ATH_CHECK( m_btagEffTool.setProperty("ScaleFactorFileName",  m_bTaggingCalibrationFilePath) );
    ATH_CHECK( m_btagEffTool.setProperty("OperatingPoint", m_BtagWP ) );
    ATH_CHECK( m_btagEffTool.setProperty("JetAuthor",      jetcollBTag ) );
    ATH_CHECK( m_btagEffTool.setProperty("SystematicsStrategy", m_BtagSystStrategy ) );
    ATH_CHECK( m_btagEffTool.setProperty("EfficiencyBCalibrations",     MCshowerID   ));
    ATH_CHECK( m_btagEffTool.setProperty("EfficiencyCCalibrations",     MCshowerID   ));
    ATH_CHECK( m_btagEffTool.setProperty("EfficiencyTCalibrations",     MCshowerID   ));
    ATH_CHECK( m_btagEffTool.setProperty("EfficiencyLightCalibrations", MCshowerID   ));
    ATH_CHECK( m_btagEffTool.retrieve() );
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise MET tools

  if (!m_metMaker.isUserConfigured()) {
    SET_DUAL_TOOL(m_metMaker, met::METMaker, "METMaker_SUSYTools");
    
    ATH_CHECK( m_metMaker.setProperty("ORCaloTaggedMuons", m_metRemoveOverlappingCaloTaggedMuons) );
    ATH_CHECK( m_metMaker.setProperty("DoSetMuonJetEMScale", m_metDoSetMuonJetEMScale) );
    //ATH_CHECK( m_metMaker.setProperty("DoMuonJetOR", m_metDoMuonJetOR) );
    // set the jet selection if default empty string is overridden through config file
    if (m_metJetSelection.size()) 
      ATH_CHECK( m_metMaker.setProperty("JetSelection", m_metJetSelection) );

    if (m_jetInputType == xAOD::JetInput::EMPFlow) {
      ATH_CHECK( m_metMaker.setProperty("DoPFlow", true) );
      ATH_CHECK( m_metMaker.setProperty("JetSelection", "Expert") );
      ATH_CHECK( m_metMaker.setProperty("CustomJetJvtCut", -1.) );
    }

    ATH_CHECK( m_metMaker.setProperty("GreedyPhotons", m_metGreedyPhotons) );

    ATH_CHECK( m_metMaker.retrieve() );
  }

  if (!m_metSystTool.isUserConfigured()) {
    SET_DUAL_TOOL(m_metSystTool, met::METSystematicsTool, "METSystTool");
    
    if (!m_trkMETsyst) {
      ATH_CHECK( m_metSystTool.setProperty("ConfigSoftTrkFile", "") );
    }
    if (!m_caloMETsyst) {
      ATH_CHECK( m_metSystTool.setProperty("ConfigSoftCaloFile", "") );
    }

    if (m_trkMETsyst && m_caloMETsyst){
      ATH_MSG_ERROR( "Can only have CST *or* TST configured for MET maker.  Please unset either METDoCaloSyst or METDoTrkSyst in your config file" );
      return StatusCode::FAILURE;
    }
    if (m_caloMETsyst) {
      // Recommendations from a thread with TJ.  CST is not officially supported, but might be used for cross-checks
      ATH_CHECK( m_metSystTool.setProperty("DoIsolMuonEloss",true) );
      ATH_CHECK( m_metSystTool.setProperty("DoMuonEloss",true) );
      if ("AntiKt4EMTopoJets"==jetcoll) {
        // Recommendation from TJ: if we are using EM topo jets, make sure the clusters are considered at LC scale
        ATH_CHECK( m_metSystTool.setProperty("JetConstitScaleMom","JetLCScaleMomentum") );
      }
    }
    ATH_CHECK( m_metSystTool.retrieve());
  }

///////////////////////////////////////////////////////////////////////////////////////////
// Initialise trigger tools

  if (!m_trigDecTool.isUserConfigured()) {
    toolName = "xAODConfigTool";
    SET_DUAL_TOOL(m_trigConfTool, TrigConf::xAODConfigTool, toolName);
    ATH_CHECK(m_trigConfTool.retrieve() );

    // The decision tool
    toolName = "TrigDecisionTool";
    SET_DUAL_TOOL(m_trigDecTool, Trig::TrigDecisionTool, toolName);
    ATH_CHECK( m_trigDecTool.setProperty("ConfigTool", m_trigConfTool.getHandle()) );
    ATH_CHECK( m_trigDecTool.setProperty("TrigDecisionKey", "xTrigDecision") );
    ATH_CHECK( m_trigDecTool.retrieve() );
  }

  if (!m_trigMatchingTool.isUserConfigured()) {
    toolName = "TrigMatchTool";
    SET_DUAL_TOOL(m_trigMatchingTool, Trig::MatchingTool, toolName);
    ATH_CHECK(m_trigMatchingTool.setProperty("TrigDecisionTool", m_trigDecTool.getHandle()));
    ATH_CHECK(m_trigMatchingTool.setProperty("OutputLevel", MSG::WARNING));
    ATH_CHECK(m_trigMatchingTool.retrieve() );
  }
  
// /////////////////////////////////////////////////////////////////////////////////////////
// Initialise Isolation Correction Tool
  
  if ( !m_isoCorrTool.isUserConfigured() ) {
    SET_DUAL_TOOL(m_isoCorrTool, CP::IsolationCorrectionTool, "IsoCorrTool");
    ATH_CHECK( m_isoCorrTool.setProperty( "IsMC", !isData()) );
    ATH_CHECK( m_isoCorrTool.setProperty( "AFII_corr", isAtlfast()) );
    ATH_CHECK( m_isoCorrTool.retrieve() );
  }
  
// /////////////////////////////////////////////////////////////////////////////////////////
// Initialise Isolation Tool
  if (!m_isoTool.isUserConfigured()) {
    SET_DUAL_TOOL(m_isoTool, CP::IsolationSelectionTool, "IsoTool" );
    ATH_CHECK( m_isoTool.setProperty("ElectronWP", m_eleIso_WP) );
    ATH_CHECK( m_isoTool.setProperty("MuonWP",     m_muIso_WP ) );
    ATH_CHECK( m_isoTool.setProperty("PhotonWP",   m_photonIso_WP ) );
    ATH_CHECK( m_isoTool.retrieve() );
  }


// /////////////////////////////////////////////////////////////////////////////////////////
// Initialise IsolationCloseByCorrectionTool Tool
  if (!m_isoCloseByTool.isUserConfigured()) {
    SET_DUAL_TOOL(m_isoCloseByTool, CP::IsolationCloseByCorrectionTool, "IsoCloseByTool" );
    ATH_CHECK( m_isoCloseByTool.setProperty("IsolationSelectionTool", m_isoTool) );
    ATH_CHECK( m_isoCloseByTool.retrieve() );
  }


// /////////////////////////////////////////////////////////////////////////////////////////
// Initialise Overlap Removal Tool
  if ( m_orToolbox.masterTool.empty() ){

    // set up the master tool
    std::string suffix = "";
    if (m_orDoTau) suffix += "Tau";
    if (m_orDoPhoton) suffix += "Gamma";
    if (m_orDoBjet) suffix += "Bjet";
    std::string toolName = "ORTool" + suffix;
    ATH_MSG_INFO("SUSYTools: Autoconfiguring " << toolName);

    std::string bJetLabel = "";
    //overwrite lepton flags if the global is false (yes?)
    if (!m_orDoBjet || !m_useBtagging) {
      m_orDoElBjet = false;
      m_orDoMuBjet = false;
      m_orDoTauBjet = false;
    }
    if (m_orDoElBjet || m_orDoMuBjet || m_orDoTauBjet) {
      bJetLabel = "bjet_loose";
    }

    // Set the generic flags
    ORUtils::ORFlags orFlags(toolName, m_orInputLabel, "passOR");
    orFlags.bJetLabel      = bJetLabel;
    orFlags.boostedLeptons = (m_orDoBoostedElectron || m_orDoBoostedMuon);
    orFlags.outputPassValue = true;
    orFlags.doEleEleOR = false;
    orFlags.doElectrons = true;
    orFlags.doMuons = true;
    orFlags.doJets = true;
    orFlags.doTaus = m_orDoTau;
    orFlags.doPhotons = m_orDoPhoton;
    orFlags.doFatJets = m_orDoFatjets;

    //set up all recommended tools
    ATH_CHECK( ORUtils::recommendedTools(orFlags, m_orToolbox));

    // We don't currently have a good way to determine here which object
    // definitions are disabled, so we currently just configure all overlap
    // tools and disable the pointer safety checks
    ATH_CHECK( m_orToolbox.masterTool.setProperty("RequireExpectedPointers", false) );

    // Override boosted OR sliding cone options
    ATH_CHECK( m_orToolbox.eleJetORT.setProperty("UseSlidingDR", m_orDoBoostedElectron) );
    ATH_CHECK( m_orToolbox.muJetORT.setProperty("UseSlidingDR", m_orDoBoostedMuon) );

    //add custom tau-jet OR tool 
    if(m_orDoTau){
      m_orToolbox.tauJetORT.setTypeAndName("ORUtils::TauJetOverlapTool/" + orFlags.masterName + ".TauJetORT");
      ATH_CHECK( m_orToolbox.tauJetORT.setProperty("BJetLabel", m_orDoTauBjet?bJetLabel:"") );
    }

    // override sliding cone params if sliding dR is on and the user-provided parameter values are non-negative
    if (m_orDoBoostedElectron) {
      if (m_orBoostedElectronC1 > 0) ATH_CHECK( m_orToolbox.eleJetORT.setProperty("SlidingDRC1", m_orBoostedElectronC1) );
      if (m_orBoostedElectronC2 > 0) ATH_CHECK( m_orToolbox.eleJetORT.setProperty("SlidingDRC2", m_orBoostedElectronC2) );
      if (m_orBoostedElectronMaxConeSize > 0) ATH_CHECK( m_orToolbox.eleJetORT.setProperty("SlidingDRMaxCone", m_orBoostedElectronMaxConeSize) );
    }
    if (m_orDoBoostedMuon) {
      if (m_orBoostedMuonC1 > 0) ATH_CHECK( m_orToolbox.muJetORT.setProperty("SlidingDRC1", m_orBoostedMuonC1) );
      if (m_orBoostedMuonC2 > 0) ATH_CHECK( m_orToolbox.muJetORT.setProperty("SlidingDRC2", m_orBoostedMuonC2) );
      if (m_orBoostedMuonMaxConeSize > 0) ATH_CHECK( m_orToolbox.muJetORT.setProperty("SlidingDRMaxCone", m_orBoostedMuonMaxConeSize) );
    }

    // and switch off lep-bjet check if not requested
    if (!m_orDoElBjet) {
      ATH_CHECK(m_orToolbox.eleJetORT.setProperty("BJetLabel", ""));
    }
    if (!m_orDoMuBjet) {
      ATH_CHECK(m_orToolbox.muJetORT.setProperty("BJetLabel", ""));
    }

    // propagate the mu-jet ghost-association option which might be set by the user (default is true)
    ATH_CHECK(m_orToolbox.muJetORT.setProperty("UseGhostAssociation", m_orDoMuonJetGhostAssociation));

    // propagate mu-jet OR settings if requested
    ATH_CHECK(m_orToolbox.muJetORT.setProperty("ApplyRelPt", m_orApplyRelPt) );
    if(m_orApplyRelPt){
      if (m_orMuJetPtRatio > 0)    ATH_CHECK(m_orToolbox.muJetORT.setProperty("MuJetPtRatio", m_orMuJetPtRatio) );
      if (m_orMuJetTrkPtRatio > 0) ATH_CHECK(m_orToolbox.muJetORT.setProperty("MuJetTrkPtRatio", m_orMuJetTrkPtRatio) );
    }
    if (m_orMuJetInnerDR > 0)    ATH_CHECK(m_orToolbox.muJetORT.setProperty("InnerDR", m_orMuJetInnerDR) );

    // propagate the calo muon setting for EleMuORT
    ATH_CHECK(m_orToolbox.eleMuORT.setProperty("RemoveCaloMuons", m_orRemoveCaloMuons) );

    // propagate the fatjets OR settings
    if(m_orDoFatjets){
      if(m_EleFatJetDR>0) ATH_CHECK(m_orToolbox.eleFatJetORT.setProperty("DR", m_EleFatJetDR));
      if(m_JetFatJetDR>0) ATH_CHECK(m_orToolbox.jetFatJetORT.setProperty("DR", m_JetFatJetDR));
    }

    // Make sure that we deal with prorities correctly
    ATH_CHECK(m_orToolbox.eleJetORT.setProperty("EnableUserPriority", true));
    ATH_CHECK(m_orToolbox.muJetORT.setProperty("EnableUserPriority", true));
    if (m_orDoTau) ATH_CHECK(m_orToolbox.tauJetORT.setProperty("EnableUserPriority", true));
    if (m_orDoPhoton) ATH_CHECK(m_orToolbox.phoJetORT.setProperty("EnableUserPriority", true));

    ATH_CHECK(m_orToolbox.initialize());

  } // Done with the OR toolbox setup!

// /////////////////////////////////////////////////////////////////////////////////////////
// Initialise PMG Tools
  if (!m_pmgSHnjetWeighter.isUserConfigured()) {
    SET_DUAL_TOOL(m_pmgSHnjetWeighter, PMGTools::PMGSherpa22VJetsWeightTool, "PMGSHVjetReweighter");
    ATH_CHECK( m_pmgSHnjetWeighter.setProperty( "TruthJetContainer", "AntiKt4TruthJets"));
    ATH_CHECK( m_pmgSHnjetWeighter.retrieve());
  }

  if (!m_pmgSHnjetWeighterWZ.isUserConfigured()) {
    SET_DUAL_TOOL(m_pmgSHnjetWeighterWZ, PMGTools::PMGSherpa22VJetsWeightTool, "PMGSHVjetReweighterWZ");
    ATH_CHECK( m_pmgSHnjetWeighterWZ.setProperty( "TruthJetContainer", "AntiKt4TruthWZJets"));
    ATH_CHECK( m_pmgSHnjetWeighterWZ.retrieve() );
  }

  // prevent these initialiation snippets from being run again
  m_subtool_init = true;

  ATH_MSG_INFO("Done initialising SUSYTools");

  return StatusCode::SUCCESS;
}