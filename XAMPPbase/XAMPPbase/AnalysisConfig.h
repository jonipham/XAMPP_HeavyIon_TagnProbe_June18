#ifndef XAMPPbase_AnalysisConfig_H
#define XAMPPbase_AnalysisConfig_H

#include <AsgTools/AsgTool.h>
#include <AsgTools/IAsgTool.h>

#include <XAMPPbase/Cuts.h>
#include <string>
#include <vector>
#include <map>

namespace CP {
    class SystematicSet;
}
namespace XAMPP {
    class HistoBase;
    class EventInfo;
    class ISystematics;
    class IHistoVariable;
    enum CutKind {
        CutFlowHisto, Output
    };

    typedef std::vector<Cut*> CutRow;

    class CutFlow {
        public:
            CutFlow();
            CutFlow(const std::string &name);
            ~CutFlow();
            void push_back(Cut* cut);
            int hash() const;

            const std::string& name() const;
            void setName(const std::string &N);
            const CutRow& GetCuts() const;
            CutFlow& operator=(const CutFlow &cutflow);
        private:
            CutRow m_AnaCuts; /*!< vector of @c Cut objects */
            std::string m_Name;
            int m_Hash;

    };

    class IAnalysisConfig: virtual public asg::IAsgTool {
            ASG_TOOL_INTERFACE (IAnalysisConfig)
        public:
            virtual StatusCode initialize()=0;
            virtual bool ApplyCuts(CutKind K)=0;
            virtual std::vector<std::string> GetCutNames(int hash = 0) const =0;
//            virtual std::vector<std::string> GetCutFlows() const =0;
            virtual std::string TreeName() const = 0;
            virtual bool RegisterHistoBase(HistoBase* Base)=0;
            virtual ~IAnalysisConfig() {
            }
    };

    class AnalysisConfig: public asg::AsgTool, virtual public IAnalysisConfig {
        public:
            // Create a proper constructor for Athena
            ASG_TOOL_CLASS(AnalysisConfig, XAMPP::IAnalysisConfig)

            AnalysisConfig(std::string Analysis);

            virtual ~AnalysisConfig();
            virtual StatusCode initialize();
            virtual bool ApplyCuts(CutKind K);
            virtual std::vector<std::string> GetCutNames(int hash = 0) const;
//            virtual std::vector<std::string> GetCutFlows() const;
            virtual std::string TreeName() const;
            virtual bool RegisterHistoBase(HistoBase* Base);
        protected:
            virtual bool isData() const;
            virtual StatusCode initializeCustomCuts();
            virtual StatusCode initializeStandardCuts();
            bool isActive(CutFlow& cf) const;
            StatusCode AddToCutFlows(CutFlow& cf);
            Cut* NewCut(const std::string&Name, Cut::CutType T, bool IsSkimming = true);
            Cut* NewSkimmingCut(const std::string& Name, Cut::CutType T);
            Cut* NewCutFlowCut(const std::string& Name, Cut::CutType T);
            

            EventInfo* m_XAMPPInfo;
            unsigned int  NumActiveCutFlows() const;
        private:
         
            bool PassStandardCuts(unsigned int &N) const;
            IHistoVariable* FindCutFlowHisto(const CP::SystematicSet* Set, const CutFlow* Flow) const;

            // ASG properties
            std::string m_treeName;
#ifdef XAOD_STANDALONE
            int m_output_level;
#endif

            std::vector<std::string> m_ActiveCutflows;
            std::string m_ActiveCutflowsString;
protected:
            // internal variables
            CutRow m_StandardCuts;
private:
            std::vector<CutFlow> m_CutFlows;
            CutRow m_DefinedCuts; // book-keeping of ALL defined cuts for cleaning up in the end

            typedef std::pair<const CP::SystematicSet*, const CutFlow*> SystSelectionPair;
            std::map<SystSelectionPair, IHistoVariable*> m_CutFlowHistos;
            bool m_init;
            asg::AnaToolHandle<XAMPP::IEventInfo> m_InfoHandle;
            ToolHandle<ISystematics> m_systematics;
    };

    class TruthAnalysisConfig: public AnalysisConfig {

        public:

            ASG_TOOL_CLASS(TruthAnalysisConfig, XAMPP::IAnalysisConfig)
            //
            TruthAnalysisConfig(std::string Analysis = "Custom");
            // Create a proper constructor for Athena
        protected:
            virtual StatusCode initializeStandardCuts();
    };
}
#endif
