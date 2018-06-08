#ifndef XAMPPbase_TreeBase_H
#define XAMPPbase_TreeBase_H
#include <XAMPPbase/TreeHelpers.h>
#include <XAMPPbase/EventInfo.h>

#include <AsgTools/ToolHandle.h>

#include <algorithm>
#include <string>
#include <iostream>
#include <cctype>
#include <vector>

#include "TFile.h"
#include "TTree.h"

namespace CP {
    class SystematicSet;
}

#ifndef XAOD_STANDALONE
class ITHistSvc;
#endif

namespace XAMPP {
    class IAnalysisConfig;
    class ISystematics;
    class ITreeBranchVariable;
  
    class TreeBase {
        public:
            TreeBase(const CP::SystematicSet * set);
            virtual ~TreeBase();
            StatusCode InitializeTree();
            StatusCode FillTree();
            StatusCode FinalizeTree();
#ifdef XAOD_STANDALONE
            void SetOutFile(TFile* file);
#else
            void SetHistService(ServiceHandle<ITHistSvc> & Handle);
#endif
            void SetEventInfoHandler(const XAMPP::EventInfo* Info);
            void SetEventInfoHandler(const ToolHandle<XAMPP::IEventInfo>& EvInfo);
            void SetSystematicsTool(const ToolHandle<XAMPP::ISystematics>& Syst);
            void SetAnalysisConfig(const ToolHandle<XAMPP::IAnalysisConfig>&config);

        protected:
            virtual void CreateBranches();
            template<typename T> void CreateEventBranches() {
                std::vector<XAMPP::Storage<T>*> Vector = m_XAMPPInfo->GetStorages<T>(XAMPP::EventInfo::OutputElement::Tree, m_set != m_systematics->GetNominal());
                for (auto & S : Vector) {
                    m_Branches.push_back(new EventBranchVariable<T>(S, m_tree));
                }
            }
        private:
            const CP::SystematicSet* m_set;
            const XAMPP::EventInfo * m_XAMPPInfo;
            const XAMPP::ISystematics* m_systematics;
            const XAMPP::IAnalysisConfig* m_config;
            TTree* m_tree;
            bool m_init;
            bool m_isWritten;

#ifdef XAOD_STANDALONE            
            TFile* m_outFile;
#else
            ServiceHandle<ITHistSvc> m_histSvc;
#endif 

            std::vector<XAMPP::ITreeBranchVariable*> m_Branches;
            // from original EventInfo
            int m_mcChannelNumber;
            // ---------
    };
}
#endif
