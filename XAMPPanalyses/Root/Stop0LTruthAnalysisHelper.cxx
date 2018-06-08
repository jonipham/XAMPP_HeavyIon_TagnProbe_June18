#include <XAMPPanalyses/Stop0LTruthAnalysisHelper.h>
#include <XAMPPanalyses/Stop0LAnalysisUtils.h>
#include <XAMPPbase/SUSYTruthSelector.h>

#define SET_DUAL_TOOL( TOOLHANDLE, TOOLTYPE, TOOLNAME )                \
  ASG_SET_ANA_TOOL_TYPE(TOOLHANDLE, TOOLTYPE);                        \
  TOOLHANDLE.setName(TOOLNAME);

namespace XAMPP {
    Stop0LTruthAnalysisHelper::Stop0LTruthAnalysisHelper(std::string myname) :
                    SUSYTruthAnalysisHelper(myname) {
        declareProperty("doReclustering", m_doReclustering = true);
        declareProperty("CustomReclusterPt", m_CustomReclusterPt = 20000.);
    }
    StatusCode Stop0LTruthAnalysisHelper::RemoveOverlap() {
        // electron - muon
        float dRemu = 0.01;
        ATH_CHECK(XAMPP::RemoveOverlap(m_truth_selection->GetTruthPreMuons(), m_truth_selection->GetTruthPreElectrons(), dRemu));
        ATH_CHECK(XAMPP::RemoveOverlap(m_truth_selection->GetTruthPreElectrons(), m_truth_selection->GetTruthPreMuons(), dRemu));

        // jet - electron
        auto jetelectron = [] (const xAOD::IParticle *Jet, const xAOD::IParticle * ) {
            static CharAccessor dec_bjet("bjet");
            if (!dec_bjet(*Jet)) return 0.2;
            else return 0.;
        };
        ATH_CHECK(XAMPP::RemoveOverlap(m_truth_selection->GetTruthPreJets(), m_truth_selection->GetTruthPreElectrons(), jetelectron));
        ATH_CHECK(XAMPP::RemoveOverlap(m_truth_selection->GetTruthPreElectrons(), m_truth_selection->GetTruthPreJets(), 0.2));
        
        // jet - muon
        auto muJetSpecial = [] (const xAOD::IParticle *Jet, const xAOD::IParticle *Muon) {
            static CharAccessor dec_bjet("bjet");
            if (!dec_bjet(*Jet) && Muon->pt()/Jet->pt()>0.7) return 0.2;
            else return 0.;
        };
        ATH_CHECK(XAMPP::RemoveOverlap(m_truth_selection->GetTruthPreJets(), m_truth_selection->GetTruthPreMuons(), muJetSpecial));
        ATH_CHECK(XAMPP::RemoveOverlap(m_truth_selection->GetTruthPreMuons(), m_truth_selection->GetTruthPreJets(), 0.2));
        
        auto radiusCalcLepton = [] (const xAOD::IParticle *Lep, const xAOD::IParticle * ) {return std::min(0.4, 0.04 + 10000./Lep->pt());};
        ATH_CHECK(XAMPP::RemoveOverlap(m_truth_selection->GetTruthPreMuons(), m_truth_selection->GetTruthPreJets(), radiusCalcLepton));
        ATH_CHECK(XAMPP::RemoveOverlap(m_truth_selection->GetTruthPreElectrons(), m_truth_selection->GetTruthPreJets(), radiusCalcLepton));

        return StatusCode::SUCCESS;
    }

    StatusCode Stop0LTruthAnalysisHelper::initializeEventVariables() {
        //Lets define some variables which we want to store in the output tree / use in the cutflow
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_BJets"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_SignalLeptons"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_BaselineLeptons", false)); //A variable on which we are cutting does not need to be stored in the tree
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_SignalPhotons"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_BaselinePhotons", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>("N_Jets"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtBMin"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtNonBMin"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtJetMin"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtBMax"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtNonBMax"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtJetMax"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MtLepMet"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("AsymmetryMTopR12"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("HtSig"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("MinMt"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Top1CandM", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Top2CandM", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("HT"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("Meff"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("AbsDeltaPhiMin2", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("DeltaPhiMin2"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("AbsDeltaPhiMin4", false));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("DeltaPhiMin4"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("dRBB"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("mt2_chi2"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>("mt2_chi2_chi2"));

        // Now add the branches for the particle containers
        ATH_CHECK(m_XAMPPInfo->BookParticleStorage("Elec"));
        XAMPP::ParticleStorage* ElectronStore = m_XAMPPInfo->GetParticleStorage("Elec");
        ATH_CHECK(ElectronStore->SaveFloat("charge"));
        ATH_CHECK(ElectronStore->SaveChar("passOR"));
        ATH_CHECK(ElectronStore->SaveChar("signal"));
        ATH_CHECK(m_XAMPPInfo->BookParticleStorage("Muon"));
        XAMPP::ParticleStorage* MuonStore = m_XAMPPInfo->GetParticleStorage("Muon");
        ATH_CHECK(MuonStore->SaveFloat("charge"));
        ATH_CHECK(MuonStore->SaveChar("passOR"));
        ATH_CHECK(MuonStore->SaveChar("signal"));
        ATH_CHECK(m_XAMPPInfo->BookParticleStorage("Phot"));
        XAMPP::ParticleStorage* PhotonStore = m_XAMPPInfo->GetParticleStorage("Phot");
        ATH_CHECK(PhotonStore->SaveChar("passOR"));
        ATH_CHECK(PhotonStore->SaveChar("signal"));
        ATH_CHECK(m_XAMPPInfo->BookParticleStorage("Jet"));
        XAMPP::ParticleStorage* JetStore = m_XAMPPInfo->GetParticleStorage("Jet");
        ATH_CHECK(JetStore->SaveChar("bjet"));
        ATH_CHECK(m_XAMPPInfo->BookParticleStorage("TruthParticles", true));
        XAMPP::ParticleStorage* TruthStore = m_XAMPPInfo->GetParticleStorage("TruthParticles");
        ATH_CHECK(TruthStore->SaveChar("isRealW"));
        ATH_CHECK(TruthStore->SaveInteger("pdgId"));
        ATH_CHECK(TruthStore->SaveFloat("charge"));

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
            ATH_CHECK(m_XAMPPInfo->BookParticleStorage("FatJet12", true));
            XAMPP::ParticleStorage* FatJet12Store = m_XAMPPInfo->GetParticleStorage("FatJet12");
            ATH_CHECK(FatJet12Store->SaveInteger("constituents"));
        }

        return StatusCode::SUCCESS;
    }
    StatusCode Stop0LTruthAnalysisHelper::ComputeEventVariables() {

        static XAMPP::Storage<int>* dec_NBJets = m_XAMPPInfo->GetVariableStorage<int>("N_BJets");
        static XAMPP::Storage<int>* dec_NLeps = m_XAMPPInfo->GetVariableStorage<int>("N_SignalLeptons");
        static XAMPP::Storage<int>* dec_NBaseLeps = m_XAMPPInfo->GetVariableStorage<int>("N_BaselineLeptons");
        static XAMPP::Storage<int>* dec_NPhots = m_XAMPPInfo->GetVariableStorage<int>("N_SignalPhotons");
        static XAMPP::Storage<int>* dec_NBasePhots = m_XAMPPInfo->GetVariableStorage<int>("N_BaselinePhotons");
        static XAMPP::Storage<int>* dec_NJets = m_XAMPPInfo->GetVariableStorage<int>("N_Jets");
        static XAMPP::Storage<float>* dec_MtBMin = m_XAMPPInfo->GetVariableStorage<float>("MtBMin");
        static XAMPP::Storage<float>* dec_MtNonBMin = m_XAMPPInfo->GetVariableStorage<float>("MtNonBMin");
        static XAMPP::Storage<float>* dec_MtJetMin = m_XAMPPInfo->GetVariableStorage<float>("MtJetMin");
        static XAMPP::Storage<float>* dec_MtBMax = m_XAMPPInfo->GetVariableStorage<float>("MtBMax");
        static XAMPP::Storage<float>* dec_MtNonBMax = m_XAMPPInfo->GetVariableStorage<float>("MtNonBMax");
        static XAMPP::Storage<float>* dec_MtJetMax = m_XAMPPInfo->GetVariableStorage<float>("MtJetMax");
        static XAMPP::Storage<float>* dec_MtLepMet = m_XAMPPInfo->GetVariableStorage<float>("MtLepMet");
        static XAMPP::Storage<float>* dec_AsymmetryMTopR12 = m_XAMPPInfo->GetVariableStorage<float>("AsymmetryMTopR12");
        static XAMPP::Storage<float>* dec_HtSig = m_XAMPPInfo->GetVariableStorage<float>("HtSig");
        static XAMPP::Storage<float>* dec_MinMt = m_XAMPPInfo->GetVariableStorage<float>("MinMt");
        static XAMPP::Storage<float>* dec_Top1CandM = m_XAMPPInfo->GetVariableStorage<float>("Top1CandM");
        static XAMPP::Storage<float>* dec_Top2CandM = m_XAMPPInfo->GetVariableStorage<float>("Top2CandM");
        static XAMPP::Storage<float>* dec_HT = m_XAMPPInfo->GetVariableStorage<float>("HT");
        static XAMPP::Storage<float>* dec_Meff = m_XAMPPInfo->GetVariableStorage<float>("Meff");
        static XAMPP::Storage<float>* dec_AbsDeltaPhiMin2 = m_XAMPPInfo->GetVariableStorage<float>("AbsDeltaPhiMin2");
        static XAMPP::Storage<float>* dec_DeltaPhiMin2 = m_XAMPPInfo->GetVariableStorage<float>("DeltaPhiMin2");
        static XAMPP::Storage<float>* dec_AbsDeltaPhiMin4 = m_XAMPPInfo->GetVariableStorage<float>("AbsDeltaPhiMin4");
        static XAMPP::Storage<float>* dec_DeltaPhiMin4 = m_XAMPPInfo->GetVariableStorage<float>("DeltaPhiMin4");
        static XAMPP::Storage<float>* dec_dRBB = m_XAMPPInfo->GetVariableStorage<float>("dRBB");
        static XAMPP::Storage<float>* dec_mt2_chi2 = m_XAMPPInfo->GetVariableStorage<float>("mt2_chi2");
        static XAMPP::Storage<float>* dec_mt2_chi2_chi2 = m_XAMPPInfo->GetVariableStorage<float>("mt2_chi2_chi2");

        static XAMPP::Storage<XAMPPmet>* dec_XAMPPmet = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("TruthMET");

        xAOD::Particle* W1Cand = m_ParticleConstructor->CreateEmptyParticle();
        xAOD::Particle* W2Cand = m_ParticleConstructor->CreateEmptyParticle();
        xAOD::Particle* Top1Cand = m_ParticleConstructor->CreateEmptyParticle();
        xAOD::Particle* Top2Cand = m_ParticleConstructor->CreateEmptyParticle();
        XAMPP::GetTopCandidates(m_truth_selection->GetTruthBJets(), m_truth_selection->GetTruthLightJets(), W1Cand, W2Cand, Top1Cand, Top2Cand);
        ATH_CHECK(dec_Top1CandM->Store(Top1Cand->m()));
        ATH_CHECK(dec_Top2CandM->Store(Top2Cand->m()));

        if (m_doReclustering) {
            ATH_MSG_DEBUG("Now recluster R=1.2 jets");
            ATH_CHECK(m_truth_selection->ReclusterTruthJets(m_truth_selection->GetTruthSignalJets(), 1.2));
            ATH_MSG_DEBUG("Now recluster R=0.8 jets");
            ATH_CHECK(m_truth_selection->ReclusterTruthJets(m_truth_selection->GetTruthSignalJets(), 0.8));
        }

        // Then lets calculate our event variables... there are lots of functions in the AnalysisUtils in order to do that, feel free to add other functions for this purpose
        int NBaseLeps = m_truth_selection->GetTruthBaselineElectrons()->size() + m_truth_selection->GetTruthBaselineMuons()->size();
        int NJets = m_truth_selection->GetTruthSignalJets()->size();

        int NBJets = m_truth_selection->GetTruthBJets()->size();
        float Ht = XAMPP::CalculateJetHt(m_truth_selection->GetTruthSignalJets());
        float MtBMin = XAMPP::ComputeMtMin(m_truth_selection->GetTruthBJets(), dec_XAMPPmet);
        float MtNonBMin = XAMPP::ComputeMtMin(m_truth_selection->GetTruthLightJets(), dec_XAMPPmet);
        float MtJetMin = XAMPP::ComputeMtMin(m_truth_selection->GetTruthSignalJets(), dec_XAMPPmet);
        float MtBMax = XAMPP::ComputeMtMax(m_truth_selection->GetTruthBJets(), dec_XAMPPmet);
        float MtNonBMax = XAMPP::ComputeMtMax(m_truth_selection->GetTruthLightJets(), dec_XAMPPmet);
        float MtJetMax = XAMPP::ComputeMtMax(m_truth_selection->GetTruthSignalJets(), dec_XAMPPmet);

        // use SG::VIEW_ELEMENTS container for combining electrons and muons
        xAOD::IParticleContainer SignalLeptons(SG::VIEW_ELEMENTS);
        for (auto const &elec : *m_truth_selection->GetTruthSignalElectrons())
            SignalLeptons.push_back(elec);
        for (auto const &muon : *m_truth_selection->GetTruthSignalMuons())
            SignalLeptons.push_back(muon);
        xAOD::IParticle* LeadingLep = XAMPP::FindLeadingParticle(&SignalLeptons);
        if (LeadingLep) ATH_CHECK(dec_MtLepMet->Store(XAMPP::ComputeMt(*LeadingLep, dec_XAMPPmet)));
        else ATH_CHECK(dec_MtLepMet->Store(-1));
        int NLeps = SignalLeptons.size();

        xAOD::IParticle* leadingFatJet = XAMPP::FindLeadingParticle(m_truth_selection->GetTruthCustomJets("FatJet1.2"));
        float AsymmetryMTopR12 = -1.;
        if (m_truth_selection->GetTruthCustomJets("FatJet1.2")->size() > 1) {
            AsymmetryMTopR12 = (fabs(leadingFatJet->m() - m_truth_selection->GetTruthCustomJets("FatJet1.2")->at(1)->m())) / (leadingFatJet->m() + m_truth_selection->GetTruthCustomJets("FatJet1.2")->at(1)->m());
        }
        float HtSig = (dec_XAMPPmet->GetValue()->met()) * XAMPP::MeVToGeV / (TMath::Sqrt(Ht * XAMPP::MeVToGeV));
        float MinMt = XAMPP::ComputeMinMt(m_truth_selection->GetTruthSignalJets(), dec_XAMPPmet->GetValue());
        float DeltaPhiMin2 = ComputeDPhiMin(m_truth_selection->GetTruthSignalJets(), dec_XAMPPmet, 2);
        if (m_truth_selection->GetTruthSignalJets()->size() && DeltaPhiMin2 == -FLT_MAX) return StatusCode::FAILURE;
        float DeltaPhiMin4 = ComputeDPhiMin(m_truth_selection->GetTruthSignalJets(), dec_XAMPPmet, 4);
        if (m_truth_selection->GetTruthSignalJets()->size() && DeltaPhiMin4 == -FLT_MAX) return StatusCode::FAILURE;

        xAOD::IParticle *BJet1(NULL), *BJet2(NULL);
        if (m_truth_selection->GetTruthBJets()->size() > 0) BJet1 = m_truth_selection->GetTruthBJets()->at(0);
        if (m_truth_selection->GetTruthBJets()->size() > 1) BJet2 = m_truth_selection->GetTruthBJets()->at(1);
        float dRBB(-1.);
        if (BJet1 && BJet2) dRBB = xAOD::P4Helpers::deltaR(BJet1, BJet2);

        ATH_CHECK(dec_NPhots->Store(m_truth_selection->GetTruthSignalPhotons()->size()));
        ATH_CHECK(dec_NBasePhots->Store(m_truth_selection->GetTruthBaselinePhotons()->size()));

        float mt2_chi2(-1), mt2_chi2_chi2(-1);
        ATH_CHECK(XAMPP::GetTopCandidatesChi2(m_truth_selection->GetTruthSignalJets(), m_truth_selection->GetTruthBJets(), m_ParticleConstructor));
        if (!std::isnan(dec_XAMPPmet->GetValue()->met())) {
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
                    mt2_chi2_chi2 = dec_Chi2(*TopCandChi2_1) + dec_Chi2(*TopCandChi2_2);
                    ATH_MSG_DEBUG("- Starting Chi2-MT2 calculation...");
                    mt2_chi2 = CalculateMT2(*TopCandChi2_1, *TopCandChi2_2, dec_XAMPPmet, 173210);
                }
            }
        }

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
        ATH_CHECK(dec_AsymmetryMTopR12->Store(AsymmetryMTopR12));
        ATH_CHECK(dec_HtSig->Store(HtSig));
        ATH_CHECK(dec_MinMt->Store(MinMt));
        ATH_CHECK(dec_HT->Store(Ht));
        ATH_CHECK(dec_Meff->Store(Ht + dec_XAMPPmet->GetValue()->met()));
        ATH_CHECK(dec_AbsDeltaPhiMin2->Store(fabs(DeltaPhiMin2)));
        ATH_CHECK(dec_DeltaPhiMin2->Store(DeltaPhiMin2));
        ATH_CHECK(dec_AbsDeltaPhiMin4->Store(fabs(DeltaPhiMin4)));
        ATH_CHECK(dec_DeltaPhiMin4->Store(DeltaPhiMin4));
        ATH_CHECK(dec_dRBB->Store(dRBB));
        ATH_CHECK(dec_mt2_chi2->Store(mt2_chi2));
        ATH_CHECK(dec_mt2_chi2_chi2->Store(mt2_chi2_chi2));

        // fill the particle containers for the output
        static XAMPP::ParticleStorage* ElectronStore = m_XAMPPInfo->GetParticleStorage("Elec");
        ATH_CHECK(ElectronStore->Fill(m_truth_selection->GetTruthBaselineElectrons()));
        static XAMPP::ParticleStorage* MuonStore = m_XAMPPInfo->GetParticleStorage("Muon");
        ATH_CHECK(MuonStore->Fill(m_truth_selection->GetTruthBaselineMuons()));
        static XAMPP::ParticleStorage* PhotonStore = m_XAMPPInfo->GetParticleStorage("Phot");
        ATH_CHECK(PhotonStore->Fill(m_truth_selection->GetTruthBaselinePhotons()));
        static XAMPP::ParticleStorage* JetStore = m_XAMPPInfo->GetParticleStorage("Jet");
        ATH_CHECK(JetStore->Fill(m_truth_selection->GetTruthSignalJets()));
        static XAMPP::ParticleStorage* TruthStore = m_XAMPPInfo->GetParticleStorage("TruthParticles");
        ATH_CHECK(TruthStore->Fill(m_truth_selection->GetTruthParticles()));
        static XAMPP::ParticleStorage* RecoStore = m_XAMPPInfo->GetParticleStorage("RecoCandidates");
        ATH_CHECK(RecoStore->Fill(m_ParticleConstructor->GetContainer()));

        if (m_doReclustering) {
            static XAMPP::ParticleStorage* FatJet08Store = m_XAMPPInfo->GetParticleStorage("FatJet08");
            ATH_CHECK(FatJet08Store->Fill(m_truth_selection->GetTruthCustomJets("FatJet0.8")));
            static XAMPP::ParticleStorage* FatJet12Store = m_XAMPPInfo->GetParticleStorage("FatJet12");
            ATH_CHECK(FatJet12Store->Fill(m_truth_selection->GetTruthCustomJets("FatJet1.2")));
        }

        return StatusCode::SUCCESS;
    }
}
