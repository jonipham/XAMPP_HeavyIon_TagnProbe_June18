#include <XAMPPanalyses/DMbbAnalysisConfig.h>

namespace XAMPP {
    DMbbAnalysisConfig::DMbbAnalysisConfig(std::string Analysis) :
            AnalysisConfig(Analysis) {
    }

    StatusCode DMbbAnalysisConfig::initializeCustomCuts() {

        // --------------------------------------
        // Signal regions
        // --------------------------------------

        CutFlow SRCutFlow("DMbb_SR");

        Cut* MetSampleOR = NewCut("MetSampleOR", Cut::CutType::CutChar, false);
        if (!MetSampleOR->initialize("isMetSampleOR", "==0")) return StatusCode::FAILURE;
//        MetSampleOR->Debug(true);
        SRCutFlow.push_back(MetSampleOR);

        // define a new cut on the number of signal leptons which is a int cut and specify the 'Real' option as true since then, this cut is also applied on the dump of the ntuple
        Cut* SignalLeptonVeto = NewCut("0 signal leptons", Cut::CutType::CutInt, true);
        if (!SignalLeptonVeto->initialize("N_SignalLeptons", "==0")) return StatusCode::FAILURE;
        Cut* BaselineLeptonVeto = NewCut("0 baseline leptons", Cut::CutType::CutInt, true);
        if (!BaselineLeptonVeto->initialize("N_BaselineLeptons", "==0")) return StatusCode::FAILURE;
        // combine the cut on the number of signal leptons with the cut on the number of baseline leptons which is also a 'Real' cut
        Cut* LeptonVeto = SignalLeptonVeto->combine(BaselineLeptonVeto, Cut::Combine::AND);
        SRCutFlow.push_back(LeptonVeto);

        Cut* OneBJetDump = NewCut(">=1 b-jets", Cut::CutType::CutInt, true);
        if (!OneBJetDump->initialize("N_BJets", ">=1")) return StatusCode::FAILURE;
        SRCutFlow.push_back(OneBJetDump);

	//Cut* MaxTwoBJets = NewCut("<=2 b-jets", Cut::CutType::CutInt, true);
	//if (!MaxTwoBJets->initialize("N_BJets", "<=2")) return StatusCode::FAILURE;
        //SRCutFlow.push_back(MaxTwoBJets);

        Cut* Met100 = NewCut("Met > 100", Cut::CutType::CutXAMPPmet, true);
        if (!Met100->initialize("MetTST", ">=100000.")) return StatusCode::FAILURE;
        SRCutFlow.push_back(Met100);

        // now add a cut on the pt of the leading jet, which is not a 'Real' cut, meaning that it only gets applied in the cutflow histogram but not for the dump of the ntuple
        Cut* Jet1Pt = NewCut("Jet1Pt > 85", Cut::CutType::CutFloat, true);
        if (!Jet1Pt->initialize("Jet1Pt", ">=85000.")) return StatusCode::FAILURE;
        SRCutFlow.push_back(Jet1Pt);
        //Cut* Jet2Pt = NewCut("Jet2Pt > 20", Cut::CutType::CutFloat, true);
        //if (!Jet2Pt->initialize("Jet2Pt", ">=20000.")) return StatusCode::FAILURE;
        //SRCutFlow.push_back(Jet2Pt);

        Cut* BJet1Pt = NewCut("BJet1Pt > 50", Cut::CutType::CutFloat, true);
        if (!BJet1Pt->initialize("BJet1Pt", ">=50000")) return StatusCode::FAILURE;  
        SRCutFlow.push_back(BJet1Pt);

        Cut* AtLeastTwoJets = NewCut("2<=n_jets", Cut::CutType::CutInt, true);
        if (!AtLeastTwoJets->initialize("N_Jets", ">=2")) return StatusCode::FAILURE;
        SRCutFlow.push_back(AtLeastTwoJets);

        Cut* AtMostThreeJets = NewCut("3>=n_jets", Cut::CutType::CutInt, true);
        if (!AtMostThreeJets->initialize("N_Jets", "<=3")) return StatusCode::FAILURE;
        SRCutFlow.push_back(AtMostThreeJets);

        //Cut* DeltaPhiMin = NewCut("DeltaPhiMin2", Cut::CutType::CutFloat, false);
        //if (!DeltaPhiMin->initialize("AbsDeltaPhiMin2", ">0.4")) return StatusCode::FAILURE;
        //SRCutFlow.push_back(DeltaPhiMin);
       
        Cut* DeltaPhiMin = NewCut("DeltaPhiMinJetsMeT", Cut::CutType::CutFloat, true);
        if (!DeltaPhiMin->initialize("AbsDeltaPhiMinJetsMeT", ">0.4")) return StatusCode::FAILURE;
        SRCutFlow.push_back(DeltaPhiMin);
	 //Cut* MetTrack30 = NewCut("MetTrack > 30", Cut::CutType::CutXAMPPmet, false);
        //if (!MetTrack30->initialize("MetTrack", ">=30000.")) return StatusCode::FAILURE;
        //SRCutFlow.push_back(MetTrack30);
        //Cut* DPhiMetTrackMet = NewCut("DPhiMetTrackMet", Cut::CutType::CutFloat, false);
        //if (!DPhiMetTrackMet->initialize("AbsDPhiMetTrackMet", "<=1.047198")) return StatusCode::FAILURE; // TMath::Pi() / 3.
        //SRCutFlow.push_back(DPhiMetTrackMet);


        // Finally, add the CutFlow to the analysis
        ATH_CHECK(AddToCutFlows(SRCutFlow));

        return StatusCode::SUCCESS;
    }
}
