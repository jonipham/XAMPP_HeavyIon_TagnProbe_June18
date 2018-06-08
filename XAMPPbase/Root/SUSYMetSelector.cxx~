#include <XAMPPbase/SUSYMetSelector.h>

#include <XAMPPbase/AnalysisUtils.h>
#include <XAMPPbase/Defs.h>

#include <XAMPPbase/IElectronSelector.h>
#include <XAMPPbase/IJetSelector.h>
#include <XAMPPbase/IMuonSelector.h>
#include <XAMPPbase/IPhotonSelector.h>
#include <XAMPPbase/ITauSelector.h>
#include <XAMPPbase/EventInfo.h>
namespace XAMPP {

    SUSYMetSelector::SUSYMetSelector(std::string myname) :
                AsgTool(myname),
                m_XAMPPInfo(),
                m_susytools("SUSYTools"),
                m_InfoHandle("EventInfoHandler"),
#ifdef XAOD_STANDALONE
                m_output_level(MSG::INFO),
#endif
                m_init(false),
                m_doTrackMet(false),
                m_IncludePhotons(false),
                m_IncludeTaus(false),
                m_MetTaus(NULL),
                m_MetPhotons(NULL),
                m_elec_selection("SUSYElectronSelector"),
                m_muon_selection("SUSYMuonSelector"),
                m_jet_selection("SUSYJetSelector"),
                m_phot_selection("SUSYPhotonSelector"),
                m_tau_selection("SUSYTauSelector"),
                m_systName() {
#ifdef XAOD_STANDALONE
        declareProperty("OutputLevel", m_output_level);
#endif
        declareProperty("DoTrackMet", m_doTrackMet = false);
        declareProperty("IncludeTaus", m_IncludeTaus = false);
        declareProperty("IncludePhotons", m_IncludePhotons = false);
        m_susytools.declarePropertyFor(this, "SUSYTools", "The SUSYTools instance");
        m_InfoHandle.declarePropertyFor(this, "EventInfoHandler", "The XAMPP EventInfo handle");

    }
    StatusCode SUSYMetSelector::initialize() {
        if (isInitialized()) {
            return StatusCode::SUCCESS;
        }

        ATH_MSG_INFO("Initialising...");

        ATH_CHECK(m_susytools.retrieve());
        ATH_CHECK(m_elec_selection.retrieve());
        ATH_CHECK(m_jet_selection.retrieve());
        ATH_CHECK(m_muon_selection.retrieve());
        ATH_CHECK(m_phot_selection.retrieve());
        ATH_CHECK(m_tau_selection.retrieve());

        ATH_CHECK(m_InfoHandle.retrieve());
        m_XAMPPInfo = dynamic_cast<XAMPP::EventInfo*>(m_InfoHandle.operator->());

        ATH_CHECK(m_XAMPPInfo->NewEventVariable<XAMPPmet>("MetTST"));
        ATH_CHECK(m_XAMPPInfo->NewEventVariable<XAMPPmet>("MetCST"));
        if (m_doTrackMet) ATH_CHECK(m_XAMPPInfo->NewEventVariable<XAMPPmet>("MetTrack"));

#ifdef XAOD_STANDALONE
        this->msg().setLevel(XAMPP::setOutputLevel(m_output_level));
#endif
        m_init = true;
        return StatusCode::SUCCESS;
    }
    bool SUSYMetSelector::isInitialized() const {
        return m_init;
    }
    StatusCode SUSYMetSelector::LoadContainers() {

        ATH_CHECK(evtStore()->retrieve(xAODMet, "MET_Reference_AntiKt4EMTopo"));
        if (m_doTrackMet) ATH_CHECK(evtStore()->retrieve(xAODTrackMet, "MET_Track"));
        return StatusCode::SUCCESS;
    }

    xAOD::MissingETContainer* SUSYMetSelector::GetCustomMet(std::string kind) {
        std::cout << kind << std::endl;
        return MetTST;

    }
    StatusCode SUSYMetSelector::CreateContainer(std::string Name, xAOD::MissingETContainer* &Cont, xAOD::MissingETAuxContainer* &Aux) {
        std::string storeName = name() + Name + m_systName;
        Cont = new xAOD::MissingETContainer;
        Aux = new xAOD::MissingETAuxContainer;
        Cont->setStore(Aux);
        ATH_CHECK(evtStore()->record(Cont, storeName));
        ATH_CHECK(evtStore()->record(Aux, storeName + "Aux."));
        return StatusCode::SUCCESS;
    }
    StatusCode SUSYMetSelector::FillMet(const CP::SystematicSet &systset) {

        ATH_CHECK(m_XAMPPInfo->SetSystematic(&systset));
        m_systName = systset.name();

        ATH_CHECK(CreateContainer("METContainers_", MetTST, MetTSTAux));
        ATH_CHECK(CreateContainer("METContainers_CST_", MetCST, MetCSTAux));
        ATH_CHECK(CreateContainer("METContainers_Track_", MetTrack, MetTrackAux));

        m_MetTaus = m_IncludeTaus ? m_tau_selection->GetPreTaus() : NULL;
        m_MetPhotons = m_IncludePhotons ? m_phot_selection->GetPrePhotons() : NULL;

        ATH_CHECK(m_susytools->GetMET(*MetTST, m_jet_selection->GetJets(), m_elec_selection->GetPreElectrons(), m_muon_selection->GetPreMuons(), m_MetPhotons, m_MetTaus, true));

        ATH_CHECK(m_susytools->GetMET(*MetCST, m_jet_selection->GetJets(), m_elec_selection->GetPreElectrons(), m_muon_selection->GetPreMuons(), m_MetPhotons, m_MetTaus, false, false));

        if (m_doTrackMet) {
            static XAMPP::Storage<XAMPPmet>* s_MetTrack = m_XAMPPInfo->GetVariableStorage<XAMPPmet>("MetTrack");
            ATH_CHECK(m_susytools->GetTrackMET(*MetTrack, m_jet_selection->GetJets(), m_elec_selection->GetPreElectrons(), m_muon_selection->GetPreMuons()));
            ATH_CHECK(s_MetTrack->Store(XAMPP::GetMET_obj("Track", MetTrack)));
        }

        // now create decorators which are pointers to the MET objects itself
        static XAMPP::Storage<XAMPPmet>* s_MetTST = m_XAMPPInfo->GetVariableStorage<XAMPPmet>("MetTST");
        static XAMPP::Storage<XAMPPmet>* s_MetCST = m_XAMPPInfo->GetVariableStorage<XAMPPmet>("MetCST");

        ATH_CHECK(s_MetTST->Store(XAMPP::GetMET_obj("Final", MetTST)));
        ATH_CHECK(s_MetCST->Store(XAMPP::GetMET_obj("Final", MetCST)));

        // static XAMPP::Storage<char>* dec_passTSTCleaning = m_XAMPPInfo->GetVariableStorage<char>("passTSTCleaning");
        // ATH_CHECK(dec_passTSTCleaning->Store(m_susytools->passTSTCleaning(*MetTST)));

        return StatusCode::SUCCESS;

    }
}
