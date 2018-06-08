#ifndef XAMPPbase_SUSYElectronSelector_H
#define XAMPPbase_SUSYElectronSelector_H

#include <XAMPPbase/IElectronSelector.h>
#include <XAMPPbase/ITriggerTool.h>
#include <XAMPPbase/SUSYParticleSelector.h>

#include <AsgTools/AnaToolHandle.h>

class IAsgElectronEfficiencyCorrectionTool;

namespace ST {
    class ISUSYObjDef_xAODTool;
}
namespace XAMPP {
    class ElectronTriggerSFHandler;

    struct EleWeightDecorators {
            const CP::SystematicSet* Systematic;
            SG::AuxElement::Decorator<double>* EleSF;
            SG::AuxElement::Decorator<double>* EleSF_Reco;
            SG::AuxElement::Decorator<double>* EleSF_Isol;
            SG::AuxElement::Decorator<double>* EleSF_ID;

            XAMPP::Storage<double>* TotalSF;
            XAMPP::Storage<double>* TotalSF_Reco;
            XAMPP::Storage<double>* TotalSF_Isol;
            XAMPP::Storage<double>* TotalSF_ID;

            std::vector<XAMPP::ElectronTriggerSFHandler*> TriggerSFs;
    };
    typedef IAsgElectronEfficiencyCorrectionTool EleEffTool;
    typedef ToolHandle<IAsgElectronEfficiencyCorrectionTool> EleEffToolHandle;

    class SUSYElectronSelector: public SUSYParticleSelector, virtual public IElectronSelector {
        public:
            // Create a proper constructor for Athena
            ASG_TOOL_CLASS(SUSYElectronSelector, XAMPP::IElectronSelector)

            SUSYElectronSelector(std::string myname);
            virtual ~SUSYElectronSelector();

            virtual StatusCode initialize();

            virtual StatusCode LoadSelection(const CP::SystematicSet &systset);

            virtual StatusCode LoadContainers();
            virtual StatusCode InitialFill(const CP::SystematicSet &systset);
            virtual StatusCode FillElectrons(const CP::SystematicSet &systset);

            virtual EleLink GetLink(const xAOD::Electron &el) const;
            virtual EleLink GetOrigLink(const xAOD::Electron &el) const;

            virtual const xAOD::ElectronContainer* GetElectronContainer() const {
                return m_xAODElectrons;
            }
            virtual xAOD::ElectronContainer* GetElectrons() const {
                return m_Electrons;
            }
            virtual xAOD::ElectronContainer* GetPreElectrons() const {
                return m_PreElectrons;
            }
            virtual xAOD::ElectronContainer* GetSignalElectrons() const {
                return m_SignalElectrons;
            }
            virtual xAOD::ElectronContainer* GetBaselineElectrons() const {
                return m_BaselineElectrons;
            }
            virtual xAOD::ElectronContainer* GetCustomElectrons(std::string kind);

            virtual StatusCode SaveScaleFactor();

        protected:
            virtual StatusCode CallSUSYTools();
            virtual bool PassBaseline(const xAOD::IParticle &P) const;
            virtual bool PassSignal(const xAOD::IParticle &P) const;
        private:
            float m_BaselineD0SigCut;
            float m_BaselineZ0SinThetaCut;
            float m_SignalD0SigCut;
            float m_SignalZ0SinThetaCut;
            bool m_RequireIsoSignal;

            const xAOD::ElectronContainer* m_xAODElectrons;
            xAOD::ElectronContainer* m_Electrons;
            xAOD::ShallowAuxContainer* m_ElectronsAux;
        protected:
            xAOD::ElectronContainer * m_PreElectrons; // before OR
            xAOD::ElectronContainer * m_BaselineElectrons; // after OR
            xAOD::ElectronContainer * m_SignalElectrons;

        private:
            typedef std::pair<std::string, EleEffToolHandle> TrigSFTool;

            void LoadSFToolFromSUSYTools(const std::string &Key, const std::string& PropertyName);
            StatusCode initializeTriggerSFTools();
            StatusCode SaveElectronSF(const xAOD::Electron& el, EleEffToolHandle &SFtool, SG::AuxElement::Decorator<double>* SFdecor, double &TotSF, bool doSF = true);
            void DecorateTotalElectronSF(const xAOD::Electron& el, const EleWeightDecorators& Decors);

            // ElectronTriggerSFHandler* CreateTriggerSFHandler(const std::string &TriggerSF);
            std::string FindBestSFTool(const std::string& TriggerSF);

            std::vector<EleWeightDecorators> m_SF;
            std::map<std::string, EleEffToolHandle> m_SFTools;
            EleEffToolHandle m_IdHandle;
            EleEffToolHandle m_RecoHandle;
            EleEffToolHandle m_IsoHandle;

            bool m_SeparateSF;
            bool m_doRecoSF;
            bool m_doIdSF;
            bool m_doTriggerSF;
            bool m_doIsoSF;
            std::vector<std::string> m_TriggerExp;
            std::vector<std::string> m_TriggerSFConf;
#ifdef XAOD_STANDALONE
            std::string m_TriggerExp_str;
            std::string m_TrigSFConf_str;
#endif
            bool m_StoreMultiTrigSF;
    };

    class ElectronTriggerSFHandler {
        public:

            ElectronTriggerSFHandler(const std::string &Trigger, const ToolHandle<ST::ISUSYObjDef_xAODTool> &SUSYTools, EleEffToolHandle& TriggerSF, XAMPP::EventInfo* Info);

            StatusCode initialize(const EleWeightDecorators& Decors);
            StatusCode initialize();

            StatusCode SaveTriggerSF(const xAOD::Electron* El);
            StatusCode ApplyTriggerSF();
            virtual ~ElectronTriggerSFHandler();
            void SetTriggerStorage(XAMPP::Storage<double>* store);
            void SetTriggerDecorator(SG::AuxElement::Decorator<double>* decor);

        protected:
            struct EleTrigMatcher {
                    EleTrigMatcher(XAMPP::Storage<char>* T, XAMPP::Storage<MatchedEl>* M) {
                        Trigger = T;
                        Matched = M;
                    }
                    XAMPP::Storage<char>* Trigger;
                    XAMPP::Storage<MatchedEl>* Matched;
            };
            bool RetrieveMatchers(const std::string &Trigger);
            bool IsTriggerMachted(const xAOD::Electron* El);
            bool IsTriggerMachted(const xAOD::Electron* El, ElectronTriggerSFHandler::EleTrigMatcher& M);

            std::string m_TrigStr;
            EleEffToolHandle m_TriggerSFTool;
            XAMPP::EventInfo* m_XAMPPInfo;
            ToolHandle<ST::ISUSYObjDef_xAODTool> m_susytools;

            std::vector<ElectronTriggerSFHandler::EleTrigMatcher> m_Triggers;
            XAMPP::Storage<double>* m_TotTrigSF;
            XAMPP::Storage<double>* m_TotSF;
            SG::AuxElement::Decorator<double>* m_decTrigSF;
            SG::AuxElement::Decorator<double>* m_decSF;
    };
}
#endif
