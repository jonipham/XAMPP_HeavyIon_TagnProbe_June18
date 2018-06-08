#ifndef XAMPPbase_MetaDataTree_H
#define XAMPPbase_MetaDataTree_H

#include <AsgTools/IAsgTool.h>
#include <AsgTools/AsgMetadataTool.h>
#include <AsgTools/ToolHandle.h>
#include <AsgTools/AnaToolHandle.h>

#include <xAODEventInfo/EventInfo.h>
#include <xAODCutFlow/CutBookkeeper.h>
#include <xAODCutFlow/CutBookkeeperContainer.h>
#include <xAODLuminosity/LumiBlockRangeContainer.h>
#include <xAODLuminosity/LumiBlockRange.h>

#include <TTree.h>
#include <TFile.h>

#include<map>
#include<set>

#ifndef XAOD_STANDALONE
class ITHistSvc;
#endif

namespace XAMPP {
    class IAnalysisHelper;
    class IEventInfo;
    class IMetaDataTree: virtual public asg::IAsgTool {
            ASG_TOOL_INTERFACE (XAMPP::IMetaDataTree)
        public:
            virtual StatusCode initialize()=0;
            virtual StatusCode beginInputFile()=0;
            virtual StatusCode beginEvent()=0;
            virtual StatusCode finalize()=0;
            virtual StatusCode fillLHEMetaData(unsigned int Idx)=0;
            virtual ~IMetaDataTree() {
            }
    };
    class MetaDataElement {
        public:
            enum BookKeeperType {
                Original = 1, SUSY = 2, LHE3 = 4, PDF = 3,
            };
            virtual ~MetaDataElement() {
            }
            virtual StatusCode newFile(const xAOD::CutBookkeeperContainer* container, MetaDataElement::BookKeeperType Type = MetaDataElement::BookKeeperType::Original)=0;
            virtual StatusCode newFile(const xAOD::LumiBlockRange* LumiBlock) = 0;
            virtual StatusCode newEvent()=0;
            virtual void CutBookKeeperAvailable(bool B)=0;
            virtual StatusCode finalize(TTree* MetaDataTree)=0;
            virtual StatusCode CopyStore(const MetaDataElement* Store)=0;
            virtual StatusCode fillVariation(unsigned int, double)=0;

        protected:
            template<typename T> bool SetBranchAddress(TTree* tree, const std::string &Name, T &Element) {
                if (!tree) {
                    // Error("MetaDataTreeElement::SetBranchAddress()", "noTreeElement given");
                    return false;
                }
                if (tree->FindBranch(Name.c_str())) {
                    if (tree->SetBranchAddress(Name.c_str(), &Element) != 0) {
                        return false;
                    }
                    return true;
                }
                if (tree->Branch(Name.c_str(), &Element) == NULL) {
                    return false;
                }
                return true;
            }
            const xAOD::CutBookkeeper* FindCutBookKeeper(const xAOD::CutBookkeeperContainer* container, MetaDataElement::BookKeeperType Type = MetaDataElement::BookKeeperType::Original, unsigned int procID = 0);

    };

    class MetaDataTree: public asg::AsgMetadataTool, virtual public IMetaDataTree {
        public:
            MetaDataTree(std::string myname);
            // Create a proper constructor for Athena
            ASG_TOOL_CLASS(MetaDataTree, XAMPP::IMetaDataTree)

            virtual ~MetaDataTree();
            virtual StatusCode beginInputFile();
            virtual StatusCode beginEvent();
            virtual StatusCode finalize();
            virtual StatusCode initialize();
            virtual StatusCode fillLHEMetaData(unsigned int Idx);
        private:
            void LoadMCMetaData(unsigned int mcChannel);
            void LoadRunMetaData(unsigned int run);
            void DeleteMetaData(MetaDataElement* &Meta);
            StatusCode CheckLumiBlockContainer(const std::string& Container, bool &HasCont);
            std::string m_TreeName;
            bool m_UseFileMetaData;
            TTree* m_tree;
#ifdef XAOD_STANDALONE
            TFile* m_outFile;
            int m_output_level;
#else
            ServiceHandle<ITHistSvc> m_histSvc;
#endif

            bool m_isData;
            bool m_init;
            ToolHandle<XAMPP::IAnalysisHelper> m_analysis_helper;
            asg::AnaToolHandle<XAMPP::IEventInfo> m_XAMPPInfo;

            std::map<unsigned int, MetaDataElement*> m_MetaDB;
            std::map<unsigned int, MetaDataElement*>::iterator m_ActDB;
            MetaDataElement* m_DummyEntry;
    };

    class MetaDataMC: virtual public MetaDataElement {
        public:
            MetaDataMC(unsigned int mcChannel, ToolHandle<XAMPP::IAnalysisHelper> &helper, ToolHandle<XAMPP::IEventInfo> info);
            virtual StatusCode newFile(const xAOD::CutBookkeeperContainer* container, MetaDataElement::BookKeeperType Type = MetaDataElement::BookKeeperType::Original);
            virtual StatusCode newFile(const xAOD::LumiBlockRange*);
            virtual StatusCode newEvent();
            virtual void CutBookKeeperAvailable(bool B);
            virtual StatusCode finalize(TTree* MetaDataTree);
            virtual StatusCode CopyStore(const MetaDataElement* Store);
            virtual StatusCode fillVariation(unsigned int Id, double W);
            virtual ~MetaDataMC();

        private:
            struct MetaData {
                    MetaData(unsigned int ID) {
                        ProcID = ID;
                        runNumber = 0;
                        xSec = 0.;
                        kFaktor = 0.;
                        FilterEff = 0.;
                        NumTotalEvents = 0.;
                        NumProcessedEvents = 0.;
                        SumW = 0.;
                        SumW2 = 0.;
                        MetaInit = false;
                        KeeperAvailable = false;
                    }
                    unsigned int ProcID;
                    unsigned int runNumber;
                    double xSec;
                    double kFaktor;
                    double FilterEff;
                    Long64_t NumTotalEvents;
                    Long64_t NumProcessedEvents;
                    double SumW;
                    double SumW2;
                    bool MetaInit;
                    bool KeeperAvailable;
            };
            void AddEventInformation(MetaDataMC::MetaData* Meta, double GenWeight);
            void AddFileInformation(MetaDataMC::MetaData* Meta, Long64_t TotEv, double SumW, double SumW2);
            void LoadMetaData(unsigned int ID);
            bool SaveMetaDataInTree(MetaDataMC::MetaData* Meta, TTree* tree);

            unsigned int m_MC;
            ToolHandle<XAMPP::IAnalysisHelper> m_helper;
            ToolHandle<XAMPP::IEventInfo> m_XAMPPInfo;
            std::map<unsigned int, MetaDataMC::MetaData*> m_Data;
            std::map<unsigned int, MetaDataMC::MetaData*>::iterator m_ActMeta;
            MetaDataMC::MetaData* m_Inclusive;
            bool m_init;
    };
    class runMetaData: virtual public MetaDataElement {
        public:
            runMetaData(unsigned int runNumber, ToolHandle<XAMPP::IEventInfo> info);
            virtual StatusCode newFile(const xAOD::CutBookkeeperContainer* Container, MetaDataElement::BookKeeperType Type = MetaDataElement::BookKeeperType::Original);
            virtual StatusCode newFile(const xAOD::LumiBlockRange* LumiBlock);
            virtual StatusCode newEvent();
            virtual void CutBookKeeperAvailable(bool B);
            virtual StatusCode finalize(TTree* MetaDataTree);
            virtual StatusCode CopyStore(const MetaDataElement* Store);
            virtual StatusCode fillVariation(unsigned int, double);

            virtual ~runMetaData();
        private:
            unsigned int m_runNumber;
            ToolHandle<XAMPP::IEventInfo> m_XAMPPInfo;

            Long64_t m_NumTotalEvents;
            Long64_t m_NumProcessedEvents;
            std::set<unsigned int> m_ProcessedBlocks;
            std::set<unsigned int> m_TotalBlocks;
            bool m_KeeperAvailable;

    };
}
#endif
