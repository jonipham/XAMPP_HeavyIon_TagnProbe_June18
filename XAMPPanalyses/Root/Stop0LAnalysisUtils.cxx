#include <XAMPPanalyses/Stop0LAnalysisUtils.h>
#include <XAMPPbase/EventInfo.h>

#include <XAMPPanalyses/MT2.h>
#include <XAMPPanalyses/mt2_bisect.h>

namespace XAMPP {
    bool isStop(const xAOD::TruthParticle& P) {
        return (P.absPdgId() == 1000006) || (P.absPdgId() == 2000006);
    }

    float CalculateMT2(const xAOD::IParticle &P1, const xAOD::IParticle &P2, XAMPP::Storage<XAMPPmet>* met, float ParticleMass) {
        return CalculateMT2(P1, P2, met->GetValue(), ParticleMass);
    }
    float CalculateMT2(const xAOD::IParticle &P1, const xAOD::IParticle &P2, const xAOD::MissingET* met, float ParticleMass) {
        if (!met) return -1;
        double patr[3] = { (ParticleMass != -1) ? ParticleMass : P1.m(), P1.p4().Px(), P1.p4().Py() };
        double pbtr[3] = { (ParticleMass != -1) ? ParticleMass : P2.m(), P2.p4().Px(), P2.p4().Py() };
        double pmisstr[3] = { 0, met->mpx(), met->mpy() };
        mt2_bisect::mt2 mt2calculator;
        mt2calculator.set_momenta(patr, pbtr, pmisstr);
        mt2calculator.set_mn(0);
        return mt2calculator.get_mt2();
    }

#ifdef DORJIGSAW
#include "xAODTrigMissingET/TrigMissingETContainer.h"

RJigsaw::RJigsaw() :
LAB_R(new RestFrames::LabRecoFrame("LAB_R", "LAB")),
GG_R(new RestFrames::DecayRecoFrame("GG_R", "#tilde{g}#tilde{g}")),
Ga_R(new RestFrames::DecayRecoFrame("Ga_R", "#tilde{g}_{a}")),
Gb_R(new RestFrames::DecayRecoFrame("Gb_R", "#tilde{g}_{b}")),
Ca_R(new RestFrames::DecayRecoFrame("Ca_R", "C_{a}")),
Cb_R(new RestFrames::DecayRecoFrame("Cb_R", "C_{b}")),
V1a_R(new RestFrames::VisibleRecoFrame("V1a_R", "j_{1a}")),
V2a_R(new RestFrames::VisibleRecoFrame("V2a_R", "j_{2a}")),
Xa_R(new RestFrames::InvisibleRecoFrame("Xa_R", "#tilde{#chi}_{a}")),
V1b_R(new RestFrames::VisibleRecoFrame("V1b_R", "j_{1b}")),
V2b_R(new RestFrames::VisibleRecoFrame("V2b_R", "j_{2b}")),
Xb_R(new RestFrames::InvisibleRecoFrame("Xb_R", "#tilde{#chi}_{b}")),
INV_R(new RestFrames::InvisibleGroup("INV_R", "WIMP Jigsaws")),
VIS_R(new RestFrames::CombinatoricGroup("VIS", "Visible Object Jigsaws")),
MinMassJigsaw_R(new RestFrames::SetMassInvJigsaw("MINMASS_R", "Invisible system mass Jigsaw")),
RapidityJigsaw_R(new RestFrames::SetRapidityInvJigsaw("RAPIDITY_R", "Invisible system rapidity Jigsaw")),
ContraBoostJigsaw_R(new RestFrames::ContraBoostInvJigsaw("CONTRA_R", "Contraboost invariant Jigsaw")),
HemiJigsaw_R(new RestFrames::MinMassesCombJigsaw("HEM_JIGSAW_R", "Minimize m _{V_{a,b}} Jigsaw")),
CaHemiJigsaw_R(new RestFrames::MinMassesCombJigsaw("CbHEM_JIGSAW_R", "Minimize m _{C_{a}} Jigsaw")),
CbHemiJigsaw_R(new RestFrames::MinMassesCombJigsaw("CaHEM_JIGSAW_R", "Minimize m _{C_{b}} Jigsaw")) {
    // Set up 'signal-like' analysis tree
    LAB_R->SetChildFrame(*GG_R);
    GG_R->AddChildFrame(*Ga_R);
    GG_R->AddChildFrame(*Gb_R);
    Ga_R->AddChildFrame(*V1a_R);
    Ga_R->AddChildFrame(*Ca_R);
    Ca_R->AddChildFrame(*V2a_R);
    Ca_R->AddChildFrame(*Xa_R);
    Gb_R->AddChildFrame(*V1b_R);
    Gb_R->AddChildFrame(*Cb_R);
    Cb_R->AddChildFrame(*V2b_R);
    Cb_R->AddChildFrame(*Xb_R);

    //if(!LAB_R->InitializeTree()) cout << "Problem with signal-like reconstruction tree" << endl;
    LAB_R->InitializeTree();

    INV_R->AddFrame(*Xa_R);
    INV_R->AddFrame(*Xb_R);
    // visible frames in first decay step must always have at least one element
    VIS_R->AddFrame(*V1a_R);
    VIS_R->AddFrame(*V1b_R);
    VIS_R->SetNElementsForFrame(*V1a_R, 1, false);
    VIS_R->SetNElementsForFrame(*V1b_R, 1, false);
    // visible frames in second decay step can have zero elements
    VIS_R->AddFrame(*V2a_R);
    VIS_R->AddFrame(*V2b_R);
    VIS_R->SetNElementsForFrame(*V2a_R, 0, false);
    VIS_R->SetNElementsForFrame(*V2b_R, 0, false);

    INV_R->AddJigsaw(*MinMassJigsaw_R);
    INV_R->AddJigsaw(*RapidityJigsaw_R);
    RapidityJigsaw_R->AddVisibleFrames((LAB_R->GetListVisibleFrames()));
    INV_R->AddJigsaw(*ContraBoostJigsaw_R);
    ContraBoostJigsaw_R->AddVisibleFrames((Ga_R->GetListVisibleFrames()), 0);
    ContraBoostJigsaw_R->AddVisibleFrames((Gb_R->GetListVisibleFrames()), 1);
    ContraBoostJigsaw_R->AddInvisibleFrames((Ga_R->GetListInvisibleFrames()), 0);
    ContraBoostJigsaw_R->AddInvisibleFrames((Gb_R->GetListInvisibleFrames()), 1);
    VIS_R->AddJigsaw(*HemiJigsaw_R);
    HemiJigsaw_R->AddFrame(*V1a_R, 0);
    HemiJigsaw_R->AddFrame(*V1b_R, 1);
    HemiJigsaw_R->AddFrame(*V2a_R, 0);
    HemiJigsaw_R->AddFrame(*V2b_R, 1);
    VIS_R->AddJigsaw(*CaHemiJigsaw_R);
    CaHemiJigsaw_R->AddFrame(*V1a_R, 0);
    CaHemiJigsaw_R->AddFrame(*V2a_R, 1);
    // CaHemiJigsaw_R->AddFrame(*Xa_R,1); //This and the next line removed because they allow for inv particle to be the only thing left in second order frame.
    VIS_R->AddJigsaw(*CbHemiJigsaw_R);
    CbHemiJigsaw_R->AddFrame(*V1b_R, 0);
    CbHemiJigsaw_R->AddFrame(*V2b_R, 1);
    // CbHemiJigsaw_R->AddFrame(*Xb_R,1);

    //if(!LAB_R->InitializeAnalysis()) cout << "Problem with signal-tree jigsaws" << endl;
    LAB_R->InitializeAnalysis();
}

RJigsaw::~RJigsaw() {
}

void RJigsaw::AnalyzeEvent(const TObjArray& jets, Double_t metx, Double_t mety) {
    LAB_R->ClearEventRecursive();
    VIS_R->ClearEvent();

    std::vector < RestFrames::RFKey > jetID_R; // ID for tracking jets in tree

    //std::cout << "number of jets is " << jets.GetEntries() << std::endl;
    if (jets.GetEntries() < 2) {
        //Less than two jets.
        MDeltaR = 0;
        return;
    }

    // Still need to add jets to frames ///////////////
    for (int ijet = 0; ijet < jets.GetEntries(); ijet++) {
        auto jet = static_cast<TLorentzVector*>(jets[ijet]);
        jetID_R.push_back(VIS_R->AddLabFrameFourVector(*jet));
        //TLorentzVector temp = myjets[ijet];
        //temp.SetPtEtaPhiM(temp.Pt(), 0., temp.Phi(), temp.M());
        //VIS_B->AddLabFrameFourVector(temp);
    }

    TVector3 MET_TV3;

    MET_TV3.SetZ(0.);
    MET_TV3.SetX(metx);
    MET_TV3.SetY(mety);

    INV_R->SetLabFrameThreeVector(MET_TV3);
    LAB_R->AnalyzeEvent();

    ComputeVariable();
}

void RJigsaw::FillTriggerBits(ST::SUSYObjDef_xAOD & stobjtool, xAOD::TEvent& tevent) {
    const xAOD::TrigMissingETContainer* trigmet = 0;
    if (!tevent.retrieve(trigmet, "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET").isSuccess()) { // retrieve arguments: contain key
        throw std::runtime_error("Could not retrieve HLT MET");
    }

    TVector2 hltmet(trigmet->at(0)->ex(), trigmet->at(0)->ey());

    if (hltmet.Mod() > 60000) {
        HLT_j30_xe60_razor170 = stobjtool.IsTrigPassed("HLT_j30_xe10_razor170");
    } else {
        HLT_j30_xe60_razor170 = false;
    }
}

void RJigsaw::ComputeVariable() {
    MDeltaR = GG_R->GetVisibleShape() * GG_R->GetMass();
}
#endif

}
