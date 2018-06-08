#include <XAMPPanalyses/HeavyIonAnalysisHelper.h>
#include <XAMPPanalyses/Stop0LAnalysisUtils.h>
//#include <XAMPPanalyses/Stop0LISRSelector.h>
#include "xAODHIEvent/HIEventShapeContainer.h"
#include <HIEventUtils/IHICentralityTool.h>
#include <HIEventUtils/HICentralityTool.h>
//#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "MuonMomentumCorrections/IMuonCalibrationAndSmearingTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

// Local include(s):
#include "MuonSelectorTools/errorcheck.h"
#include "MuonSelectorTools/MuonSelectionTool.h"
#include <MuonEfficiencyCorrections/MuonTriggerScaleFactors.h>
#include <MuonEfficiencyCorrections/IMuonEfficiencyScaleFactors.h>
#include <SUSYTools/SUSYObjDef_xAOD.h>
#include <XAMPPbase/SUSYMuonSelector.h>


#include "xAODCore/ShallowAuxContainer.h"
#include "xAODCore/ShallowCopy.h"


using std::string;
using std::unique_ptr;
using CxxUtils::make_unique;
namespace XAMPP {
    HeavyIonAnalysisHelper::HeavyIonAnalysisHelper(std::string myname) :
            SUSYAnalysisHelper(myname){
    }
    StatusCode HeavyIonAnalysisHelper::initializeObjectTools() {
        ATH_CHECK(SUSYAnalysisHelper::initializeObjectTools());
        return StatusCode::SUCCESS;
    }
    StatusCode HeavyIonAnalysisHelper::initialize() {
        ATH_MSG_INFO("Initialising Heavy Ion AnalysisHelper...");
        ATH_CHECK(SUSYAnalysisHelper::initialize());
        //m_inDetTrackSelectionTool = GetCPTool < InDetTrackSelectionTool > ("InDetTrackSelectionTool");
        return StatusCode::SUCCESS;
    }

    StatusCode HeavyIonAnalysisHelper::initializeEventVariables() {

        //Lets define some variables which we want to store in the output tree / use in the cutflow
        //ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_BJets"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_SignalLeptons"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_BaselineLeptons")); //A variable on which we are cutting does not need to be stored in the tree
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_SignalMuons"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_BaselineMuons")); 
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_Jets"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<char>("IsOS"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<char>("IsSF"));
        //ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Mmuon"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Mll"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("InvMass"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("IDprobeCharge"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("IDprobePt"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("IDprobeEta"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("HT"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("HtSig"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Meff"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("ZWindow"));
        if (!isData()) ATH_CHECK(m_XAMPPInfo->NewEventVariable<double>("TotalWeight"));
        if (!isData()) ATH_CHECK(m_XAMPPInfo->NewEventVariable<double>("SherpaVjetsNjetsWeight"));
        if (!isData()) ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("treatAsYear"));
	ATH_CHECK(m_XAMPPInfo->NewEventVariable<char>("HasTauCand"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtTauCand"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("NtracksTauCand"));
        
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MetTST_mpx"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MetTST_mpy"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MetCST_mpx"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MetCST_mpy"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MetTrack_mpx"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MetTrack_mpy"));
        
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("ETsumFCal"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Centrality"));
        
      /*ATH_CHECK(m_XAMPPInfo->NewEventVariable<unsigned char>("numberOfPixelHits"));
      ATH_CHECK(m_XAMPPInfo->NewEventVariable<unsigned char>("numberOfPixelDeadSensors"));
      ATH_CHECK(m_XAMPPInfo->NewEventVariable<unsigned char>("numberOfSCTHits"));
      ATH_CHECK(m_XAMPPInfo->NewEventVariable<unsigned char>("numberOfSCTDeadSensors"));
      ATH_CHECK(m_XAMPPInfo->NewEventVariable<unsigned char>("numberOfTRTHits"));
      ATH_CHECK(m_XAMPPInfo->NewEventVariable<unsigned char>("numberOfTRTOutliers"));
      ATH_CHECK(m_XAMPPInfo->NewEventVariable<unsigned char>("numberOfPixelHoles"));
      ATH_CHECK(m_XAMPPInfo->NewEventVariable<unsigned char>("numberOfSCTHoles"));*/
        
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_RecoTracks"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_RecoTracks_highPt"));
        
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack1Pt"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack2Pt"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack3Pt"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack4Pt"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack5Pt"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack6Pt"));
        
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack1Eta"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack2Eta"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack3Eta"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack4Eta"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack5Eta"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack6Eta"));
        
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack1Phi"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack2Phi"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack3Phi"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack4Phi"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack5Phi"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack6Phi"));
        
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack1Mass"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack2Mass"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack3Mass"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack4Mass"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack5Mass"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack6Mass"));
        
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack1charge"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack2charge"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack3charge"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack4charge"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack5charge"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack6charge"));
        
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack1d0"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack2d0"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack3d0"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack4d0"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack5d0"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack6d0"));
        
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack1d0sig"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack2d0sig"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack3d0sig"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack4d0sig"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack5d0sig"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack6d0sig"));
        
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack1z0"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack2z0"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack3z0"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack4z0"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack5z0"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("RecoTrack6z0"));
        
        
        //ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("PU_Vtx"));
        //ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("NTracks"));
        
        //ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("MuonQuality"));
        //ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("p_Tracks"));
        
        
        // Now add the branches for the particle containers
        ATH_CHECK(m_XAMPPInfo->BookParticleStorage("Elec"));
        XAMPP::ParticleStorage* ElectronStore = m_XAMPPInfo->GetParticleStorage("Elec");
        ATH_CHECK(ElectronStore->SaveFloat("charge"));

        ATH_CHECK(m_XAMPPInfo->BookParticleStorage("Muon"));
        XAMPP::ParticleStorage* MuonStore = m_XAMPPInfo->GetParticleStorage("Muon");
        ATH_CHECK(MuonStore->SaveFloat("charge"));
        ATH_CHECK(MuonStore->SaveFloat("ptcone20"));
        ATH_CHECK(MuonStore->SaveFloat("ptcone30"));
        ATH_CHECK(MuonStore->SaveFloat("ptcone40"));
        ATH_CHECK(MuonStore->SaveVariable<unsigned char>("quality"));
        ATH_CHECK(MuonStore->SaveVariable<unsigned char>("outerSmallHits"));
        ATH_CHECK(MuonStore->SaveVariable<unsigned char>("outerLargeHits"));
        ATH_CHECK(MuonStore->SaveVariable<unsigned char>("outerLargeHoles"));
        ATH_CHECK(MuonStore->SaveVariable<unsigned char>("outerSmallHoles"));
        //ATH_CHECK(MuonStore->SaveFloat("eta"));
        //ATH_CHECK(MuonStore->SaveVariable<unsigned char>("passesIDCuts"));
        //ATH_CHECK(MuonStore->SaveVariable<unsigned char>("passesHighPtCuts"));
        ATH_CHECK(MuonStore->SaveVariable<unsigned short>("muonType"));
        //ATH_CHECK(MuonStore->SaveVariable<unsigned char>("numberOfPrecisionHoleLayers"));
        //ATH_CHECK(MuonStore->SaveVariable<unsigned char>("numberOfPresionLayers"));
        //ATH_CHECK(MuonStore->SaveVariable<unsigned char>("type"));

        ATH_CHECK(m_XAMPPInfo->BookParticleStorage("Jet"));
        XAMPP::ParticleStorage* JetStore = m_XAMPPInfo->GetParticleStorage("Jet");
        //ATH_CHECK(JetStore->SaveChar("bjet"));
        ATH_CHECK(JetStore->SaveFloat("Jvt"));
        ATH_CHECK(JetStore->SaveFloat("JvtJvfcorr"));
        ATH_CHECK(JetStore->SaveFloat("JvtRpt"));
        //ATH_CHECK(JetStore->SaveFloat("TrackSumPt"));
        //ATH_CHECK(JetStore->SaveFloat("SumPtTrkPt500"));
        ATH_CHECK(JetStore->SaveChar("isTauCand"));
        //ATH_CHECK(JetStore->SaveFloat("EMFrac"));
        //ATH_CHECK(JetStore->SaveFloat("HECFrac"));
        //ATH_CHECK(JetStore->SaveFloat("DetectorEta"));
        //ATH_CHECK(JetStore->SaveDouble("MV2c10"));
        ATH_CHECK(JetStore->SaveInteger("NTrks"));
        
        //ATH_CHECK(m_XAMPPInfo->BookParticleStorage("CaloSum"));
        //XAMPP::ParticleStorage* CaloSumStore = m_XAMPPInfo->GetParticleStorage("CaloSum");
        //ATH_CHECK(CaloSumStore->SaveFloat("et"));
 
        //if (!isData()) ATH_CHECK(JetStore->SaveInteger("ConeTruthLabelID"));

        if (doTruth()) {
            ATH_CHECK(m_XAMPPInfo->BookParticleStorage("TruthParticles", false, false));
            XAMPP::ParticleStorage* TruthStore = m_XAMPPInfo->GetParticleStorage("TruthParticles");
            ATH_CHECK(TruthStore->SaveFloat("charge"));
            ATH_CHECK(TruthStore->SaveInteger("pdgId"));

            // for TruthJet, save trees with (eta, phi, pt, m) -> set StoreMass=true
            ATH_CHECK(m_XAMPPInfo->BookParticleStorage("TruthJet", true));
            //XAMPP::ParticleStorage* TruthStore = m_XAMPPInfo->GetParticleStorage("TruthJet");
        }

        return StatusCode::SUCCESS;
    }

    double HeavyIonAnalysisHelper::GetTotalWeight() {
        static XAMPP::Storage<double>* dec_GenW = m_XAMPPInfo->GetVariableStorage<double>("GenWeight");
        static XAMPP::Storage<double>* dec_EleWeight = m_XAMPPInfo->GetVariableStorage<double>("EleWeight");
        static XAMPP::Storage<double>* dec_MuoWeight = m_XAMPPInfo->GetVariableStorage<double>("MuoWeight");
        static XAMPP::Storage<double>* dec_JetWeight = m_XAMPPInfo->GetVariableStorage<double>("JetWeight");
        static XAMPP::Storage<double>* dec_muWeight = m_XAMPPInfo->GetVariableStorage<double>("muWeight");
        //static XAMPP::Storage<double>* dec_SherpaVjetsNjetsWeight = m_XAMPPInfo->GetVariableStorage<double>("SherpaVjetsNjetsWeight");

        double w = dec_GenW->GetValue();
        if (dec_EleWeight) w *= dec_EleWeight->GetValue();
        if (dec_MuoWeight) w *= dec_MuoWeight->GetValue();
        if (dec_JetWeight) w *= dec_JetWeight->GetValue();
        if (dec_muWeight) w *= dec_muWeight->GetValue();
        //if (dec_SherpaVjetsNjetsWeight) w *= dec_SherpaVjetsNjetsWeight->GetValue();
        if (!isData()) {
            static XAMPP::Storage<double>* dec_SherpaVjetsNjetsWeight = m_XAMPPInfo->GetVariableStorage<double>("SherpaVjetsNjetsWeight");
            if (dec_SherpaVjetsNjetsWeight) w *= dec_SherpaVjetsNjetsWeight->GetValue();
        }
        return w;
    }
    
    

    StatusCode HeavyIonAnalysisHelper::ComputeEventVariables() {

        //First lets define the Pointer to the Storage elements
        //static XAMPP::Storage<int>* dec_NBJets = m_XAMPPInfo->GetVariableStorage<int>("N_BJets");
        static XAMPP::Storage<int>* dec_NLeps = m_XAMPPInfo->GetVariableStorage<int>("N_SignalLeptons");
        static XAMPP::Storage<int>* dec_NBaseLeps = m_XAMPPInfo->GetVariableStorage<int>("N_BaselineLeptons");
        static XAMPP::Storage<int>* dec_NSigMuons = m_XAMPPInfo->GetVariableStorage<int>("N_SignalMuons");
        static XAMPP::Storage<int>* dec_NBaseMuons = m_XAMPPInfo->GetVariableStorage<int>("N_BaselineMuons");
        static XAMPP::Storage<int>* dec_NJets = m_XAMPPInfo->GetVariableStorage<int>("N_Jets");
        static XAMPP::Storage<float>* dec_Mll = m_XAMPPInfo->GetVariableStorage<float>("Mll");
        static XAMPP::Storage<float>* dec_InvMass = m_XAMPPInfo->GetVariableStorage<float>("InvMass");
        static XAMPP::Storage<float>* dec_IDprobeCharge = m_XAMPPInfo->GetVariableStorage<float>("IDprobeCharge");
        static XAMPP::Storage<float>* dec_IDprobePt = m_XAMPPInfo->GetVariableStorage<float>("IDprobePt");
        static XAMPP::Storage<float>* dec_IDprobeEta = m_XAMPPInfo->GetVariableStorage<float>("IDprobeEta");
        //static XAMPP::Storage<float>* dec_Mmuon = m_XAMPPInfo->GetVariableStorage<float>("Mmuon");
        static XAMPP::Storage<char>* dec_IsOS = m_XAMPPInfo->GetVariableStorage<char>("IsOS");
        static XAMPP::Storage<char>* dec_IsSF = m_XAMPPInfo->GetVariableStorage<char>("IsSF");
        static XAMPP::Storage<float>* dec_HT = m_XAMPPInfo->GetVariableStorage<float>("HT");
        static XAMPP::Storage<float>* dec_HtSig = m_XAMPPInfo->GetVariableStorage<float>("HtSig");
        static XAMPP::Storage<float>* dec_Meff = m_XAMPPInfo->GetVariableStorage<float>("Meff");
        static XAMPP::Storage<float>* dec_ZWindow = m_XAMPPInfo->GetVariableStorage<float>("ZWindow");
        static XAMPP::Storage<XAMPPmet>* dec_XAMPPmetTST = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetTST");
        static XAMPP::Storage<XAMPPmet>* dec_XAMPPmetCST = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetCST");
        static XAMPP::Storage<XAMPPmet>* dec_XAMPPmetTrack = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetTrack");
        
        static XAMPP::Storage<float>* dec_XAMPPmetTST_mpx = m_XAMPPInfo->GetVariableStorage < float > ("MetTST_mpx");
        static XAMPP::Storage<float>* dec_XAMPPmetCST_mpx = m_XAMPPInfo->GetVariableStorage < float > ("MetCST_mpx");
        static XAMPP::Storage<float>* dec_XAMPPmetTrack_mpx = m_XAMPPInfo->GetVariableStorage < float > ("MetTrack_mpx");
        static XAMPP::Storage<float>* dec_XAMPPmetTST_mpy = m_XAMPPInfo->GetVariableStorage < float > ("MetTST_mpy");
        static XAMPP::Storage<float>* dec_XAMPPmetCST_mpy = m_XAMPPInfo->GetVariableStorage < float > ("MetCST_mpy");
        static XAMPP::Storage<float>* dec_XAMPPmetTrack_mpy = m_XAMPPInfo->GetVariableStorage < float > ("MetTrack_mpy");
        
        static XAMPP::Storage<float>* dec_ETsumFCal = m_XAMPPInfo->GetVariableStorage<float>("ETsumFCal");
        static XAMPP::Storage<float>* dec_Centrality = m_XAMPPInfo->GetVariableStorage<float>("Centrality");
        
      /*static XAMPP::Storage<int>* dec_numberOfPixelHits = m_XAMPPInfo->GetVariableStorage<int>("numberOfPixelHits");
      static XAMPP::Storage<int>* dec_numberOfPixelDeadSensors = m_XAMPPInfo->GetVariableStorage<int>("numberOfPixelDeadSensors");
      static XAMPP::Storage<int>* dec_numberOfSCTHits = m_XAMPPInfo->GetVariableStorage<int>("numberOfSCTHits");
      static XAMPP::Storage<int>* dec_numberOfSCTDeadSensors = m_XAMPPInfo->GetVariableStorage<int>("numberOfSCTDeadSensors");
      static XAMPP::Storage<int>* dec_numberOfTRTHits = m_XAMPPInfo->GetVariableStorage<int>("numberOfTRTHits");
      static XAMPP::Storage<int>* dec_numberOfTRTOutliers = m_XAMPPInfo->GetVariableStorage<int>("numberOfTRTOutliers");
      static XAMPP::Storage<int>* dec_numberOfPixelHoles = m_XAMPPInfo->GetVariableStorage<int>("numberOfPixelHoles");
      static XAMPP::Storage<int>* dec_numberOfSCTHoles = m_XAMPPInfo->GetVariableStorage<int>("numberOfSCTHoles");*/
        
        static XAMPP::Storage<int>* dec_N_RecoTracks = m_XAMPPInfo->GetVariableStorage<int>("N_RecoTracks");
        static XAMPP::Storage<int>* dec_N_RecoTracks_highPt = m_XAMPPInfo->GetVariableStorage<int>("N_RecoTracks_highPt");
        
        //static XAMPP::Storage<float>* dec_p_Track = m_XAMPPInfo->GetVariableStorage<float>("p_Tracks");
        //static XAMPP::Storage<int>* dec_PU_Vtx = m_XAMPPInfo->GetVariableStorage<int>("PU_Vtx");
        //static XAMPP::Storage<int>* dec_NTracks = m_XAMPPInfo->GetVariableStorage<int>("NTracks");
        //static XAMPP::Storage<int>* dec_MuonQuality = m_XAMPPInfo->GetVariableStorage<int>("MuonQuality");
        
        static XAMPP::Storage<float>* dec_RecoTrack1Pt = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack1Pt");
        static XAMPP::Storage<float>* dec_RecoTrack2Pt = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack2Pt");
        static XAMPP::Storage<float>* dec_RecoTrack3Pt = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack3Pt");
        static XAMPP::Storage<float>* dec_RecoTrack4Pt = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack4Pt");
        static XAMPP::Storage<float>* dec_RecoTrack5Pt = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack5Pt");
        static XAMPP::Storage<float>* dec_RecoTrack6Pt = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack6Pt");
        
        static XAMPP::Storage<float>* dec_RecoTrack1Eta = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack1Eta");
        static XAMPP::Storage<float>* dec_RecoTrack2Eta = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack2Eta");
        static XAMPP::Storage<float>* dec_RecoTrack3Eta = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack3Eta");
        static XAMPP::Storage<float>* dec_RecoTrack4Eta = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack4Eta");
        static XAMPP::Storage<float>* dec_RecoTrack5Eta = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack5Eta");
        static XAMPP::Storage<float>* dec_RecoTrack6Eta = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack6Eta");
        
        static XAMPP::Storage<float>* dec_RecoTrack1Phi = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack1Phi");
        static XAMPP::Storage<float>* dec_RecoTrack2Phi = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack2Phi");
        static XAMPP::Storage<float>* dec_RecoTrack3Phi = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack3Phi");
        static XAMPP::Storage<float>* dec_RecoTrack4Phi = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack4Phi");
        static XAMPP::Storage<float>* dec_RecoTrack5Phi = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack5Phi");
        static XAMPP::Storage<float>* dec_RecoTrack6Phi = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack6Phi");
        
        static XAMPP::Storage<float>* dec_RecoTrack1Mass = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack1Mass");
        static XAMPP::Storage<float>* dec_RecoTrack2Mass = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack2Mass");
        static XAMPP::Storage<float>* dec_RecoTrack3Mass = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack3Mass");
        static XAMPP::Storage<float>* dec_RecoTrack4Mass = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack4Mass");
        static XAMPP::Storage<float>* dec_RecoTrack5Mass = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack5Mass");
        static XAMPP::Storage<float>* dec_RecoTrack6Mass = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack6Mass");
        
        static XAMPP::Storage<float>* dec_RecoTrack1charge = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack1charge");
        static XAMPP::Storage<float>* dec_RecoTrack2charge = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack2charge");
        static XAMPP::Storage<float>* dec_RecoTrack3charge = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack3charge");
        static XAMPP::Storage<float>* dec_RecoTrack4charge = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack4charge");
        static XAMPP::Storage<float>* dec_RecoTrack5charge = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack5charge");
        static XAMPP::Storage<float>* dec_RecoTrack6charge = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack6charge");
        
        static XAMPP::Storage<float>* dec_RecoTrack1d0 = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack1d0");
        static XAMPP::Storage<float>* dec_RecoTrack2d0 = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack2d0");
        static XAMPP::Storage<float>* dec_RecoTrack3d0 = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack3d0");
        static XAMPP::Storage<float>* dec_RecoTrack4d0 = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack4d0");
        static XAMPP::Storage<float>* dec_RecoTrack5d0 = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack5d0");
        static XAMPP::Storage<float>* dec_RecoTrack6d0 = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack6d0");
        
        static XAMPP::Storage<float>* dec_RecoTrack1d0sig = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack1d0sig");
        static XAMPP::Storage<float>* dec_RecoTrack2d0sig = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack2d0sig");
        static XAMPP::Storage<float>* dec_RecoTrack3d0sig = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack3d0sig");
        static XAMPP::Storage<float>* dec_RecoTrack4d0sig = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack4d0sig");
        static XAMPP::Storage<float>* dec_RecoTrack5d0sig = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack5d0sig");
        static XAMPP::Storage<float>* dec_RecoTrack6d0sig = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack6d0sig");
        
        static XAMPP::Storage<float>* dec_RecoTrack1z0 = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack1z0");
        static XAMPP::Storage<float>* dec_RecoTrack2z0 = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack2z0");
        static XAMPP::Storage<float>* dec_RecoTrack3z0 = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack3z0");
        static XAMPP::Storage<float>* dec_RecoTrack4z0 = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack4z0");
        static XAMPP::Storage<float>* dec_RecoTrack5z0 = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack5z0");
        static XAMPP::Storage<float>* dec_RecoTrack6z0 = m_XAMPPInfo->GetVariableStorage<float>("RecoTrack6z0");

        if (!isData()) {
            static XAMPP::Storage<double>* dec_TotalWeight = m_XAMPPInfo->GetVariableStorage<double>("TotalWeight");
            ATH_CHECK(dec_TotalWeight->Store(GetTotalWeight()));
        }
        static XAMPP::Storage<char>* dec_HasTauCand = m_XAMPPInfo->GetVariableStorage<char>("HasTauCand");
        static XAMPP::Storage<float>* dec_MtTauCand = m_XAMPPInfo->GetVariableStorage<float>("MtTauCand");
        static XAMPP::Storage<int>* dec_NtracksTauCand = m_XAMPPInfo->GetVariableStorage<int>("NtracksTauCand");        

        // Then lets calculate our event variables... there are lots of functions in the AnalysisUtils in order to do that, feel free to add other functions for this purpose
        int NLeps = m_electron_selection->GetSignalElectrons()->size() + m_muon_selection->GetSignalMuons()->size();
        int NBaseLeps = m_electron_selection->GetBaselineElectrons()->size() + m_muon_selection->GetBaselineMuons()->size();
        int NSigMuons = m_muon_selection->GetSignalMuons()->size();
        int NBaseMuons = m_muon_selection->GetBaselineMuons()->size();
        int NJets = m_jet_selection->GetSignalJets()->size();
        
        
        float MetTST_mpx = dec_XAMPPmetTST->GetValue()->mpx();
        float MetTST_mpy = dec_XAMPPmetTST->GetValue()->mpy();
        float MetCST_mpx = dec_XAMPPmetCST->GetValue()->mpx();
        float MetCST_mpy = dec_XAMPPmetCST->GetValue()->mpy();
        float MetTrack_mpx = dec_XAMPPmetTrack->GetValue()->mpx();
        float MetTrack_mpy = dec_XAMPPmetTrack->GetValue()->mpy();
        
        /*evtStore();
        const xAOD::HIEventShapeContainer *hiue(0); 

   	if ( evtStore()->retrieve(hiue, "CaloSums").isFailure() ) {
      		std::cout << "Can not get HIEventShape Container " << std::endl;
      		return -999; 
    	} 
    	
    	xAOD::HIEventShapeContainer::const_iterator es_itr = hiue->begin(); 
  	xAOD::HIEventShapeContainer::const_iterator es_end = hiue->end();

  	double m_fcalEt = 0;

  	for (;es_itr!=es_end;es_itr++ )

	    {
	     float et = (*es_itr)->et();
	      const std::string name = (*es_itr)->auxdataConst<std::string>("Summary");
	      if (name=="FCal") m_fcalEt = et;
	    }

  	//return m_fcalEt;
  	float ETsumFCal = m_fcalEt;*/
     
     //m_muonCorr = new CP::MuonCalibrationAndSmearingTool( "muonCorrectionTool" );
    //m_muonCorr->msg().setLevel( MSG::INFO );
    ////m_muonCorr.setProperty( "Release", "PreRecs" );
    //ANA_CHECK(m_muonCorr->initialize());
    
    /*m_muonSelection1 = new CP::MuonSelectionTool("MuonSelection1");
    m_muonSelection1->msg().setLevel(MSG::ERROR);
    ANA_CHECK(m_muonSelection1->setProperty("MaxEta",2.5));
    ANA_CHECK(m_muonSelection1->setProperty("MuQuality",0));
	m_muonSelection2 = new CP::MuonSelectionTool("MuonSelection2");
    m_muonSelection2->msg().setLevel(MSG::ERROR);
    ANA_CHECK(m_muonSelection2->setProperty("MaxEta",2.5));
    ANA_CHECK(m_muonSelection2->setProperty("MuQuality",1));
    m_muonSelection3 = new CP::MuonSelectionTool("MuonSelection2");
    m_muonSelection3->msg().setLevel(MSG::ERROR);*/
    
      //Open and work with muon container
    const xAOD::MuonContainer* muonsnotcorr = 0;
    ANA_CHECK(evtStore()->retrieve( muonsnotcorr, "Muons" ));
    int m_Muon_number = 0;
    // create a shallow copy of the muons container
    std::pair< xAOD::MuonContainer*, xAOD::ShallowAuxContainer* > muons = xAOD::shallowCopyContainer( *muonsnotcorr );
    
    for (auto Muon: *(muons.first)) //Those muons are shallow muon copy
       {

         //Apply the calibration
         //if(m_muonCorr->applyCorrection(*Muon) == CP::CorrectionCode::Error)
         //{ Error("execute()", "MuonCalibrationAndSmearingTool returns Error CorrectionCode");}


         bool passTightID  = false;
	     bool passMediumID = false;
	     m_Muon_number++;
	     //if (m_muonSelection1->accept(Muon)) passTightID = true; //based on eta quality and ID cuts - tight muons
	     //if (m_muonSelection2->accept(Muon)) passMediumID = true; //medium muons



	      bool isCombined   = false; if(Muon->muonType() == xAOD::Muon_v1::Combined) isCombined=true;
	      bool isSegTag     = false; if(Muon->muonType() == xAOD::Muon_v1::SegmentTagged) isSegTag=true;
	      bool isCaloTag    = false; if(Muon->muonType() == xAOD::Muon_v1::CaloTagged) isCaloTag=true;
	      bool isStandAl    = false; if(Muon->muonType() == xAOD::Muon_v1::MuonStandAlone) isStandAl=true;

	      //M_isCombined              .push_back(isCombined);
	      //M_isSegTag                .push_back(isSegTag);
	      //M_isCaloTag               .push_back(isCaloTag);
	      //M_isStandAl               .push_back(isStandAl);



         //Fill in the muon reco info
	/*     m_Muon_number++;
	     m_Muon_pt	                .push_back( Muon->pt()*1e-3);
	     m_Muon_eta	                .push_back( Muon->eta() );
	     m_Muon_phi	                .push_back( Muon->phi() );
	     m_Muon_charge	            .push_back( Muon->charge() );
	     m_Muon_quality	            .push_back( m_muonSelection2->getQuality(*Muon) );
	     m_Muon_type	            .push_back( Muon->muonType() ); //Combined of sigment tag and so on.
	     m_Muon_ELoss               .push_back( Muon->floatParameter(xAOD::Muon::EnergyLoss) );


         m_Muon_isTight             .push_back( passTightID );
	     m_Muon_isMedium            .push_back( passMediumID );

	     m_Muon_MatchdR_mu4         .push_back(m_trigMuonMatching->minDelR(Muon,"HLT_mu4",1));
         m_Muon_MatchdR_mu6         .push_back(m_trigMuonMatching->minDelR(Muon,"HLT_mu6",1));
         m_Muon_MatchdR_mu8         .push_back(m_trigMuonMatching->minDelR(Muon,"HLT_mu8",1));

         m_Muon_MatchdR_mu15        .push_back(m_trigMuonMatching->minDelR(Muon,"HLT_mu15",1));
         m_Muon_MatchdR_mu15_MU10   .push_back(m_trigMuonMatching->minDelR(Muon,"HLT_mu15_L1MU10",1));

         m_Muon_MatchdR_mu10_MU6    .push_back(m_trigMuonMatching->minDelR(Muon,"HLT_mu10_L1MU6",1));
         m_Muon_MatchdR_mu15_MU6    .push_back(m_trigMuonMatching->minDelR(Muon,"HLT_mu15_L1MU6",1)); 




         //ID track information
         const xAOD::TrackParticle* Track = Muon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );


         if (Track)
            {
               m_Muon_passedIDCutsMed        .push_back( m_muonSelection2->passedIDCuts(*Track) );
               m_Muon_passedIDCutsTight      .push_back( m_muonSelection1->passedIDCuts(*Track) );
               m_Muon_Track_d0               .push_back( Track->d0()    );
               m_Muon_Track_z0               .push_back( Track->z0()    );
               m_Muon_id_theta               .push_back( Track->theta() );
               m_Muon_id_phi                 .push_back( Track->phi()   );
               m_Muon_id_pt                  .push_back( Track->pt()    );
               m_Muon_id_charge              .push_back( Track->charge());
               m_Muon_id_eta                 .push_back( Track->eta()   );


               int IdTrkValue=0;

               //Lets find the number of holes in the detector and so on left from muons
               IdTrkValue=Track->auxdata< unsigned char >("numberOfPixelHits");
               m_Muon_Track_nPixelHits            .push_back(IdTrkValue);
               IdTrkValue=Track->auxdata< unsigned char >("numberOfPixelDeadSensors");
               m_Muon_Track_nPixelDeadSensors     .push_back(IdTrkValue);
               IdTrkValue=Track->auxdata< unsigned char >("numberOfSCTHits");
               m_Muon_Track_nSCTHits              .push_back(IdTrkValue);
               IdTrkValue=Track->auxdata< unsigned char >("numberOfSCTDeadSensors");
               m_Muon_Track_nSCTDeadSensors       .push_back(IdTrkValue);
               IdTrkValue=Track->auxdata< unsigned char >("numberOfTRTHits");
               m_Muon_Track_nTRTHits              .push_back(IdTrkValue);
               IdTrkValue=Track->auxdata< unsigned char >("numberOfTRTOutliers");
               m_Muon_Track_nTRTOutliers          .push_back(IdTrkValue);
               IdTrkValue=Track->auxdata< unsigned char >("numberOfPixelHoles");
               m_Muon_Track_nPixelHoles           .push_back(IdTrkValue);
               IdTrkValue=Track->auxdata< unsigned char >("numberOfSCTHoles");
               m_Muon_Track_nSCTHoles             .push_back(IdTrkValue);
            } //End of track if
         else
            {
               m_Muon_passedIDCutsMed        .push_back( -2222 );
               m_Muon_passedIDCutsTight      .push_back( -2222 );
               m_Muon_Track_d0               .push_back( -2222 );
               m_Muon_Track_z0               .push_back( -2222 );
               m_Muon_Track_nPixelHits       .push_back( -2222 );
               m_Muon_Track_nPixelDeadSensors.push_back( -2222 );
               m_Muon_Track_nSCTHits         .push_back( -2222 );
               m_Muon_Track_nSCTDeadSensors  .push_back( -2222 );
               m_Muon_Track_nTRTHits         .push_back( -2222 );
               m_Muon_Track_nTRTOutliers     .push_back( -2222 );
               m_Muon_Track_nPixelHoles      .push_back( -2222 );
               m_Muon_Track_nSCTHoles        .push_back( -2222 );
               m_Muon_id_theta               .push_back( -2222 );
               m_Muon_id_phi                 .push_back( -2222 );
               m_Muon_id_pt                  .push_back( -2222 );
               m_Muon_id_charge              .push_back( -2222 );
               m_Muon_id_eta                 .push_back( -2222 );
            }//End of track else
	*/
          ////////////////////////////////////////////////////////////////////////////////////////
         //Lets do the ID efficiency///////////////////////////////////////////////////////////////
         /////////////////////////////////////////////////////////////////////////////////////////
         int keep = 0;
         const xAOD::TrackParticle* MeTrack = Muon->trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );
         if(MeTrack)
            {

              //m_me_pt                       .push_back(MeTrack->pt()*0.001);
              //m_me_eta                      .push_back(MeTrack->eta());
              //m_me_phi                      .push_back(MeTrack->phi());
              //m_me_charge                   .push_back(MeTrack->charge());
              //m_me_d0                       .push_back(MeTrack->d0());
              //m_me_z0                       .push_back(MeTrack->z0());
              //m_me_theta                    .push_back(MeTrack->theta());
              //m_me_ed0                      .push_back(sqrt(MeTrack->definingParametersCovMatrix()(0,0)));

	      
              ElementLink< xAOD::TrackParticleContainer > trkmulink = Muon->auxdata<ElementLink< xAOD::TrackParticleContainer > >("inDetTrackParticleLink");
              if(trkmulink.isValid())
               {
                 //int keep = 1;
                 //Lets do some track cuts
                 if ((*trkmulink)->pt()*0.001<0.4) keep=0; //Pt cut

                 float eta=(*trkmulink)->eta();
	             if(fabs(eta)>2.5) keep=0; //eta cut


                 float theta=(*trkmulink)->theta();
	             if(theta==0.0) keep=0;

                 //float z0=(trk->z0())*sin(theta);
	             int nPixhits   = (*trkmulink)->auxdata< unsigned char >("numberOfPixelHits") +   (*trkmulink)->auxdata< unsigned char >("numberOfPixelDeadSensors") ;
                 int nSCThits   = (*trkmulink)->auxdata< unsigned char >("numberOfSCTHits")   +   (*trkmulink)->auxdata< unsigned char >("numberOfSCTDeadSensors");
	             int nPixholes  = (*trkmulink)->auxdata< unsigned char >("numberOfPixelHoles");
	             //int nSCTholes  = (*trkmulink)->auxdata< unsigned char >("numberOfSCTHoles");
                 int nIBLHits   = (*trkmulink)->auxdata< unsigned char >("numberOfInnermostPixelLayerHits");
                 int nBLHits    = (*trkmulink)->auxdata< unsigned char >("numberOfNextToInnermostPixelLayerHits");
                 int expIBLHits = (*trkmulink)->auxdata< unsigned char >("expectInnermostPixelLayerHit");
                 int expBLHits  = (*trkmulink)->auxdata< unsigned char >("expectNextToInnermostPixelLayerHit");

	             if((nPixhits+nSCThits)<8)keep=0;
	             if((nPixhits+nSCThits)<10 && fabs(eta)>1.65)keep=0;
	             //if((nPixholes+nSCTholes)>=2)continue;
	             if( nPixholes >=1)keep=0;
	             if( (nIBLHits + nBLHits) <1 && expIBLHits && expBLHits)keep=0;
                 //if(keep) m_isTrack=1;


                }

               //m_Muon_isTrack. push_back(m_isTrack);


            }




        }// End of the muon loop
      
 	//Trakcs
   	const xAOD::TrackParticleContainer* Recotracks = 0;
    	ANA_CHECK(evtStore()->retrieve( Recotracks, "InDetTrackParticles" ));
    	int nRecotracks = Recotracks->size();
    	std::cout << "N_reco_track = " << nRecotracks << std::endl;
    	
    	
    	int ireco = 0;
    	 for (const xAOD::TrackParticle* trk: *Recotracks) {
    	 	//if ( fabs(trk->m() - 106.) < 20.) std::cout << "Recotrack mass = " << trk->m() << std::endl;
    	 	if (trk->pt() > 15000. ) {
    	 		std::cout << "Recotrack pt>15 GeV, pt  = " << trk->pt()/1000. << ", mass = " << trk->m() << std::endl;
    	 		//std::cout << "Recotrack mass = " << trk->m() << std::endl;
    	 		ireco ++;
 		}
    	 }
    	 std::cout << "N_recotracks with pt > 15 GeV = " << ireco << std::endl;
    	 
    	//int m_isTrack=0;
        //ElementLink< xAOD::TrackParticle > trkmulink = Recotracks->auxdata<ElementLink< xAOD::TrackParticleContainer > >("inDetTrackParticleLink");
    	
    	float RecoTrack1Pt = Recotracks->at(0)->pt();
    	float RecoTrack1Eta = Recotracks->at(0)->eta();
    	float RecoTrack1Phi = Recotracks->at(0)->phi();
    	float RecoTrack1Mass = Recotracks->at(0)->m();
    	float RecoTrack1charge = Recotracks->at(0)->charge();
    	float RecoTrack1d0 = Recotracks->at(0)->d0();
    	float RecoTrack1z0 = Recotracks->at(0)->z0();
    	float RecoTrack1d0sig = TMath::Sqrt(Recotracks->at(0)->definingParametersCovMatrix()(0,0));
    	
	float RecoTrack2Pt = -99.;
    	float RecoTrack2Eta = -99.;
    	float RecoTrack2Phi = -99.;
    	float RecoTrack2Mass = -99.;
    	float RecoTrack2charge = -99.;
    	float RecoTrack2d0 = -99.;
    	float RecoTrack2d0sig = -99.;
    	float RecoTrack2z0 = -99.;
    	float RecoTrack3Pt = -99.;
    	float RecoTrack3Eta = -99.;
    	float RecoTrack3Phi = -99.;
    	float RecoTrack3Mass = -99.;
    	float RecoTrack3charge = -99.;
    	float RecoTrack3d0 = -99.;
    	float RecoTrack3d0sig = -99.;
    	float RecoTrack3z0 = -99.;
    	float RecoTrack4Pt = -99.;
    	float RecoTrack4Eta = -99.;
    	float RecoTrack4Phi = -99.;
    	float RecoTrack4Mass = -99.;
    	float RecoTrack4charge = -99.;
    	float RecoTrack4d0 = -99.;
    	float RecoTrack4d0sig = -99.;
    	float RecoTrack4z0 = -99.;
    	float RecoTrack5Pt = -99.;
    	float RecoTrack5Eta = -99.;
    	float RecoTrack5Phi = -99.;
    	float RecoTrack5Mass = -99.;
    	float RecoTrack5charge = -99.;
    	float RecoTrack5d0 = -99.;
    	float RecoTrack5d0sig = -99.;
    	float RecoTrack5z0 = -99.;
    	float RecoTrack6Pt = -99.;
    	float RecoTrack6Eta = -99.;
    	float RecoTrack6Phi = -99.;
    	float RecoTrack6Mass = -99.;
    	float RecoTrack6charge = -99.;
    	float RecoTrack6d0 = -99.;
    	float RecoTrack6d0sig = -99.;
    	float RecoTrack6z0 = -99.;
    	
    	for (auto Muon: *(muons.first)) {
    	    ElementLink< xAOD::TrackParticleContainer > trkmulink = Muon->auxdata<ElementLink< xAOD::TrackParticleContainer > >("inDetTrackParticleLink");
	    if(trkmulink.isValid()) {
    	
	    	if (Recotracks->size() >=2) {
		    	RecoTrack2Pt = Recotracks->at(1)->pt();
		    	RecoTrack2Eta = Recotracks->at(1)->eta();
		    	RecoTrack2Phi = Recotracks->at(1)->phi();
		    	RecoTrack2Mass = Recotracks->at(1)->m();
		    	RecoTrack2charge = Recotracks->at(1)->charge();
		    	RecoTrack2d0 = Recotracks->at(1)->d0();
		    	RecoTrack2d0sig = TMath::Sqrt(Recotracks->at(1)->definingParametersCovMatrix()(0,0));
		    	RecoTrack2z0 = Recotracks->at(1)->z0();
	
			if (Recotracks->size() >=3) {
			    	RecoTrack3Pt = Recotracks->at(2)->pt();
			    	RecoTrack3Eta = Recotracks->at(2)->eta();
			    	RecoTrack3Phi = Recotracks->at(2)->phi();
			    	RecoTrack3Mass = Recotracks->at(2)->m();
			    	RecoTrack3charge = Recotracks->at(2)->charge();
			    	RecoTrack3d0 = Recotracks->at(2)->d0();
			    	RecoTrack3d0sig = TMath::Sqrt(Recotracks->at(2)->definingParametersCovMatrix()(0,0));
			    	RecoTrack3z0 = Recotracks->at(2)->z0();
			    	
			    	if (Recotracks->size() >=4) {
				    	RecoTrack4Pt = Recotracks->at(3)->pt();
				    	RecoTrack4Eta = Recotracks->at(3)->eta();
				    	RecoTrack4Phi = Recotracks->at(3)->phi();
				    	RecoTrack4Mass = Recotracks->at(3)->m();
				    	RecoTrack4charge = Recotracks->at(3)->charge();
				    	RecoTrack4d0 = Recotracks->at(3)->d0();
				    	RecoTrack4d0sig = TMath::Sqrt(Recotracks->at(3)->definingParametersCovMatrix()(0,0));
				    	RecoTrack4z0 = Recotracks->at(3)->z0();
			    	
				    	if (Recotracks->size() >=5) {
					    	RecoTrack5Pt = Recotracks->at(4)->pt();
					    	RecoTrack5Eta = Recotracks->at(4)->eta();
					    	RecoTrack5Phi = Recotracks->at(4)->phi();
					    	RecoTrack5Mass = Recotracks->at(4)->m();
					    	RecoTrack5charge = Recotracks->at(4)->charge();
					    	RecoTrack5d0 = Recotracks->at(4)->d0();
					    	RecoTrack5d0sig = TMath::Sqrt(Recotracks->at(4)->definingParametersCovMatrix()(0,0));
					    	RecoTrack5z0 = Recotracks->at(4)->z0();
					    	
					    	if (Recotracks->size() >=6) {
						    	RecoTrack6Pt = Recotracks->at(5)->pt();
						    	RecoTrack6Eta = Recotracks->at(5)->eta();
						    	RecoTrack6Phi = Recotracks->at(5)->phi();
						    	RecoTrack6Mass = Recotracks->at(5)->m();
						    	RecoTrack6charge = Recotracks->at(5)->charge();
						    	RecoTrack6d0 = Recotracks->at(5)->d0();
						    	RecoTrack6d0sig = TMath::Sqrt(Recotracks->at(5)->definingParametersCovMatrix()(0,0));
						    	RecoTrack6z0 = Recotracks->at(5)->z0();
						    	
						    	}
				    	}		    	
			    	}		    	
			}    	
	    	}
    	}//isValid
    	}
    	
    	
    	
    	//for (auto Muon: *(muons.first)) {
 	//	const xAOD::TrackParticle* Track = Muon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
 	//	}
 	
 	//const xAOD::Muon* mu = 0;
 	//const xAOD::TrackParticle* Track = mu->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
 	
 	//const xAOD::TrackParticle* mu_trk = mu->primaryTrackParticle();
  	
  	auto centTool = make_unique<HI::HICentralityTool>("CentralityTool");
	ANA_CHECK( centTool->setProperty("RunSpecies","pPb2016") );
	ANA_CHECK( centTool->initialize() );
	
	float ETsumFCal = 0;
        ETsumFCal = centTool->getCentralityEstimator();
	float Centrality = 0;
	Centrality = centTool->getCentralityPercentile();
	

	

        // the following lines can be used for event based debugging
//        const xAOD::EventInfo* Info = m_XAMPPInfo->GetOrigInfo();
//        if ((Info->eventNumber() == 146754) || (Info->eventNumber() == 44091) || (Info->eventNumber() == 7830)) {
//            std::cout << "--------------------------------------" << std::endl;
//            std::cout << "Event No. " << Info->eventNumber() << std::endl;
//            static SG::AuxElement::Decorator<char> dec_baseline("baseline");
//            static SG::AuxElement::Decorator<char> dec_bad("bad");
//            static SG::AuxElement::Decorator<char> dec_bjet("bjet");
//            static SG::AuxElement::Decorator<char> dec_passOR("passOR");
//            static SG::AuxElement::Decorator<int> dec_NTrks("NTrks");
//            static SG::AuxElement::Decorator<char> dec_signal("signal");
//            static SG::AuxElement::Decorator<double> dec_MV2c20("MV2c20");
//            static SG::AuxElement::Decorator<char> dec_cosmic("cosmic");
//            for (auto const &jet : *m_jet_selection->GetBaselineJets()) {
//                XAMPP::PromptParticle(jet, Form("PassOR: %d, isBad: %d, isB: %d, NTrks: %i, MV2c20: %f, isBaseline: %d, isSignal: %d", dec_passOR(*jet), dec_bad(*jet), dec_bjet(*jet), dec_NTrks(*jet), dec_MV2c20(*jet), dec_baseline(*jet), dec_signal(*jet)));
//            }
//            for (auto const &el : *m_electron_selection->GetBaselineElectrons()) {
//                XAMPP::PromptParticle(el, Form("PassOR: %d, isBaseline: %d, isSignal: %d", dec_passOR(*el), dec_baseline(*el), dec_signal(*el)));
//            }
//            for (auto const &mu : *m_muon_selection->GetBaselineMuons()) {
//                XAMPP::PromptParticle(mu, Form("PassOR: %d, isBaseline: %d, isSignal: %d, badMuon: %d, cosmic: %d", dec_passOR(*mu), dec_baseline(*mu), dec_signal(*mu), dec_bad(*mu), dec_cosmic(*mu)));
//            }
//            if (dec_XAMPPmetTST->GetValue()) {
                //std::cout << "MetTST: met()=" << dec_XAMPPmetTST->GetValue()->met() << " phi()=" << dec_XAMPPmetTST->GetValue()->phi() << " mpx = " << dec_XAMPPmetTST->GetValue()->mpx() << " mpy = " << dec_XAMPPmetTST->GetValue()->mpy() << std::endl;
                //std::cout << "MetCST: met()=" << dec_XAMPPmetCST->GetValue()->met() << " phi()=" << dec_XAMPPmetCST->GetValue()->phi() << " mpx = " << dec_XAMPPmetCST->GetValue()->mpx() << " mpy = " << dec_XAMPPmetCST->GetValue()->mpy() << std::endl;
                //std::cout << "MetTrack: met()=" << dec_XAMPPmetTrack->GetValue()->met() << " phi()=" << dec_XAMPPmetTrack->GetValue()->phi() << " mpx = " << dec_XAMPPmetTrack->GetValue()->mpx() << " mpy = " << dec_XAMPPmetTrack->GetValue()->mpy() << std::endl;
//            }
//        }

        //int NBJets = m_jet_selection->GetBJets()->size();
        float Ht = XAMPP::CalculateJetHt(m_jet_selection->GetSignalJets());
        float LeptonHt = XAMPP::CalculateLeptonHt(m_electron_selection->GetSignalElectrons(), m_muon_selection->GetSignalMuons());
        
        float MtTauCand = -1.;
        int NtracksTauCand = -1.;
        bool HasTauCand = XAMPP::HasTauCandidate(m_jet_selection->GetSignalJets(), dec_XAMPPmetTST->GetValue(), MtTauCand, NtracksTauCand);
        

        float HtSig = (dec_XAMPPmetTST->GetValue()->met()) * XAMPP::MeVToGeV / (TMath::Sqrt(Ht * XAMPP::MeVToGeV));
        bool IsOS = false;
        bool IsSF = false;
        float Mll = -1.;
        //static XAMPP::Storage<char>* dec_isTauCand = m_XAMPPInfo->GetVariableStorage<char>("isTauCand");
        //bool isTauCand = false;
        if (m_electron_selection->GetSignalElectrons()->size() > 1) {
            IsOS = XAMPP::OppositeSign(*m_electron_selection->GetSignalElectrons()->at(0), *m_electron_selection->GetSignalElectrons()->at(1));
            IsSF = XAMPP::SameFlavour(*m_electron_selection->GetSignalElectrons()->at(0), *m_electron_selection->GetSignalElectrons()->at(1));
            Mll = XAMPP::InvariantMass(*m_electron_selection->GetSignalElectrons()->at(0), *m_electron_selection->GetSignalElectrons()->at(1));
        }
        if (m_muon_selection->GetSignalMuons()->size() > 1) {
            IsOS = XAMPP::OppositeSign(*m_muon_selection->GetSignalMuons()->at(0), *m_muon_selection->GetSignalMuons()->at(1));
            IsSF = XAMPP::SameFlavour(*m_muon_selection->GetSignalMuons()->at(0), *m_muon_selection->GetSignalMuons()->at(1));
            Mll = XAMPP::InvariantMass(*m_muon_selection->GetSignalMuons()->at(0), *m_muon_selection->GetSignalMuons()->at(1));
            //isTauCand = XAMPP::HasTauCandidate(*m_muon_selection->GetSignalMuons()->at(0), *m_muon_selection->GetSignalMuons()->at(1));
        }
        
        int itrack = 0;
        float InvMass = -1.;
        //bool OppSign = false;
        float IDprobeCharge = -99.;
        float IDprobePt = -99.;
        float IDprobeEta = -99.;
      int numberOfPixelHits=0;
      int numberOfPixelDeadSensors=0;
      int numberOfSCTHits=0;
      int numberOfSCTDeadSensors=0;
      int numberOfTRTHits=0;
      int numberOfTRTOutliers=0;
      int numberOfPixelHoles=0;
      int numberOfSCTHoles =0;
        for (const xAOD::TrackParticle* trk: *Recotracks) {
        
           for (auto Muon: *(muons.first)) {
    	    ElementLink< xAOD::TrackParticleContainer > trkmulink = Muon->auxdata<ElementLink< xAOD::TrackParticleContainer > >("inDetTrackParticleLink");
	    if(trkmulink.isValid()) {
        	//Lets do some track cuts
               //Lets find the number of holes in the detector and so on left from muons
               /*numberOfPixelHits=trk->auxdata< unsigned char >("numberOfPixelHits");
               //int m_Muon_Track_nPixelHits            .push_back(IdTrkValue);
               numberOfPixelDeadSensors=trk->auxdata< unsigned char >("numberOfPixelDeadSensors");
               //m_Muon_Track_nPixelDeadSensors     .push_back(IdTrkValue);
               numberOfSCTHits=trk->auxdata< unsigned char >("numberOfSCTHits");
               //m_Muon_Track_nSCTHits              .push_back(IdTrkValue);
               numberOfSCTDeadSensors=trk->auxdata< unsigned char >("numberOfSCTDeadSensors");
               //m_Muon_Track_nSCTDeadSensors       .push_back(IdTrkValue);
               numberOfTRTHits=trk->auxdata< unsigned char >("numberOfTRTHits");
               //m_Muon_Track_nTRTHits              .push_back(IdTrkValue);
               numberOfTRTOutliers=trk->auxdata< unsigned char >("numberOfTRTOutliers");
               //m_Muon_Track_nTRTOutliers          .push_back(IdTrkValue);
               numberOfPixelHoles=trk->auxdata< unsigned char >("numberOfPixelHoles");
               //m_Muon_Track_nPixelHoles           .push_back(IdTrkValue);
               numberOfSCTHoles=trk->auxdata< unsigned char >("numberOfSCTHoles");
               //m_Muon_Track_nSCTHoles             .push_back(IdTrkValue);*/
               
                  if (trk->pt()*0.001<15.) continue; //Pt cut
                  float eta=trk->eta();
	              if(fabs(eta)>2.5) continue; //eta cut
                  //float theta=trk->theta();
	              //if(theta==0.0) continue;
	          //float mass=trk->m();
	              //if(mass < 10.5 || mass > 107.) continue;
	          float IDtrackd0 = trk->d0();
	          float IDtrackz0 = trk->z0();
	          float IDtrackd0sig = TMath::Sqrt(trk->definingParametersCovMatrix()(0,0));
	          	              
	          if (m_muon_selection->GetSignalMuons()->size() >= 1) {
	          	float invariantM = XAMPP::InvariantMass(*m_muon_selection->GetBaselineMuons()->at(0), *trk);
	          	std::cout << "Invariant Mass = " << invariantM/1000.  << " GeV" << std::endl;
	          	//if (InvMass/1000. < 60. || InvMass/1000. > 120.) continue;
	          	if ( fabs(invariantM/1000. - 91.0) > 25. ) continue;
	          	if ( fabs(IDtrackd0) > 1.5 || fabs(IDtrackz0) > 10. || fabs(IDtrackd0sig) > 3.) continue;
	          	
	          	//std::cout << "muon1 charge = " <<  (*m_muon_selection->GetBaselineMuons()->at(0))->charge() << ", ID track charge = " << trk->charge() << std::endl;
	          	//if ( ((*m_muon_selection->GetBaselineMuons()->at(0))->charge())*(trk->charge()) < 0.) bool OppSign = true;
	          	//std::cout << "OppositeSign is " << std::endl;
	          	//if (!OppSign) continue;
	          	//IsOS = XAMPP::OppositeSign(*m_muon_selection->GetBaselineMuons()->at(0), *trk);
	          	//float OppSign = IsOS;
	          	//if (OppSign == 0) continue;
	          	//}
        		
        		std::cout << "ID track pt = " << trk->pt()/1000. << ", ID track pasticle mass = " << trk->m()/1000. << std::endl;
        		IDprobeCharge = trk->charge();
        		IDprobePt = trk->pt();
        		IDprobeEta = trk->eta();
        		//if ( fabs(IDprobeCharge) > 
        		//if ( XAMPP::OppositeSign(*m_muon_selection->GetSignalMuons()->at(0), *trk) == 0 ) continue;
        		InvMass = invariantM;
        		itrack++;
		//if (m_muon_selection->GetBaselineMuons()->size() >= 1) {
	          	//itrack ++;
		    	//IsSF = XAMPP::SameFlavour(*m_muon_selection->GetBaselineMuons()->at(0), *trk);
		    	//Mll = XAMPP::InvariantMass(*m_muon_selection->GetBaselineMuons()->at(0), *trk);
		    //isTauCand = XAMPP::HasTauCandidate(*m_muon_selection->GetSignalMuons()->at(0), *m_muon_selection->GetSignalMuons()->at(1));
		    
		}
	}
	}
	}
	std::cout << "Number of recotracks that matched to a muon = " << itrack << std::endl;
	
	
	//if ( m_muon_selection->GetBaselineMuons()->size() >1) std::cout << "Muon1 mass = " << m_muon_selection->GetBaselineMuons()->at(0)->m() << ", Muon2 mass = " << m_muon_selection->GetBaselineMuons()->at(1)->m() << std::endl;
	
        float ZWindow = fabs(Mll - 91.0e3); // should it be 80.3e3 for W?s

        //Finally store the variables they are then used by the Cut Class or just written out into the trees
        //ATH_CHECK(dec_NBJets->Store(NBJets));
        ATH_CHECK(dec_NLeps->Store(NLeps));
        ATH_CHECK(dec_NBaseLeps->Store(NBaseLeps));
        ATH_CHECK(dec_NSigMuons->Store(NSigMuons));
        ATH_CHECK(dec_NBaseMuons->Store(NBaseMuons));
        ATH_CHECK(dec_NJets->Store(NJets));
        
        ATH_CHECK(dec_HasTauCand->Store(HasTauCand));
        ATH_CHECK(dec_MtTauCand->Store(MtTauCand));
        ATH_CHECK(dec_NtracksTauCand->Store(NtracksTauCand));        
        
        ATH_CHECK(dec_HtSig->Store(HtSig));
        ATH_CHECK(dec_IsOS->Store(IsOS));
        ATH_CHECK(dec_IsSF->Store(IsSF));
        ATH_CHECK(dec_Mll->Store(Mll));
        ATH_CHECK(dec_InvMass->Store(InvMass));
        ATH_CHECK(dec_IDprobeCharge->Store(IDprobeCharge));
        ATH_CHECK(dec_IDprobePt->Store(IDprobePt));
        ATH_CHECK(dec_IDprobeEta->Store(IDprobeEta));
        //ATH_CHECK(dec_Mmuon->Store(Mmuon));
        ATH_CHECK(dec_HT->Store(Ht));
        ATH_CHECK(dec_Meff->Store(Ht + LeptonHt + dec_XAMPPmetTST->GetValue()->met()));
        ATH_CHECK(dec_ZWindow->Store(ZWindow));
        
        ATH_CHECK(dec_XAMPPmetTST_mpx->Store(MetTST_mpx));
        ATH_CHECK(dec_XAMPPmetTST_mpy->Store(MetTST_mpy));
        ATH_CHECK(dec_XAMPPmetCST_mpx->Store(MetCST_mpx));
        ATH_CHECK(dec_XAMPPmetCST_mpy->Store(MetCST_mpy));
        ATH_CHECK(dec_XAMPPmetTrack_mpx->Store(MetTrack_mpx));
        ATH_CHECK(dec_XAMPPmetTrack_mpy->Store(MetTrack_mpy));
        
        ATH_CHECK(dec_ETsumFCal->Store(ETsumFCal));
        ATH_CHECK(dec_Centrality->Store(Centrality));
        
        ATH_CHECK(dec_N_RecoTracks->Store(nRecotracks));
        ATH_CHECK(dec_N_RecoTracks_highPt->Store(ireco));
        
        //ATH_CHECK(dec_PU_Vtx->Store(PU_Vtx));
        //ATH_CHECK(dec_NTracks->Store(NTracks));
        //ATH_CHECK(dec_MuonQuality->Store(MuonQuality));
        
        ATH_CHECK(dec_RecoTrack1Pt->Store(RecoTrack1Pt));
        ATH_CHECK(dec_RecoTrack2Pt->Store(RecoTrack2Pt));
        ATH_CHECK(dec_RecoTrack3Pt->Store(RecoTrack3Pt));
        ATH_CHECK(dec_RecoTrack4Pt->Store(RecoTrack4Pt));
        ATH_CHECK(dec_RecoTrack5Pt->Store(RecoTrack5Pt));
        ATH_CHECK(dec_RecoTrack6Pt->Store(RecoTrack6Pt));
        
        ATH_CHECK(dec_RecoTrack1Eta->Store(RecoTrack1Eta));
        ATH_CHECK(dec_RecoTrack2Eta->Store(RecoTrack2Eta));
        ATH_CHECK(dec_RecoTrack3Eta->Store(RecoTrack3Eta));
        ATH_CHECK(dec_RecoTrack4Eta->Store(RecoTrack4Eta));
        ATH_CHECK(dec_RecoTrack5Eta->Store(RecoTrack5Eta));
        ATH_CHECK(dec_RecoTrack6Eta->Store(RecoTrack6Eta));
        
        ATH_CHECK(dec_RecoTrack1Phi->Store(RecoTrack1Phi));
        ATH_CHECK(dec_RecoTrack2Phi->Store(RecoTrack2Phi));
        ATH_CHECK(dec_RecoTrack3Phi->Store(RecoTrack3Phi));
        ATH_CHECK(dec_RecoTrack4Phi->Store(RecoTrack4Phi));
        ATH_CHECK(dec_RecoTrack5Phi->Store(RecoTrack5Phi));
        ATH_CHECK(dec_RecoTrack6Phi->Store(RecoTrack6Phi));
        
        ATH_CHECK(dec_RecoTrack1Mass->Store(RecoTrack1Mass));
        ATH_CHECK(dec_RecoTrack2Mass->Store(RecoTrack2Mass));
        ATH_CHECK(dec_RecoTrack3Mass->Store(RecoTrack3Mass));
        ATH_CHECK(dec_RecoTrack4Mass->Store(RecoTrack4Mass));
        ATH_CHECK(dec_RecoTrack5Mass->Store(RecoTrack5Mass));
        ATH_CHECK(dec_RecoTrack6Mass->Store(RecoTrack6Mass));
        
        ATH_CHECK(dec_RecoTrack1charge->Store(RecoTrack1charge));
        ATH_CHECK(dec_RecoTrack2charge->Store(RecoTrack2charge));
        ATH_CHECK(dec_RecoTrack3charge->Store(RecoTrack3charge));
        ATH_CHECK(dec_RecoTrack4charge->Store(RecoTrack4charge));
        ATH_CHECK(dec_RecoTrack5charge->Store(RecoTrack5charge));
        ATH_CHECK(dec_RecoTrack6charge->Store(RecoTrack6charge));
        
        ATH_CHECK(dec_RecoTrack1d0->Store(RecoTrack1d0));
        ATH_CHECK(dec_RecoTrack2d0->Store(RecoTrack2d0));
        ATH_CHECK(dec_RecoTrack3d0->Store(RecoTrack3d0));
        ATH_CHECK(dec_RecoTrack4d0->Store(RecoTrack4d0));
        ATH_CHECK(dec_RecoTrack5d0->Store(RecoTrack5d0));
        ATH_CHECK(dec_RecoTrack6d0->Store(RecoTrack6d0));
        
        ATH_CHECK(dec_RecoTrack1d0sig->Store(RecoTrack1d0sig));
        ATH_CHECK(dec_RecoTrack2d0sig->Store(RecoTrack2d0sig));
        ATH_CHECK(dec_RecoTrack3d0sig->Store(RecoTrack3d0sig));
        ATH_CHECK(dec_RecoTrack4d0sig->Store(RecoTrack4d0sig));
        ATH_CHECK(dec_RecoTrack5d0sig->Store(RecoTrack5d0sig));
        ATH_CHECK(dec_RecoTrack6d0sig->Store(RecoTrack6d0sig));
        
        ATH_CHECK(dec_RecoTrack1z0->Store(RecoTrack1z0));
        ATH_CHECK(dec_RecoTrack2z0->Store(RecoTrack2z0));
        ATH_CHECK(dec_RecoTrack3z0->Store(RecoTrack3z0));
        ATH_CHECK(dec_RecoTrack4z0->Store(RecoTrack4z0));
        ATH_CHECK(dec_RecoTrack5z0->Store(RecoTrack5z0));
        ATH_CHECK(dec_RecoTrack6z0->Store(RecoTrack6z0));
        
        /*ATH_CHECK(dec_numberOfPixelHits->Store(numberOfPixelHits));
        ATH_CHECK(dec_numberOfPixelDeadSensors->Store(numberOfPixelDeadSensors));
        ATH_CHECK(dec_numberOfSCTHits->Store(numberOfSCTHits));
        ATH_CHECK(dec_numberOfSCTDeadSensors->Store(numberOfSCTDeadSensors));
        ATH_CHECK(dec_numberOfTRTHits->Store(numberOfTRTHits));
        ATH_CHECK(dec_numberOfTRTOutliers->Store(numberOfTRTOutliers));
        ATH_CHECK(dec_numberOfPixelHoles->Store(numberOfPixelHoles));
        ATH_CHECK(dec_numberOfSCTHoles->Store(numberOfSCTHoles));*/
        

        // fill the particle containers for the output
        static XAMPP::ParticleStorage* ElectronStore = m_XAMPPInfo->GetParticleStorage("Elec");
        ATH_CHECK(ElectronStore->Fill(m_electron_selection->GetSignalElectrons()));
        static XAMPP::ParticleStorage* MuonStore = m_XAMPPInfo->GetParticleStorage("Muon");
        //ATH_CHECK(MuonStore->Fill(m_muon_selection->GetSignalMuons()));
        ATH_CHECK(MuonStore->Fill(m_muon_selection->GetBaselineMuons()));
        static XAMPP::ParticleStorage* JetStore = m_XAMPPInfo->GetParticleStorage("Jet");
        ATH_CHECK(JetStore->Fill(m_jet_selection->GetSignalJets()));

        if (doTruth() && m_XAMPPInfo->GetSystematic() == m_systematics->GetNominal()) {
            XAMPP::ParticleStorage* TruthStore = m_XAMPPInfo->GetParticleStorage("TruthParticles");
            ATH_CHECK(TruthStore->Fill(m_truth_selection->GetTruthParticles()));
            XAMPP::ParticleStorage* TruthJetStore = m_XAMPPInfo->GetParticleStorage("TruthJet");
            ATH_CHECK(TruthJetStore->Fill(m_truth_selection->GetTruthSignalJets()));

        }

       
        //ATH_CHECK(dec_isTauCand->Store(isTauCand));


        return StatusCode::SUCCESS;
    }

    bool HeavyIonAnalysisHelper::AcceptEvent() {
        if (isData()) return true;

        // METFilterDuplicateCut:
        int mcChannelNumber = m_XAMPPInfo->mcChannelNumber();

        static XAMPP::Storage<double>* dec_SherpaVjetsNjetsWeight = m_XAMPPInfo->GetVariableStorage<double>("SherpaVjetsNjetsWeight");
        float SherpaVjetsNjetsWeight = 1.;
        if (((mcChannelNumber >= 363102) && (mcChannelNumber <= 363122)) || ((mcChannelNumber >= 363331) && (mcChannelNumber <= 363483))) SherpaVjetsNjetsWeight = m_susytools->getSherpaVjetsNjetsWeight();
        ATH_CHECK(dec_SherpaVjetsNjetsWeight->ConstStore(SherpaVjetsNjetsWeight));

        static XAMPP::Storage<int>* dec_treatAsYear = m_XAMPPInfo->GetVariableStorage<int>("treatAsYear");
        ATH_CHECK(dec_treatAsYear->ConstStore(m_susytools->treatAsYear()));
        
        // do not use anymore for ttV samples with e4631
//        return !m_truth_selection->ttZqqVeto(Info->mcChannelNumber());
        return true;
    }

}
