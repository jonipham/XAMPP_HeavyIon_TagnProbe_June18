#ifndef XAMPPbase_SUSYAnalysisHelper_H
#define XAMPPbase_SUSYAnalysisHelper_H

#include <XAMPPbase/AnalysisUtils.h>
#include <XAMPPbase/IAnalysisHelper.h>
#include <XAMPPbase/HistoBase.h>
#include <XAMPPbase/TreeHelpers.h>

#include <XAMPPbase/IElectronSelector.h>
#include <XAMPPbase/IMuonSelector.h>
#include <XAMPPbase/IJetSelector.h>
#include <XAMPPbase/ITauSelector.h>
#include <XAMPPbase/ITruthSelector.h>
#include <XAMPPbase/IPhotonSelector.h>

#include <XAMPPbase/ITriggerTool.h>
#include <XAMPPbase/EventInfo.h>
#include <XAMPPbase/EventStorage.h>

#include <AsgTools/AnaToolHandle.h>
#include <AsgTools/ToolHandle.h>
#include <AsgTools/AsgMetadataTool.h>
#include <SUSYTools/SUSYObjDef_xAOD.h>
#include "TFile.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>

#ifndef XAOD_STANDALONE
class ITHistSvc;
#endif

namespace ST {
    class SUSYObjDef_xAOD;
    class ISUSYObjDef_xAODTool;
}
namespace SUSY {
    class CrossSectionDB;
}
namespace CP {
    class SystematicSet;
    class IJetTileCorrectionTool;
}
class IGoodRunsListSelectionTool;

namespace XAMPP {
    class TreeBase;
    class IMetaDataTree;
    class IAnalysisConfig;
    class IEventInfo;
    class EventInfo;
    class CxAODMaker;
    class ReconstructedParticles;
    class IElectronSelector;
    class IMuonSelector;
    class IPhotonSelector;
    class ITauSelector;
    class IJetSelector;
    class IMetSelector;
    class ITruthSelector;
    class ITriggerTool;
    class ISystematics;

    class SUSYAnalysisHelper: public asg::AsgMetadataTool, virtual public IAnalysisHelper {
        public:
            // Create a proper constructor for Athena
            ASG_TOOL_CLASS(SUSYAnalysisHelper, XAMPP::IAnalysisHelper)

            SUSYAnalysisHelper(std::string myname);
            virtual ~SUSYAnalysisHelper();
            virtual StatusCode initialize();
            virtual StatusCode CheckCutFlow(const CP::SystematicSet* systset);
            virtual StatusCode finalize();
            virtual StatusCode LoadContainers();
            virtual StatusCode FillInitialObjects(const CP::SystematicSet* systset);
            virtual StatusCode FillObjects(const CP::SystematicSet* systset);
            virtual StatusCode RemoveOverlap();

            virtual bool AcceptEvent();
            virtual bool EventCleaning() const;
            virtual bool CleanObjects(const CP::SystematicSet* systset);

            virtual bool CheckTrigger();

            virtual StatusCode FillEvent(const CP::SystematicSet* set);
            virtual StatusCode DumpCxAOD();

            // call GetMCXsec before GetMCFilterEff/GetMCkFactor/GetMCXsectTimesEff since it computes the SUSY::finalState
            virtual unsigned int finalState();
            virtual double GetMCXsec(unsigned int mc_channel_number, unsigned int finalState = 0);
            virtual double GetMCFilterEff(unsigned int mc_channel_number, unsigned int finalState = 0);
            virtual double GetMCkFactor(unsigned int mc_channel_number, unsigned int finalState = 0);
            virtual double GetMCXsectTimesEff(unsigned int mc_channel_number, unsigned int finalState = 0);

        protected:
            virtual bool isData() const;
            virtual bool doTruth() const;
            virtual bool SUSYdsid(int DSID) const;
            void DisableRecoFlags();

            virtual StatusCode initializeEventVariables();
            virtual StatusCode ComputeEventVariables();

            //These functions should only be overwritten if there are additional tools needed to  be initialized
            virtual StatusCode initializeObjectTools();
            virtual StatusCode initializeAnalysisTools();
            virtual StatusCode initializeSUSYTools();
            virtual StatusCode initializeGRLTool();

            virtual StatusCode FillEventWeights();
            StatusCode SaveCrossSection();

            //ToolHandles of tools which have to be created externally.
            //In 20.7 the name of the tools has to match the name of the Handles
            ToolHandle<IElectronSelector> m_electron_selection;
            ToolHandle<IJetSelector> m_jet_selection;
            ToolHandle<IMetSelector> m_met_selection;
            ToolHandle<IMuonSelector> m_muon_selection;
            ToolHandle<IPhotonSelector> m_photon_selection;
            ToolHandle<ITauSelector> m_tau_selection;
            ToolHandle<ITruthSelector> m_truth_selection;
            ToolHandle<ISystematics> m_systematics;
            ToolHandle<ITriggerTool> m_triggers;

            //SUSYTools as AnaToolHandle because we want to have the possibility in place that SUSYTools
            //is created by the Helper itself
            asg::AnaToolHandle<ST::ISUSYObjDef_xAODTool> m_susytools;

            //Retrieve the SUSYTools pointer from the Tool Handle
            ST::SUSYObjDef_xAOD* SUSYToolsPtr();
            template<typename T> ToolHandle<T> GetCPTool(const std::string &name) {
#ifdef XAOD_STANDALONE    
                ToolHandle<T> handle;
                const ToolHandle<T>* HandlePtr = SUSYToolsPtr()->getProperty < ToolHandle < T >> (name);
                if (!HandlePtr) {
                    ATH_MSG_ERROR("Failed to retrieve CP tool " << name);
                } else {
                    handle = (*HandlePtr);
                }
                return handle;
#else
                return ToolHandle < T > (SUSYToolsPtr()->getProperty(name).toString());
#endif 
            }
        private:
            StatusCode DumpNtuple(const CP::SystematicSet * sys);
            StatusCode DumpHistos(const CP::SystematicSet * sys);
            HistoBase* CreateHistoClass(const CP::SystematicSet * set);

            StatusCode initHistoClass(HistoBase* HistoClass);
            void CleaningForOutput(const std::string& DecorName, XAMPP::Storage<int>* &Store, bool DoCleaning);
            TreeBase* CreateTreeClass(const CP::SystematicSet * set);
            StatusCode initTreeClass(TreeBase* TreeClass);

#ifdef XAOD_STANDALONE
            std::string m_config_name;
            int m_output_level;
            int m_output_level_ST;
#endif
            bool m_init;
            bool m_RunCutFlow;
            bool m_UseFileMetadata;

            bool m_doHistos;
            bool m_doTrees;
            bool m_doxAODs;
            bool m_doTruth;
            bool m_doPRW;
            bool m_isAF2;

            bool m_StoreRecoFlags;
            bool m_CleanEvent;
            bool m_CleanBadMuon;
            bool m_CleanCosmicMuon;
            bool m_CleanBadJet;
#ifdef XAOD_STANDALONE
            std::string m_OutFileName;
            TFile * m_outFile;
#else
            ServiceHandle<ITHistSvc> m_histSvc;
#endif

            bool m_FillLHEWeights;
            std::map<unsigned int, XAMPP::Storage<double>*> m_LHEWeights;
            XAMPP::Storage<int>* m_dec_NumBadMuon;
            XAMPP::Storage<int>* m_decNumBadJet;
            XAMPP::Storage<int>* m_decNumCosmicMuon;

            // SUSYTools properties and settings
            std::string m_STConfigFile;
            std::string m_XsecDBDir;
            std::vector<std::string> m_GoodRunsListVec;
            std::vector<std::string> m_PRWConfigFiles;
            std::vector<std::string> m_PRWLumiCalcFiles;
            // also set single string as property for RC python config
#ifdef XAOD_STANDALONE
            std::string m_GoodRunsListVec_string;
            std::string m_PRWConfigFiles_string;
            std::string m_PRWLumiCalcFiles_string;
#endif

            SUSY::CrossSectionDB *m_XsecDB;
            std::map<const CP::SystematicSet*, HistoBase *> m_histoVec;
            std::map<const CP::SystematicSet*, TreeBase *> m_treeVec;
            ToolHandle<XAMPP::IAnalysisConfig> m_config;
            //Use anaTool handle for tools which might be created by the Helper itself
            asg::AnaToolHandle<XAMPP::IEventInfo> m_InfoHandle;
            asg::AnaToolHandle<XAMPP::IMetaDataTree> m_MDTree;
            asg::AnaToolHandle<IGoodRunsListSelectionTool> m_grl;
//            XAMPP::CxAODMaker* m_OutxAOD;
        protected:
            asg::AnaToolHandle<XAMPP::IReconstructedParticles> m_ParticleConstructor;

            XAMPP::EventInfo* m_XAMPPInfo;
            XAMPP::CxAODMaker* m_OutxAOD;

    };
}
#endif
