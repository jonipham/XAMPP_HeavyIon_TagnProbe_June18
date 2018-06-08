#include <XAMPPbase/MetaDataTree.h>
#include <iostream>

#include <XAMPPbase/IAnalysisHelper.h>

#include <XAMPPbase/EventInfo.h>
#include <XAMPPbase/AnalysisUtils.h>
#include <xAODCutFlow/CutBookkeeper.h>
#include <xAODCutFlow/CutBookkeeperContainer.h>

#include <xAODMetaData/FileMetaData.h>

#include <xAODLuminosity/LumiBlockRange.h>
#include <xAODLuminosity/LumiBlockRangeContainer.h>

#ifndef XAOD_STANDALONE
#include <GaudiKernel/ITHistSvc.h>
#include <EventInfo/EventStreamInfo.h>
#endif
namespace XAMPP {
    MetaDataTree::MetaDataTree(std::string myname) :
                AsgMetadataTool(myname),
                m_TreeName(myname),
                m_UseFileMetaData(true),
                m_tree(0),
#ifdef XAOD_STANDALONE
                m_outFile(0),
                m_output_level(MSG::Level::INFO),
#else
                m_histSvc("THistSvc", myname),
#endif
                m_isData(false),
                m_init(false),
                m_analysis_helper("AnalysisHelper"),
                m_XAMPPInfo("EventInfoHandler"),
                m_MetaDB(),
                m_ActDB(m_MetaDB.end()),
                m_DummyEntry(NULL) {
#ifdef XAOD_STANDALONE
        declareProperty("OutputLevel", m_output_level);
        declareProperty("OutputFile", m_outFile);

#endif
        declareProperty("isData", m_isData);
        declareProperty("TreeName", m_TreeName);
        declareProperty("AnalysisHelper", m_analysis_helper);
        declareProperty("useFileMetaData", m_UseFileMetaData);
        m_XAMPPInfo.declarePropertyFor(this, "EventInfoHandler", "The XAMPPInfo event Handler");
    }

    StatusCode MetaDataTree::initialize() {
        ATH_MSG_INFO("Initialising...");
        ATH_CHECK(m_analysis_helper.retrieve());
        ATH_CHECK(m_XAMPPInfo.retrieve());
        if (m_init) {
            ATH_MSG_WARNING("The metadata tree is already intialized");
            return StatusCode::SUCCESS;
        }
#ifdef XAOD_STANDALONE
        if (!m_outFile) {
            ATH_MSG_ERROR("Output File not correctly set! Not possible to initialize Output Meta Data Tree");
            return StatusCode::FAILURE;
        }
        this->msg().setLevel(XAMPP::setOutputLevel(m_output_level));
        m_outFile->cd();
#endif
        m_tree = new TTree(m_TreeName.c_str(), "MetaData Tree for Small Analysis Ntuples");
        m_tree->Branch("isData", &m_isData);
#ifndef XAOD_STANDALONE
        ATH_CHECK(m_histSvc->regTree("/XAMPP/" + m_TreeName, m_tree));
#endif
        m_init = true;
        return StatusCode::SUCCESS;
    }
    MetaDataTree::~MetaDataTree() {
        for (auto& mcMeta : m_MetaDB)
            DeleteMetaData(mcMeta.second);
        DeleteMetaData(m_DummyEntry);
    }
    void MetaDataTree::LoadMCMetaData(unsigned int mcChannel) {
        if (m_ActDB == m_MetaDB.end() || m_ActDB->first != mcChannel) {
            m_ActDB = m_MetaDB.find(mcChannel);
            if (m_ActDB == m_MetaDB.end()) {
                ATH_MSG_INFO("Found new mcChannel " << mcChannel << ".  Create new MCMetaData entry.");
                m_MetaDB.insert(std::pair<unsigned int, MetaDataElement*>(mcChannel, new MetaDataMC(mcChannel, m_analysis_helper, m_XAMPPInfo.getHandle())));
                m_ActDB = m_MetaDB.find(mcChannel);
            }
        }
    }
    void MetaDataTree::LoadRunMetaData(unsigned int run) {
        if (m_ActDB == m_MetaDB.end() || m_ActDB->first != run) {
            m_ActDB = m_MetaDB.find(run);
            if (m_ActDB == m_MetaDB.end()) {
                ATH_MSG_INFO("Found new runNumber " << run << ".  Create new runMetaData entry.");
                m_MetaDB.insert(std::pair<unsigned int, MetaDataElement*>(run, new runMetaData(run, m_XAMPPInfo.getHandle())));
                m_ActDB = m_MetaDB.find(run);
            }
        }
    }
    void MetaDataTree::DeleteMetaData(MetaDataElement* &Meta) {
        if (Meta) delete Meta;
        Meta = NULL;
    }
    StatusCode MetaDataTree::beginEvent() {
        ATH_CHECK(m_analysis_helper->LoadContainers());
        if (m_XAMPPInfo->isMC() == m_isData) {
            ATH_MSG_FATAL("The analysis is configured to run over data while the input file is MC...");
            return StatusCode::FAILURE;
        }
        //The current event is MC
        if (m_XAMPPInfo->isMC()) {
            LoadMCMetaData(m_XAMPPInfo->mcChannelNumber());
        }
        //The event is data
        else LoadRunMetaData(m_XAMPPInfo->runNumber());

        if (m_DummyEntry) {
            ATH_CHECK(m_ActDB->second->CopyStore(m_DummyEntry));
            DeleteMetaData(m_DummyEntry);
        }
        ATH_CHECK(m_ActDB->second->newEvent());
        return StatusCode::SUCCESS;
    }
    StatusCode MetaDataTree::finalize() {
        if (!m_tree) {
            return StatusCode::SUCCESS;
        }
        for (const auto& Meta : m_MetaDB)
            ATH_CHECK(Meta.second->finalize(m_tree));
#ifdef XAOD_STANDALONE
        if (!m_outFile) {
            ATH_MSG_ERROR(" Something wrong: cannot find outFile to fill MD Tree");
            return StatusCode::FAILURE;
        }
        m_outFile->cd();
        m_tree->Write("", TObject::kOverwrite);

#endif
        m_tree = NULL;
        return StatusCode::SUCCESS;
    }
    StatusCode MetaDataTree::CheckLumiBlockContainer(const std::string& Container, bool &HasCont) {
        if (!inputMetaStore()->contains < xAOD::LumiBlockRangeContainer > (Container)) {
            ATH_MSG_DEBUG("Lumi block range container " << Container << " not present");
            return StatusCode::SUCCESS;
        }
        const xAOD::LumiBlockRangeContainer* LumiBlocks(0);
        ATH_CHECK(inputMetaStore()->retrieve(LumiBlocks, Container));
        for (const auto& lumi : *LumiBlocks) {
            if (lumi->startRunNumber() != lumi->stopRunNumber()) ATH_MSG_WARNING("Found different start (" << lumi->startRunNumber() << " and end (" << lumi->stopRunNumber() << ") runNumbers for lumiblock range " << lumi->startRunNumber() << "-" << lumi->stopLumiBlockNumber());
            LoadRunMetaData(lumi->startRunNumber());
            ATH_CHECK(m_ActDB->second->newFile(lumi));

        }
        HasCont = true;
        return StatusCode::SUCCESS;
    }
    StatusCode MetaDataTree::fillLHEMetaData(unsigned int Idx) {
        if (Idx == 0) {
            ATH_MSG_ERROR("This method is meant to store the variational weights in the metadata. Not nominal");
            return StatusCode::FAILURE;
        }
        if (m_isData) {
            ATH_MSG_ERROR("Data does not have any weight variations");
            return StatusCode::FAILURE;
        }
        const xAOD::EventInfo* info = m_XAMPPInfo->GetOrigInfo();

        if (Idx >= info->mcEventWeights().size()) {
            ATH_MSG_ERROR("Index " << Idx << " is out of range");
            return StatusCode::FAILURE;
        }
        LoadMCMetaData(m_XAMPPInfo->mcChannelNumber());
        double W = m_XAMPPInfo->GetGenWeight(Idx);
        unsigned int finalState = m_analysis_helper->finalState();
        if (finalState > 0 && !m_ActDB->second->fillVariation((Idx + 1000) * 1.e4 + finalState, W).isSuccess()) {
            return StatusCode::FAILURE;
        }
        return m_ActDB->second->fillVariation(Idx + 1000, W);
    }
    StatusCode MetaDataTree::beginInputFile() {
        if (!m_UseFileMetaData) return StatusCode::SUCCESS;
#ifdef XAOD_STANDALONE
        int mcChannel = -1;
        if (inputMetaStore()->contains < xAOD::FileMetaData > ("FileMetaData")) {
            const xAOD::FileMetaData* metaData;
            ATH_CHECK(inputMetaStore()->retrieve(metaData, "FileMetaData"));
            float fileMC = 0;
            if (metaData->value(xAOD::FileMetaData::MetaDataType::mcProcID, fileMC))
            mcChannel = fileMC;
            else ATH_MSG_WARNING("Failed to retrieve mcChannel from FileMetaData");
        }
#endif
        bool HasLumiCont = false;
        if (m_isData) {
            ATH_CHECK(CheckLumiBlockContainer("LumiBlocks", HasLumiCont));
            ATH_CHECK(CheckLumiBlockContainer("IncompleteLumiBlocks", HasLumiCont));
            for (auto& Meta : m_MetaDB)
                Meta.second->CutBookKeeperAvailable(HasLumiCont);
        }
        if (inputMetaStore()->contains < xAOD::CutBookkeeperContainer > ("CutBookkeepers")) {
            const xAOD::CutBookkeeperContainer* bks = 0;
            ATH_CHECK(inputMetaStore()->retrieve(bks, "CutBookkeepers"));
            if (!m_isData) {
                MetaDataElement* MC = NULL;
#ifdef XAOD_STANDALONE
                if (m_DummyEntry) {
                    ATH_MSG_ERROR("There is still an instance of the dummy meta");
                    return StatusCode::FAILURE;
                }
                if (mcChannel <= 0) {
                    ATH_MSG_INFO("The current file does not contain any event and it was not possible to derive the mcChannel");
                    m_DummyEntry = new MetaDataMC(-1, m_analysis_helper, m_XAMPPInfo.getHandle());
                    MC = m_DummyEntry;
                } else {
                    LoadMCMetaData(mcChannel);
                    MC = m_ActDB->second;
                }
#else
                const EventStreamInfo* esi = 0;
                ATH_CHECK(inputMetaStore()->retrieve(esi));
                if (esi->getEventTypes().size() > 1) ATH_MSG_WARNING("There seem to be more event types than one");
                LoadMCMetaData(esi->getEventTypes().begin()->mc_channel_number());
                MC = m_ActDB->second;
#endif
                ATH_CHECK(MC->newFile(bks, MetaDataElement::BookKeeperType::Original));
                if (inputMetaStore()->contains < xAOD::CutBookkeeperContainer > ("SUSYSumOfWeights")) {
                    ATH_CHECK(inputMetaStore()->retrieve(bks, "SUSYSumOfWeights"));
                    ATH_CHECK(MC->newFile(bks, MetaDataElement::BookKeeperType::SUSY));
                }
                if (inputMetaStore()->contains < xAOD::CutBookkeeperContainer > ("LHE3SumOfWeights")) {
                    ATH_CHECK(inputMetaStore()->retrieve(bks, "SUSYSumOfWeights"));
                    ATH_CHECK(MC->newFile(bks, MetaDataElement::BookKeeperType::LHE3));
                }

            }
            //Data instance and no lumi container is present
            else if (!HasLumiCont) {
                if (m_DummyEntry) {
                    ATH_MSG_WARNING("There is still an instance of the dummy meta");
                    DeleteMetaData(m_DummyEntry);
                }
                m_DummyEntry = new runMetaData(-1, m_XAMPPInfo.getHandle());
                ATH_CHECK(m_DummyEntry->newFile(bks));
            }
        } else {
            ATH_MSG_DEBUG("No CutBookKeeper has been found.");
            for (auto& Meta : m_MetaDB)
                Meta.second->CutBookKeeperAvailable(false);
        }
        return StatusCode::SUCCESS;
    }

    //########################################################################################################
    //                                 MetaDataElement
    //########################################################################################################
    const xAOD::CutBookkeeper* MetaDataElement::FindCutBookKeeper(const xAOD::CutBookkeeperContainer* container, MetaDataElement::BookKeeperType Type, unsigned int procID) {
        const xAOD::CutBookkeeper* all = 0;
        int maxCycle = -1; //need to find the max cycle where input stream is StreamAOD and the name is AllExecutedEvents       
        for (auto cbk : *container) {
            bool Stream = (Type == MetaDataElement::BookKeeperType::Original && (cbk->inputStream() == "StreamAOD" || cbk->inputStream().find("DAOD_TRUTH") != std::string::npos));
            if (Type == MetaDataElement::BookKeeperType::SUSY && procID > 0) {
                std::cout << cbk->inputStream() << " " << cbk->name() << std::endl;
            }
            if (Stream && cbk->cycle() > maxCycle) {
                maxCycle = cbk->cycle();
                all = cbk;
            }
        }
        return all;
    }
    //########################################################################################################
    //                                 MetaDataMC
    //########################################################################################################

    MetaDataMC::MetaDataMC(unsigned int mcChannel, ToolHandle<XAMPP::IAnalysisHelper> &helper, ToolHandle<XAMPP::IEventInfo> info) :
                m_MC(mcChannel),
                m_helper(helper),
                m_XAMPPInfo(info),
                m_Data(),
                m_ActMeta(m_Data.end()),
                m_Inclusive(0),
                m_init(false) {
        m_init = m_helper.retrieve().isSuccess() && m_XAMPPInfo.retrieve().isSuccess();
        if (m_init) LoadMetaData(0);
    }
    StatusCode MetaDataMC::newFile(const xAOD::LumiBlockRange*) {
        Warning("MetaDataMC::newFile()", "Lumi blocks not supported.");
        return StatusCode::SUCCESS;
    }
    void MetaDataMC::LoadMetaData(unsigned int ID) {
        //See whether the process ID is already Loaded;
        if (m_ActMeta == m_Data.end() || m_ActMeta->first != ID) {
            m_ActMeta = m_Data.find(ID);
            //No Meta data has been found
            if (m_ActMeta == m_Data.end()) {
                Info("MetaDataMC::LoadMetaData()", Form("New process Id %u found for mcChannelNumber %u", ID, m_MC));
                m_Data.insert(std::pair<unsigned int, MetaDataMC::MetaData*>(ID, new MetaDataMC::MetaData(ID)));
                m_ActMeta = m_Data.find(ID);
                //If the SUSY process id = 0 -> inclusive state
                if (ID == 0) m_Inclusive = m_ActMeta->second;
            }
        }
    }
    StatusCode MetaDataMC::newFile(const xAOD::CutBookkeeperContainer* container, MetaDataElement::BookKeeperType Type) {
        if (!m_init) {
            Error("MetaDataMC::newFile()", "The component has not been intialized");
            return StatusCode::FAILURE;
        }
        if (!container) {
            Error("MetaDataMC::newFile()", "No CutBookkeeperContainer was given");
            return StatusCode::FAILURE;
        }

        if (Type == MetaDataElement::BookKeeperType::Original) {
            CutBookKeeperAvailable(false);
            const xAOD::CutBookkeeper* all = FindCutBookKeeper(container);
            if (!all) {
                Error("MetaDataMC::newFile()", "Could not read out the CutBookKeeper");
                return StatusCode::FAILURE;
            }
            AddFileInformation(m_Inclusive, all->nAcceptedEvents(), all->sumOfEventWeights(), all->sumOfEventWeightsSquared());
        } else if (Type == MetaDataElement::BookKeeperType::SUSY) {
            //List of prcoess IDs
            //https://twiki.cern.ch/twiki/bin/view/AtlasProtected/SUSYSignalUncertainties#Subprocess_IDs
            for (int i = 1; i <= 220; ++i) {
                //Skip all ranges not occupied by any SUSYprocess
                if ((i > 4 && i < 51) || (i > 52 && i < 61) || (i > 62 && i < 71) || (i > 78 && i < 81) || (i > 89 && i < 111)) continue;
                if ((i > 118 && i < 122) || (i > 128 && i < 133) || (i > 138 && i < 144) || (i > 158 && i < 167)) continue;
                if ((i > 168 && i < 201) || i == 215) continue;
                const xAOD::CutBookkeeper* proc = FindCutBookKeeper(container, Type, i);
                if (!proc || !proc->nAcceptedEvents()) continue;
                LoadMetaData(i);
                AddFileInformation(m_ActMeta->second, proc->nAcceptedEvents(), proc->sumOfEventWeights(), proc->sumOfEventWeightsSquared());
                return StatusCode::SUCCESS;
            }
        } else if (Type == MetaDataElement::BookKeeperType::LHE3) {
            return StatusCode::SUCCESS;
        }

        return StatusCode::SUCCESS;
    }
    StatusCode MetaDataMC::newEvent() {
        if (!m_init) {
            Error("MetaDataMC::newFile()", "The component has not been inialized");
            return StatusCode::FAILURE;
        }
        unsigned int ProcID = m_helper->finalState();
        double Weight = m_XAMPPInfo->GetGenWeight();

        if (ProcID > 0) {
            LoadMetaData(ProcID);
            AddEventInformation(m_ActMeta->second, Weight);
        }
        AddEventInformation(m_Inclusive, Weight);
        return StatusCode::SUCCESS;
    }
    StatusCode MetaDataMC::fillVariation(unsigned int Id, double W) {
        if (Id <= 1000) {
            Error("MetaDataMC::fillVariation", Form("Process id %u is protected.", Id));
            return StatusCode::FAILURE;
        }
        LoadMetaData(Id);
        AddEventInformation(m_ActMeta->second, W);
        return StatusCode::SUCCESS;
    }

    void MetaDataMC::AddEventInformation(MetaDataMC::MetaData* Meta, double GenWeight) {
        ++Meta->NumProcessedEvents;
        if (!Meta->MetaInit) {
            //Retrieve the processId
            unsigned int SUSYId = Meta->ProcID != 0 ? m_helper->finalState() : 0;
            Meta->runNumber = m_XAMPPInfo->runNumber();
            Meta->xSec = m_helper->GetMCXsec(m_MC, SUSYId);
            Meta->FilterEff = m_helper->GetMCFilterEff(m_MC, SUSYId);
            Meta->kFaktor = m_helper->GetMCkFactor(m_MC, SUSYId);
            Meta->MetaInit = true;
            double xSecTimes = Meta->xSec * Meta->kFaktor * Meta->FilterEff;
            Info("MetaDataMC::AddEventInformation()", Form("Set xSection to %f for process %u with mcChannelNumber %u", xSecTimes, Meta->ProcID, m_MC));
        }
        if (Meta->KeeperAvailable) return;
        Meta->SumW = Meta->SumW + GenWeight;
        Meta->SumW2 = Meta->SumW2 + GenWeight * GenWeight;
        ++Meta->NumTotalEvents;
    }
    void MetaDataMC::CutBookKeeperAvailable(bool B) {
        for (auto& MC : m_Data)
            MC.second->KeeperAvailable = B;
    }
    StatusCode MetaDataMC::finalize(TTree* MetaDataTree) {
        if (!m_init) {
            Error("MetaDataMC::finalize()", "The component has not been inialized");
            return StatusCode::FAILURE;
        }
        if (!SetBranchAddress(MetaDataTree, "mcChannelNumber", m_MC)) {
            Error("MetaDataMC::finalize()", Form("Cannot finalize DSID %u", m_MC));
            return StatusCode::FAILURE;
        }
        for (auto& MC : m_Data) {
            if (!SaveMetaDataInTree(MC.second, MetaDataTree)) {
                Error("MetaDataMC::finalize()", Form("Cannot finalize DSID %u", m_MC));
                return StatusCode::FAILURE;
            }
        }
        m_init = false;
        return StatusCode::SUCCESS;
    }
    bool MetaDataMC::SaveMetaDataInTree(MetaDataMC::MetaData* Meta, TTree* tree) {
        if (!Meta) {
            Error("MetaDataMC::SaveMetaDataInTree()", "No element has been given");
            return false;
        }
        if (!SetBranchAddress(tree, "ProcessID", Meta->ProcID)) return false;
        if (!SetBranchAddress(tree, "runNumber", Meta->runNumber)) return false;
        if (!SetBranchAddress(tree, "xSection", Meta->xSec)) return false;
        if (!SetBranchAddress(tree, "kFactor", Meta->kFaktor)) return false;
        if (!SetBranchAddress(tree, "FilterEfficiency", Meta->FilterEff)) return false;
        if (!SetBranchAddress(tree, "TotalEvents", Meta->NumTotalEvents)) return false;
        if (!SetBranchAddress(tree, "TotalSumW", Meta->SumW)) return false;
        if (!SetBranchAddress(tree, "TotalSumW2", Meta->SumW2)) return false;
        if (!SetBranchAddress(tree, "ProcessedEvents", Meta->NumProcessedEvents)) return false;

        tree->Fill();
        Info("MetaDataMC::SaveMetaDataInTree()", Form("Write metadata in file with DSID: %u, ProcessID : %u, SumW: %f, SumW2: %f, TotalEvents: %llu, ProcessedEvents: %llu", m_MC, Meta->ProcID, Meta->SumW, Meta->SumW2, Meta->NumTotalEvents, Meta->NumProcessedEvents));
        return true;
    }
    MetaDataMC::~MetaDataMC() {
        for (auto& Meta : m_Data) {
            if (Meta.second) delete Meta.second;
        }
        m_Data.clear();
    }
    void MetaDataMC::AddFileInformation(MetaDataMC::MetaData* Meta, Long64_t TotEv, double SumW, double SumW2) {
        Meta->NumTotalEvents = Meta->NumTotalEvents + TotEv;
        Meta->SumW = Meta->SumW + SumW;
        Meta->SumW2 = Meta->SumW2 + SumW2;
        Meta->KeeperAvailable = true;
    }
    StatusCode MetaDataMC::CopyStore(const MetaDataElement* Store) {
        const MetaDataMC* Other = dynamic_cast<const MetaDataMC*>(Store);
        if (!Store) {
            Error("MetaDataMC::CopyStore()", "Could not copy the input store");
            return StatusCode::FAILURE;
        }
        if (Other->m_MC != m_MC && Other->m_MC != (unsigned int) -1) {
            Error("MetaDataMC::CopyStore()", "Wrong MC channel");
            return StatusCode::FAILURE;
        }
        for (auto& toCopy : Other->m_Data) {
            LoadMetaData(toCopy.second->ProcID);
            AddFileInformation(m_ActMeta->second, toCopy.second->NumTotalEvents, toCopy.second->SumW, toCopy.second->SumW2);
        }
        return StatusCode::SUCCESS;
    }
//########################################################################################################
//                                 runMetaData
//########################################################################################################
    runMetaData::runMetaData(unsigned int runNumber, ToolHandle<XAMPP::IEventInfo> info) :
                m_runNumber(runNumber),
                m_XAMPPInfo(info),
                m_NumTotalEvents(0),
                m_NumProcessedEvents(0),
                m_ProcessedBlocks(),
                m_TotalBlocks(),
                m_KeeperAvailable(false) {
    }
    StatusCode runMetaData::newFile(const xAOD::CutBookkeeperContainer* container, MetaDataElement::BookKeeperType Type) {
        const xAOD::CutBookkeeper* all = FindCutBookKeeper(container);
        if (!all) {
            Error("MetaDataMC::newFile()", "Could not read out the CutBookKeeper");
            return StatusCode::FAILURE;
        }
        if (Type != MetaDataElement::BookKeeperType::Original) {
            Error("MetaDataMC::newFile()", "Wrong CutBookKeeper given");
            return StatusCode::FAILURE;
        }
        CutBookKeeperAvailable(true);
        m_NumTotalEvents = m_NumTotalEvents + all->nAcceptedEvents();
        return StatusCode::SUCCESS;
    }
    StatusCode runMetaData::newEvent() {
        ++m_NumProcessedEvents;
        m_ProcessedBlocks.insert(m_XAMPPInfo->GetOrigInfo()->lumiBlock());
        if (!m_KeeperAvailable) {
            ++m_NumTotalEvents;
            m_TotalBlocks.insert(m_XAMPPInfo->GetOrigInfo()->lumiBlock());
        }
        return StatusCode::SUCCESS;
    }
    void runMetaData::CutBookKeeperAvailable(bool B) {
        m_KeeperAvailable = B;
    }
    StatusCode runMetaData::finalize(TTree* MetaDataTree) {
        if (!SetBranchAddress(MetaDataTree, "runNumber", m_runNumber)) return StatusCode::FAILURE;
        if (!SetBranchAddress(MetaDataTree, "TotalEvents", m_NumTotalEvents)) return StatusCode::FAILURE;
        if (!SetBranchAddress(MetaDataTree, "ProcessedEvents", m_NumProcessedEvents)) return StatusCode::FAILURE;
        std::set<unsigned int>* ProcLumiPtr = &m_ProcessedBlocks;
        if (!SetBranchAddress(MetaDataTree, "ProcessedLumiBlocks", ProcLumiPtr)) return StatusCode::FAILURE;
        std::set<unsigned int>* TotalLumiPtr = &m_TotalBlocks;
        if (!SetBranchAddress(MetaDataTree, "TotalLumiBlocks", TotalLumiPtr)) return StatusCode::FAILURE;

        Info("runMetaData::finalize()", Form("Fill metadata tree with runNumber: %u, TotalEvents: %llu, ProcessedEvents: %llu", m_runNumber, m_NumTotalEvents, m_NumProcessedEvents));
        MetaDataTree->Fill();
        return StatusCode::SUCCESS;
    }
    StatusCode runMetaData::CopyStore(const MetaDataElement* Store) {
        const runMetaData* toCopy = dynamic_cast<const runMetaData*>(Store);
        if (!toCopy) {
            Error("runMetaData::CopyStore()", "Failed to copy information");
            return StatusCode::FAILURE;
        }
        if (toCopy->m_runNumber != m_runNumber && toCopy->m_runNumber != (unsigned int) -1) {
            Error("runMetaData::CopyStore()", "Wrong run to copy");
            return StatusCode::FAILURE;
        }
        m_NumProcessedEvents += toCopy->m_NumProcessedEvents;
        m_NumTotalEvents += toCopy->m_NumTotalEvents;
        for (auto& BCID : toCopy->m_ProcessedBlocks)
            m_ProcessedBlocks.insert(BCID);
        for (auto& BCID : toCopy->m_TotalBlocks)
            m_TotalBlocks.insert(BCID);
        return StatusCode::SUCCESS;
    }
    runMetaData::~runMetaData() {
        m_TotalBlocks.clear();
        m_ProcessedBlocks.clear();
    }
    StatusCode runMetaData::newFile(const xAOD::LumiBlockRange* LumiBlock) {
        if (!LumiBlock) {
            Error("runMetaData::newFile()", "No LumiBlock element given");
            return StatusCode::FAILURE;
        }
        m_NumTotalEvents = m_NumTotalEvents + LumiBlock->eventsSeen();
        for (unsigned int L = LumiBlock->startLumiBlockNumber(); L <= LumiBlock->stopLumiBlockNumber(); ++L)
            m_TotalBlocks.insert(L);

        return StatusCode::SUCCESS;
    }
    StatusCode runMetaData::fillVariation(unsigned int, double) {
        return StatusCode::FAILURE;
    }

}
