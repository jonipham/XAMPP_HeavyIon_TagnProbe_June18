#ifndef XAMPPBASE_XAMPPALGORITHM_H
#define XAMPPBASE_XAMPPALGORITHM_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!
#include <TStopwatch.h>
#include <string>

namespace XAMPP {
    class IxAODLoop;
    class IAnalysisHelper;
    class ISystematics;
    class XAMPPalgorithm: public ::AthAlgorithm {
        public:
            XAMPPalgorithm(const std::string& name, ISvcLocator* pSvcLocator);
            virtual ~XAMPPalgorithm();

            virtual StatusCode initialize();
            virtual StatusCode execute();
            virtual StatusCode finalize();

        private:

            StatusCode CheckCutflow();
            StatusCode ExecuteEvent();

            ToolHandle<XAMPP::ISystematics> m_systematics;
            ToolHandle<XAMPP::IAnalysisHelper> m_helper;
        
            bool m_RunCutFlow;
            bool m_init;
            
            TStopwatch m_tsw;
            long long int m_Events;
            long long int m_CurrentEvent;
            long long int m_printInterval;
            unsigned int m_TotSyst;
        
            std::string TimeHMS(float t) const;

    };

}
#endif //> !XAMPPBASE_XAMPPALGORITHM_H
