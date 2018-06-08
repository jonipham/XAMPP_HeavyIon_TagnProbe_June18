#include "GaudiKernel/DeclareFactoryEntries.h"
#include "HIEventUtils/HIEventShapeSummaryTool.h"
#include "HIEventUtils/HIVertexSelectionTool.h"
#include "HIEventUtils/HIEventSelectionTool.h"
#include "HIEventUtils/HICentralityTool.h"
#include "HIEventUtils/HITowerWeightTool.h"
#include "HIEventUtils/ZdcRecTool.h"

DECLARE_TOOL_FACTORY( HIEventShapeSummaryTool )
DECLARE_TOOL_FACTORY( HITowerWeightTool )
DECLARE_TOOL_FACTORY( HI::HIVertexSelectionTool )
DECLARE_TOOL_FACTORY( HI::HIEventSelectionTool )
DECLARE_TOOL_FACTORY( HI::HICentralityTool )
DECLARE_TOOL_FACTORY( ZDC::ZdcRecTool )


#include "../TestCentralityCalib.h"
DECLARE_ALGORITHM_FACTORY( TestCentralityCalib )

DECLARE_FACTORY_ENTRIES( HIEventUtils ) {
  DECLARE_ALGORITHM( TestCentralityCalib );
  DECLARE_TOOL( HIEventShapeSummaryTool );
  DECLARE_TOOL( HITowerWeightTool );
  DECLARE_TOOL( HI::HIVertexSelectionTool );
  DECLARE_TOOL( HI::HIEventSelectionTool );
  DECLARE_TOOL( HI::HICentralityTool );
  DECLARE_TOOL( ZDC::ZdcRecTool );
}

