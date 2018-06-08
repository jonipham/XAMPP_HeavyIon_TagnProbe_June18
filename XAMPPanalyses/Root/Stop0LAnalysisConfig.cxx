#include <XAMPPanalyses/Stop0LAnalysisConfig.h>

namespace XAMPP {
    Stop0LAnalysisConfig::Stop0LAnalysisConfig(std::string Analysis) :
                    AnalysisConfig(Analysis) {
        declareProperty("doISR", m_doISR = false);
        declareProperty("DoTrackMet", m_doTrackMet = true);
        declareProperty("DoReclustering", m_doReclustering = true);
    }

    StatusCode Stop0LAnalysisConfig::initializeCustomCuts() {

        // --------------------------------------
        // 0-lepton regions
        // --------------------------------------

        CutFlow SRCutFlow("Stop0L_SR");

        Cut* MetSampleOR = NewCut("MetSampleOR", Cut::CutType::CutChar, true);
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

        // now add a cut on the pt of the leading jet, which is not a 'Real' cut, meaning that it only gets applied in the cutflow histogram but not for the dump of the ntuple
        // XAMPP also supports now the direct cut on scalar variables of the n-th particle
        Cut* Jet1Pt = NewCut("Jet1Pt > 80", Cut::CutType::CutPartFloat, false);
        if (!Jet1Pt->initialize("Jet pt[0]", ">=80000")) return StatusCode::FAILURE;
        SRCutFlow.push_back(Jet1Pt);
        Cut* Jet2Pt = NewCut("Jet2Pt > 80", Cut::CutType::CutPartFloat, false);
        if (!Jet2Pt->initialize("Jet pt[1]", ">=80000")) return StatusCode::FAILURE;
        SRCutFlow.push_back(Jet2Pt);
        Cut* Jet3Pt = NewCut("Jet3Pt > 40", Cut::CutType::CutPartFloat, false);
        if (!Jet3Pt->initialize("Jet pt[2]", ">=80000")) return StatusCode::FAILURE;
        SRCutFlow.push_back(Jet3Pt);
        Cut* Jet4Pt = NewCut("Jet4Pt > 40", Cut::CutType::CutPartFloat, false);
        if (!Jet4Pt->initialize("Jet pt[3]", ">=40000")) return StatusCode::FAILURE;
        SRCutFlow.push_back(Jet4Pt);

        Cut* OneBJet = NewCut(">=1 b-jets", Cut::CutType::CutInt, false);
        if (!OneBJet->initialize("N_BJets", ">=1")) return StatusCode::FAILURE;
        SRCutFlow.push_back(OneBJet);

        Cut* MetTST250 = NewCut("MetTST>250", Cut::CutType::CutXAMPPmet, false);
        if (!MetTST250->initialize("MetTST", ">=250000.")) return StatusCode::FAILURE;
        SRCutFlow.push_back(MetTST250);

        Cut* DeltaPhiMin = NewCut("DeltaPhiMin2", Cut::CutType::CutFloat, false);
        if (!DeltaPhiMin->initialize("AbsDeltaPhiMin2", ">=0.4")) return StatusCode::FAILURE;
        SRCutFlow.push_back(DeltaPhiMin);
        Cut* MetTrack30 = 0;
        if (m_doTrackMet) {
            MetTrack30 = NewCut("MetTrack > 30", Cut::CutType::CutXAMPPmet, false);
            if (!MetTrack30->initialize("MetTrack", ">=30000.")) return StatusCode::FAILURE;
            SRCutFlow.push_back(MetTrack30);
        }
        Cut* DPhiMetTrackMet = NewCut("DPhiMetTrackMet", Cut::CutType::CutFloat, false);
        if (!DPhiMetTrackMet->initialize("AbsDPhiMetTrackMet", "<=1.047198")) return StatusCode::FAILURE; // TMath::Pi() / 3.
        SRCutFlow.push_back(DPhiMetTrackMet);

        Cut* TwoBJets = NewCut(">=2 b-jets", Cut::CutType::CutInt, false);
        if (!TwoBJets->initialize("N_BJets", ">=2")) return StatusCode::FAILURE;
        SRCutFlow.push_back(TwoBJets);

        // Now, do the 0L dump: Met>200 + >3 jets + >0 b-jets
        Cut* DumpMetTST200 = NewCut("MetTST>200", Cut::CutType::CutXAMPPmet, true);
        if (!DumpMetTST200->initialize("MetTST", ">=200000.")) return StatusCode::FAILURE;
        Cut* Dump4Jets = NewCut(">=4 jets", Cut::CutType::CutInt, true);
        if (!Dump4Jets->initialize("N_Jets", ">=4")) return StatusCode::FAILURE;
        Cut* Dump1BJet = NewCut(">=1 b-jets", Cut::CutType::CutInt, true);
        if (!Dump1BJet->initialize("N_BJets", ">=1")) return StatusCode::FAILURE;
        Cut* Dump0LCut = DumpMetTST200->combine(Dump4Jets, Cut::Combine::AND);
        Dump0LCut = Dump0LCut->combine(Dump1BJet, Cut::Combine::AND);
        SRCutFlow.push_back(Dump0LCut);

        Cut* TauVeto = NewCut("Tau veto", Cut::CutType::CutChar, false);
        if (!TauVeto->initialize("HasTauCand", "==0")) return StatusCode::FAILURE;
        SRCutFlow.push_back(TauVeto);
        Cut* Mtbmin200 = NewCut("MtBMin > 200", Cut::CutType::CutFloat, false);
        if (!Mtbmin200->initialize("MtBMin", ">=200000.")) return StatusCode::FAILURE;
        SRCutFlow.push_back(Mtbmin200);

        Cut* Top1CandM50 = NewCut("Top1CandM>50", Cut::CutType::CutFloat, false);
        if (!Top1CandM50->initialize("Top1CandM", ">=50000.")) return StatusCode::FAILURE;
        Cut* Top1CandM250 = NewCut("Top1CandM<250", Cut::CutType::CutFloat, false);
        if (!Top1CandM250->initialize("Top1CandM", "<=250000.")) return StatusCode::FAILURE;
        Cut* Top1CandM50_250 = Top1CandM50->combine(Top1CandM250, Cut::Combine::AND);
        SRCutFlow.push_back(Top1CandM50_250);

        Cut* Top2CandM50 = NewCut("Top2CandM>50", Cut::CutType::CutFloat, false);
        if (!Top2CandM50->initialize("Top2CandM", ">=50000.")) return StatusCode::FAILURE;
        Cut* Top2CandM400 = NewCut("Top2CandM<400", Cut::CutType::CutFloat, false);
        if (!Top2CandM400->initialize("Top2CandM", "<=400000.")) return StatusCode::FAILURE;
        Cut* Top2CandM50_400 = Top2CandM50->combine(Top2CandM400, Cut::Combine::AND);
        SRCutFlow.push_back(Top2CandM50_400);

        Cut* MinMtJets = NewCut("MinMtJets > 50", Cut::CutType::CutFloat, false);
        if (!MinMtJets->initialize("MinMt", ">=50000.")) return StatusCode::FAILURE;
        SRCutFlow.push_back(MinMtJets);
        Cut* Met350 = NewCut("Met > 350", Cut::CutType::CutXAMPPmet, false);
        if (!Met350->initialize("MetTST", ">=350000.")) return StatusCode::FAILURE;
        SRCutFlow.push_back(Met350);

        if (isActive(SRCutFlow)) { // check this in order to avoid initializing a cut on a trigger string which is not available during runtime
            Cut* MetTrigger = NewCut("MetTrigger", Cut::CutType::CutChar, false);
            if (!MetTrigger->initialize("TrigHLT_xe70_tc_lcw", "==1")) return StatusCode::FAILURE;
            for (auto &cut : m_StandardCuts) {
                if (cut->GetName() == "Trigger") cut = cut->combine(MetTrigger, Cut::Combine::AND);
            }
        }

        // Finally, add the CutFlow to the analysis
        ATH_CHECK(AddToCutFlows(SRCutFlow));

        // ----------------------------------------------------------------------------------
        CutFlow SRHardCutFlow("Stop0L_SRHard");

        SRHardCutFlow.push_back(MetSampleOR);
        SRHardCutFlow.push_back(LeptonVeto);
        SRHardCutFlow.push_back(Jet1Pt);
        SRHardCutFlow.push_back(Jet2Pt);
        SRHardCutFlow.push_back(Jet3Pt);
        SRHardCutFlow.push_back(Jet4Pt);
        SRHardCutFlow.push_back(OneBJet);

        SRHardCutFlow.push_back(MetTST250);

        // Now, do the 0L dump: Met>200 + >3 jets + >0 b-jets
        SRHardCutFlow.push_back(Dump0LCut);

        SRHardCutFlow.push_back(DeltaPhiMin);
        if (m_doTrackMet) SRHardCutFlow.push_back(MetTrack30);
        SRHardCutFlow.push_back(DPhiMetTrackMet);

        SRHardCutFlow.push_back(TwoBJets);

        if (m_doReclustering) {
            Cut* LeadingFatJet12M120 = NewCut("LeadingFatJet12M > 120", Cut::CutType::CutPartFloat, false);
            if (!LeadingFatJet12M120->initialize("FatJet12 m[0]", ">=120000")) return StatusCode::FAILURE;
//            if (!LeadingFatJet12M120->initialize("LeadingFatJet12M", ">=120000.")) return StatusCode::FAILURE;
            SRHardCutFlow.push_back(LeadingFatJet12M120);
            Cut* SecondFatJet12M120 = NewCut("SecLeadingFatJet12M>120", Cut::CutType::CutPartFloat, false);
            if (!SecondFatJet12M120->initialize("FatJet12 m[1]", ">=120000")) return StatusCode::FAILURE;
//            if (!SecondFatJet12M120->initialize("SecLeadingFatJet12M", ">=120000.")) return StatusCode::FAILURE;
            SRHardCutFlow.push_back(SecondFatJet12M120);

            Cut* LeadingFatJet8M60 = NewCut("LeadingFatJet8M > 60", Cut::CutType::CutPartFloat, false);
            if (!LeadingFatJet8M60->initialize("FatJet08 m[0]", ">=60000")) return StatusCode::FAILURE;
//            if (!LeadingFatJet8M60->initialize("LeadingFatJet8M", ">=60000.")) return StatusCode::FAILURE;
            SRHardCutFlow.push_back(LeadingFatJet8M60);
        }

        SRHardCutFlow.push_back(Mtbmin200);
        SRHardCutFlow.push_back(TauVeto);

        Cut* MetTST450 = NewCut("MetTST>450", Cut::CutType::CutXAMPPmet, false);
        if (!MetTST450->initialize("MetTST", ">=450000.")) return StatusCode::FAILURE;
        SRHardCutFlow.push_back(MetTST450);

        Cut* dRBB1 = NewCut("dRBB > 1", Cut::CutType::CutFloat, false);
        if (!dRBB1->initialize("dRBB", ">1.")) return StatusCode::FAILURE;
        SRHardCutFlow.push_back(dRBB1);
        Cut* MT2_400 = NewCut("MT2 (chi2) > 400", Cut::CutType::CutFloat, false);
        if (!MT2_400->initialize("mt2", ">400000.")) return StatusCode::FAILURE;
        SRHardCutFlow.push_back(MT2_400);

        if (isActive(SRHardCutFlow)) { // check this in order to avoid initializing a cut on a trigger string which is not available during runtime
            Cut* MetTrigger = NewCut("MetTrigger", Cut::CutType::CutChar, false);
            if (!MetTrigger->initialize("TrigHLT_xe70_tc_lcw", "==1")) return StatusCode::FAILURE;
            for (auto &cut : m_StandardCuts) {
                if (cut->GetName() == "Trigger") cut = cut->combine(MetTrigger, Cut::Combine::AND);
            }
        }

        ATH_CHECK(AddToCutFlows(SRHardCutFlow));

        // ----------------------------------------------------------------------------------

        // --------------------------------------
        // 1-lepton regions
        // --------------------------------------

        CutFlow OneLCRCutFlow("Stop0L_1LCR");

        OneLCRCutFlow.push_back(MetSampleOR);

        Cut* OneSignalLepton = NewCut("1 signal lepton", Cut::CutType::CutInt, true);
        if (!OneSignalLepton->initialize("N_SignalLeptons", "==1")) return StatusCode::FAILURE;
        Cut* OneBaselineLepton = NewCut("1 baseline lepton", Cut::CutType::CutInt, true);
        if (!OneBaselineLepton->initialize("N_BaselineLeptons", "==1")) return StatusCode::FAILURE;
        Cut* OneLepton = OneSignalLepton->combine(OneBaselineLepton, Cut::Combine::AND);
        OneLCRCutFlow.push_back(OneLepton);

        // Now, do the 1L dump: >2 jets + >0 b-jets + Met > 200
        Cut* Dump3Jets = NewCut(">=3 jets", Cut::CutType::CutInt, true);
        if (!Dump3Jets->initialize("N_Jets", ">=3")) return StatusCode::FAILURE;
        Cut* Dump1LCut = Dump3Jets->combine(Dump1BJet, Cut::Combine::AND);
        Cut* DumpMet100 = NewCut("Met > 200", Cut::CutType::CutXAMPPmet, true);
        if (!DumpMet100->initialize("MetTST", ">=200000.")) return StatusCode::FAILURE;
        Dump1LCut = Dump1LCut->combine(DumpMet100, Cut::Combine::AND);
        OneLCRCutFlow.push_back(Dump1LCut);

        if (isActive(OneLCRCutFlow) && NumActiveCutFlows() == 1) { // check this in order to avoid initializing a cut on a trigger string which is not available during runtime
            Cut* MetTrigger = NewCut("MetTrigger", Cut::CutType::CutChar, false);
            if (!MetTrigger->initialize("TrigHLT_xe70_tc_lcw", "==1")) return StatusCode::FAILURE;
            for (auto &cut : m_StandardCuts) {
                if (cut->GetName() == "Trigger") cut = cut->combine(MetTrigger, Cut::Combine::AND);
            }
        }

        ATH_CHECK(AddToCutFlows(OneLCRCutFlow));

        // ----------------------------------------------------------------------------------

        // --------------------------------------
        // 2-lepton regions
        // --------------------------------------

        CutFlow TwoLCRCutFlow("Stop0L_2LCR");

        TwoLCRCutFlow.push_back(MetSampleOR);
        Cut* TreatAsYear15 = NewCut("2015", Cut::CutType::CutInt, false);
        if (!TreatAsYear15->initialize("treatAsYear", "=2015")) return StatusCode::FAILURE;
        Cut* TreatAsYear16 = NewCut("2016", Cut::CutType::CutInt, false);
        if (!TreatAsYear16->initialize("treatAsYear", "=2016")) return StatusCode::FAILURE;

        if (isActive(TwoLCRCutFlow) && NumActiveCutFlows() == 1) { // check this in order to avoid initializing a cut on a trigger string which is not available during runtime
            Cut* HLT_e24_lhmedium_L1EM18VH = NewCut("HLT_e24_lhmedium_L1EM18VH", Cut::CutType::CutChar, false);
            if (!HLT_e24_lhmedium_L1EM18VH->initialize("TrigHLT_e24_lhmedium_L1EM18VH", "=1")) return StatusCode::FAILURE;
            Cut* HLT_e120_lhloose = NewCut("HLT_e120_lhloose", Cut::CutType::CutChar, false);
            if (!HLT_e120_lhloose->initialize("TrigHLT_e120_lhloose", "=1")) return StatusCode::FAILURE;
            Cut* HLT_e60_lhmedium = NewCut("HLT_e60_lhmedium", Cut::CutType::CutChar, false);
            if (!HLT_e60_lhmedium->initialize("TrigHLT_e60_lhmedium", "=1")) return StatusCode::FAILURE;
            Cut* HLT_e26_lhtight_nod0_ivarloose = NewCut("HLT_e26_lhtight_nod0_ivarloose", Cut::CutType::CutChar, false);
            if (!HLT_e26_lhtight_nod0_ivarloose->initialize("TrigHLT_e26_lhtight_nod0_ivarloose", "=1")) return StatusCode::FAILURE;
            Cut* HLT_e140_lhloose_nod0 = NewCut("HLT_e140_lhloose_nod0", Cut::CutType::CutChar, false);
            if (!HLT_e140_lhloose_nod0->initialize("TrigHLT_e140_lhloose_nod0", "=1")) return StatusCode::FAILURE;
            Cut* HLT_e60_lhmedium_nod0 = NewCut("HLT_e60_lhmedium_nod0", Cut::CutType::CutChar, false);
            if (!HLT_e60_lhmedium_nod0->initialize("TrigHLT_e60_lhmedium_nod0", "=1")) return StatusCode::FAILURE;
            Cut* HLT_mu50 = NewCut("HLT_mu50", Cut::CutType::CutChar, false);
            if (!HLT_mu50->initialize("TrigHLT_mu50", "=1")) return StatusCode::FAILURE;
            Cut* HLT_mu20_iloose_L1MU15 = NewCut("HLT_mu20_iloose_L1MU15", Cut::CutType::CutChar, false);
            if (!HLT_mu20_iloose_L1MU15->initialize("TrigHLT_mu20_iloose_L1MU15", "=1")) return StatusCode::FAILURE;
            Cut* HLT_mu26_ivarmedium = NewCut("HLT_mu26_ivarmedium", Cut::CutType::CutChar, false);
            if (!HLT_mu26_ivarmedium->initialize("TrigHLT_mu26_ivarmedium", "=1")) return StatusCode::FAILURE;
            // 2015
            Cut* TwoLepTrigger15 = HLT_e24_lhmedium_L1EM18VH->combine(HLT_e120_lhloose, Cut::Combine::OR);
            TwoLepTrigger15 = TwoLepTrigger15->combine(HLT_e60_lhmedium, Cut::Combine::OR);
            TwoLepTrigger15 = TwoLepTrigger15->combine(HLT_mu50, Cut::Combine::OR);
            TwoLepTrigger15 = TwoLepTrigger15->combine(HLT_mu20_iloose_L1MU15, Cut::Combine::OR);
            TwoLepTrigger15 = TwoLepTrigger15->combine(TreatAsYear15, Cut::Combine::AND);
            // 2016
            Cut* TwoLepTrigger16 = HLT_e60_lhmedium_nod0->combine(HLT_e26_lhtight_nod0_ivarloose, Cut::Combine::OR);
            TwoLepTrigger16 = TwoLepTrigger16->combine(HLT_e140_lhloose_nod0, Cut::Combine::OR);
            TwoLepTrigger16 = TwoLepTrigger16->combine(HLT_mu50, Cut::Combine::OR);
            TwoLepTrigger16 = TwoLepTrigger16->combine(HLT_mu26_ivarmedium, Cut::Combine::OR);
            TwoLepTrigger16 = TwoLepTrigger16->combine(TreatAsYear16, Cut::Combine::AND);
            // combine 2015+2016 and add them to the Trigger cut
            Cut* TwoLepTrigger = TwoLepTrigger15->combine(TwoLepTrigger16, Cut::Combine::OR);
            for (auto &cut : m_StandardCuts) {
                if (cut->GetName() == "Trigger") {
                    cut = cut->combine(TwoLepTrigger, Cut::Combine::AND);
                }
            }
        }

        Cut* TwoSignalLeptons = NewCut("2 signal leptons", Cut::CutType::CutInt, true);
        if (!TwoSignalLeptons->initialize("N_SignalLeptons", "=2")) return StatusCode::FAILURE;
        Cut* TwoBaselineLeptons = NewCut("2 baseline leptons", Cut::CutType::CutInt, true);
        if (!TwoBaselineLeptons->initialize("N_BaselineLeptons", "=2")) return StatusCode::FAILURE;
        Cut* TwoLeptons = TwoSignalLeptons->combine(TwoBaselineLeptons, Cut::Combine::AND);
        TwoLCRCutFlow.push_back(TwoLeptons);

        // for cutflow comparisons, also require same flavour
        Cut* IsSFpair = NewCut("IsSF", Cut::CutType::CutChar, true);
        if (!IsSFpair->initialize("IsSF", "=1")) return StatusCode::FAILURE;
        TwoLCRCutFlow.push_back(IsSFpair);

        if (isActive(TwoLCRCutFlow) && NumActiveCutFlows() == 1) { // check this in order to avoid initializing a cut on a trigger string which is not available during runtime
            Cut* HLT_e24_lhmedium_L1EM18VH = NewCut("HLT_e24_lhmedium_L1EM18VH", Cut::CutType::CutChar, false);
            if (!HLT_e24_lhmedium_L1EM18VH->initialize("TrigMatchHLT_e24_lhmedium_L1EM18VH", "=1")) return StatusCode::FAILURE;
            Cut* HLT_e120_lhloose = NewCut("HLT_e120_lhloose", Cut::CutType::CutChar, false);
            if (!HLT_e120_lhloose->initialize("TrigMatchHLT_e120_lhloose", "=1")) return StatusCode::FAILURE;
            Cut* HLT_e60_lhmedium = NewCut("HLT_e60_lhmedium", Cut::CutType::CutChar, false);
            if (!HLT_e60_lhmedium->initialize("TrigMatchHLT_e60_lhmedium", "=1")) return StatusCode::FAILURE;
            Cut* HLT_e26_lhtight_nod0_ivarloose = NewCut("HLT_e26_lhtight_nod0_ivarloose", Cut::CutType::CutChar, false);
            if (!HLT_e26_lhtight_nod0_ivarloose->initialize("TrigMatchHLT_e26_lhtight_nod0_ivarloose", "=1")) return StatusCode::FAILURE;
            Cut* HLT_e140_lhloose_nod0 = NewCut("HLT_e140_lhloose_nod0", Cut::CutType::CutChar, false);
            if (!HLT_e140_lhloose_nod0->initialize("TrigMatchHLT_e140_lhloose_nod0", "=1")) return StatusCode::FAILURE;
            Cut* HLT_e60_lhmedium_nod0 = NewCut("HLT_e60_lhmedium_nod0", Cut::CutType::CutChar, false);
            if (!HLT_e60_lhmedium_nod0->initialize("TrigMatchHLT_e60_lhmedium_nod0", "=1")) return StatusCode::FAILURE;
            Cut* HLT_mu50 = NewCut("HLT_mu50", Cut::CutType::CutChar, false);
            if (!HLT_mu50->initialize("TrigMatchHLT_mu50", "=1")) return StatusCode::FAILURE;
            Cut* HLT_mu20_iloose_L1MU15 = NewCut("HLT_mu20_iloose_L1MU15", Cut::CutType::CutChar, false);
            if (!HLT_mu20_iloose_L1MU15->initialize("TrigMatchHLT_mu20_iloose_L1MU15", "=1")) return StatusCode::FAILURE;
            Cut* HLT_mu26_ivarmedium = NewCut("HLT_mu26_ivarmedium", Cut::CutType::CutChar, false);
            if (!HLT_mu26_ivarmedium->initialize("TrigMatchHLT_mu26_ivarmedium", "=1")) return StatusCode::FAILURE;
            // 2015
            Cut* TwoLepTrigger15 = HLT_e24_lhmedium_L1EM18VH->combine(HLT_e120_lhloose, Cut::Combine::OR);
            TwoLepTrigger15 = TwoLepTrigger15->combine(HLT_e60_lhmedium, Cut::Combine::OR);
            TwoLepTrigger15 = TwoLepTrigger15->combine(HLT_mu50, Cut::Combine::OR);
            TwoLepTrigger15 = TwoLepTrigger15->combine(HLT_mu20_iloose_L1MU15, Cut::Combine::OR);
            TwoLepTrigger15 = TwoLepTrigger15->combine(TreatAsYear15, Cut::Combine::AND);
            // 2016
            Cut* TwoLepTrigger16 = HLT_e60_lhmedium_nod0->combine(HLT_e26_lhtight_nod0_ivarloose, Cut::Combine::OR);
            TwoLepTrigger16 = TwoLepTrigger16->combine(HLT_e140_lhloose_nod0, Cut::Combine::OR);
            TwoLepTrigger16 = TwoLepTrigger16->combine(HLT_mu50, Cut::Combine::OR);
            TwoLepTrigger16 = TwoLepTrigger16->combine(HLT_mu26_ivarmedium, Cut::Combine::OR);
            TwoLepTrigger16 = TwoLepTrigger16->combine(TreatAsYear16, Cut::Combine::AND);
            // combine 2015+2016
            Cut* LeptonTrigMatching = TwoLepTrigger15->combine(TwoLepTrigger16, Cut::Combine::OR);
            TwoLCRCutFlow.push_back(LeptonTrigMatching);
        }

        Cut* MetLess50 = NewCut("Met < 50", Cut::CutType::CutXAMPPmet, false);
        if (!MetLess50->initialize("MetTST", "<=50000.")) return StatusCode::FAILURE;
        TwoLCRCutFlow.push_back(MetLess50);

        Cut* IsOSpair = NewCut("IsOS", Cut::CutType::CutChar, false);
        if (!IsOSpair->initialize("IsOS", "=1")) return StatusCode::FAILURE;
        TwoLCRCutFlow.push_back(IsOSpair);

        TwoLCRCutFlow.push_back(Jet1Pt);
        TwoLCRCutFlow.push_back(Jet2Pt);
        TwoLCRCutFlow.push_back(Jet3Pt);
        TwoLCRCutFlow.push_back(Jet4Pt);

        // Now, do the 2L dump: >1 jet
        Cut* Dump2LCut = NewCut(">=2 jets", Cut::CutType::CutInt, true);
        if (!Dump2LCut->initialize("N_Jets", ">=2")) return StatusCode::FAILURE;
        TwoLCRCutFlow.push_back(Dump2LCut);

        Cut* InZWindow = NewCut("ZWindow", Cut::CutType::CutFloat, false);
        if (!InZWindow->initialize("ZWindow", "<=5000.")) return StatusCode::FAILURE;
        TwoLCRCutFlow.push_back(InZWindow);

        Cut* MetInv70 = NewCut("MetLepInv > 70", Cut::CutType::CutXAMPPmet, false);
        if (!MetInv70->initialize("MetLepInv", ">=70000.")) return StatusCode::FAILURE;
        TwoLCRCutFlow.push_back(MetInv70);

        TwoLCRCutFlow.push_back(TwoBJets);

        ATH_CHECK(AddToCutFlows(TwoLCRCutFlow));

        if (m_doISR) {
            // --------------------------------------
            // ISR signal region
            // --------------------------------------

            CutFlow ISRSRCutFlow("Stop0L_ISR_SR");

            ISRSRCutFlow.push_back(MetSampleOR);
            ISRSRCutFlow.push_back(LeptonVeto);
            ISRSRCutFlow.push_back(Jet1Pt);
            ISRSRCutFlow.push_back(Jet2Pt);
            ISRSRCutFlow.push_back(Jet3Pt);
            ISRSRCutFlow.push_back(Jet4Pt);
            ISRSRCutFlow.push_back(OneBJet);
            ISRSRCutFlow.push_back(MetTST250);

            ISRSRCutFlow.push_back(DeltaPhiMin);
            if (m_doTrackMet) ISRSRCutFlow.push_back(MetTrack30);
            ISRSRCutFlow.push_back(DPhiMetTrackMet);

            ISRSRCutFlow.push_back(TwoBJets);

            // ISR variables
            Cut* DumpISR_NbV1 = NewCut("NbV >= 1", Cut::CutType::CutInt, true);
            if (!DumpISR_NbV1->initialize("ISR_NbV", ">=1")) return StatusCode::FAILURE;
            ISRSRCutFlow.push_back(DumpISR_NbV1);
            Cut* ISR_NjV = NewCut("NjV >= 5", Cut::CutType::CutInt, false);
            if (!ISR_NjV->initialize("ISR_NjV", ">=5")) return StatusCode::FAILURE;
            ISRSRCutFlow.push_back(ISR_NjV);
            Cut* ISR_MS = NewCut("MS > 300", Cut::CutType::CutFloat, false);
            if (!ISR_MS->initialize("ISR_MS", ">=300000.")) return StatusCode::FAILURE;
            ISRSRCutFlow.push_back(ISR_MS);
            Cut* ISR_dphiISRI = NewCut("dphiISRI > 3", Cut::CutType::CutFloat, false);
            if (!ISR_dphiISRI->initialize("ISR_dphiISRI", ">3.")) return StatusCode::FAILURE;
            ISRSRCutFlow.push_back(ISR_dphiISRI);
            Cut* ISR_PTISR = NewCut("PTISR > 400", Cut::CutType::CutFloat, false);
            if (!ISR_PTISR->initialize("ISR_PTISR", ">400000.")) return StatusCode::FAILURE;
            ISRSRCutFlow.push_back(ISR_PTISR);

            Cut* ISR_Vjet4Pt = NewCut("Vjet4Pt > 50", Cut::CutType::CutFloat, false);
            if (!ISR_Vjet4Pt->initialize("ISR_Vjet4Pt", ">50000.")) return StatusCode::FAILURE;
            ISRSRCutFlow.push_back(ISR_Vjet4Pt);

            Cut* ISR_RISR1 = NewCut("RISR > 0.25", Cut::CutType::CutFloat, false);
            if (!ISR_RISR1->initialize("ISR_RISR", ">0.25")) return StatusCode::FAILURE;
            Cut* ISR_RISR2 = NewCut("RISR < 0.4", Cut::CutType::CutFloat, false);
            if (!ISR_RISR2->initialize("ISR_RISR", "<0.4")) return StatusCode::FAILURE;
            Cut* ISR_RISR = ISR_RISR1->combine(ISR_RISR2, Cut::Combine::AND);
            ISRSRCutFlow.push_back(ISR_RISR);

            if (isActive(ISRSRCutFlow)) { // check this in order to avoid initializing a cut on a trigger string which is not available during runtime
                Cut* MetTrigger = NewCut("MetTrigger", Cut::CutType::CutChar, false);
                if (!MetTrigger->initialize("TrigHLT_xe70_tc_lcw", "==1")) return StatusCode::FAILURE;
                for (auto &cut : m_StandardCuts) {
                    if (cut->GetName() == "Trigger") cut = cut->combine(MetTrigger, Cut::Combine::AND);
                }
            }

            // Now, do the 0L dump: >3 jets + >0 b-jets + PTISR>0 + (NbV > 0)
            Cut* Dump0LCutISR = Dump4Jets->combine(Dump1BJet, Cut::Combine::AND);
            Cut* DumpISR_PTISR = NewCut("PTISR > 0", Cut::CutType::CutFloat, true);
            if (!DumpISR_PTISR->initialize("ISR_PTISR", ">0.")) return StatusCode::FAILURE;
            Dump0LCutISR = Dump0LCutISR->combine(DumpISR_PTISR, Cut::Combine::AND);
            ISRSRCutFlow.push_back(Dump0LCutISR);

            ATH_CHECK(AddToCutFlows(ISRSRCutFlow));

            CutFlow ISR_1LCRCutFlow("Stop0L_ISR_1LCR");

            ISR_1LCRCutFlow.push_back(MetSampleOR);

            Cut* OneSignalLepton = NewCut("1 signal lepton", Cut::CutType::CutInt, true);
            if (!OneSignalLepton->initialize("N_SignalLeptons", "==1")) return StatusCode::FAILURE;
            Cut* OneBaselineLepton = NewCut("1 baseline lepton", Cut::CutType::CutInt, true);
            if (!OneBaselineLepton->initialize("N_BaselineLeptons", "==1")) return StatusCode::FAILURE;
            Cut* OneLepton = OneSignalLepton->combine(OneBaselineLepton, Cut::Combine::AND);
            ISR_1LCRCutFlow.push_back(OneLepton);

            // Now, do the 1L dump: >2 jets + >0 b-jets + PTISR>0 + NbV > 0
            Cut* Dump3Jets = NewCut(">2 jets", Cut::CutType::CutInt, true);
            if (!Dump3Jets->initialize("N_Jets", ">=3")) return StatusCode::FAILURE;
            Cut* Dump1LCutISR = Dump3Jets->combine(Dump1BJet, Cut::Combine::AND);
            Dump1LCutISR->combine(DumpISR_PTISR, Cut::Combine::AND);
            Dump1LCutISR->combine(DumpISR_NbV1, Cut::Combine::AND);
            ISR_1LCRCutFlow.push_back(Dump1LCutISR);

            ATH_CHECK(AddToCutFlows(ISR_1LCRCutFlow));

        }

        return StatusCode::SUCCESS;
    }
    Stop0LGammaAnalysisConfig::Stop0LGammaAnalysisConfig(std::string Analysis) :
                    AnalysisConfig(Analysis) {
    }

    StatusCode Stop0LGammaAnalysisConfig::initializeCustomCuts() {

        // --------------------------------------
        // Gamma region
        // --------------------------------------

        CutFlow GCRCutFlow("Stop0L_1Y");

        Cut* MetSampleOR = NewCut("MetSampleOR", Cut::CutType::CutChar, true);
        if (!MetSampleOR->initialize("isMetSampleOR", "==0")) return StatusCode::FAILURE;
        // do not apply cut here for ttbar gamma CR cutflow comparison
        GCRCutFlow.push_back(MetSampleOR);

        Cut* SignalPhoton = NewCut("1 signal photon", Cut::CutType::CutInt, true);
        if (!SignalPhoton->initialize("N_SignalPhotons", "==1")) return StatusCode::FAILURE;
        Cut* BaselinePhoton = NewCut(">=1 baseline photon", Cut::CutType::CutInt, true);
        if (!BaselinePhoton->initialize("N_BaselinePhotons", "==1")) return StatusCode::FAILURE;
        Cut* OnePhoton = SignalPhoton->combine(BaselinePhoton, Cut::Combine::AND);
        GCRCutFlow.push_back(OnePhoton);

        Cut* OneSignalLepton = NewCut("1 signal lepton", Cut::CutType::CutInt, true);
        if (!OneSignalLepton->initialize("N_SignalLeptons", "==1")) return StatusCode::FAILURE;
        Cut* OneBaselineLepton = NewCut("1 baseline lepton", Cut::CutType::CutInt, true);
        if (!OneBaselineLepton->initialize("N_BaselineLeptons", "==1")) return StatusCode::FAILURE;
        Cut* OneLepton = OneSignalLepton->combine(OneBaselineLepton, Cut::Combine::AND);
        GCRCutFlow.push_back(OneLepton);

        Cut* PhotonTrigMatching = NewCut("Trigger match", Cut::CutType::CutChar, false);
        if (!PhotonTrigMatching->initialize("TrigMatchHLT_g120_loose", "==1")) return StatusCode::FAILURE;
        GCRCutFlow.push_back(PhotonTrigMatching);

        // now add a cut on the pt of the leading jet, which is not a 'Real' cut, meaning that it only gets applied in the cutflow histogram but not for the dump of the ntuple
        Cut* Jet1Pt = NewCut("Jet1Pt > 80", Cut::CutType::CutPartFloat, false);
        if (!Jet1Pt->initialize("Jet pt[0]", ">=80000")) return StatusCode::FAILURE;
        GCRCutFlow.push_back(Jet1Pt);
        Cut* Jet2Pt = NewCut("Jet2Pt > 50", Cut::CutType::CutPartFloat, false);
        if (!Jet2Pt->initialize("Jet pt[1]", ">=50000")) return StatusCode::FAILURE;
        GCRCutFlow.push_back(Jet2Pt);

        // Now, do the gamma dump: >3 jets + >1 b-jets
        Cut* DumpFourJets = NewCut(">=4 jets", Cut::CutType::CutInt, true);
        if (!DumpFourJets->initialize("N_Jets", ">=4")) return StatusCode::FAILURE;
        GCRCutFlow.push_back(DumpFourJets);
        Cut* DumpTwoBJets = NewCut(">=2 b-jets", Cut::CutType::CutInt, true);
        if (!DumpTwoBJets->initialize("N_BJets", ">=2")) return StatusCode::FAILURE;
        GCRCutFlow.push_back(DumpTwoBJets);

        Cut* Photon1Pt = NewCut("Photon1Pt > 150", Cut::CutType::CutPartFloat, false);
        if (!Photon1Pt->initialize("Phot pt[0]", ">=150000")) return StatusCode::FAILURE;
        GCRCutFlow.push_back(Photon1Pt);

        if (isActive(GCRCutFlow)) { // check this in order to avoid initializing a cut on a trigger string which is not available during runtime
            Cut* PhotonTrigger = NewCut("PhotonTrigger", Cut::CutType::CutChar, false);
            if (!PhotonTrigger->initialize("TrigHLT_g120_loose", "==1")) return StatusCode::FAILURE;
            for (auto &cut : m_StandardCuts) {
                if (cut->GetName() == "Trigger") cut = cut->combine(PhotonTrigger, Cut::Combine::AND);
            }
        }

        if (!isData()) {
            Cut* MEPhoton = NewCut("MEPhoton", Cut::CutType::CutChar, false);
            if (!MEPhoton->initialize("MEPhoton", "==1")) return StatusCode::FAILURE;
            GCRCutFlow.push_back(MEPhoton);
        }

        Cut* Met50 = NewCut("Met < 50", Cut::CutType::CutXAMPPmet, false);
        if (!Met50->initialize("MetTST", "<50000.")) return StatusCode::FAILURE;
        GCRCutFlow.push_back(Met50);

        Cut* MetInv70 = NewCut("MetLepInv > 70", Cut::CutType::CutXAMPPmet, false);
        if (!MetInv70->initialize("MetGammaInv", ">=70000.")) return StatusCode::FAILURE;
        GCRCutFlow.push_back(MetInv70);

        ATH_CHECK(AddToCutFlows(GCRCutFlow));

        return StatusCode::SUCCESS;

    }

    Stop0LTruthAnalysisConfig::Stop0LTruthAnalysisConfig(std::string Analysis) :
                    TruthAnalysisConfig(Analysis) {
        declareProperty("DoReclustering", m_doReclustering = true);
    }

    StatusCode Stop0LTruthAnalysisConfig::initializeCustomCuts() {

        // --------------------------------------
        // 0-lepton regions
        // --------------------------------------

        CutFlow SRA_TTCutFlow("Truth_SRA_TT");

        Cut* DumpLeptonVeto = NewCut("0 baseline leptons", Cut::CutType::CutInt, true);
        if (!DumpLeptonVeto->initialize("N_BaselineLeptons", "==0")) return StatusCode::FAILURE;
        SRA_TTCutFlow.push_back(DumpLeptonVeto);
        Cut* DumpFourJets = NewCut(">=4 jets", Cut::CutType::CutInt, true);
        if (!DumpFourJets->initialize("N_Jets", ">=4")) return StatusCode::FAILURE;
        SRA_TTCutFlow.push_back(DumpFourJets);
        Cut* Jet1Pt = NewCut("Jet1Pt > 80", Cut::CutType::CutPartFloat, false);
        if (!Jet1Pt->initialize("Jet pt[0]", ">=80000")) return StatusCode::FAILURE;
        SRA_TTCutFlow.push_back(Jet1Pt);
        Cut* Jet2Pt = NewCut("Jet2Pt > 80", Cut::CutType::CutPartFloat, false);
        if (!Jet2Pt->initialize("Jet pt[1]", ">=80000")) return StatusCode::FAILURE;
        SRA_TTCutFlow.push_back(Jet2Pt);
        Cut* Jet3Pt = NewCut("Jet3Pt > 40", Cut::CutType::CutPartFloat, false);
        if (!Jet3Pt->initialize("Jet pt[2]", ">=40000")) return StatusCode::FAILURE;
        SRA_TTCutFlow.push_back(Jet3Pt);
        Cut* Jet4Pt = NewCut("Jet4Pt > 40", Cut::CutType::CutPartFloat, false);
        if (!Jet4Pt->initialize("Jet pt[3]", ">=40000")) return StatusCode::FAILURE;
        SRA_TTCutFlow.push_back(Jet4Pt);
        Cut* Met250 = NewCut("Met > 250", Cut::CutType::CutXAMPPmet, false);
        if (!Met250->initialize("TruthMET", ">=250000.")) return StatusCode::FAILURE;
        SRA_TTCutFlow.push_back(Met250);
        Cut* SixJets = NewCut(">=6 jets", Cut::CutType::CutInt, false);
        if (!SixJets->initialize("N_Jets", ">=6")) return StatusCode::FAILURE;
        SRA_TTCutFlow.push_back(SixJets);
        Cut* TwoBJets = NewCut(">=2 b-jets", Cut::CutType::CutInt, false);
        if (!TwoBJets->initialize("N_BJets", ">=2")) return StatusCode::FAILURE;
        SRA_TTCutFlow.push_back(TwoBJets);
        Cut* Mtbmin = NewCut("MtBMin > 175", Cut::CutType::CutFloat, false);
        if (!Mtbmin->initialize("MtBMin", ">=175000.")) return StatusCode::FAILURE;
        SRA_TTCutFlow.push_back(Mtbmin);
        Cut* MinMtJets = NewCut("MinMtJets > 50", Cut::CutType::CutFloat, false);
        if (!MinMtJets->initialize("MinMt", ">=50000.")) return StatusCode::FAILURE;
        SRA_TTCutFlow.push_back(MinMtJets);
        Cut* MT2_400 = NewCut("MT2 (chi2) > 400", Cut::CutType::CutFloat, false);
        if (!MT2_400->initialize("mt2_chi2", ">400000.")) return StatusCode::FAILURE;
        SRA_TTCutFlow.push_back(MT2_400);

        ATH_CHECK(AddToCutFlows(SRA_TTCutFlow));

        CutFlow SRHighCutFlow("Truth_DMtt_SRHigh");

        SRHighCutFlow.push_back(DumpLeptonVeto);
        SRHighCutFlow.push_back(DumpFourJets);
        SRHighCutFlow.push_back(Jet1Pt);
        SRHighCutFlow.push_back(Jet2Pt);
        SRHighCutFlow.push_back(Jet3Pt);
        SRHighCutFlow.push_back(Jet4Pt);
        SRHighCutFlow.push_back(TwoBJets);
        Cut* AbsDeltaPhiMin4 = NewCut("AbsDeltaPhiMin4 > 0.4", Cut::CutType::CutFloat, false);
        if (!AbsDeltaPhiMin4->initialize("AbsDeltaPhiMin4", ">=0.4")) return StatusCode::FAILURE;
        SRHighCutFlow.push_back(AbsDeltaPhiMin4);
        if (m_doReclustering) {
            Cut* LeadingFatJet12M140 = NewCut("LeadingFatJet12M > 140", Cut::CutType::CutPartFloat, false);
            if (!LeadingFatJet12M140->initialize("FatJet12 m[0]", ">=140000")) return StatusCode::FAILURE;
            SRHighCutFlow.push_back(LeadingFatJet12M140);
            Cut* SecondFatJet12M60 = NewCut("SecLeadingFatJet12M>80", Cut::CutType::CutPartFloat, false);
            if (!SecondFatJet12M60->initialize("FatJet12 m[1]", ">=80000")) return StatusCode::FAILURE;
            SRHighCutFlow.push_back(SecondFatJet12M60);
        }
        Cut* Mtbmin200 = NewCut("MtBMin > 200", Cut::CutType::CutFloat, false);
        if (!Mtbmin200->initialize("MtBMin", ">=200000.")) return StatusCode::FAILURE;
        SRHighCutFlow.push_back(Mtbmin200);
        Cut* HtSig = NewCut("HtSig > 12", Cut::CutType::CutFloat, false);
        if (!HtSig->initialize("HtSig", ">=12.")) return StatusCode::FAILURE;
        SRHighCutFlow.push_back(HtSig);
        Cut* Met300 = NewCut("Met > 300", Cut::CutType::CutXAMPPmet, false);
        if (!Met300->initialize("TruthMET", ">=300000.")) return StatusCode::FAILURE;
        SRHighCutFlow.push_back(Met300);
        Cut* dRBB = NewCut("dRBB > 1.5", Cut::CutType::CutFloat, false);
        if (!dRBB->initialize("dRBB", ">=1.5")) return StatusCode::FAILURE;
        SRHighCutFlow.push_back(dRBB);

        ATH_CHECK(AddToCutFlows(SRHighCutFlow));

        // --------------------------------------
        // 1L region
        // --------------------------------------
        CutFlow Truth1LCutFlow("Truth_1L");

        Cut* OneSignalLepton = NewCut("1 signal lepton", Cut::CutType::CutInt, true);
        if (!OneSignalLepton->initialize("N_SignalLeptons", "==1")) return StatusCode::FAILURE;
        Cut* OneBaselineLepton = NewCut("1 baseline lepton", Cut::CutType::CutInt, true);
        if (!OneBaselineLepton->initialize("N_BaselineLeptons", "==1")) return StatusCode::FAILURE;
        Cut* DumpOneLepton = OneSignalLepton->combine(OneBaselineLepton, Cut::Combine::AND);
        Truth1LCutFlow.push_back(DumpOneLepton);

        Cut* DumpThreeJets = NewCut(">=3 jets", Cut::CutType::CutInt, true);
        if (!DumpThreeJets->initialize("N_Jets", ">=3")) return StatusCode::FAILURE;
        Truth1LCutFlow.push_back(DumpThreeJets);

        ATH_CHECK(AddToCutFlows(Truth1LCutFlow));

        // --------------------------------------
        // 2L region
        // --------------------------------------
        CutFlow Truth2LCutFlow("Truth_2L");

        Cut* TwoSignalLeptons = NewCut("1 signal lepton", Cut::CutType::CutInt, true);
        if (!TwoSignalLeptons->initialize("N_SignalLeptons", "==2")) return StatusCode::FAILURE;
        Cut* TwoBaselineLeptons = NewCut("1 baseline lepton", Cut::CutType::CutInt, true);
        if (!TwoBaselineLeptons->initialize("N_BaselineLeptons", "==2")) return StatusCode::FAILURE;
        Cut* DumpTwoLeptons = TwoSignalLeptons->combine(TwoBaselineLeptons, Cut::Combine::AND);
        Truth2LCutFlow.push_back(DumpTwoLeptons);

        Cut* DumpTwoJets = NewCut(">=2 jets", Cut::CutType::CutInt, true);
        if (!DumpTwoJets->initialize("N_Jets", ">=2")) return StatusCode::FAILURE;
        Truth2LCutFlow.push_back(DumpTwoJets);

        ATH_CHECK(AddToCutFlows(Truth2LCutFlow));


        // --------------------------------------
        // gamma region
        // --------------------------------------

        CutFlow GammaCRCutFlow("Truth_GammaCR");

        Cut* SignalPhoton = NewCut("1 signal photon", Cut::CutType::CutInt, true);
        if (!SignalPhoton->initialize("N_SignalPhotons", "==1")) return StatusCode::FAILURE;
        Cut* BaselinePhoton = NewCut(">=1 baseline photon", Cut::CutType::CutInt, true);
        if (!BaselinePhoton->initialize("N_BaselinePhotons", "==1")) return StatusCode::FAILURE;
        Cut* DumpOnePhoton = SignalPhoton->combine(BaselinePhoton, Cut::Combine::AND);
        GammaCRCutFlow.push_back(DumpOnePhoton);
        Cut* Phot1Pt = NewCut("Phot1Pt > 150", Cut::CutType::CutPartFloat, false);
        if (!Phot1Pt->initialize("Phot pt[0]", ">=150000")) return StatusCode::FAILURE;
        GammaCRCutFlow.push_back(Phot1Pt);
        GammaCRCutFlow.push_back(DumpOneLepton);
        Cut* Elec1Pt = NewCut("Elec1Pt > 28", Cut::CutType::CutPartFloat, false);
        if (!Elec1Pt->initialize("Elec pt[0]", ">=28000")) return StatusCode::FAILURE;
        Cut* Muon1Pt = NewCut("Muon1Pt > 28", Cut::CutType::CutPartFloat, false);
        if (!Muon1Pt->initialize("Muon pt[0]", ">=28000")) return StatusCode::FAILURE;
        Cut* Lep1Pt = Elec1Pt->combine(Muon1Pt, Cut::Combine::OR);
        GammaCRCutFlow.push_back(Lep1Pt);
        GammaCRCutFlow.push_back(DumpFourJets);
        GammaCRCutFlow.push_back(Jet1Pt);
        GammaCRCutFlow.push_back(Jet2Pt);
        GammaCRCutFlow.push_back(Jet3Pt);
        GammaCRCutFlow.push_back(Jet4Pt);
        GammaCRCutFlow.push_back(TwoBJets);

        ATH_CHECK(AddToCutFlows(GammaCRCutFlow));

        return StatusCode::SUCCESS;

    }
}
