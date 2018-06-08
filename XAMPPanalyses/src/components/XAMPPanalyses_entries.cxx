#include "GaudiKernel/DeclareFactoryEntries.h"

//FourLepton includes
#include <XAMPPanalyses/Stop0LAnalysisHelper.h>
#include <XAMPPanalyses/Stop0LAnalysisConfig.h>
#include <XAMPPanalyses/Stop0LMetSelector.h>
#include <XAMPPanalyses/Stop0LTruthAnalysisHelper.h>
#include <XAMPPanalyses/Stop0LTruthSelector.h>
#include <XAMPPanalyses/DMbbAnalysisConfig.h>
#include <XAMPPanalyses/DMbbAnalysisHelper.h>
#include <XAMPPanalyses/JVTAnalysisHelper.h>
#include <XAMPPanalyses/JVTAnalysisConfig.h>


// using namespace XAMPP;
DECLARE_NAMESPACE_TOOL_FACTORY(XAMPP, Stop0LAnalysisConfig)
DECLARE_NAMESPACE_TOOL_FACTORY(XAMPP, Stop0LAnalysisHelper)

DECLARE_NAMESPACE_TOOL_FACTORY(XAMPP, Stop0LGammaAnalysisConfig)
DECLARE_NAMESPACE_TOOL_FACTORY(XAMPP, Stop0LGammaAnalysisHelper)

DECLARE_NAMESPACE_TOOL_FACTORY(XAMPP, Stop0LTruthAnalysisHelper)

DECLARE_NAMESPACE_TOOL_FACTORY(XAMPP, Stop0LTruthSelector)
DECLARE_NAMESPACE_TOOL_FACTORY(XAMPP, Stop0LMetSelector)
DECLARE_NAMESPACE_TOOL_FACTORY(XAMPP, Stop0LISRSelector)

DECLARE_NAMESPACE_TOOL_FACTORY(XAMPP, DMbbAnalysisConfig)
DECLARE_NAMESPACE_TOOL_FACTORY(XAMPP, DMbbAnalysisHelper)

DECLARE_NAMESPACE_TOOL_FACTORY(XAMPP, JVTAnalysisHelper)
DECLARE_NAMESPACE_TOOL_FACTORY(XAMPP, JVTAnalysisConfig)

DECLARE_FACTORY_ENTRIES(XAMPPanalyses) {

DECLARE_NAMESPACE_TOOL(XAMPP, Stop0LAnalysisConfig)
DECLARE_NAMESPACE_TOOL(XAMPP, Stop0LAnalysisHelper)

DECLARE_NAMESPACE_TOOL(XAMPP, Stop0LTruthAnalysisHelper)
DECLARE_NAMESPACE_TOOL(XAMPP, Stop0LTruthSelector)
DECLARE_NAMESPACE_TOOL(XAMPP, Stop0LMetSelector)
DECLARE_NAMESPACE_TOOL(XAMPP, Stop0LISRSelector)

DECLARE_NAMESPACE_TOOL(XAMPP, Stop0LGammaAnalysisConfig)
DECLARE_NAMESPACE_TOOL(XAMPP, Stop0LGammaAnalysisHelper)

DECLARE_NAMESPACE_TOOL(XAMPP, DMbbAnalysisConfig)
DECLARE_NAMESPACE_TOOL(XAMPP, DMbbAnalysisHelper)

DECLARE_NAMESPACE_TOOL(XAMPP, JVTAnalysisHelper)
DECLARE_NAMESPACE_TOOL(XAMPP, JVTAnalysisConfig)
}
