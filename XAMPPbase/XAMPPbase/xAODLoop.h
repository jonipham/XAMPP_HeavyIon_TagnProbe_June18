#ifndef xAODLoop_H
#define xAODLoop_H

#include <AsgTools/IAsgTool.h>
#include <AsgTools/AsgMetadataTool.h>
#include <AsgTools/ToolHandle.h>

#include <XAMPPbase/Defs.h>
#include <xAODEventInfo/EventInfo.h>

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <csignal>
#include <TFile.h>
#include <iomanip>

namespace xAOD {
    class TEvent;
    class TStore;
}

namespace XAMPP {
#ifdef XAOD_STANDALONE

    class ISystematics;
    class IAnalysisHelper;

    class xAODLoop: public asg::AsgMetadataTool{
        public:

            xAODLoop(std::string myname);

            virtual StatusCode initialize();
            virtual StatusCode beginEvent();
            virtual StatusCode beginInputFile();
            virtual StatusCode finalize();

            virtual ~xAODLoop();
            StatusCode ExecuteLoop();
            virtual StatusCode CheckCutflow();
            virtual StatusCode ExecuteEvent();

        private:
            bool isData()const;
            // utility method - print seconds in HH::MM::SS format
            std::string TimeHMS(float t);
            ToolHandle<XAMPP::ISystematics> m_systematics;
            ToolHandle<XAMPP::IAnalysisHelper> m_helper;
            bool m_RunCutFlow;

            int m_output_level;
            int m_printInterval;
            bool m_isPrimaryAOD;
            std::string m_InFileName;
            unsigned int m_SkipEvents;
            unsigned int m_MaxEvents;
            int m_RunType;
            bool m_printProgress;
            bool m_init_LoadFirstEv;
            TFile* m_InFile;

            bool m_GotFileMetaData;
            bool m_init;

            StatusCode RetrieveEventInfo();
            StatusCode CheckInputFile(const std::string &filename);
            StatusCode BuildFileLists();
            StatusCode LoadFile(const std::string & filename);

            void CloseFile();

            std::vector<std::string> ListDirectory(std::string Path, std::string WildCard);
            std::vector<std::string> m_inputFiles;
            xAOD::TEvent* m_event;
            xAOD::TStore* m_store;

            static void SignalHandler(int SigNum);

            Long64_t m_totEntries;
            unsigned int m_totSyst;

            const xAOD::EventInfo* m_ConstEvtInfo = NULL;

    };
#endif
}

#endif
