#ifndef XAMPPbase_SUSYMetSelector_H
#define XAMPPbase_SUSYMetSelector_H
#include <SUSYTools/ISUSYObjDef_xAODTool.h>

#include <XAMPPbase/IMetSelector.h>
#include <AsgTools/AsgTool.h>
#include <AsgTools/AnaToolHandle.h>


namespace XAMPP {
    class IElectronSelector;
    class IJetSelector;
    class IMuonSelector;
    class IPhotonSelector;
    class ITauSelector;    
    class IEventInfo;
    class EventInfo;
    
    class SUSYMetSelector: public asg::AsgTool, virtual public IMetSelector {
        public:
            // Create a proper constructor for Athena
            ASG_TOOL_CLASS(SUSYMetSelector, XAMPP::IMetSelector)

            SUSYMetSelector(std::string myname);
            virtual ~SUSYMetSelector() {
                ATH_MSG_INFO("Destructor called");
            }

            virtual StatusCode initialize();

            virtual StatusCode LoadContainers();
            virtual StatusCode FillMet(const CP::SystematicSet &systset);

            virtual xAOD::MissingETContainer* GetCustomMet(std::string kind);

        protected:
            StatusCode CreateContainer(std::string Name, xAOD::MissingETContainer* &Cont, xAOD::MissingETAuxContainer* &Aux);

            bool isInitialized() const;

            XAMPP::EventInfo* m_XAMPPInfo;
            asg::AnaToolHandle<ST::ISUSYObjDef_xAODTool> m_susytools;
            asg::AnaToolHandle<XAMPP::IEventInfo> m_InfoHandle;
            
#ifdef XAOD_STANDALONE
            int m_output_level;
#endif
            bool m_init;
            bool m_doTrackMet;
            bool m_IncludePhotons;
            bool m_IncludeTaus;

            xAOD::TauJetContainer* m_MetTaus;
            xAOD::PhotonContainer* m_MetPhotons;

            ToolHandle<IElectronSelector> m_elec_selection;
            ToolHandle<IMuonSelector> m_muon_selection;
            ToolHandle<IJetSelector> m_jet_selection;
            ToolHandle<IPhotonSelector> m_phot_selection;
            ToolHandle<ITauSelector> m_tau_selection;

            xAOD::MissingETContainer* MetTST = NULL;
            xAOD::MissingETAuxContainer* MetTSTAux = NULL;
            xAOD::MissingETContainer* MetCST = NULL;
            xAOD::MissingETAuxContainer* MetCSTAux = NULL;
            const xAOD::MissingETContainer* xAODMet = NULL;
            xAOD::MissingETContainer* MetTrack = NULL;
            xAOD::MissingETAuxContainer* MetTrackAux = NULL;
            const xAOD::MissingETContainer* xAODTrackMet = NULL;

            std::string m_systName;

    };
}
#endif
