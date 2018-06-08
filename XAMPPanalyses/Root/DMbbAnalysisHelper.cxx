#include <XAMPPanalyses/DMbbAnalysisHelper.h>
#include <XAMPPanalyses/DMbbAnalysisUtils.h>

#include <JetResolution/JERTool.h>
#include <JetResolution/JERSmearingTool.h>

namespace XAMPP {
    DMbbAnalysisHelper::DMbbAnalysisHelper(std::string myname) :
            SUSYAnalysisHelper(myname),
            m_jerTool("") {
    }
    StatusCode DMbbAnalysisHelper::initializeObjectTools() {
        ATH_CHECK(SUSYAnalysisHelper::initializeObjectTools());
        return StatusCode::SUCCESS;
    }
    StatusCode DMbbAnalysisHelper::initialize() {
        ATH_MSG_INFO("Initialising DMbb AnalysisHelper...");

        ATH_CHECK(SUSYAnalysisHelper::initialize());
        //Retrieve the CP Tool directly from SUSYTools
        m_jerTool = GetCPTool <  IJERTool > ("JERTool");

        return StatusCode::SUCCESS;
    }

    StatusCode DMbbAnalysisHelper::initializeEventVariables() {

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
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtNonBMax"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtJetMax"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtLepMet"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("BJet1Pt", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("JetImbalance"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("AngularJetSep"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("PseudoRapiditySep"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Jet1Pt", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Jet2Pt", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Jet3Pt", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Jet4Pt", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("AbsDeltaPhiMin2", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("AbsDeltaPhiMin3", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("AbsDeltaPhiMinJetsMeT", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("DeltaPhiMin2", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("DeltaPhiMin3", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("DeltaPhiMinJetsMeT"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("DPhiMetTrackMet"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("AbsDPhiMetTrackMet", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<char>("HasTauCand"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtTauCand"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("NtracksTauCand"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("HtSig"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MinMt"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Top1CandM", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Top2CandM", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<char>("IsOS", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<char>("IsSF", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Mll"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("HT"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Meff"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("ZWindow", false));
        if (!isData()) {
            ATH_CHECK(m_XAMPPInfo->NewEventVariable<double>("TotalWeight"));
            ATH_CHECK(m_XAMPPInfo->NewEventVariable<double>("SherpaVjetsNjetsWeight"));
            ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("GenFiltMET"));
            ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("treatAsYear"));
        }

        //variables to have the correct jet resolution and MetSignificance with it
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("TotResoJets"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MetSigJetReso"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MetSig200JetReso"));

        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("dRB1L"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("dRB2L"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("dRBLMin"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("dRBB"));

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
        //if (m_doISR) ATH_CHECK(JetStore->SaveChar("isISR_V"));
        if (!isData()) ATH_CHECK(JetStore->SaveInteger("ConeTruthLabelID"));

        // for RecoCandidates, save trees with (eta, phi, pt, m) -> set StoreMass=true
        ATH_CHECK(m_XAMPPInfo->BookParticleStorage("RecoCandidates", true));
        XAMPP::ParticleStorage* RecoStore = m_XAMPPInfo->GetParticleStorage("RecoCandidates");
        ATH_CHECK(RecoStore->SaveFloat("charge"));
        ATH_CHECK(RecoStore->SaveInteger("pdgId"));
        ATH_CHECK(RecoStore->SaveInteger("RecoType"));

        if (doTruth()) {
            ATH_CHECK(m_XAMPPInfo->BookParticleStorage("TruthParticles", false, false));
            XAMPP::ParticleStorage* TruthStore = m_XAMPPInfo->GetParticleStorage("TruthParticles");
            ATH_CHECK(TruthStore->SaveFloat("charge"));
            ATH_CHECK(TruthStore->SaveInteger("pdgId"));
        }

        return StatusCode::SUCCESS;
    }

    double DMbbAnalysisHelper::GetTotalWeight() {
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

    StatusCode DMbbAnalysisHelper::ComputeEventVariables() {

        //First lets define the Pointer to the Storage elements
        static XAMPP::Storage<int>* dec_NBJets = m_XAMPPInfo->GetVariableStorage<int>("N_BJets");
        static XAMPP::Storage<int>* dec_NLeps = m_XAMPPInfo->GetVariableStorage<int>("N_SignalLeptons");
        static XAMPP::Storage<int>* dec_NBaseLeps = m_XAMPPInfo->GetVariableStorage<int>("N_BaselineLeptons");
        static XAMPP::Storage<int>* dec_NJets = m_XAMPPInfo->GetVariableStorage<int>("N_Jets");
        static XAMPP::Storage<float>* dec_MtBMin = m_XAMPPInfo->GetVariableStorage<float>("MtBMin");
        static XAMPP::Storage<float>* dec_MtNonBMin = m_XAMPPInfo->GetVariableStorage<float>("MtNonBMin");
        static XAMPP::Storage<float>* dec_MtJetMin = m_XAMPPInfo->GetVariableStorage<float>("MtJetMin");
        static XAMPP::Storage<float>* dec_MtBMax = m_XAMPPInfo->GetVariableStorage<float>("MtBMax");
        static XAMPP::Storage<float>* dec_MtNonBMax = m_XAMPPInfo->GetVariableStorage<float>("MtNonBMax");
        static XAMPP::Storage<float>* dec_MtJetMax = m_XAMPPInfo->GetVariableStorage<float>("MtJetMax");
        static XAMPP::Storage<float>* dec_MtLepMet = m_XAMPPInfo->GetVariableStorage<float>("MtLepMet");
        static XAMPP::Storage<float>* dec_Jet1Pt = m_XAMPPInfo->GetVariableStorage<float>("Jet1Pt");
        static XAMPP::Storage<float>* dec_Jet2Pt = m_XAMPPInfo->GetVariableStorage<float>("Jet2Pt");
        static XAMPP::Storage<float>* dec_Jet3Pt = m_XAMPPInfo->GetVariableStorage<float>("Jet3Pt");
        static XAMPP::Storage<float>* dec_Jet4Pt = m_XAMPPInfo->GetVariableStorage<float>("Jet4Pt");
        static XAMPP::Storage<float>* dec_AbsDeltaPhiMin2 = m_XAMPPInfo->GetVariableStorage<float>("AbsDeltaPhiMin2");
        static XAMPP::Storage<float>* dec_AbsDeltaPhiMin3 = m_XAMPPInfo->GetVariableStorage<float>("AbsDeltaPhiMin3");
        static XAMPP::Storage<float>* dec_AbsDeltaPhiMinJetsMeT = m_XAMPPInfo->GetVariableStorage<float>("AbsDeltaPhiMinJetsMeT");
        static XAMPP::Storage<float>* dec_DeltaPhiMin2 = m_XAMPPInfo->GetVariableStorage<float>("DeltaPhiMin2");
        static XAMPP::Storage<float>* dec_DeltaPhiMin3 = m_XAMPPInfo->GetVariableStorage<float>("DeltaPhiMin3");
        static XAMPP::Storage<float>* dec_DeltaPhiMinJetsMeT = m_XAMPPInfo->GetVariableStorage<float>("DeltaPhiMinJetsMeT");
        static XAMPP::Storage<float>* dec_DPhiMetTrackMet = m_XAMPPInfo->GetVariableStorage<float>("DPhiMetTrackMet");
        static XAMPP::Storage<float>* dec_AbsDPhiMetTrackMet = m_XAMPPInfo->GetVariableStorage<float>("AbsDPhiMetTrackMet");
        static XAMPP::Storage<char>* dec_HasTauCand = m_XAMPPInfo->GetVariableStorage<char>("HasTauCand");
        static XAMPP::Storage<float>* dec_MtTauCand = m_XAMPPInfo->GetVariableStorage<float>("MtTauCand");
        static XAMPP::Storage<int>* dec_NtracksTauCand = m_XAMPPInfo->GetVariableStorage<int>("NtracksTauCand");
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
	static XAMPP::Storage<float>* dec_BJet1Pt = m_XAMPPInfo->GetVariableStorage<float>("BJet1Pt");
	static XAMPP::Storage<float>* dec_JetImbalance = m_XAMPPInfo->GetVariableStorage<float>("JetImbalance");
	static XAMPP::Storage<float>* dec_AngularJetSep = m_XAMPPInfo->GetVariableStorage<float>("AngularJetSep");
	static XAMPP::Storage<float>* dec_PseudoRapiditySep = m_XAMPPInfo->GetVariableStorage<float>("PseudoRapiditySep");
        static XAMPP::Storage<XAMPPmet>* dec_XAMPPmetTST = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetTST");

        if (!isData()) {
            static XAMPP::Storage<double>* dec_TotalWeight = m_XAMPPInfo->GetVariableStorage<double>("TotalWeight");
            ATH_CHECK(dec_TotalWeight->Store(GetTotalWeight()));
        }

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

        // Then lets calculate our event variables... there are lots of functions in the AnalysisUtils in order to do that, feel free to add other functions for this purpose
        int NLeps = m_electron_selection->GetSignalElectrons()->size() + m_muon_selection->GetSignalMuons()->size();
        int NBaseLeps = m_electron_selection->GetBaselineElectrons()->size() + m_muon_selection->GetBaselineMuons()->size();
        int NJets = m_jet_selection->GetSignalJets()->size();

        int NBJets = m_jet_selection->GetBJets()->size();
        float Ht = XAMPP::CalculateJetHt(m_jet_selection->GetSignalJets());
        float LeptonHt = XAMPP::CalculateLeptonHt(m_electron_selection->GetSignalElectrons(), m_muon_selection->GetSignalMuons());
        float MtBMin = XAMPP::ComputeMtMin(m_jet_selection->GetBJets(), dec_XAMPPmetTST);
        float MtNonBMin = XAMPP::ComputeMtMin(m_jet_selection->GetLightJets(), dec_XAMPPmetTST);
        float MtJetMin = XAMPP::ComputeMtMin(m_jet_selection->GetSignalJets(), dec_XAMPPmetTST);
        float MtBMax = XAMPP::ComputeMtMax(m_jet_selection->GetBJets(), dec_XAMPPmetTST);
        float MtNonBMax = XAMPP::ComputeMtMax(m_jet_selection->GetLightJets(), dec_XAMPPmetTST);
        float MtJetMax = XAMPP::ComputeMtMax(m_jet_selection->GetSignalJets(), dec_XAMPPmetTST);
        xAOD::IParticle *BJet1(NULL), *BJet2(NULL);
        if (m_jet_selection->GetBJets()->size() > 0) BJet1 = m_jet_selection->GetBJets()->at(0);
        if (m_jet_selection->GetBJets()->size() > 1) BJet2 = m_jet_selection->GetBJets()->at(1);

        float MtElMet(-1.), MtMuMet(-1.), dRBElMin(-1.), dRBMuMin(-1.), dRB1El(-1.), dRB1Mu(-1.), dRB2El(-1.), dRB2Mu(-1.);
        xAOD::IParticle* leadingEl = XAMPP::FindLeadingParticle(m_electron_selection->GetSignalElectrons());
        if (leadingEl) {
            MtElMet = XAMPP::ComputeMt(*leadingEl, dec_XAMPPmetTST);
            if (BJet1) dRB1El = xAOD::P4Helpers::deltaR(leadingEl, BJet1);
            if (BJet1 && BJet2) {
                dRB2El = xAOD::P4Helpers::deltaR(leadingEl, BJet2);
                dRBElMin = std::min(dRB1El, dRB2El);
            }
        }
        xAOD::IParticle* leadingMu = XAMPP::FindLeadingParticle(m_muon_selection->GetSignalMuons());
        if (leadingMu) {
            MtMuMet = XAMPP::ComputeMt(*leadingMu, dec_XAMPPmetTST);
            if (BJet1) dRB1Mu = xAOD::P4Helpers::deltaR(leadingMu, BJet1);
            if (BJet1 && BJet2) {
                dRB2Mu = xAOD::P4Helpers::deltaR(leadingMu, BJet2);
                dRBMuMin = std::min(dRB1Mu, dRB2Mu);
            }
        }
        if (MtElMet != -1) {
            ATH_CHECK(dec_MtLepMet->Store(MtElMet));
            ATH_CHECK(dec_dRB1L->Store(dRB1El));
            ATH_CHECK(dec_dRB2L->Store(dRB2El));
            ATH_CHECK(dec_dRBLMin->Store(dRBElMin));
        } else if (MtMuMet != -1) {
            ATH_CHECK(dec_MtLepMet->Store(MtMuMet));
            ATH_CHECK(dec_dRB1L->Store(dRB1Mu));
            ATH_CHECK(dec_dRB2L->Store(dRB2Mu));
            ATH_CHECK(dec_dRBLMin->Store(dRBMuMin));
        } else {
            ATH_CHECK(dec_MtLepMet->Store(-1));
            ATH_CHECK(dec_dRB1L->Store(-1));
            ATH_CHECK(dec_dRB2L->Store(-1));
            ATH_CHECK(dec_dRBLMin->Store(-1));
        }

        float dRBB(-1.);
        if (BJet1 && BJet2) dRBB = xAOD::P4Helpers::deltaR(BJet1, BJet2);
        ATH_CHECK(dec_dRBB->Store(dRBB));

        float BJet1Pt = -1.;   
        xAOD::IParticle* leadingBJet = XAMPP::FindLeadingParticle(m_jet_selection->GetBJets());
        if (leadingBJet) BJet1Pt = leadingBJet->pt();   

        float Jet1Pt = -1.;
        xAOD::IParticle* leadingJet = XAMPP::FindLeadingParticle(m_jet_selection->GetSignalJets());
        if (leadingJet) Jet1Pt = leadingJet->pt();
        float Jet2Pt = -1.;
        if (m_jet_selection->GetSignalJets()->size() > 1) Jet2Pt = m_jet_selection->GetSignalJets()->at(1)->pt();
        float Jet3Pt = -1.;
        if (m_jet_selection->GetSignalJets()->size() > 2) Jet3Pt = m_jet_selection->GetSignalJets()->at(2)->pt();
        float Jet4Pt = -1.;
        if (m_jet_selection->GetSignalJets()->size() > 3) Jet4Pt = m_jet_selection->GetSignalJets()->at(3)->pt();

        float DeltaPhiMin2 = ComputeDPhiMin(m_jet_selection->GetSignalJets(), dec_XAMPPmetTST, 2);
        if (m_jet_selection->GetSignalJets()->size() && DeltaPhiMin2 == -FLT_MAX) return StatusCode::FAILURE;
        ATH_CHECK(dec_AbsDeltaPhiMin2->Store(fabs(DeltaPhiMin2)));
        ATH_CHECK(dec_DeltaPhiMin2->Store(DeltaPhiMin2));

        float DeltaPhiMin3 = ComputeDPhiMin(m_jet_selection->GetSignalJets(), dec_XAMPPmetTST, 3);
        if (m_jet_selection->GetSignalJets()->size() && DeltaPhiMin3 == -FLT_MAX) return StatusCode::FAILURE;
        ATH_CHECK(dec_AbsDeltaPhiMin3->Store(fabs(DeltaPhiMin3)));
        ATH_CHECK(dec_DeltaPhiMin3->Store(DeltaPhiMin3));

        float DeltaPhiMinJetsMeT =-1.;
	if (m_jet_selection->GetSignalJets()->size() == 2) DeltaPhiMinJetsMeT = ComputeDPhiMin(m_jet_selection->GetSignalJets(), dec_XAMPPmetTST, 2);
	if (m_jet_selection->GetSignalJets()->size() == 3) DeltaPhiMinJetsMeT = ComputeDPhiMin(m_jet_selection->GetSignalJets(), dec_XAMPPmetTST, 3);
        if (m_jet_selection->GetSignalJets()->size() && DeltaPhiMin3 == -FLT_MAX) return StatusCode::FAILURE;
        ATH_CHECK(dec_AbsDeltaPhiMinJetsMeT->Store(fabs(DeltaPhiMinJetsMeT)));
        ATH_CHECK(dec_DeltaPhiMinJetsMeT->Store(DeltaPhiMinJetsMeT));

	float JetImbalance =-1.;
	if (m_jet_selection->GetSignalJets()->size() == 2) JetImbalance = ( (Jet1Pt-Jet2Pt)/(Jet1Pt+Jet2Pt));
	if (m_jet_selection->GetSignalJets()->size() ==3) {
	//m_jet_selection->GetSignalJets()->sort(XAMPP::btagweightsorter); 
	Jet1Pt = m_jet_selection->GetSignalJets()->at(0)->pt();
	Jet2Pt = m_jet_selection->GetSignalJets()->at(1)->pt();
	JetImbalance = ( (Jet1Pt-Jet2Pt)/(Jet1Pt+Jet2Pt));
	}
	ATH_CHECK(dec_JetImbalance->Store(JetImbalance));

	float AngularJetSep =-1.;
	AngularJetSep = ComputeJetAngularSep(m_jet_selection->GetSignalJets());
	ATH_CHECK(dec_AngularJetSep->Store(AngularJetSep));

	float PseudoRapiditySep =-1.;
	if (NJets >= 2){
	if (NBJets == 1) {
	//m_jet_selection->GetSignalJets()->sort(XAMPP::btagweightsorter);
	float n1 = m_jet_selection->GetSignalJets()->at(0)->eta();
	float n2 = m_jet_selection->GetSignalJets()->at(1)->eta(); 
        PseudoRapiditySep = n1-n2;
	}
	}
	if (NBJets == 2){
        float n1 = m_jet_selection->GetBJets()->at(0)->eta();
        float n2 = m_jet_selection->GetBJets()->at(1)->eta();
	PseudoRapiditySep = n1-n2;
	}
        if (NBJets == 3){
	//m_jet_selection->GetBJets()->sort(XAMPP::btagweightsorter);
        float n1 = m_jet_selection->GetBJets()->at(0)->eta();
        float n2 = m_jet_selection->GetBJets()->at(1)->eta();
        PseudoRapiditySep = n1-n2;
        }
	ATH_CHECK(dec_PseudoRapiditySep->Store(PseudoRapiditySep));
	


        float DPhiMetTrackMet = -1.;
        if (m_doTrackMet) DPhiMetTrackMet = XAMPP::ComputeDeltaPhi(dec_XAMPPmetTST, m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetTrack"));
        float MtTauCand = -1.;
        int NtracksTauCand = -1.;
        bool HasTauCand = XAMPP::HasTauCandidate(m_jet_selection->GetSignalJets(), dec_XAMPPmetTST->GetValue(), MtTauCand, NtracksTauCand);

        float HtSig = (dec_XAMPPmetTST->GetValue()->met()) * XAMPP::MeVToGeV / (TMath::Sqrt(Ht * XAMPP::MeVToGeV));
        float MinMt = XAMPP::ComputeMinMt(m_jet_selection->GetSignalJets(), dec_XAMPPmetTST->GetValue());

        bool IsOS = false;
        bool IsSF = false;
        float Mll = -1.;
        if (m_electron_selection->GetSignalElectrons()->size() > 1) {
            IsOS = XAMPP::OppositeSign(*m_electron_selection->GetSignalElectrons()->at(0), *m_electron_selection->GetSignalElectrons()->at(1));
            IsSF = XAMPP::SameFlavour(*m_electron_selection->GetSignalElectrons()->at(0), *m_electron_selection->GetSignalElectrons()->at(1));
            Mll = XAMPP::InvariantMass(*m_electron_selection->GetSignalElectrons()->at(0), *m_electron_selection->GetSignalElectrons()->at(1));
        }
        if (m_muon_selection->GetSignalMuons()->size() > 1) {
            IsOS = XAMPP::OppositeSign(*m_muon_selection->GetSignalMuons()->at(0), *m_muon_selection->GetSignalMuons()->at(1));
            IsSF = XAMPP::SameFlavour(*m_muon_selection->GetSignalMuons()->at(0), *m_muon_selection->GetSignalMuons()->at(1));
            Mll = XAMPP::InvariantMass(*m_muon_selection->GetSignalMuons()->at(0), *m_muon_selection->GetSignalMuons()->at(1));
        }
        float ZWindow = fabs(Mll - 91.0e3);

        float jetPtResoTot = 0.;
        for (const auto& ijet : *m_jet_selection->GetSignalJets()) {
            float jreso = m_jerTool->getRelResolutionData(ijet);
            jetPtResoTot += (ijet->pt() * jreso) * (ijet->pt() * jreso);
            ATH_MSG_DEBUG("Jet pt = " << ijet->pt() << " MeV, relative resolution = " << jreso);

        }
        jetPtResoTot = sqrt(jetPtResoTot);

        float MetSigJetReso = (dec_XAMPPmetTST->GetValue()->met() / jetPtResoTot);
        float MetSig200JetReso = ((dec_XAMPPmetTST->GetValue()->met() - 200000.) / jetPtResoTot);

        //Finally store the variables they are then used by the Cut Class or just written out into the trees
        ATH_CHECK(dec_NBJets->Store(NBJets));
        ATH_CHECK(dec_NLeps->Store(NLeps));
        ATH_CHECK(dec_NBaseLeps->Store(NBaseLeps));
        ATH_CHECK(dec_NJets->Store(NJets));
        ATH_CHECK(dec_MtBMin->Store(MtBMin));
        ATH_CHECK(dec_MtNonBMin->Store(MtNonBMin));
        ATH_CHECK(dec_MtJetMin->Store(MtJetMin));
        ATH_CHECK(dec_MtBMax->Store(MtBMax));
        ATH_CHECK(dec_MtNonBMax->Store(MtNonBMax));
        ATH_CHECK(dec_MtJetMax->Store(MtJetMax));
        ATH_CHECK(dec_BJet1Pt->Store(BJet1Pt)); 
        ATH_CHECK(dec_Jet1Pt->Store(Jet1Pt));
        ATH_CHECK(dec_Jet2Pt->Store(Jet2Pt));
        ATH_CHECK(dec_Jet3Pt->Store(Jet3Pt));
        ATH_CHECK(dec_Jet4Pt->Store(Jet4Pt));
        ATH_CHECK(dec_DPhiMetTrackMet->Store(DPhiMetTrackMet));
        ATH_CHECK(dec_AbsDPhiMetTrackMet->Store(fabs(DPhiMetTrackMet)));
        ATH_CHECK(dec_HasTauCand->Store(HasTauCand));
        ATH_CHECK(dec_MtTauCand->Store(MtTauCand));
        ATH_CHECK(dec_NtracksTauCand->Store(NtracksTauCand));
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

        // fill the particle containers for the output
        static XAMPP::ParticleStorage* ElectronStore = m_XAMPPInfo->GetParticleStorage("Elec");
        ATH_CHECK(ElectronStore->Fill(m_electron_selection->GetSignalElectrons()));
        static XAMPP::ParticleStorage* MuonStore = m_XAMPPInfo->GetParticleStorage("Muon");
        ATH_CHECK(MuonStore->Fill(m_muon_selection->GetSignalMuons()));
        static XAMPP::ParticleStorage* JetStore = m_XAMPPInfo->GetParticleStorage("Jet");
        ATH_CHECK(JetStore->Fill(m_jet_selection->GetSignalJets()));
        static XAMPP::ParticleStorage* RecoStore = m_XAMPPInfo->GetParticleStorage("RecoCandidates");
        ATH_CHECK(RecoStore->Fill(m_ParticleConstructor->GetContainer()));

        if (doTruth() && m_XAMPPInfo->GetSystematic() == m_systematics->GetNominal()) {
            XAMPP::ParticleStorage* TruthStore = m_XAMPPInfo->GetParticleStorage("TruthParticles");
            ATH_CHECK(TruthStore->Fill(m_truth_selection->GetTruthParticles()));
        }

        return StatusCode::SUCCESS;
    }

    bool DMbbAnalysisHelper::AcceptEvent() {
        static XAMPP::Storage<char>* dec_isMetSampleOR = m_XAMPPInfo->GetVariableStorage<char>("isMetSampleOR");

        static XAMPP::Storage<char>* dec_IsMETTrigPassed = m_XAMPPInfo->GetVariableStorage<char>("IsMETTrigPassed");
        ATH_CHECK(dec_IsMETTrigPassed->ConstStore(m_susytools->IsMETTrigPassed()));
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
        ATH_CHECK(dec_isMetSampleOR->ConstStore(isMetSampleOR));

        static XAMPP::Storage<double>* dec_SherpaVjetsNjetsWeight = m_XAMPPInfo->GetVariableStorage<double>("SherpaVjetsNjetsWeight");
        float SherpaVjetsNjetsWeight = 1.;
        if (((mcChannelNumber >= 363102) && (mcChannelNumber <= 363122)) || ((mcChannelNumber >= 363331) && (mcChannelNumber <= 363483))) SherpaVjetsNjetsWeight = m_susytools->getSherpaVjetsNjetsWeight();
        ATH_CHECK(dec_SherpaVjetsNjetsWeight->ConstStore(SherpaVjetsNjetsWeight));

        static XAMPP::Storage<int>* dec_treatAsYear = m_XAMPPInfo->GetVariableStorage<int>("treatAsYear");
        ATH_CHECK(dec_treatAsYear->ConstStore(m_susytools->treatAsYear()));

        return true;
    }

    DMbbGammaAnalysisHelper::DMbbGammaAnalysisHelper(std::string myname) :
            DMbbAnalysisHelper(myname) {
    }
    StatusCode DMbbGammaAnalysisHelper::initialize() {
        ATH_MSG_INFO("Initialising DMbbGamma AnalysisHelper...");
        ATH_CHECK(DMbbAnalysisHelper::initialize());
        return StatusCode::SUCCESS;
    }
    double DMbbGammaAnalysisHelper::GetTotalWeight() {
        double w = DMbbAnalysisHelper::GetTotalWeight();
        static XAMPP::Storage<double>* dec_PhotWeight = m_XAMPPInfo->GetVariableStorage<double>("PhoWeight");
        if (dec_PhotWeight) w *= dec_PhotWeight->GetValue();
        return w;
    }
    StatusCode DMbbGammaAnalysisHelper::initializeEventVariables() {
        ATH_CHECK(DMbbAnalysisHelper::initializeEventVariables());
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_SignalPhotons"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_BaselinePhotons", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Photon1Pt", false));
        // Now add the branches for the particle containers
        ATH_CHECK(m_XAMPPInfo->BookParticleStorage("Phot"));
        return StatusCode::SUCCESS;
    }
    StatusCode DMbbGammaAnalysisHelper::ComputeEventVariables() {
        ATH_CHECK(DMbbAnalysisHelper::ComputeEventVariables());
        static XAMPP::Storage<int>* dec_NPhotons = m_XAMPPInfo->GetVariableStorage<int>("N_SignalPhotons");
        static XAMPP::Storage<int>* dec_NBasePhotons = m_XAMPPInfo->GetVariableStorage<int>("N_BaselinePhotons");
        static XAMPP::Storage<float>* dec_Photon1Pt = m_XAMPPInfo->GetVariableStorage<float>("Photon1Pt");
        int NPhotons = m_photon_selection->GetSignalPhotons()->size();
        int NBasePhotons = m_photon_selection->GetBaselinePhotons()->size();
        float Photon1Pt = -1.;
        xAOD::IParticle* leadingPhot = XAMPP::FindLeadingParticle(m_photon_selection->GetSignalPhotons());
        if (leadingPhot) Photon1Pt = leadingPhot->pt();
        ATH_CHECK(dec_NPhotons->Store(NPhotons));
        ATH_CHECK(dec_NBasePhotons->Store(NBasePhotons));
        ATH_CHECK(dec_Photon1Pt->Store(Photon1Pt));
        // fill the particle containers for the output
        static XAMPP::ParticleStorage* PhotonStore = m_XAMPPInfo->GetParticleStorage("Phot");
        ATH_CHECK(PhotonStore->Fill(m_photon_selection->GetSignalPhotons()));
        return StatusCode::SUCCESS;
    }

}
