#include <XAMPPanalyses/Stop0LAnalysisHelper.h>
#include <XAMPPanalyses/Stop0LAnalysisUtils.h>
#include <XAMPPanalyses/Stop0LISRSelector.h>

#include <JetResolution/JERTool.h>
#include <JetResolution/JERSmearingTool.h>

#define SET_DUAL_TOOL( TOOLHANDLE, TOOLTYPE, TOOLNAME )                \
  ASG_SET_ANA_TOOL_TYPE(TOOLHANDLE, TOOLTYPE);                        \
  TOOLHANDLE.setName(TOOLNAME);

namespace XAMPP {
    Stop0LAnalysisHelper::Stop0LAnalysisHelper(std::string myname) :
                SUSYAnalysisHelper(myname),
                m_jerTool(""),
                m_ISR_selection("ISRTool"),
                m_ISR_selection_truth("ISRTruthTool"){
        declareProperty("doReclustering", m_doReclustering = true);
        declareProperty("CustomReclusterPt", m_CustomReclusterPt = 20000.);
        declareProperty("doISR", m_doISR = false);
        declareProperty("DoTrackMet", m_doTrackMet = true);
        declareProperty("doWDecays", m_doWDecays = false);
        m_ISR_selection.declarePropertyFor(this, "ISRTool", "Extra tool for stop ISR analysis");
        m_ISR_selection_truth.declarePropertyFor(this, "ISRTruthTool", "Extra tool for stop ISR analysis");        
    }
    StatusCode Stop0LAnalysisHelper::initializeObjectTools() {
        ATH_CHECK(SUSYAnalysisHelper::initializeObjectTools());
        if (m_doISR) {           
            if (!m_ISR_selection.isUserConfigured()){
                SET_DUAL_TOOL(m_ISR_selection,XAMPP::Stop0LISRSelector, "ISRTool");                
                ATH_CHECK(m_ISR_selection.retrieve());
            }
            if (doTruth() && !m_ISR_selection_truth.isUserConfigured()){
                SET_DUAL_TOOL(m_ISR_selection_truth,XAMPP::Stop0LISRSelector, "ISRTruthTool");                
                ATH_CHECK(m_ISR_selection_truth.retrieve());
            }
        }
        return StatusCode::SUCCESS;
    }
    StatusCode Stop0LAnalysisHelper::initialize() {
        ATH_MSG_INFO("Initialising Stop0L AnalysisHelper...");
        ATH_CHECK(SUSYAnalysisHelper::initialize());
        m_jerTool = GetCPTool < IJERTool > ("JERTool");
        return StatusCode::SUCCESS;
    }

    StatusCode Stop0LAnalysisHelper::initializeEventVariables() {

        //Lets define some variables which we want to store in the output tree / use in the cutflow
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<char>("isMetSampleOR", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<char>("IsMETTrigPassed"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_BJets"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_SignalLeptons"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_BaselineLeptons", false)); //A variable on which we are cutting does not need to be stored in the tree
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_Jets"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtBMin"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtNonBMin"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtJetMin"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtBMax"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtBMinLep"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtBMaxLep"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtNonBMax"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtJetMax"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtLepMet"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("JetLepPt1"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("JetLepPt2"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("JetLepPt3"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("JetLepPt4"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("AbsDeltaPhiMin2", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("AbsDeltaPhiMin4"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("DeltaPhiMin2"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("DPhiMetTrackMet"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("AbsDPhiMetTrackMet", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<char>("HasTauCand"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtTauCand"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("NtracksTauCand"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_FatJets12Pt35", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("AsymmetryMTopR12"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("AsymmetryMTopR08"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("HtSig"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MinMt"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Top1CandM", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Top2CandM", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<char>("IsOS"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<char>("IsSF"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Mll"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("HT"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Meff"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("ZWindow", false));
        if (!isData()) {
            ATH_CHECK(m_XAMPPInfo->NewEventVariable<double>("TotalWeight"));
            ATH_CHECK(m_XAMPPInfo->NewEventVariable<double>("SherpaVjetsNjetsWeight"));
            ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("GenFiltMET"));

        }
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("treatAsYear"));

        //variables to have the correct jet resolution and MetSignificance with it
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("TotResoJets"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MetSigJetReso"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MetSig200JetReso"));

        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("dRB1L"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("dRB2L"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("dRBLMin"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("dRBB"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("mt2"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("mt2_chi2"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("mt2_AKt08"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("mt2_AKt12"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("mt2_dRBMin"));
        if (m_doWDecays) ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("TruthTopDecays"));


        if (m_doISR) {
            ATH_CHECK(m_ISR_selection->initializeISREventVariables());
            if (doTruth()) ATH_CHECK(m_ISR_selection_truth->initializeISREventVariables(true));
        }

        // Now add the branches for the particle containers
        ATH_CHECK(m_XAMPPInfo->BookParticleStorage("Elec"));
        XAMPP::ParticleStorage* ElectronStore = m_XAMPPInfo->GetParticleStorage("Elec");
        ATH_CHECK(ElectronStore->SaveFloat("charge"));

        ATH_CHECK(m_XAMPPInfo->BookParticleStorage("Muon"));
        XAMPP::ParticleStorage* MuonStore = m_XAMPPInfo->GetParticleStorage("Muon");
        ATH_CHECK(MuonStore->SaveFloat("charge"));

        ATH_CHECK(m_XAMPPInfo->BookParticleStorage("Jet"));
        XAMPP::ParticleStorage* JetStore = m_XAMPPInfo->GetParticleStorage("Jet");
        ATH_CHECK(JetStore->SaveChar("bjet"));
        ATH_CHECK(JetStore->SaveFloat("Jvt"));
        ATH_CHECK(JetStore->SaveChar("isTauCand"));
        ATH_CHECK(JetStore->SaveFloat("EMFrac"));
        ATH_CHECK(JetStore->SaveFloat("HECFrac"));
        ATH_CHECK(JetStore->SaveDouble("MV2c10"));
        ATH_CHECK(JetStore->SaveInteger("NTrks"));

        if (m_doISR) ATH_CHECK(JetStore->SaveChar("isISR_V"));
        if (!isData()) ATH_CHECK(JetStore->SaveInteger("ConeTruthLabelID"));

        // for RecoCandidates, save trees with (eta, phi, pt, m) -> set StoreMass=true
        ATH_CHECK(m_XAMPPInfo->BookParticleStorage("RecoCandidates", true));
        XAMPP::ParticleStorage* RecoStore = m_XAMPPInfo->GetParticleStorage("RecoCandidates");
        ATH_CHECK(RecoStore->SaveFloat("charge"));
        ATH_CHECK(RecoStore->SaveInteger("pdgId"));
        ATH_CHECK(RecoStore->SaveInteger("RecoType"));

        if (m_doReclustering) {
            ATH_CHECK(m_XAMPPInfo->BookParticleStorage("FatJet08", true));
            XAMPP::ParticleStorage* FatJet08Store = m_XAMPPInfo->GetParticleStorage("FatJet08");
            ATH_CHECK(FatJet08Store->SaveInteger("constituents"));
            ATH_CHECK(m_XAMPPInfo->BookParticleStorage("Pt35_FatJet08", true));
            XAMPP::ParticleStorage* Pt35_FatJet08Store = m_XAMPPInfo->GetParticleStorage("Pt35_FatJet08");
            ATH_CHECK(Pt35_FatJet08Store->SaveInteger("constituents"));
            ATH_CHECK(m_XAMPPInfo->BookParticleStorage("FatJet12", true));
            XAMPP::ParticleStorage* FatJet12Store = m_XAMPPInfo->GetParticleStorage("FatJet12");
            ATH_CHECK(FatJet12Store->SaveInteger("constituents"));
            ATH_CHECK(m_XAMPPInfo->BookParticleStorage("Pt35_FatJet12", true));
            XAMPP::ParticleStorage* Pt35_FatJet12Store = m_XAMPPInfo->GetParticleStorage("Pt35_FatJet12");
            ATH_CHECK(Pt35_FatJet12Store->SaveInteger("constituents"));
        }

        if (doTruth()) {
            ATH_CHECK(m_XAMPPInfo->BookParticleStorage("TruthParticles", false, false));
            XAMPP::ParticleStorage* TruthStore = m_XAMPPInfo->GetParticleStorage("TruthParticles");
            ATH_CHECK(TruthStore->SaveFloat("charge"));
            ATH_CHECK(TruthStore->SaveInteger("pdgId"));
        }

        return StatusCode::SUCCESS;
    }

    double Stop0LAnalysisHelper::GetTotalWeight() {
        static XAMPP::Storage<double>* dec_GenW = m_XAMPPInfo->GetVariableStorage<double>("GenWeight");
        static XAMPP::Storage<double>* dec_EleWeight = m_XAMPPInfo->GetVariableStorage<double>("EleWeight");
        static XAMPP::Storage<double>* dec_MuoWeight = m_XAMPPInfo->GetVariableStorage<double>("MuoWeight");
        static XAMPP::Storage<double>* dec_JetWeight = m_XAMPPInfo->GetVariableStorage<double>("JetWeight");
        static XAMPP::Storage<double>* dec_muWeight = m_XAMPPInfo->GetVariableStorage<double>("muWeight");
        double w = dec_GenW->GetValue();
        if (dec_EleWeight) w *= dec_EleWeight->GetValue();
        if (dec_MuoWeight) w *= dec_MuoWeight->GetValue();
        if (dec_JetWeight) w *= dec_JetWeight->GetValue();
        if (dec_muWeight) w *= dec_muWeight->GetValue();
        if (!isData()) {
            static XAMPP::Storage<double>* dec_SherpaVjetsNjetsWeight = m_XAMPPInfo->GetVariableStorage<double>("SherpaVjetsNjetsWeight");
            if (dec_SherpaVjetsNjetsWeight) w *= dec_SherpaVjetsNjetsWeight->GetValue();
        }
        return w;
    }

    StatusCode Stop0LAnalysisHelper::ComputeEventVariables() {

        //First lets define the Pointer to the Storage elements
        static XAMPP::Storage<int>* dec_NBJets = m_XAMPPInfo->GetVariableStorage<int>("N_BJets");
        static XAMPP::Storage<int>* dec_NLeps = m_XAMPPInfo->GetVariableStorage<int>("N_SignalLeptons");
        static XAMPP::Storage<int>* dec_NBaseLeps = m_XAMPPInfo->GetVariableStorage<int>("N_BaselineLeptons");
        static XAMPP::Storage<int>* dec_NJets = m_XAMPPInfo->GetVariableStorage<int>("N_Jets");
        static XAMPP::Storage<float>* dec_MtBMin = m_XAMPPInfo->GetVariableStorage<float>("MtBMin");
        static XAMPP::Storage<float>* dec_MtNonBMin = m_XAMPPInfo->GetVariableStorage<float>("MtNonBMin");
        static XAMPP::Storage<float>* dec_MtJetMin = m_XAMPPInfo->GetVariableStorage<float>("MtJetMin");
        static XAMPP::Storage<float>* dec_MtBMax = m_XAMPPInfo->GetVariableStorage<float>("MtBMax");
        static XAMPP::Storage<float>* dec_MtBMinLep = m_XAMPPInfo->GetVariableStorage<float>("MtBMinLep");
        static XAMPP::Storage<float>* dec_MtBMaxLep = m_XAMPPInfo->GetVariableStorage<float>("MtBMaxLep");
        static XAMPP::Storage<float>* dec_MtNonBMax = m_XAMPPInfo->GetVariableStorage<float>("MtNonBMax");
        static XAMPP::Storage<float>* dec_MtJetMax = m_XAMPPInfo->GetVariableStorage<float>("MtJetMax");
        static XAMPP::Storage<float>* dec_MtLepMet = m_XAMPPInfo->GetVariableStorage<float>("MtLepMet");
        static XAMPP::Storage<float>* dec_AbsDeltaPhiMin2 = m_XAMPPInfo->GetVariableStorage<float>("AbsDeltaPhiMin2");
        static XAMPP::Storage<float>* dec_AbsDeltaPhiMin4 = m_XAMPPInfo->GetVariableStorage<float>("AbsDeltaPhiMin4");
        static XAMPP::Storage<float>* dec_lepJetPt1 = m_XAMPPInfo->GetVariableStorage<float>("JetLepPt1");
        static XAMPP::Storage<float>* dec_lepJetPt2 = m_XAMPPInfo->GetVariableStorage<float>("JetLepPt2");
        static XAMPP::Storage<float>* dec_lepJetPt3 = m_XAMPPInfo->GetVariableStorage<float>("JetLepPt3");
        static XAMPP::Storage<float>* dec_lepJetPt4 = m_XAMPPInfo->GetVariableStorage<float>("JetLepPt4");
        static XAMPP::Storage<float>* dec_DeltaPhiMin2 = m_XAMPPInfo->GetVariableStorage<float>("DeltaPhiMin2");
        static XAMPP::Storage<float>* dec_DPhiMetTrackMet = m_XAMPPInfo->GetVariableStorage<float>("DPhiMetTrackMet");
        static XAMPP::Storage<float>* dec_AbsDPhiMetTrackMet = m_XAMPPInfo->GetVariableStorage<float>("AbsDPhiMetTrackMet");
        static XAMPP::Storage<char>* dec_HasTauCand = m_XAMPPInfo->GetVariableStorage<char>("HasTauCand");
        static XAMPP::Storage<float>* dec_MtTauCand = m_XAMPPInfo->GetVariableStorage<float>("MtTauCand");
        static XAMPP::Storage<int>* dec_NtracksTauCand = m_XAMPPInfo->GetVariableStorage<int>("NtracksTauCand");
        static XAMPP::Storage<int>* dec_N_FatJets12Pt35 = m_XAMPPInfo->GetVariableStorage<int>("N_FatJets12Pt35");
        static XAMPP::Storage<float>* dec_AsymmetryMTopR12 = m_XAMPPInfo->GetVariableStorage<float>("AsymmetryMTopR12");
        static XAMPP::Storage<float>* dec_AsymmetryMTopR08 = m_XAMPPInfo->GetVariableStorage<float>("AsymmetryMTopR08");
        static XAMPP::Storage<float>* dec_HtSig = m_XAMPPInfo->GetVariableStorage<float>("HtSig");
        static XAMPP::Storage<float>* dec_MinMt = m_XAMPPInfo->GetVariableStorage<float>("MinMt");
        static XAMPP::Storage<float>* dec_Top1CandM = m_XAMPPInfo->GetVariableStorage<float>("Top1CandM");
        static XAMPP::Storage<float>* dec_Top2CandM = m_XAMPPInfo->GetVariableStorage<float>("Top2CandM");
        static XAMPP::Storage<float>* dec_Mll = m_XAMPPInfo->GetVariableStorage<float>("Mll");
        static XAMPP::Storage<char>* dec_IsOS = m_XAMPPInfo->GetVariableStorage<char>("IsOS");
        static XAMPP::Storage<char>* dec_IsSF = m_XAMPPInfo->GetVariableStorage<char>("IsSF");
        static XAMPP::Storage<float>* dec_HT = m_XAMPPInfo->GetVariableStorage<float>("HT");
        static XAMPP::Storage<float>* dec_Meff = m_XAMPPInfo->GetVariableStorage<float>("Meff");
        static XAMPP::Storage<float>* dec_ZWindow = m_XAMPPInfo->GetVariableStorage<float>("ZWindow");
        static XAMPP::Storage<float>* dec_TotResoJets = m_XAMPPInfo->GetVariableStorage<float>("TotResoJets");
        static XAMPP::Storage<float>* dec_MetSigJetReso = m_XAMPPInfo->GetVariableStorage<float>("MetSigJetReso");
        static XAMPP::Storage<float>* dec_MetSig200JetReso = m_XAMPPInfo->GetVariableStorage<float>("MetSig200JetReso");
        static XAMPP::Storage<float>* dec_dRB1L = m_XAMPPInfo->GetVariableStorage<float>("dRB1L");
        static XAMPP::Storage<float>* dec_dRB2L = m_XAMPPInfo->GetVariableStorage<float>("dRB2L");
        static XAMPP::Storage<float>* dec_dRBLMin = m_XAMPPInfo->GetVariableStorage<float>("dRBLMin");
        static XAMPP::Storage<float>* dec_dRBB = m_XAMPPInfo->GetVariableStorage<float>("dRBB");
        static XAMPP::Storage<float>* dec_mt2 = m_XAMPPInfo->GetVariableStorage<float>("mt2");
        static XAMPP::Storage<float>* dec_mt2_chi2 = m_XAMPPInfo->GetVariableStorage<float>("mt2_chi2");
        static XAMPP::Storage<float>* dec_mt2_AKt08 = m_XAMPPInfo->GetVariableStorage<float>("mt2_AKt08");
        static XAMPP::Storage<float>* dec_mt2_AKt12 = m_XAMPPInfo->GetVariableStorage<float>("mt2_AKt12");
        static XAMPP::Storage<float>* dec_mt2_dRBMin = m_XAMPPInfo->GetVariableStorage<float>("mt2_dRBMin");
        static XAMPP::Storage<int>* dec_TruthTopDecays = m_XAMPPInfo->GetVariableStorage<int>("TruthTopDecays");

        static XAMPP::Storage<XAMPPmet>* dec_XAMPPmetTST = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetTST");
        static XAMPP::Storage<XAMPPmet>* dec_XAMPPmetLepInv = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetLepInv");

        if (!isData()) {
            static XAMPP::Storage<double>* dec_TotalWeight = m_XAMPPInfo->GetVariableStorage<double>("TotalWeight");
            ATH_CHECK(dec_TotalWeight->Store(GetTotalWeight()));
        }

        // use xAOD::JetContainer* in order to call 'm_jet_selection->GetSignalJets()' only once:
        xAOD::JetContainer *SignalJets = m_jet_selection->GetSignalJets();

        xAOD::Particle* W1Cand = m_ParticleConstructor->CreateEmptyParticle();
        xAOD::Particle* W2Cand = m_ParticleConstructor->CreateEmptyParticle();
        xAOD::Particle* Top1Cand = m_ParticleConstructor->CreateEmptyParticle();
        xAOD::Particle* Top2Cand = m_ParticleConstructor->CreateEmptyParticle();
        XAMPP::GetTopCandidates(m_jet_selection->GetBJets(), m_jet_selection->GetLightJets(), W1Cand, W2Cand, Top1Cand, Top2Cand);
        ATH_CHECK(dec_Top1CandM->Store(Top1Cand->m()));
        ATH_CHECK(dec_Top2CandM->Store(Top2Cand->m()));

        xAOD::Particle* W1CandDRB4 = m_ParticleConstructor->CreateEmptyParticle();
        xAOD::Particle* W2CandDRB4 = m_ParticleConstructor->CreateEmptyParticle();
        xAOD::Particle* Top1CandDRB4 = m_ParticleConstructor->CreateEmptyParticle();
        xAOD::Particle* Top2CandDRB4 = m_ParticleConstructor->CreateEmptyParticle();
        XAMPP::GetTopCandidatesDRB4(m_jet_selection->GetBJets(), m_jet_selection->GetLightJets(), W1CandDRB4, W2CandDRB4, Top1CandDRB4, Top2CandDRB4);

        xAOD::Particle* W1CandMinMass = m_ParticleConstructor->CreateEmptyParticle();
        xAOD::Particle* W2CandMinMass = m_ParticleConstructor->CreateEmptyParticle();
        xAOD::Particle* Top1CandMinMass = m_ParticleConstructor->CreateEmptyParticle();
        xAOD::Particle* Top2CandMinMass = m_ParticleConstructor->CreateEmptyParticle();
        XAMPP::GetTopCandidatesMinMass(m_jet_selection->GetBJets(), m_jet_selection->GetLightJets(), W1CandMinMass, W2CandMinMass, Top1CandMinMass, Top2CandMinMass);

        ATH_CHECK(XAMPP::GetTopCandidatesChi2(SignalJets, m_jet_selection->GetBJets(), m_ParticleConstructor));

        // use SG::VIEW_ELEMENTS container for combining electrons and muons
        xAOD::IParticleContainer SignalLeptons(SG::VIEW_ELEMENTS);
        std::vector<float> lepJetPt;
        for (auto const &elec : *m_electron_selection->GetSignalElectrons()) {
            SignalLeptons.push_back(elec);
            lepJetPt.push_back(elec->pt());
        }
        for (auto const &muon : *m_muon_selection->GetSignalMuons()) {
            SignalLeptons.push_back(muon);
            lepJetPt.push_back(muon->pt());
        }
        for (auto const &jet : *m_jet_selection->GetSignalJets()) {
            lepJetPt.push_back(jet->pt());
        }
        //reverse iterators so descending order
        std::sort(lepJetPt.rbegin(), lepJetPt.rend());

        if (lepJetPt.size() >= 4) {
            ATH_CHECK(dec_lepJetPt4->Store(lepJetPt[3]));
        }
        if (lepJetPt.size() >= 3) {
            ATH_CHECK(dec_lepJetPt3->Store(lepJetPt[2]));
        }
        if (lepJetPt.size() >= 2) {
            ATH_CHECK(dec_lepJetPt2->Store(lepJetPt[1]));
        }
        if (lepJetPt.size() >= 1) {
            ATH_CHECK(dec_lepJetPt1->Store(lepJetPt[0]));
        }

        int NLeps = SignalLeptons.size();
        int NBaseLeps = m_electron_selection->GetBaselineElectrons()->size() + m_muon_selection->GetBaselineMuons()->size();
        int NJets = SignalJets->size();

        int N_FatJets12Pt35 = -1.;
        float AsymmetryMTopR12 = -1.;
        float AsymmetryMTopR08 = -1.;
        if (m_doReclustering) {

            xAOD::IParticleContainer JetsToRecluster(SG::VIEW_ELEMENTS);
            for (auto const &jet : *SignalJets)
                JetsToRecluster.push_back(jet);

            // for 1LCRs, add signal lepton to to the jet reclustering
            if (NLeps == 1) for (auto const &lep : SignalLeptons)
                JetsToRecluster.push_back(lep);

            ATH_MSG_DEBUG("Now recluster R=1.2 jets");
            ATH_CHECK(m_jet_selection->ReclusterJets(&JetsToRecluster, 1.2));
            ATH_CHECK(m_jet_selection->ReclusterJets(&JetsToRecluster, 1.2, m_CustomReclusterPt, "Pt35_"));

            ATH_MSG_DEBUG("Now recluster R=0.8 jets");
            ATH_CHECK(m_jet_selection->ReclusterJets(&JetsToRecluster, 0.8));
            ATH_CHECK(m_jet_selection->ReclusterJets(&JetsToRecluster, 0.8, m_CustomReclusterPt, "Pt35_"));

            // the reclustered jets can now be accessed using m_jet_selection->GetCustomJets("FatJet12") (the last 2 digits are the radius without a dot)
            N_FatJets12Pt35 = m_jet_selection->GetCustomJets("FatJet1.2")->size();
            xAOD::IParticle* leadingFatJet = XAMPP::FindLeadingParticle(m_jet_selection->GetCustomJets("FatJet1.2"));
            if (m_jet_selection->GetCustomJets("FatJet1.2")->size() > 1) {
                AsymmetryMTopR12 = (fabs(leadingFatJet->m() - m_jet_selection->GetCustomJets("FatJet1.2")->at(1)->m())) / (leadingFatJet->m() + m_jet_selection->GetCustomJets("FatJet1.2")->at(1)->m());
            }
            xAOD::IParticle* leadingFatJet08 = XAMPP::FindLeadingParticle(m_jet_selection->GetCustomJets("FatJet0.8"));
            if (m_jet_selection->GetCustomJets("FatJet0.8")->size() > 1) {
                AsymmetryMTopR08 = (fabs(leadingFatJet08->m() - m_jet_selection->GetCustomJets("FatJet0.8")->at(1)->m())) / (leadingFatJet08->m() + m_jet_selection->GetCustomJets("FatJet0.8")->at(1)->m());
            }
            ATH_MSG_DEBUG("Jet reclustering done");

        }

        // the following lines can be used for event based debugging
        // const xAOD::EventInfo* Info = m_XAMPPInfo->GetOrigInfo();
        //  if ((Info->eventNumber() == 81763)||(Info->eventNumber() == 119769)) {
        //  std::cout << "--------------------------------------" << std::endl;
        //  std::cout << "Event No. " << Info->eventNumber() << std::endl;
        //  static SG::AuxElement::Decorator<char> dec_baseline("baseline");
        //  static SG::AuxElement::Decorator<char> dec_bad("bad");
        //  static SG::AuxElement::Decorator<char> dec_bjet("bjet");
        //  static SG::AuxElement::Decorator<char> dec_passOR("passOR");
        //  static SG::AuxElement::Decorator<int> dec_NTrks("NTrks");
        //  static SG::AuxElement::Decorator<char> dec_signal("signal");
        //  static SG::AuxElement::Decorator<double> dec_MV2c20("MV2c20");
        //  static SG::AuxElement::Decorator<char> dec_cosmic("cosmic");
        //  for (auto const &jet : *m_jet_selection->GetJets()) {
        //  XAMPP::PromptParticle(jet, Form("PassOR: %d, isBad: %d, isB: %d, NTrks: %i, MV2c20: %f, isBaseline: %d, isSignal: %d", dec_passOR(*jet), dec_bad(*jet), dec_bjet(*jet), dec_NTrks(*jet), dec_MV2c20(*jet), dec_baseline(*jet), dec_signal(*jet)));
        //  }
        //  for (auto const &el : *m_electron_selection->GetBaselineElectrons()) {
        //  XAMPP::PromptParticle(el, Form("PassOR: %d, isBaseline: %d, isSignal: %d", dec_passOR(*el), dec_baseline(*el), dec_signal(*el)));
        //  }
        //  for (auto const &mu : *m_muon_selection->GetBaselineMuons()) {
        //  XAMPP::PromptParticle(mu, Form("PassOR: %d, isBaseline: %d, isSignal: %d, badMuon: %d, cosmic: %d", dec_passOR(*mu), dec_baseline(*mu), dec_signal(*mu), dec_bad(*mu), dec_cosmic(*mu)));
        //  }
        //  if (dec_XAMPPmetTST->GetValue()) {
        //  std::cout << "MetTST: met()=" << dec_XAMPPmetTST->GetValue()->met() << " phi()=" << dec_XAMPPmetTST->GetValue()->phi() << std::endl;
        //  }
        //  }

        int NBJets = m_jet_selection->GetBJets()->size();
        float Ht = XAMPP::CalculateJetHt(SignalJets);
        float LeptonHt = XAMPP::CalculateLeptonHt(m_electron_selection->GetSignalElectrons(), m_muon_selection->GetSignalMuons());
        float MtBMin = XAMPP::ComputeMtMin(m_jet_selection->GetBJets(), dec_XAMPPmetTST);
        float MtNonBMin = XAMPP::ComputeMtMin(m_jet_selection->GetLightJets(), dec_XAMPPmetTST);
        float MtJetMin = XAMPP::ComputeMtMin(SignalJets, dec_XAMPPmetTST);
        float MtBMax = XAMPP::ComputeMtMax(m_jet_selection->GetBJets(), dec_XAMPPmetTST);
        float MtBMinLep = XAMPP::ComputeMtMin(m_jet_selection->GetBJets(), dec_XAMPPmetLepInv);
        float MtBMaxLep = XAMPP::ComputeMtMax(m_jet_selection->GetBJets(), dec_XAMPPmetLepInv);
        float MtNonBMax = XAMPP::ComputeMtMax(m_jet_selection->GetLightJets(), dec_XAMPPmetTST);
        float MtJetMax = XAMPP::ComputeMtMax(SignalJets, dec_XAMPPmetTST);
        xAOD::IParticle *BJet1(NULL), *BJet2(NULL);
        if (m_jet_selection->GetBJets()->size() > 0) BJet1 = m_jet_selection->GetBJets()->at(0);
        if (m_jet_selection->GetBJets()->size() > 1) BJet2 = m_jet_selection->GetBJets()->at(1);

        float MtMetLep(-1.), dRB1Lep(-1.), dRB2Lep(-1.), dRBLepMin(-1.);
        xAOD::IParticle* LeadingLep = XAMPP::FindLeadingParticle(&SignalLeptons);
        if (LeadingLep) {
            MtMetLep = XAMPP::ComputeMt(*LeadingLep, dec_XAMPPmetTST);
            if (BJet1) dRB1Lep = xAOD::P4Helpers::deltaR(LeadingLep, BJet1);
            if (BJet1 && BJet2) {
                dRB2Lep = xAOD::P4Helpers::deltaR(LeadingLep, BJet2);
                dRBLepMin = std::min(dRB1Lep, dRB2Lep);
            }
        }
        ATH_CHECK(dec_MtLepMet->Store(MtMetLep));
        ATH_CHECK(dec_dRB1L->Store(dRB1Lep));
        ATH_CHECK(dec_dRB2L->Store(dRB2Lep));
        ATH_CHECK(dec_dRBLMin->Store(dRBLepMin));

        float dRBB(-1.);
        if (BJet1 && BJet2) dRBB = xAOD::P4Helpers::deltaR(BJet1, BJet2);
        ATH_CHECK(dec_dRBB->Store(dRBB));

        float DeltaPhiMin2 = ComputeDPhiMin(SignalJets, dec_XAMPPmetTST, 2);
        if (SignalJets->size() && DeltaPhiMin2 == -FLT_MAX) return StatusCode::FAILURE;
        ATH_CHECK(dec_AbsDeltaPhiMin2->Store(fabs(DeltaPhiMin2)));
        ATH_CHECK(dec_DeltaPhiMin2->Store(DeltaPhiMin2));

        float DeltaPhiMin4 = ComputeDPhiMin(SignalJets, dec_XAMPPmetTST, 4);
        if (SignalJets->size() && DeltaPhiMin4 == -FLT_MAX) return StatusCode::FAILURE;
        ATH_CHECK(dec_AbsDeltaPhiMin4->Store(fabs(DeltaPhiMin4)));

        float DPhiMetTrackMet = -1.;
        if (m_doTrackMet) DPhiMetTrackMet = XAMPP::ComputeDeltaPhi(dec_XAMPPmetTST, m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetTrack"));
        float MtTauCand = -1.;
        int NtracksTauCand = -1.;
        bool HasTauCand = XAMPP::HasTauCandidate(SignalJets, dec_XAMPPmetTST->GetValue(), MtTauCand, NtracksTauCand);

        float HtSig = (dec_XAMPPmetTST->GetValue()->met()) * XAMPP::MeVToGeV / (TMath::Sqrt(Ht * XAMPP::MeVToGeV));
        float MinMt = XAMPP::ComputeMinMt(SignalJets, dec_XAMPPmetTST->GetValue());

        bool IsOS = false;
        bool IsSF = false;
        float Mll = -1.;
        if (NLeps > 1) {
            IsOS = XAMPP::OppositeSign(SignalLeptons.at(0), SignalLeptons.at(1));
            IsSF = XAMPP::SameFlavour(SignalLeptons.at(0), SignalLeptons.at(1));
            Mll = XAMPP::InvariantMass(SignalLeptons.at(0), SignalLeptons.at(1));
        }
        float ZWindow = fabs(Mll - 91.0e3);

        float jetPtResoTot = 0.;
        for (const auto& ijet : *SignalJets) {
            float jreso = m_jerTool->getRelResolutionData(ijet);
            jetPtResoTot += (ijet->pt() * jreso) * (ijet->pt() * jreso);
            ATH_MSG_DEBUG("Jet pt = " << ijet->pt() << " MeV, relative resolution = " << jreso);

        }
        jetPtResoTot = sqrt(jetPtResoTot);

        float MetSigJetReso = (dec_XAMPPmetTST->GetValue()->met() / jetPtResoTot);
        float MetSig200JetReso = ((dec_XAMPPmetTST->GetValue()->met() - 200000.) / jetPtResoTot);

        float mt2(-1), mt2_AKt08(-1), mt2_AKt12(-1), mt2_dRBMin(-1), mt2_chi2(-1);
        if (!std::isnan(dec_XAMPPmetTST->GetValue()->met())) {
            ATH_MSG_DEBUG("Starting MT2 calculation...");
            static SG::AuxElement::Decorator<char> dec_WellDefined("WellDefined");
            if (NBJets > 1) {
                xAOD::ParticleContainer* topCandidates = m_ParticleConstructor->GetSubContainer("TopCandidates");
                static SG::AuxElement::Decorator<int> dec_RecoType("RecoType");
                static SG::AuxElement::Decorator<float> dec_Chi2("Chi2");
                xAOD::IParticle* TopCandChi2_1 = NULL;
                xAOD::IParticle* TopCandChi2_2 = NULL;
                for (auto const &part : *topCandidates) {
                    if (!dec_WellDefined(*part)) continue;
                    if (part->pdgId() != 6) continue;
                    if (dec_RecoType(*part) == XAMPP::Stop_RecoTypes::Chi2TopWCandidate1) {
                        if (TopCandChi2_1) ATH_MSG_WARNING("2 tops?!?");
                        TopCandChi2_1 = part;
                    }
                    if (dec_RecoType(*part) == XAMPP::Stop_RecoTypes::Chi2TopWCandidate2) TopCandChi2_2 = part;
                }
                if (TopCandChi2_1 && TopCandChi2_2) {
                    mt2_chi2 = dec_Chi2(*TopCandChi2_1) + dec_Chi2(*TopCandChi2_2);
                    ATH_MSG_DEBUG("- Starting Chi2-MT2 calculation...");
                    mt2 = CalculateMT2(*TopCandChi2_1,*TopCandChi2_2,dec_XAMPPmetTST, 173210);
                }
            }
            xAOD::JetContainer *FatJetsR08 = m_jet_selection->GetCustomJets("FatJet0.8");
            if (FatJetsR08->size() > 1) {
                ATH_MSG_DEBUG("- Starting FatJetsR08-MT2 calculation...");
                mt2_AKt08 = CalculateMT2(*FatJetsR08->at(0),*FatJetsR08->at(1),dec_XAMPPmetTST);
            }
            xAOD::JetContainer *FatJetsR12 = m_jet_selection->GetCustomJets("FatJet1.2");
            if (FatJetsR12->size() > 1) {
                ATH_MSG_DEBUG("- Starting FatJetsR12-MT2 calculation...");
                mt2_AKt12 = CalculateMT2(*FatJetsR12->at(0),*FatJetsR12->at(1),dec_XAMPPmetTST);
            }
            if (dec_WellDefined(*Top1Cand) && dec_WellDefined(*Top2Cand)) {
                ATH_MSG_DEBUG("- Starting TopCand-MT2 calculation...");
                mt2_dRBMin = CalculateMT2(*Top1Cand,*Top2Cand,dec_XAMPPmetTST);
            }
            ATH_MSG_DEBUG("MT2 calculation done.");
        }

        ATH_MSG_DEBUG("Starting TruthTopDecays...");
        int TruthTopDecays = 0;
        if (m_doWDecays) {
            int TruthWDecay = 0;
            int had = 0;
            int lep = 0;
            int taulep = 0;
            int tauhad = 0;
            if (doTruth()) {
                for (auto const &truth_part : *m_truth_selection->GetTruthParticles()) {
                    TruthWDecay = fabs(m_truth_selection->classifyWDecays(truth_part));
                    if (TruthWDecay == 1) had++;
                    if (TruthWDecay == 2 || TruthWDecay == 3) lep++;
                    if (TruthWDecay == 4) taulep++;
                    if (TruthWDecay == 5) tauhad++;
                }
            }
            if (had == 2) TruthTopDecays = 1;
            if (had == 1 && tauhad == 1) TruthTopDecays = 2;
            if (had == 1 && (taulep == 1 || lep == 1)) TruthTopDecays = 3;
            if (tauhad == 2) TruthTopDecays = 4;
            if (lep == 1 && tauhad == 1) TruthTopDecays = 5;
            if (lep == 2 || (lep == 1 && taulep == 1) || taulep == 2) TruthTopDecays = 6;
        }
        ATH_MSG_DEBUG("TruthTopDecays done.");

        //Finally store the variables they are then used by the Cut Class or just written out into the trees
        ATH_CHECK(dec_NBJets->Store(NBJets));
        ATH_CHECK(dec_NLeps->Store(NLeps));
        ATH_CHECK(dec_NBaseLeps->Store(NBaseLeps));
        ATH_CHECK(dec_NJets->Store(NJets));
        ATH_CHECK(dec_MtBMin->Store(MtBMin));
        ATH_CHECK(dec_MtNonBMin->Store(MtNonBMin));
        ATH_CHECK(dec_MtJetMin->Store(MtJetMin));
        ATH_CHECK(dec_MtBMax->Store(MtBMax));
        ATH_CHECK(dec_MtBMinLep->Store(MtBMinLep));
        ATH_CHECK(dec_MtBMaxLep->Store(MtBMaxLep));
        ATH_CHECK(dec_MtNonBMax->Store(MtNonBMax));
        ATH_CHECK(dec_MtJetMax->Store(MtJetMax));
        ATH_CHECK(dec_DPhiMetTrackMet->Store(DPhiMetTrackMet));
        ATH_CHECK(dec_AbsDPhiMetTrackMet->Store(fabs(DPhiMetTrackMet)));
        ATH_CHECK(dec_HasTauCand->Store(HasTauCand));
        ATH_CHECK(dec_MtTauCand->Store(MtTauCand));
        ATH_CHECK(dec_NtracksTauCand->Store(NtracksTauCand));
        ATH_CHECK(dec_N_FatJets12Pt35->Store(N_FatJets12Pt35));
        ATH_CHECK(dec_AsymmetryMTopR12->Store(AsymmetryMTopR12));
        ATH_CHECK(dec_AsymmetryMTopR08->Store(AsymmetryMTopR08));
        ATH_CHECK(dec_HtSig->Store(HtSig));
        ATH_CHECK(dec_MinMt->Store(MinMt));
        ATH_CHECK(dec_IsOS->Store(IsOS));
        ATH_CHECK(dec_IsSF->Store(IsSF));
        ATH_CHECK(dec_Mll->Store(Mll));
        ATH_CHECK(dec_HT->Store(Ht));
        ATH_CHECK(dec_Meff->Store(Ht + LeptonHt + dec_XAMPPmetTST->GetValue()->met()));
        ATH_CHECK(dec_ZWindow->Store(ZWindow));

        ATH_CHECK(dec_TotResoJets->Store(jetPtResoTot));
        ATH_CHECK(dec_MetSigJetReso->Store(MetSigJetReso));
        ATH_CHECK(dec_MetSig200JetReso->Store(MetSig200JetReso));

        ATH_CHECK(dec_mt2->Store(mt2));
        ATH_CHECK(dec_mt2_chi2->Store(mt2_chi2));
        ATH_CHECK(dec_mt2_AKt08->Store(mt2_AKt08));
        ATH_CHECK(dec_mt2_AKt12->Store(mt2_AKt12));
        ATH_CHECK(dec_mt2_dRBMin->Store(mt2_dRBMin));
        if (m_doWDecays) ATH_CHECK(dec_TruthTopDecays->Store(TruthTopDecays));

        if (m_doISR) {
            ATH_CHECK(m_ISR_selection->ComputeISREventVariables(SignalJets, m_electron_selection->GetSignalElectrons(), m_muon_selection->GetSignalMuons(), dec_XAMPPmetTST));
            if (doTruth() && m_XAMPPInfo->GetSystematic() == m_systematics->GetNominal()) {
                static XAMPP::Storage<XAMPPmet>* dec_TruthMet = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("TruthMET");
                ATH_CHECK(m_ISR_selection_truth->ComputeISREventVariables(m_truth_selection->GetTruthSignalJets(), m_truth_selection->GetTruthSignalElectrons(), m_truth_selection->GetTruthSignalMuons(), dec_TruthMet));
            }
        }

        // fill the particle containers for the output
        static XAMPP::ParticleStorage* ElectronStore = m_XAMPPInfo->GetParticleStorage("Elec");
        ATH_CHECK(ElectronStore->Fill(m_electron_selection->GetSignalElectrons()));
        static XAMPP::ParticleStorage* MuonStore = m_XAMPPInfo->GetParticleStorage("Muon");
        ATH_CHECK(MuonStore->Fill(m_muon_selection->GetSignalMuons()));
        static XAMPP::ParticleStorage* JetStore = m_XAMPPInfo->GetParticleStorage("Jet");
        ATH_CHECK(JetStore->Fill(SignalJets));
        static XAMPP::ParticleStorage* RecoStore = m_XAMPPInfo->GetParticleStorage("RecoCandidates");
        ATH_CHECK(RecoStore->Fill(m_ParticleConstructor->GetContainer()));

        if (m_doReclustering) {
            static XAMPP::ParticleStorage* FatJet08Store = m_XAMPPInfo->GetParticleStorage("FatJet08");
            ATH_CHECK(FatJet08Store->Fill(m_jet_selection->GetCustomJets("FatJet0.8")));
            static XAMPP::ParticleStorage* Pt35_FatJet08Store = m_XAMPPInfo->GetParticleStorage("Pt35_FatJet08");
            ATH_CHECK(Pt35_FatJet08Store->Fill(m_jet_selection->GetCustomJets("Pt35_FatJet0.8")));
            static XAMPP::ParticleStorage* FatJet12Store = m_XAMPPInfo->GetParticleStorage("FatJet12");
            ATH_CHECK(FatJet12Store->Fill(m_jet_selection->GetCustomJets("FatJet1.2")));
            static XAMPP::ParticleStorage* Pt35_FatJet12Store = m_XAMPPInfo->GetParticleStorage("Pt35_FatJet12");
            ATH_CHECK(Pt35_FatJet12Store->Fill(m_jet_selection->GetCustomJets("Pt35_FatJet1.2")));
        }

        if (doTruth() && m_XAMPPInfo->GetSystematic() == m_systematics->GetNominal()) {
            XAMPP::ParticleStorage* TruthStore = m_XAMPPInfo->GetParticleStorage("TruthParticles");
            ATH_CHECK(TruthStore->Fill(m_truth_selection->GetTruthParticles()));
        }

        return StatusCode::SUCCESS;
    }

    bool Stop0LAnalysisHelper::AcceptEvent() {
        // check MetTrigger from SUSYTools
        static XAMPP::Storage<char>* dec_IsMETTrigPassed = m_XAMPPInfo->GetVariableStorage<char>("IsMETTrigPassed");
        ATH_CHECK(dec_IsMETTrigPassed->ConstStore(m_susytools->IsMETTrigPassed()));
        // check MET200 sample overlap
        static XAMPP::Storage<char>* dec_isMetSampleOR = m_XAMPPInfo->GetVariableStorage<char>("isMetSampleOR");
        // check SUSYTools treatAsYear
        static XAMPP::Storage<int>* dec_treatAsYear = m_XAMPPInfo->GetVariableStorage<int>("treatAsYear");
        ATH_CHECK(dec_treatAsYear->ConstStore(m_susytools->treatAsYear()));

        if (isData()) {
            ATH_CHECK(dec_isMetSampleOR->ConstStore(false));
            return true;
        }

        // METFilterDuplicateCut:
        bool isMetSampleOR = false;
        int mcChannelNumber = m_XAMPPInfo->mcChannelNumber();
        static XAMPP::Storage<float>* dec_GenFiltMET = m_XAMPPInfo->GetVariableStorage<float>("GenFiltMET");
        if ((mcChannelNumber == 410000) && (dec_GenFiltMET->GetValue() > 200000.)) isMetSampleOR = true;
        if ((mcChannelNumber == 410013) && (dec_GenFiltMET->GetValue() > 200000.)) isMetSampleOR = true;
        if ((mcChannelNumber == 410014) && (dec_GenFiltMET->GetValue() > 200000.)) isMetSampleOR = true;
        if ((mcChannelNumber == 410001) && (dec_GenFiltMET->GetValue() > 200000.)) isMetSampleOR = true;
        if ((mcChannelNumber == 410002) && (dec_GenFiltMET->GetValue() > 200000.)) isMetSampleOR = true;
        if ((mcChannelNumber == 410004) && (dec_GenFiltMET->GetValue() > 200000.)) isMetSampleOR = true;
        ATH_CHECK(dec_isMetSampleOR->ConstStore(isMetSampleOR));
        if (isMetSampleOR) {
            ATH_MSG_DEBUG("Reject the event: " << m_XAMPPInfo->eventNumber() << " for DSID: " << m_XAMPPInfo->mcChannelNumber() << " as it has a GenFiltMET:" << dec_GenFiltMET->GetValue());
            return false;
        }

        static XAMPP::Storage<double>* dec_SherpaVjetsNjetsWeight = m_XAMPPInfo->GetVariableStorage<double>("SherpaVjetsNjetsWeight");
        float SherpaVjetsNjetsWeight = 1.;
        if (((mcChannelNumber >= 363102) && (mcChannelNumber <= 363122)) || ((mcChannelNumber >= 363331) && (mcChannelNumber <= 363483))) SherpaVjetsNjetsWeight = m_susytools->getSherpaVjetsNjetsWeight();
        ATH_CHECK(dec_SherpaVjetsNjetsWeight->ConstStore(SherpaVjetsNjetsWeight));

        return true;
    }

    Stop0LGammaAnalysisHelper::Stop0LGammaAnalysisHelper(std::string myname) :
                Stop0LAnalysisHelper(myname) {
    }
    StatusCode Stop0LGammaAnalysisHelper::initialize() {
        ATH_MSG_INFO("Initialising Stop0LGamma AnalysisHelper...");
        ATH_CHECK(Stop0LAnalysisHelper::initialize());
        return StatusCode::SUCCESS;
    }
    double Stop0LGammaAnalysisHelper::GetTotalWeight() {
        double w = Stop0LAnalysisHelper::GetTotalWeight();
        static XAMPP::Storage<double>* dec_PhotWeight = m_XAMPPInfo->GetVariableStorage<double>("PhoWeight");
        if (dec_PhotWeight) w *= dec_PhotWeight->GetValue();
        return w;
    }
    StatusCode Stop0LGammaAnalysisHelper::initializeEventVariables() {
        ATH_CHECK(Stop0LAnalysisHelper::initializeEventVariables());
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_SignalPhotons"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_BaselinePhotons", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtBMinPhoton"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtBMaxPhoton"));
        // Now add the branches for the particle containers
        ATH_CHECK(m_XAMPPInfo->BookParticleStorage("Phot"));
        return StatusCode::SUCCESS;
    }
    StatusCode Stop0LGammaAnalysisHelper::ComputeEventVariables() {
        ATH_CHECK(Stop0LAnalysisHelper::ComputeEventVariables());
        static XAMPP::Storage<int>* dec_NPhotons = m_XAMPPInfo->GetVariableStorage<int>("N_SignalPhotons");
        static XAMPP::Storage<int>* dec_NBasePhotons = m_XAMPPInfo->GetVariableStorage<int>("N_BaselinePhotons");
        static XAMPP::Storage<float>* dec_MtBMinPhoton = m_XAMPPInfo->GetVariableStorage<float>("MtBMinPhoton");
        static XAMPP::Storage<float>* dec_MtBMaxPhoton = m_XAMPPInfo->GetVariableStorage<float>("MtBMaxPhoton");
        int NPhotons = m_photon_selection->GetSignalPhotons()->size();
        int NBasePhotons = m_photon_selection->GetBaselinePhotons()->size();
        xAOD::IParticle* leadingPhot = XAMPP::FindLeadingParticle(m_photon_selection->GetSignalPhotons());
        float MtBMinPhoton(-1), MtBMaxPhoton(-1);
        if (leadingPhot) MtBMinPhoton = XAMPP::ComputeMtMin(m_jet_selection->GetBJets(), leadingPhot);
        if (leadingPhot) MtBMaxPhoton = XAMPP::ComputeMtMax(m_jet_selection->GetBJets(), leadingPhot);
        ATH_CHECK(dec_NPhotons->Store(NPhotons));
        ATH_CHECK(dec_NBasePhotons->Store(NBasePhotons));
        ATH_CHECK(dec_MtBMinPhoton->Store(MtBMinPhoton));
        ATH_CHECK(dec_MtBMaxPhoton->Store(MtBMaxPhoton));
        // fill the particle containers for the output
        static XAMPP::ParticleStorage* PhotonStore = m_XAMPPInfo->GetParticleStorage("Phot");
        ATH_CHECK(PhotonStore->Fill(m_photon_selection->GetSignalPhotons()));
        return StatusCode::SUCCESS;
    }

}
