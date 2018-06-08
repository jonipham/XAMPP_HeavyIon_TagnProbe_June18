#include <XAMPPanalyses/HeavyIonAnalysisConfig.h>

namespace XAMPP {
    HeavyIonAnalysisConfig::HeavyIonAnalysisConfig(std::string Analysis) :
            AnalysisConfig(Analysis) {
    }

    StatusCode HeavyIonAnalysisConfig::initializeCustomCuts() {

        // --------------------------------------
        // Signal regions
        // --------------------------------------

  
        CutFlow OneLCRCutFlow("HeavyIon");

	Cut* OneSignalLepton = NewCut(">=1 SigLep", Cut::CutType::CutInt, true);
	////if (!OneSignalLepton->initialize("N_SignalLeptons", "==1") && !OneSignalLepton->initialize("N_SignalLeptons", "==2") && !OneSignalLepton->initialize("N_SignalLeptons", "==3")) return StatusCode::FAILURE;
	if (!OneSignalLepton->initialize("N_SignalLeptons", ">0")) return StatusCode::FAILURE;
	Cut* OneBaselineLepton = NewCut(">=1 BaseLep", Cut::CutType::CutInt, true);
	////if (!OneBaselineLepton->initialize("N_BaselineLeptons", "==1") && !OneBaselineLepton->initialize("N_BaselineLeptons", "==2") && !OneBaselineLepton->initialize("N_BaselineLeptons", "==3")) return StatusCode::FAILURE;
	if (!OneBaselineLepton->initialize("N_BaselineLeptons", ">0")) return StatusCode::FAILURE; 
	Cut* OneLepton = OneSignalLepton->combine(OneBaselineLepton, Cut::Combine::AND);
	OneLCRCutFlow.push_back(OneLepton);
	//OneLCRCutFlow.push_back(OneBaselineLepton);
	//OneLCRCutFlow.push_back(OneSignalLepton);

	// for cutflow comparisons, also require same flavour
	//Cut* IsSFpair = NewCut("IsSF", Cut::CutType::CutChar, true);
	//if (!IsSFpair->initialize("IsSF", "==1")) return StatusCode::FAILURE;
	//TwoLCRCutFlow.push_back(IsSFpair);

	//Cut* NJetsCut = NewCut(">=1 jets", Cut::CutType::CutInt, true);
	//if (!NJetsCut->initialize("N_Jets", ">", ">==1")) return StatusCode::FAILURE;
	//TwoLCRCutFlow.push_back(NJetsCut);
	
	//Cut* IsOSpair = NewCut("IsOS", Cut::CutType::CutChar, true);
	//if (!IsOSpair->initialize("IsOS", "==1")) return StatusCode::FAILURE;
	//TwoLCRCutFlow.push_back(IsOSpair);
	//Cut* InZWindow = NewCut("ZWindow", Cut::CutType::CutFloat, true);
	//if (!InZWindow->initialize("ZWindow", "<=10000.")) return StatusCode::FAILURE;
	//TwoLCRCutFlow.push_back(InZWindow);
	//Cut* Dump2LCut = NewCut(">=1 jets", Cut::CutType::CutInt, true);
	//if (!Dump2LCut->initialize("N_Jets", ">=1")) return StatusCode::FAILURE;
	//TwoLCRCutFlow.push_back(Dump2LCut);

	ATH_CHECK(AddToCutFlows(OneLCRCutFlow));
	

        return StatusCode::SUCCESS;

    }
}
