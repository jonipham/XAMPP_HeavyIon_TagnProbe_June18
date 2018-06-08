#ifndef XAMPPBASE_ParticleSelector_H
#define XAMPPBASE_ParticleSelector_H

#include <AsgTools/AsgTool.h>

#include <XAMPPbase/EventInfo.h>
//EDM includes mandantory for the Template functions
#include <XAMPPbase/Defs.h>
#include <xAODBase/ObjectType.h>
#include <xAODCore/ShallowCopy.h>
#include <AsgTools/ToolHandle.h>
#include <AsgTools/AnaToolHandle.h>
#include <memory>

namespace CP {
    class SystematicSet;
}
namespace XAMPP {

    class IEventInfo;
    class EventInfo;
    class ISystematics;

    class ParticleSelector: public asg::AsgTool {
        public:
            ParticleSelector(std::string myname);
            virtual ~ParticleSelector();
            virtual StatusCode initialize();
            enum LinkStatus {
                Created, Loaded, Failed
            };
        private:
            StatusCode CreateAuxElements(std::string &name, SelectionAccessor& acc, SelectionDecorator& dec);
#ifdef XAOD_STANDALONE
            int m_output_level;
#endif
            float m_baselinePt;
            float m_baselineEta;
            float m_signalPt;
            float m_signalEta;

            const CP::SystematicSet* m_ActSys;
            asg::AnaToolHandle<XAMPP::IEventInfo> m_EvInfoHandle;
            std::vector<const CP::SystematicSet*> m_RelevantSys;
            std::string m_ContainerKey;
            std::string m_storeName;
            SelectionObject m_ObjectType;
            bool m_init;

            std::string m_PreSelDecorName;
            SelectionAccessor m_acc_presel;
            SelectionDecorator m_dec_presel;

            std::string m_BaselineDecorName;
            SelectionAccessor m_acc_baseline;
            SelectionDecorator m_dec_baseline;

            std::string m_SignalDecorName;
            SelectionAccessor m_acc_signal;
            SelectionDecorator m_dec_signal;

            std::string m_IsolDecorName;
            SelectionAccessor m_acc_isol;
            SelectionDecorator m_dec_isol;

        protected:
            ToolHandle<XAMPP::ISystematics> m_systematics;
            XAMPP::EventInfo* m_XAMPPInfo;

            void SetSystematics(const CP::SystematicSet &Set);
            void SetSystematics(const CP::SystematicSet *Set);

            void SetContainerKey(const std::string &Key);
            void SetObjectType(SelectionObject Type);
            const std::string& StoreName() const;
            const std::string& ContainerKey() const;
            XAMPP::SelectionObject ObjectType() const;

            bool isInitialized() const;
            bool ProcessObject(XAMPP::SelectionObject T) const;
            bool ProcessObject() const;

            std::string SystName(bool InclUnderScore = true) const;

            bool SystematicAffects(const CP::SystematicSet &Set) const;
            bool SystematicAffects(const CP::SystematicSet* Set) const;

            virtual bool PassBaselineKinematics(const xAOD::IParticle &P) const;
            virtual bool PassSignalKinematics(const xAOD::IParticle &P) const;

            bool PassBaselineKinematics(const xAOD::IParticle* P) const;
            bool PassSignalKinematics(const xAOD::IParticle* P) const;

            virtual bool PassSignal(const xAOD::IParticle &P) const;
            virtual bool PassBaseline(const xAOD::IParticle &P) const;
            virtual bool PassPreSelection(const xAOD::IParticle &P) const;
            virtual bool PassIsolation(const xAOD::IParticle &P) const;

            bool PassSignal(const xAOD::IParticle* P) const;
            bool PassBaseline(const xAOD::IParticle* P) const;
            bool PassPreSelection(const xAOD::IParticle* P) const;
            bool PassIsolation(const xAOD::IParticle* P) const;

            void SetSelectionDecorators(const xAOD::IParticle&P, bool Pass) const;
            virtual void SetPreSelectionDecorator(const xAOD::IParticle& P, bool Pass) const;
            virtual void SetBaselineDecorator(const xAOD::IParticle& P, bool Pass) const;
            virtual void SetSignalDecorator(const xAOD::IParticle& P, bool Pass) const;

            // have this function in the particle selector for using it independently in JetSelector and TruthSelector
            virtual StatusCode ReclusterJets(const xAOD::IParticleContainer *inputJets, double Rcone, double minPtKt4 = -1., std::string PreFix = "");

            template<typename Container> StatusCode ViewElementsContainer(const std::string &Key, Container* & Cont);
            template<typename Container> StatusCode LoadViewElementsContainer(const std::string & Key, Container* &Cont, bool LoadNominal = false) const;

            template<typename Container> StatusCode LoadContainer(const std::string &Key, const Container* & Cont) const;
            template<typename Container> StatusCode LoadContainer(const std::string &Key, Container* & Cont) const;

            //This function creates the ShallowCopy links of the particle containers if the current selector is actually affected by the kinematic systematic
            //Otherwise the nominal container is loaded from the StoreGate
            template<typename Container> ParticleSelector::LinkStatus CreateContainerLinks(const std::string &Key, Container* & Cont);

            template<typename Container> ParticleSelector::LinkStatus CreateContainerLinks(const std::string &Key, Container* & Cont, xAOD::ShallowAuxContainer* &AuxContainer);

    };
}
#include <XAMPPbase/ParticleSelector.ixx>
#endif

