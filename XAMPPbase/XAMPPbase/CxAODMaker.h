#ifndef XAMPPbase_CxAODMaker_H
#define XAMPPbase_CxAODMaker_H

//#include <XAMPPbase/ITriggerTool.h>

#include <AsgTools/AsgTool.h>
#include <AsgTools/ToolHandle.h>
#include <string>
#include <iostream>
#include <cctype>
#include <map>
#include <vector>
#include <TFile.h>

//EDM includes
#include <xAODEgamma/ElectronContainer.h>
#include <xAODEgamma/ElectronAuxContainer.h>
#include <xAODMuon/MuonContainer.h>
#include <xAODMuon/MuonAuxContainer.h>
#include <xAODJet/JetContainer.h>
#include <xAODJet/JetAuxContainer.h>

namespace xAOD {
    class TEvent;
}

namespace XAMPP {

    class IAnalysisHelper;
    class IElectronSelector;
    class IMuonSelector;
    class IJetSelector;
    class IPhotonSelector;
    class ITruthSelector;
    class ITauSelector;
    class ISystematics;
    class IMetSelector;
    class AnalysisConfig;
    class EventInfo;

    class CxAODMaker: public asg::AsgTool {
        public:
            CxAODMaker(std::string Name);
            virtual ~CxAODMaker();
            StatusCode initialize();
            StatusCode SaveEvent();
            StatusCode Finalize();
        protected:
            bool DumpEvent();

            StatusCode SaveElectrons();

            template<typename Cont, typename AuxCont> void CreateContainer(Cont* &Container, AuxCont* & AuxContainer);
            template<typename Cont, typename AuxCont> StatusCode StoreContainer(std::string Key, Cont* Container, AuxCont* AuxContainer);
            template<typename Cont> StatusCode CopyContainer(Cont* Original, Cont* Copy);

            xAOD::TEvent* m_event;
            TFile* m_outFile;
            XAMPP::EventInfo* m_XAMPPInfo;
            XAMPP::AnalysisConfig* m_config;
            std::string m_config_name;

            ToolHandle<ISystematics> m_systematics;
            ToolHandle<IElectronSelector> m_electron_selection;
            ToolHandle<IMuonSelector> m_muon_selection;
            ToolHandle<IJetSelector> m_jet_selection;
            ToolHandle<IPhotonSelector> m_photon_selection;
            ToolHandle<ITauSelector> m_tau_selection;
            ToolHandle<ITruthSelector> m_truth_selection;
            ToolHandle<IMetSelector> m_met_selection;
            ToolHandle<IAnalysisHelper> m_helper;

            // typedef ElectronStore std::pair<xAOD::ElectronContainer* , xAOD::ElectronAuxContainer*>;
            // std::map<const CP::SystematicSet* ,

    };
}

#include<XAMPPbase/CxAODMaker.ixx>
#endif
