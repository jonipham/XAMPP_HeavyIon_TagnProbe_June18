// HIEventUtils includes
#include "TestCentralityCalib.h"



TestCentralityCalib::TestCentralityCalib( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator )
, m_centTool("HI::HICentralityTool")
{

  //declareProperty( "Property", m_nProperty ); //example property declaration
  declareProperty("HICentralityTool",  m_centTool);

}


TestCentralityCalib::~TestCentralityCalib() {}


StatusCode TestCentralityCalib::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ATH_CHECK( m_centTool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode TestCentralityCalib::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode TestCentralityCalib::execute() {  

  double sumEt = 0;
  sumEt = m_centTool->getCentralityEstimator();
  double percentile = 0;
  percentile = m_centTool->getCentralityPercentile();
  double percentile_up = m_centTool->getCentralityPercentileUp();
  double percentile_down = m_centTool->getCentralityPercentileDown();
  ATH_MSG_DEBUG( "SumET of the event = " << sumEt );
  ATH_MSG_DEBUG( "Centrality of the event = " << percentile << "%");
  ATH_MSG_DEBUG( "Centrality variation of the event " << percentile_down << "% ~ " << percentile_up << "%");

  return StatusCode::SUCCESS;
}


