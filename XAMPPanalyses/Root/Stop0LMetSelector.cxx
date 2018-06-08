#include <SUSYTools/ISUSYObjDef_xAODTool.h>
#include <XAMPPanalyses/Stop0LMetSelector.h>
#include <XAMPPbase/IElectronSelector.h>
#include <XAMPPbase/IJetSelector.h>
#include <XAMPPbase/IMuonSelector.h>
#include <XAMPPbase/IPhotonSelector.h>
#include <XAMPPbase/ITauSelector.h>
#include <XAMPPbase/EventInfo.h>

#include <XAMPPbase/AnalysisUtils.h>
#include <XAMPPbase/Defs.h>
namespace XAMPP {
    Stop0LMetSelector::Stop0LMetSelector(std::string myname) :
                SUSYMetSelector(myname),
                MetElInvisible(NULL),
                MetElInvisibleAux(NULL),
                MetMuInvisible(NULL),
                MetMuInvisibleAux(NULL),
                MetGammaInvisible(NULL),
                MetGammaInvisibleAux(NULL) {
    }

    StatusCode Stop0LMetSelector::initialize() {
        if (isInitialized()) {
            return StatusCode::SUCCESS;
        }
        ATH_CHECK(SUSYMetSelector::initialize());

        ATH_CHECK(m_XAMPPInfo->NewEventVariable < XAMPPmet > ("MetElInv"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable < XAMPPmet > ("MetMuInv"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable < XAMPPmet > ("MetLepInv"));

        // Save more met info
        ATH_CHECK(m_XAMPPInfo->NewEventVariable < XAMPPmet > ("MetTST_JetTerm"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable < XAMPPmet > ("MetTST_ElecTerm"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable < XAMPPmet > ("MetTST_MuonTerm"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable < XAMPPmet > ("MetTST_SoftTerm"));

        if (m_IncludePhotons) {
            ATH_CHECK(m_XAMPPInfo->NewEventVariable < XAMPPmet > ("MetGammaInv"));
            ATH_CHECK(m_XAMPPInfo->NewEventVariable < XAMPPmet > ("MetTST_GammaTerm"));
        }

//        ATH_CHECK(m_XAMPPInfo->NewEventVariable<XAMPPmet>("MetTST_TauTerm"));

        return StatusCode::SUCCESS;
    }

    StatusCode Stop0LMetSelector::FillMet(const CP::SystematicSet &systset) {

        ATH_CHECK(SUSYMetSelector::FillMet(systset));
        ATH_MSG_DEBUG("Stop0LMetSelector::FillMet...");

        ATH_CHECK(CreateContainer("METElInvisibleContainers_", MetElInvisible, MetElInvisibleAux));
        ATH_CHECK(CreateContainer("METMuInvisibleContainers_", MetMuInvisible, MetMuInvisibleAux));

        if (m_elec_selection->GetSignalElectrons()->size() == 2 && m_muon_selection->GetSignalMuons()->size() == 0) {

            //TST softterm, SignalElectrons marked as Invisible
            if (!m_susytools->GetMET(*MetElInvisible, m_jet_selection->GetJets(), m_elec_selection->GetPreElectrons(), m_muon_selection->GetPreMuons(), m_MetPhotons, m_MetTaus, true, true, m_elec_selection->GetSignalElectrons()).isSuccess()) {
                ATH_MSG_FATAL("Could not reconstruct the event METElInvisible, exiting");
                return StatusCode::FAILURE;
            }
            MetMuInvisible = NULL;
        } else if (m_elec_selection->GetSignalElectrons()->size() == 0 && m_muon_selection->GetSignalMuons()->size() == 2) {

            //TST softterm, SignalMuons marked as Invisible
            if (!m_susytools->GetMET(*MetMuInvisible, m_jet_selection->GetJets(), m_elec_selection->GetPreElectrons(), m_muon_selection->GetPreMuons(), m_MetPhotons, m_MetTaus, true, true, m_muon_selection->GetSignalMuons()).isSuccess()) {
                ATH_MSG_FATAL("Could not reconstruct the event METMuInvisible, exiting");
                return StatusCode::FAILURE;
            }
            MetElInvisible = NULL;
        } else {
            MetElInvisible = NULL;
            MetMuInvisible = NULL;
        }

        static XAMPP::Storage<XAMPPmet>* s_MetElInv = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetElInv");
        static XAMPP::Storage<XAMPPmet>* s_MetMuInv = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetMuInv");
        static XAMPP::Storage<XAMPPmet>* s_MetLepInv = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetLepInv");

        xAOD::MissingETContainer* MetLepInvisible = MetElInvisible ? MetElInvisible : MetMuInvisible;
        ATH_CHECK(s_MetElInv->Store(XAMPP::GetMET_obj("Final", MetElInvisible)));
        ATH_CHECK(s_MetMuInv->Store(XAMPP::GetMET_obj("Final", MetMuInvisible)));
        ATH_CHECK(s_MetLepInv->Store(XAMPP::GetMET_obj("Final", MetLepInvisible)));

        if (m_IncludePhotons) {
            ATH_CHECK(CreateContainer("METGammaInvisibleContainers_", MetGammaInvisible, MetGammaInvisibleAux));
            if (!m_susytools->GetMET(*MetGammaInvisible, m_jet_selection->GetJets(), m_elec_selection->GetPreElectrons(), m_muon_selection->GetPreMuons(), m_MetPhotons, m_MetTaus, true, true, m_phot_selection->GetSignalPhotons()).isSuccess()) {
                ATH_MSG_FATAL("Could not reconstruct the event MetGammaInvisible, exiting");
                return StatusCode::FAILURE;
            }
            static XAMPP::Storage<XAMPPmet>* s_MetGammaInv = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetGammaInv");
            ATH_CHECK(s_MetGammaInv->Store(XAMPP::GetMET_obj("Final", MetGammaInvisible)));
            static XAMPP::Storage<XAMPPmet>* s_MetTST_GammaTerm = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetTST_GammaTerm");
            ATH_CHECK(s_MetTST_GammaTerm->Store(XAMPP::GetMET_obj("RefGamma", MetTST)));
        }

        // now create decorators which are pointers to the MET objects itself
        static XAMPP::Storage<XAMPPmet>* s_MetTST_JetTerm = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetTST_JetTerm");
        static XAMPP::Storage<XAMPPmet>* s_MetTST_ElecTerm = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetTST_ElecTerm");
        static XAMPP::Storage<XAMPPmet>* s_MetTST_MuonTerm = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetTST_MuonTerm");
        static XAMPP::Storage<XAMPPmet>* s_MetTST_SoftTerm = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetTST_SoftTerm");
//        static XAMPP::Storage<XAMPPmet>* s_MetTST_TauTerm = m_XAMPPInfo->GetVariableStorage < XAMPPmet > ("MetTST_TauTerm");

        ATH_CHECK(s_MetTST_JetTerm->Store(XAMPP::GetMET_obj("RefJet", MetTST)));
        ATH_CHECK(s_MetTST_ElecTerm->Store(XAMPP::GetMET_obj("RefEle", MetTST)));
        ATH_CHECK(s_MetTST_MuonTerm->Store(XAMPP::GetMET_obj("Muons", MetTST)));
        ATH_CHECK(s_MetTST_SoftTerm->Store(XAMPP::GetMET_obj("PVSoftTrk", MetTST)));
//        ATH_CHECK(s_MetTST_TauTerm->Store(XAMPP::GetMET_obj("RefTau", MetTST)));
        return StatusCode::SUCCESS;
    }
}
