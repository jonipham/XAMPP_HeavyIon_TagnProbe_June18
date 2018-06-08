#ifdef XAOD_STANDALONE
#include <memory>
#include <string>
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "AsgTools/Check.h"
#include "CxxUtils/make_unique.h"
#include "HIEventUtils/HICentralityTool.h"
#include "TError.h"
#include "TFile.h"

using std::string;
using std::unique_ptr;
using CxxUtils::make_unique;

int main(int argc, char* argv[])
{
  string appName = argv[0];
  if( !xAOD::Init(appName.data()).isSuccess() ) {
    Error( appName.data(), "Could not call xAOD::Init()" );
    return 1;
  }
  std::cout << "Running " << appName.c_str() << std::endl;

  string fileName;
  // Check if we received a file name:
  if( argc < 2 ) {
    fileName = getenv("ROOTCORE_TEST_FILE");
    if (fileName.empty()) {
      Error( appName.data(), "No file name received!" );
      Error( appName.data(), "  Usage: %s [xAOD file name]", appName.data() );
      return 1;
    }
  } else {
    fileName = argv[ 1 ];
    std::cout << "ReadFile: " << fileName.c_str() << std::endl;
  }

  unique_ptr<TFile> fin( TFile::Open( fileName.data(), "READ" ) );

  auto event = make_unique<xAOD::TEvent>( fin.get(), xAOD::TEvent::kClassAccess );
  Long64_t entries = event->getEntries();
  Info( appName.data(), "Number of events in file: %llu", entries );

  auto centTool = make_unique< HI::HICentralityTool >("CentralityTool");
  ASG_CHECK_SA( appName, centTool->initialize() );

  for (Long64_t i_evt=0; i_evt<entries; ++i_evt) {
    event->getEntry( i_evt );

    double sumEt = 0;
    sumEt = centTool->getCentralityEstimator();
    double percentile = 0;
    percentile = centTool->getCentralityPercentile();
    std::cout << "SumET of the event = " << sumEt << std::endl;
    std::cout << "Centrality of the event = " << percentile << std::endl;
    
  }
  ASG_CHECK_SA( appName, centTool->finalize() );
  return 0;
}
#endif // XAOD_STANDALONE
