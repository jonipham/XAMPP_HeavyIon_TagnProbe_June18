#include <xAODRootAccess/TEvent.h>
#include <XAMPPbase/IAnalysisHelper.h>
#include <XAMPPbase/IElectronSelector.h>
#include <XAMPPbase/IJetSelector.h>
#include <XAMPPbase/IMuonSelector.h>
#include <XAMPPbase/IMetSelector.h>
#include <XAMPPbase/IPhotonSelector.h>
#include <XAMPPbase/ITauSelector.h>
#include <XAMPPbase/ITruthSelector.h>
#include <XAMPPbase/ISystematics.h>
#include <XAMPPbase/AnalysisConfig.h>
#include <XAMPPbase/EventInfo.h>
#include <XAMPPbase/CxAODMaker.h>
#include <XAMPPbase/ReconstructedParticles.h>

namespace XAMPP {

    CxAODMaker::CxAODMaker(std::string Name) :
                AsgTool(Name),
                m_event(0),
                m_outFile(0),
                m_XAMPPInfo(0),
                m_config(0),
                m_config_name(""),
                m_systematics("SystematicsTool"),
                m_electron_selection(""),
                m_muon_selection(""),
                m_jet_selection(""),
                m_photon_selection(""),
                m_tau_selection(""),
                m_truth_selection(""),
                m_met_selection(""),
                m_helper("AnalysiHelper") {
        declareProperty("ElectronSelector", m_electron_selection);
        declareProperty("JetSelector", m_jet_selection);
        declareProperty("MetSelector", m_met_selection);
        declareProperty("MuonSelector", m_muon_selection);
        declareProperty("PhotonSelector", m_photon_selection);
        declareProperty("TauSelector", m_tau_selection);
        declareProperty("TruthSelector", m_truth_selection);
//            declareProperty("SUSYTriggerTool", m_triggers);
        declareProperty("AnalysisConfigName", m_config_name);
#ifdef XAOD_STANDALONE
        declareProperty("OutputFile", m_outFile);
#endif
    }
    CxAODMaker::~CxAODMaker() {

    }
    StatusCode CxAODMaker::initialize() {

        //Retrieve the Tool handles
        ATH_CHECK(m_systematics.retrieve());
        ATH_CHECK(m_electron_selection.retrieve());
        ATH_CHECK(m_jet_selection.retrieve());
        ATH_CHECK(m_muon_selection.retrieve());
        ATH_CHECK(m_photon_selection.retrieve());
        ATH_CHECK(m_tau_selection.retrieve());
        ATH_CHECK(m_truth_selection.retrieve());

#ifdef XAOD_STANDALONE
        m_config = asg::ToolStore::get < XAMPP::AnalysisConfig > (m_config_name);
        m_XAMPPInfo = asg::ToolStore::get < XAMPP::EventInfo > ("EventInfoHandler");
#endif
        if (!m_config) {
            ATH_MSG_ERROR("Failed to retrieve the AnalysisConfig");
            return StatusCode::FAILURE;
        }
        if (!m_XAMPPInfo) {
            ATH_MSG_ERROR("No XAMPP EventInfo has been found");
            return StatusCode::FAILURE;
        }
#ifdef XAOD_STANDALONE
        xAOD::TEvent* m_event = dynamic_cast< xAOD::TEvent* >( xAOD::TActiveEvent::event() );
        if( !m_event ) {
            ATH_MSG_ERROR( "Active xAOD::TEvent didn't have a callback for this tool" );
            return StatusCode::FAILURE;
        }
        if (!m_outFile) {
            ATH_MSG_ERROR("No OutFile has been defined");
            return StatusCode::FAILURE;
        }
        if (!m_event->writeTo(m_outFile).isSuccess()) {
            ATH_MSG_ERROR("Could not write to the OutFile");
            return StatusCode::FAILURE;
        }
#endif
        return StatusCode::SUCCESS;
    }
    StatusCode CxAODMaker::Finalize() {
        if (!m_event->finishWritingTo(m_outFile).isSuccess()) {
            ATH_MSG_ERROR("Something went wrong with writing the xAODs");
            return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
    }
    StatusCode CxAODMaker::SaveEvent() {
        if (!DumpEvent()) return StatusCode::SUCCESS;
        ATH_MSG_INFO("Will store the event");
        xAOD::ElectronContainer* Elecs;
        xAOD::ElectronAuxContainer* AuxElecs;
        CreateContainer(Elecs, AuxElecs);
#ifdef XAOD_STANDALONE
        ATH_CHECK(StoreContainer("Elec", Elecs, AuxElecs));
#endif
        return StatusCode::SUCCESS;
    }
    bool CxAODMaker::DumpEvent() {
        for (const auto& systset : m_systematics->GetKinematicSystematics()) {
            ATH_CHECK(m_XAMPPInfo->SetSystematic(systset));
            if (m_config->ApplyCuts(XAMPP::CutKind::Output)) return true;
        }
        return false;
    }
    StatusCode CxAODMaker::SaveElectrons() {

        return StatusCode::SUCCESS;
    }

}
