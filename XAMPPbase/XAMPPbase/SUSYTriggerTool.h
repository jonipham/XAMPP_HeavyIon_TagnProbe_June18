#ifndef XAMPPbase_SUSYTriggerTool_H
#define XAMPPbase_SUSYTriggerTool_H

#include <AsgTools/AsgTool.h>
#include <AsgTools/ToolHandle.h>
#include <AsgTools/AnaToolHandle.h>

#include <XAMPPbase/ITriggerTool.h>
#include <xAODBase/IParticleHelpers.h>

namespace ST {
    class ISUSYObjDef_xAODTool;
}

namespace XAMPP {
    class SUSYTriggerTool;
    class TriggerInterface {
        public:
            TriggerInterface(const std::string &Name, SUSYTriggerTool*);

            static void SetMatchingThreshold(float Thresh);
            static void SaveTriggerPrescaling(bool B);
            StatusCode initialize(XAMPP::EventInfo* Info);

            bool isTriggerPassed() const;
            std::string name() const;
            std::string MatchStoreName() const;
            std::string StoreName() const;

            enum TriggerType {
                UnDefined = 0, MET, SingleLepton, DiLepton, TriLepton, Photon, Tau
            };

            void NewEvent();
            bool PassTrigger() const;

            bool PassTriggerMatching();

            bool NeedsTriggerMatching() const;
            bool NeedsElectronMatching() const;
            bool NeedsMuonMatching() const;
            bool NeedsTauMatching() const;
            bool NeedsPhotonMatching() const;
            std::string PrintMatchingThresholds() const;
        private:
            template<typename Container, typename ElementLink> bool FillLinkVector(Container* Particles, XAMPP::Storage<ElementLink>* LinkStore);
            template<typename ElementLink> unsigned int GetCalibMatches(XAMPP::Storage<ElementLink>* Store);

            void PerformUncalibratedMatching(const xAOD::IParticleContainer* UnCalibrated);

            bool IsTriggerMatched(const xAOD::IParticle* Particle) const;
            void ResetObjectMatching();

            //Methods needed during initialization
            void GetMatchingThresholds();
            int ExtractPtThreshold(std::string &TriggerString, int &M, xAOD::Type::ObjectType &T);
            void SortThresholds();
            bool AssignMatching(xAOD::Type::ObjectType T) const;

            std::string m_name;
            TriggerType m_Type;

            SUSYTriggerTool* m_TriggerTool;
            XAMPP::EventInfo* m_XAMPPInfo;
            ToolHandle<ST::ISUSYObjDef_xAODTool> m_susytools;

            XAMPP::Storage<char>* m_TriggerStore;
            XAMPP::Storage<char>* m_MatchingStore;
            XAMPP::Storage<float>* m_PreScalingStore;

            SG::AuxElement::Decorator<char> m_MatchingDecorator;
            SG::AuxElement::ConstAccessor<char> m_MatchingAccessor;

            bool m_MatchEle;
            bool m_MatchMuo;
            bool m_MatchTau;
            bool m_MatchPho;

            XAMPP::Storage<MatchedEl>* m_CalibEleStore;
            XAMPP::Storage<MatchedMuo>* m_CalibMuoStore;
            XAMPP::Storage<MatchedTau>* m_CalibTauStore;
            XAMPP::Storage<MatchedPho>* m_CalibPhoStore;

            struct OfflineMatching {
                    float PtThreshold = 0.;
                    xAOD::Type::ObjectType Object = xAOD::Type::ObjectType::Other;
                    bool ObjectMatched = false;
            };
            std::vector<OfflineMatching> m_Thresholds;

            static int m_AddOffThreshold;
            static bool m_SavePrescaling;
    };

    class SUSYTriggerTool: public asg::AsgTool, virtual public ITriggerTool {
        public:

            virtual ~SUSYTriggerTool();
            SUSYTriggerTool(std::string myname);
            // Create a proper constructor for Athena
            ASG_TOOL_CLASS(SUSYTriggerTool, XAMPP::ITriggerTool)

            virtual StatusCode initialize();
            //Checks the triggers at the beginning of the event
            virtual bool CheckTrigger();
            virtual bool CheckTriggerMatching();

            //These functions can be called after the Trigger matching has been performed
            virtual bool IsMatchedObject(const xAOD::Electron* el, const std::string& Trig);
            virtual bool IsMatchedObject(const xAOD::Muon* mu, const std::string & Trig);
            virtual bool IsMatchedObject(const xAOD::Photon* ph, const std::string & Trig);
            virtual bool CheckTrigger(const std::string &trigger_name);
            virtual StatusCode SaveObjectMatching(ParticleStorage* Storage, xAOD::Type::ObjectType Type);

            virtual const xAOD::ElectronContainer* UnCalibElectrons() const;
            virtual const xAOD::MuonContainer* UnCalibMuons() const;
            virtual const xAOD::PhotonContainer* UnCalibPhotons() const;
            virtual const xAOD::TauJetContainer* UnCalibTaus() const;

            virtual xAOD::ElectronContainer* CalibElectrons() const;
            virtual xAOD::MuonContainer* CalibMuons() const;
            virtual xAOD::PhotonContainer* CalibPhotons() const;
            virtual xAOD::TauJetContainer* CalibTaus() const;

            EleLink GetLink(const xAOD::Electron* el);
            MuoLink GetLink(const xAOD::Muon* mu);
            PhoLink GetLink(const xAOD::Photon* ph);
            TauLink GetLink(const xAOD::TauJet* tau);

        protected:
            bool isData() const;
            StatusCode MetTriggerEmulation();
            StatusCode GetHLTMet(float &met, const std::string &containerName, bool &doContainer);

            StatusCode FillTriggerVector(std::vector<std::string> &triggers_vector);

            XAMPP::EventInfo* m_XAMPPInfo;
        private:
            asg::AnaToolHandle<XAMPP::IEventInfo> m_XAMPPInfoHandle;
            ToolHandle <XAMPP::ISystematics>   m_systematics;
        protected:
            asg::AnaToolHandle<ST::ISUSYObjDef_xAODTool> m_susytools;
#ifdef XAOD_STANDALONE
            int m_output_level;
#endif
            ToolHandle<XAMPP::IElectronSelector> m_elec_selection;
            ToolHandle<XAMPP::IMuonSelector> m_muon_selection;
            ToolHandle<XAMPP::IPhotonSelector> m_phot_selection;
            ToolHandle<XAMPP::ITauSelector> m_tau_selection;

            std::vector<TriggerInterface*> m_triggers;
            std::vector<std::string> m_met_triggers;
            std::vector<std::string> m_1lep_triggers;
            std::vector<std::string> m_2lep_triggers;
            std::vector<std::string> m_3lep_triggers;
            std::vector<std::string> m_phot_triggers;
#ifdef XAOD_STANDALONE
            // also set single string as property for RC python config
            std::string m_met_triggers_string;
            std::string m_1lep_triggers_string;
            std::string m_2lep_triggers_string;
            std::string m_3lep_triggers_string;
            std::string m_phot_triggers_string;
#endif
            std::string m_PreName;
            bool m_init;
            bool m_Pass;
            bool m_NoCut;
            int m_OfflineThrs;
            bool m_StoreObjectMatching;

            bool m_StorePreScaling;
            bool m_MetTrigEmulation;

            bool m_doLVL1Met;
            bool m_doCellMet;
            bool m_doMhtMet;
            bool m_doTopoClMet;
            bool m_doTopoClPufitMet;
            bool m_doTopoClPuetaMet;

            XAMPP::Storage<char>* m_DecTrigger;
            XAMPP::Storage<char>* m_DecMatching;
    };
}
#endif
