#include <XAMPPanalyses/Stop0LISRSelector.h>

namespace XAMPP {
    Stop0LISRSelector::Stop0LISRSelector(std::string myname) :
                    AsgTool(myname),
                    m_InfoHandle("EventInfoHandler"),
                    m_XAMPPInfo(NULL),
                    m_dec_PTISR(NULL),
                    m_dec_RISR(NULL),
                    m_dec_cosS(NULL),
                    m_dec_MS(NULL),
                    m_dec_MISR(NULL),
                    m_dec_MV(NULL),
                    m_dec_dphiCMI(NULL),
                    m_dec_dphiISRI(NULL),
                    m_dec_NbV(NULL),
                    m_dec_NbISR(NULL),
                    m_dec_NjV(NULL),
                    m_dec_NjISR(NULL),
                    m_dec_NlV(NULL),
                    m_dec_NlISR(NULL),
                    m_dec_PTCM(NULL),
                    m_dec_MW1(NULL),
                    m_dec_MW2(NULL),
                    m_dec_dphiCML1(NULL),
                    m_dec_dphiCML2(NULL),
                    m_dec_dphiSL1(NULL),
                    m_dec_dphiSL2(NULL),
                    m_dec_cosIL1(NULL),
                    m_dec_cosIL2(NULL),
                    m_dec_dRMinbl1(NULL),
                    m_dec_dRMinbl2(NULL),
                    m_dec_dphiMinbl1(NULL),
                    m_dec_dphiMinbl2(NULL),
                    m_dec_Vbjet1Pt(NULL),
                    m_dec_Vjet4Pt(NULL) {
        m_InfoHandle.declarePropertyFor(this, "EventInfoHandler", "The XAMPP EventInfo handle");
    }

    Stop0LISRSelector::~Stop0LISRSelector() {

#ifdef XAOD_STANDALONE
#ifdef ROOTCORE_PACKAGE_Ext_RestFrames

        delete LAB;
        delete CM;
        delete S;
        delete ISR;
        delete V;
        delete I;
        delete INV;
        delete InvMass;
        delete VIS;
        delete SplitVis;
#endif
#endif
    }
    StatusCode Stop0LISRSelector::initialize() {
        ATH_MSG_INFO("Initializing Stop0LISRSelector...");
        ATH_CHECK(m_InfoHandle.retrieve());
        m_XAMPPInfo = dynamic_cast<XAMPP::EventInfo*>(m_InfoHandle.operator->());
#ifdef XAOD_STANDALONE
#ifdef ROOTCORE_PACKAGE_Ext_RestFrames

        LAB = new RestFrames::LabRecoFrame("LAB", "lab");
        CM = new RestFrames::DecayRecoFrame("CM", "CM");
        S = new RestFrames::DecayRecoFrame("S", "S");
        ISR = new RestFrames::VisibleRecoFrame("ISR", "ISR");
        V = new RestFrames::VisibleRecoFrame("V", "Vis");
        I = new RestFrames::InvisibleRecoFrame("I", "Inv");

        LAB->SetChildFrame(*CM);
        CM->AddChildFrame(*ISR);
        CM->AddChildFrame(*S);
        S->AddChildFrame(*V);
        S->AddChildFrame(*I);
        LAB->InitializeTree();

        INV = new RestFrames::InvisibleGroup("INV", "Invisible System");
        INV->AddFrame(*I);

        VIS = new RestFrames::CombinatoricGroup("VIS", "Visible Objects");
        VIS->AddFrame(*ISR);
        VIS->SetNElementsForFrame(*ISR, 1, false);
        VIS->AddFrame(*V);
        VIS->SetNElementsForFrame(*V, 0, false);

        // set the invisible system mass to zero
        InvMass = new RestFrames::SetMassInvJigsaw("InvMass", "Invisible system mass Jigsaw");
        INV->AddJigsaw(*InvMass);

        // define the rule for partitioning objects between "ISR" and "V"
        SplitVis = new RestFrames::MinMassesCombJigsaw("SplitVis", "Minimize M_{ISR} and M_{S} Jigsaw");
        VIS->AddJigsaw(*SplitVis);
        // "0" group (ISR)
        SplitVis->AddFrame(*ISR, 0);
        // "1" group (V + I)
        SplitVis->AddFrame(*V, 1);
        SplitVis->AddFrame(*I, 1);

        LAB->InitializeAnalysis();

#endif
#endif
        return StatusCode::SUCCESS;
    }

    StatusCode Stop0LISRSelector::initializeISREventVariables(bool isTruth) {
        std::string prefix = "ISR";
        if (isTruth) prefix = "ISRTruth";
        // for truth, only save variables to nominal tree
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_PTISR", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_RISR", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_cosS", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_MS", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_MISR", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_MV", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_dphiCMI", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_dphiISRI", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>(prefix + "_NbV", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>(prefix + "_NbISR", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>(prefix + "_NjV", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>(prefix + "_NjISR", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>(prefix + "_NlV", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<int>(prefix + "_NlISR", true, !isTruth));

        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_PTCM", true, !isTruth));

        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_MW1", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_MW2", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_dphiCML1", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_dphiCML2", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_dphiSL1", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_dphiSL2", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_cosIL1", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_cosIL2", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_dRMinbl1", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_dRMinbl2", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_dphiMinbl1", true, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_dphiMinbl2", true, !isTruth));

        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_Vbjet1Pt", false, !isTruth));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<float>(prefix + "_Vjet4Pt", false, !isTruth));

        m_dec_PTISR = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_PTISR");
        m_dec_RISR = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_RISR");
        m_dec_cosS = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_cosS");
        m_dec_MS = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_MS");
        m_dec_MISR = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_MISR");
        m_dec_MV = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_MV");
        m_dec_dphiCMI = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_dphiCMI");
        m_dec_dphiISRI = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_dphiISRI");
        m_dec_NbV = m_XAMPPInfo->GetVariableStorage<int>(prefix + "_NbV");
        m_dec_NbISR = m_XAMPPInfo->GetVariableStorage<int>(prefix + "_NbISR");
        m_dec_NjV = m_XAMPPInfo->GetVariableStorage<int>(prefix + "_NjV");
        m_dec_NjISR = m_XAMPPInfo->GetVariableStorage<int>(prefix + "_NjISR");
        m_dec_NlV = m_XAMPPInfo->GetVariableStorage<int>(prefix + "_NlV");
        m_dec_NlISR = m_XAMPPInfo->GetVariableStorage<int>(prefix + "_NlISR");

        m_dec_PTCM = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_PTCM");

        m_dec_MW1 = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_MW1");
        m_dec_MW2 = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_MW2");
        m_dec_dphiCML1 = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_dphiCML1");
        m_dec_dphiCML2 = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_dphiCML2");
        m_dec_dphiSL1 = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_dphiSL1");
        m_dec_dphiSL2 = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_dphiSL2");
        m_dec_cosIL1 = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_cosIL1");
        m_dec_cosIL2 = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_cosIL2");
        m_dec_dRMinbl1 = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_dRMinbl1");
        m_dec_dRMinbl2 = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_dRMinbl2");
        m_dec_dphiMinbl1 = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_dphiMinbl1");
        m_dec_dphiMinbl2 = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_dphiMinbl2");

        m_dec_Vbjet1Pt = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_Vbjet1Pt");
        m_dec_Vjet4Pt = m_XAMPPInfo->GetVariableStorage<float>(prefix + "_Vjet4Pt");
        return StatusCode::SUCCESS;
    }

    StatusCode Stop0LISRSelector::ComputeISREventVariables(xAOD::JetContainer* Jets, xAOD::IParticleContainer* Electrons, xAOD::IParticleContainer* Muons, XAMPP::Storage<XAMPPmet>* Met) {
        // compressed variables
        double PTISR = -1.;
        double RISR = -1.;
        double cosS = -1.;
        double MS = -1.;
        double MISR = -1.;
        double MV = -1.;
        double dphiCMI = -1.;
        double dphiISRI = -1.;
        int NbV = 0.;
        int NbISR = 0.;
        int NjV = 0.;
        int NjISR = 0.;

        float Vbjet1Pt = -1.;
        float Vjet4Pt = -1.;

        int NlV = 0;
        int NlISR = 0;
        double MW1 = -1.;
        double MW2 = -1.;
        double dphiCML1 = -1.;
        double dphiCML2 = -1.;
        double dphiSL1 = -1.;
        double dphiSL2 = -1.;
        double cosIL1 = -1.;
        double cosIL2 = -1.;
        double dRMinbl1 = -1.;
        double dRMinbl2 = -1.;
        double dphiMinbl1 = -1.;
        double dphiMinbl2 = -1.;

        double PTCM = -1.;

#ifdef XAOD_STANDALONE
#ifdef ROOTCORE_PACKAGE_Ext_RestFrames

        // Ext_RestFrames package needs TVector3 for MissingET:
        TVector3 met;
        XAMPPmet MetPointer = Met->GetValue();
        if (MetPointer) met.SetXYZ(MetPointer->mpx(), MetPointer->mpy(), 0.);
        else ATH_MSG_WARNING("No xAOD::MissingET found!");

        // Ext_RestFrames package needs at least 1 jet to work
        if (Jets->size() > 0) {
            LAB->ClearEvent();
            vector < RestFrames::RFKey > jetID;
            for (const auto& ijet : *Jets) {
                // transverse view of jet 4-vectors
                TLorentzVector jet;
                jet.SetPtEtaPhiM(ijet->pt(), 0.0, ijet->phi(), ijet->m());
                jetID.push_back(VIS->AddLabFrameFourVector(jet));
            }
            vector < RestFrames::RFKey > muID;
            for (const auto& imu : *Muons) {
                TLorentzVector muon;
                muon.SetPtEtaPhiM(imu->pt(), 0.0, imu->phi(), imu->m());
                muID.push_back(VIS->AddLabFrameFourVector(muon));
            }
            vector < RestFrames::RFKey > elID;
            for (const auto& iel : *Electrons) {
                TLorentzVector elec;
                elec.SetPtEtaPhiM(iel->pt(), 0.0, iel->phi(), iel->m());
                elID.push_back(VIS->AddLabFrameFourVector(elec));
            }

            INV->SetLabFrameThreeVector(met);
            if (!LAB->AnalyzeEvent()) {
                ATH_MSG_FATAL("AnalyzeEvent did not work");
                return StatusCode::FAILURE;
            }

            static SG::AuxElement::Decorator<char> dec_isISR_V("isISR_V");
            static SG::AuxElement::Decorator<char> dec_bjet("bjet");
            int idx = 0;
            for (const auto& ijet : *Jets) {
                dec_isISR_V(*ijet) = false;
                if (VIS->GetFrame(jetID[idx]) == *V) { // sparticle group
                    NjV++;
                    if (NjV == 4) Vjet4Pt = ijet->pt();
                    dec_isISR_V(*ijet) = true;
                    if (dec_bjet(*ijet)) {
                        NbV++;
                        if (NbV == 1) Vbjet1Pt = ijet->pt();
                    }
                } else {
                    NjISR++;
                    if (dec_bjet(*ijet)) NbISR++;
                }
                ++idx;
            }

            if (Muons->size() > 0) {
                int idx = 0;
                for (const auto& imu : *Muons) {
                    if (VIS->GetFrame(muID[idx]) == *V) // sparticle group
                    NlV++;
                    else NlISR++;

                    TLorentzVector lep;
                    lep.SetPtEtaPhiM(imu->pt(), 0.0, imu->phi(), imu->m());
                    TLorentzVector vL_CM = CM->GetFourVector(lep);
                    TLorentzVector vL_S = S->GetFourVector(lep);
                    TLorentzVector vCM_lab = CM->GetFourVector();
                    TLorentzVector vS_CM = S->GetFourVector(*CM);
                    TLorentzVector vI_S = I->GetFourVector(*S);
                    if (NlV + NlISR == 1) {
                        dphiCML1 = acos(vL_CM.Vect().Unit().Dot(vCM_lab.Vect().Unit()));

                        dphiSL1 = acos(vL_S.Vect().Unit().Dot(vS_CM.Vect().Unit()));

                        MW1 = (vL_S + vI_S).M();
                        TVector3 boostLI = (vL_S + vI_S).BoostVector();
                        vL_S.Boost(-boostLI);
                        cosIL1 = -boostLI.Unit().Dot(vL_S.Vect().Unit());

                        for (const auto& ijet : *Jets) {
                            if (dec_bjet(*ijet)) {
                                if (dRMinbl1 > imu->p4().DeltaR(ijet->p4()) || dRMinbl1 < 0.) dRMinbl1 = imu->p4().DeltaR(ijet->p4());
                                if (dphiMinbl1 > fabs(imu->p4().DeltaR(ijet->p4())) || dphiMinbl1 < 0.) dphiMinbl1 = fabs(imu->p4().DeltaR(ijet->p4()));
                            }
                        }
                    }
                    if (NlV + NlISR == 2) {
                        dphiCML2 = acos(vL_CM.Vect().Unit().Dot(vCM_lab.Vect().Unit()));

                        dphiSL2 = acos(vL_S.Vect().Unit().Dot(vS_CM.Vect().Unit()));

                        MW2 = (vL_S + vI_S).M();
                        TVector3 boostLI = (vL_S + vI_S).BoostVector();
                        vL_S.Boost(-boostLI);
                        cosIL2 = -boostLI.Unit().Dot(vL_S.Vect().Unit());

                        for (const auto& ijet : *Jets) {
                            if (dec_bjet(*ijet)) {
                                if (dRMinbl2 > imu->p4().DeltaR(ijet->p4()) || dRMinbl2 < 0.) dRMinbl2 = imu->p4().DeltaR(ijet->p4());
                                if (dphiMinbl2 > fabs(imu->p4().DeltaR(ijet->p4())) || dphiMinbl2 < 0.) dphiMinbl2 = fabs(imu->p4().DeltaR(ijet->p4()));
                            }
                        }
                    }
                    ++idx;
                }
            }

            if (Electrons->size() > 0) {
                int idx = 0;
                for (const auto& iel : *Electrons) {
                    if (VIS->GetFrame(elID[idx]) == *V) // sparticle group
                    NlV++;
                    else NlISR++;

                    TLorentzVector lep;
                    lep.SetPtEtaPhiM(iel->pt(), 0.0, iel->phi(), iel->m());
                    TLorentzVector vL_CM = CM->GetFourVector(lep);
                    TLorentzVector vL_S = S->GetFourVector(lep);
                    TLorentzVector vCM_lab = CM->GetFourVector();
                    TLorentzVector vS_CM = S->GetFourVector(*CM);
                    TLorentzVector vI_S = I->GetFourVector(*S);
                    if (NlV + NlISR == 1) {
                        dphiCML1 = acos(vL_CM.Vect().Unit().Dot(vCM_lab.Vect().Unit()));

                        dphiSL1 = acos(vL_S.Vect().Unit().Dot(vS_CM.Vect().Unit()));

                        MW1 = (vL_S + vI_S).M();
                        TVector3 boostLI = (vL_S + vI_S).BoostVector();
                        vL_S.Boost(-boostLI);
                        cosIL1 = -boostLI.Unit().Dot(vL_S.Vect().Unit());

                        for (const auto& ijet : *Jets) {
                            if (dec_bjet(*ijet)) {
                                if (dRMinbl1 > iel->p4().DeltaR(ijet->p4()) || dRMinbl1 < 0.) dRMinbl1 = iel->p4().DeltaR(ijet->p4());
                                if (dphiMinbl1 > fabs(iel->p4().DeltaR(ijet->p4())) || dphiMinbl1 < 0.) dphiMinbl1 = fabs(iel->p4().DeltaR(ijet->p4()));
                            }
                        }
                    }
                    if (NlV + NlISR == 2) {
                        dphiCML2 = acos(vL_CM.Vect().Unit().Dot(vCM_lab.Vect().Unit()));

                        dphiSL2 = acos(vL_S.Vect().Unit().Dot(vS_CM.Vect().Unit()));

                        MW2 = (vL_S + vI_S).M();
                        TVector3 boostLI = (vL_S + vI_S).BoostVector();
                        vL_S.Boost(-boostLI);
                        cosIL2 = -boostLI.Unit().Dot(vL_S.Vect().Unit());

                        for (const auto& ijet : *Jets) {
                            if (dec_bjet(*ijet)) {
                                if (dRMinbl2 > iel->p4().DeltaR(ijet->p4()) || dRMinbl2 < 0.) dRMinbl2 = iel->p4().DeltaR(ijet->p4());
                                if (dphiMinbl2 > fabs(iel->p4().DeltaR(ijet->p4())) || dphiMinbl2 < 0.) dphiMinbl2 = fabs(iel->p4().DeltaR(ijet->p4()));
                            }
                        }
                    }
                    ++idx;
                }
            }

            // need at least one jet or lepton associated with sparticle-side of event
            if (NjV + NlV < 1) {
                PTISR = -1.;
                RISR = -1.;
                cosS = -1.;
                MS = -1.;
                MV = -1.;
                MISR = -1.;
                dphiCMI = -1.;
                dphiISRI = -1.;
            } else {

                TVector3 vP_ISR = ISR->GetFourVector(*CM).Vect();
                TVector3 vP_I = I->GetFourVector(*CM).Vect();
                TVector3 vP_CM = CM->GetFourVector(*LAB).Vect();

                PTISR = vP_ISR.Mag();
                PTCM = vP_CM.Pt();
                RISR = fabs(vP_I.Dot(vP_ISR.Unit())) / PTISR;
                cosS = S->GetCosDecayAngle();
                MS = S->GetMass();
                MV = V->GetMass();
                MISR = ISR->GetMass();
                dphiCMI = acos(-1.) - fabs(CM->GetDeltaPhiBoostVisible());
                dphiISRI = fabs(vP_ISR.DeltaPhi(vP_I));

            }
        }
#endif
#endif

        ATH_CHECK(m_dec_PTISR->Store(PTISR));
        ATH_CHECK(m_dec_RISR->Store(RISR));
        ATH_CHECK(m_dec_cosS->Store(cosS));
        ATH_CHECK(m_dec_MS->Store(MS));
        ATH_CHECK(m_dec_MISR->Store(MISR));
        ATH_CHECK(m_dec_MV->Store(MV));
        ATH_CHECK(m_dec_dphiCMI->Store(dphiCMI));
        ATH_CHECK(m_dec_dphiISRI->Store(dphiISRI));
        ATH_CHECK(m_dec_NbV->Store(NbV));
        ATH_CHECK(m_dec_NbISR->Store(NbISR));
        ATH_CHECK(m_dec_NjV->Store(NjV));
        ATH_CHECK(m_dec_NjISR->Store(NjISR));
        ATH_CHECK(m_dec_NlV->Store(NlV));
        ATH_CHECK(m_dec_NlISR->Store(NlISR));

        ATH_CHECK(m_dec_PTCM->Store(PTCM));

        ATH_CHECK(m_dec_MW1->Store(MW1));
        ATH_CHECK(m_dec_MW2->Store(MW2));
        ATH_CHECK(m_dec_dphiCML1->Store(dphiCML1));
        ATH_CHECK(m_dec_dphiCML2->Store(dphiCML2));
        ATH_CHECK(m_dec_dphiSL1->Store(dphiSL1));
        ATH_CHECK(m_dec_dphiSL2->Store(dphiSL2));
        ATH_CHECK(m_dec_cosIL1->Store(cosIL1));
        ATH_CHECK(m_dec_cosIL2->Store(cosIL2));
        ATH_CHECK(m_dec_dRMinbl1->Store(dRMinbl1));
        ATH_CHECK(m_dec_dRMinbl2->Store(dRMinbl2));
        ATH_CHECK(m_dec_dphiMinbl1->Store(dphiMinbl1));
        ATH_CHECK(m_dec_dphiMinbl2->Store(dphiMinbl2));

        ATH_CHECK(m_dec_Vbjet1Pt->Store(Vbjet1Pt));
        ATH_CHECK(m_dec_Vjet4Pt->Store(Vjet4Pt));
        return StatusCode::SUCCESS;
    }
}

