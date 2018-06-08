// HICentralityTool.cxx

#include "HIEventUtils/HICentralityTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "PathResolver/PathResolver.h"

namespace HI
{

HICentralityTool::HICentralityTool(const std::string& myname) : 
  AsgTool(myname)
{
  declareProperty("RunSpecies", m_run = "PbPb2015");
}

HICentralityTool::~HICentralityTool() {
}



StatusCode HICentralityTool::initialize() {

  ATH_MSG_INFO( "Initializing HICentralityTool" );
  ATH_MSG_INFO( "!!The user should apply pile-up/diffractive rejection prior to using this tool!!" );
  // check if the set RunSpecies is supported
  if (m_run.compare("PbPb2015") && m_run.compare("pPb2016")) {
    ATH_MSG_ERROR("Unsupported RunSpecies has been set");
  }
  ATH_MSG_INFO( "Configured for run species: " << m_run.c_str() );
  ATH_MSG_INFO( "The centrality estimator returned by the tool is always [GeV] !!");

  // Load Centrality calibration files
  std::string rootFile_fullPath = "";
  rootFile_fullPath = PathResolverFindCalibFile(Form("HIEventUtils/HICentralityToolConfig_%s_CentralityCuts.root",m_run.c_str() ));

  h_FCalSumEt = 0;

  TFile* f_calib = TFile::Open(rootFile_fullPath.c_str(),"READ");
  if ( !f_calib->IsOpen() ) {
    ATH_MSG_ERROR( "Cannot read Calibration file for " << m_run << " from " << rootFile_fullPath );
    return StatusCode::FAILURE;
  }
  if (m_run.compare("PbPb2015")==0) { // To be updated with PbPb calibrations
      ATH_CHECK( getHist( f_calib,"th1_PbPb2015_CentralityCuts", h_FCalSumEt) ) ;
      ATH_CHECK( getHist( f_calib,"th1_PbPb2015_CentralityCuts_HIGH", h_FCalSumEt_Up) ) ;
      ATH_CHECK( getHist( f_calib,"th1_PbPb2015_CentralityCuts_LOW", h_FCalSumEt_Down) ) ;
  } else if (m_run.compare("pPb2016")==0) {
      ATH_CHECK( getHist( f_calib,"centralityMap_eff98", h_FCalSumEt) ) ;
      ATH_CHECK( getHist( f_calib,"centralityMap_eff100", h_FCalSumEt_Up) ) ;
      ATH_CHECK( getHist( f_calib,"centralityMap_eff97", h_FCalSumEt_Down) ) ;
  }
  //h_FCalSumEt->Print();
  f_calib->Close();
  delete f_calib;

  ATH_CHECK( initialPedestal() );

  m_sumFCalEt_tot = -999;

  return StatusCode::SUCCESS;
}



StatusCode HICentralityTool::getHist( TFile* file, const char* histName, TH1D*& hist ){
  if ( !file ) {
    ATH_MSG_ERROR( "getHist(...) NULL TFile! Check that the Tight cut map is loaded correctly"  );
    return StatusCode::FAILURE;
  }
  hist = dynamic_cast<TH1D*>( file->Get( histName ) );
  if( !hist ){
    ATH_MSG_ERROR( "Cannot retrieve histogram " << histName  );
    return StatusCode::FAILURE;
  }
  hist->SetDirectory(0);
  ATH_MSG_INFO( "Successfully read centrality calibration histogram: " << hist->GetName()  );
  return StatusCode::SUCCESS;
}



StatusCode HICentralityTool::finalize() {
  return StatusCode::SUCCESS;
}



double HICentralityTool::getCentralityPercentile() {
  // need to calculate sumFCalET first
  double _sumFCalEt = getCentralityEstimator(); 

  ATH_MSG_DEBUG( "FCal sumET =  " << _sumFCalEt);
  if (_sumFCalEt == -99) { // _sumFCalEt == -99 indicates problem with the estimator calculation
    ATH_MSG_ERROR( "Problem with run species configuration" );
    return -999;
  }

  if (m_run.compare("PbPb2015") == 0) {
    _sumFCalEt /= 1e3; //have to do this since the PbPb file is in bins of TeV
    if (_sumFCalEt < 0.063719) {
      ATH_MSG_DEBUG( "sum Fcal Et =  " << _sumFCalEt << "TeV, is below the low boundary");
      return -1;
    }
  }

  if (m_run.compare("pPb2016") == 0 && _sumFCalEt < 3.07) { // most likely pile-up or diffractive events
    ATH_MSG_DEBUG( "sum Fcal Et =  " << _sumFCalEt << "GeV, is below the low boundary");
    return -1;
  }

  int bin = h_FCalSumEt->FindBin(_sumFCalEt);
  double _percent = h_FCalSumEt->GetBinContent(bin);

  ATH_MSG_DEBUG( "Centrality percentile =  " << _percent << "%");
  return _percent;
}



double HICentralityTool::getCentralityPercentileUp() {
  // need to calculate sumFCalET first
  double _sumFCalEt = getCentralityEstimator(); 

  ATH_MSG_DEBUG( "FCal sumET =  " << _sumFCalEt);
  if (_sumFCalEt == -99) { // _sumFCalEt == -99 indicates problem with the estimator calculation
    ATH_MSG_ERROR( "Problem with run species configuration" );
    return -999;
  }

  if (m_run.compare("PbPb2015") == 0) {
    _sumFCalEt /= 1e3; //have to do this since the PbPb file is in bins of TeV
    if (_sumFCalEt < 0.069136) {
      ATH_MSG_DEBUG( "sum Fcal Et =  " << _sumFCalEt << "TeV, is below the low boundary");
      return -1;
    }
  }

  if (m_run.compare("pPb2016") == 0 && _sumFCalEt < 3.75) { // most likely pile-up or diffractive events
     ATH_MSG_DEBUG( "sum Fcal Et =  " << _sumFCalEt << "GeV, is below the low boundary");
     return -1;
  }

  int bin = h_FCalSumEt_Up->FindBin(_sumFCalEt);
  double _percent = h_FCalSumEt_Up->GetBinContent(bin);
  ATH_MSG_DEBUG( "Centrality percentile up variaiton = " << _percent << "%");
  return _percent;
}



double HICentralityTool::getCentralityPercentileDown() {
  // need to calculate sumFCalET first
  double _sumFCalEt = getCentralityEstimator(); 

  ATH_MSG_DEBUG( "FCal sumET =  " << _sumFCalEt);
  if (_sumFCalEt == -99) { // _sumFCalEt == -99 indicates problem with the estimator calculation
    ATH_MSG_ERROR( "Problem with run species configuration" );
    return -999;
  }

  if (m_run.compare("PbPb2015") == 0) {
    _sumFCalEt /= 1e3; //have to do this since the PbPb file is in bins of TeV
    if (_sumFCalEt < 0.058527) {
      ATH_MSG_DEBUG( "sum Fcal Et =  " << _sumFCalEt << "TeV, is below the low boundary");
      return -1;
    }
  }

  if (m_run.compare("pPb2016") == 0 && _sumFCalEt < 2.71) { // most likely pile-up or diffractive events
     ATH_MSG_DEBUG( "sum Fcal Et =  " << _sumFCalEt << "GeV, is below the low boundary");
     return -1;
  }

  int bin = h_FCalSumEt_Down->FindBin(_sumFCalEt);
  double _percent = h_FCalSumEt_Down->GetBinContent(bin);
  ATH_MSG_DEBUG( "Centrality percentile down variation = " << _percent << "%");
  return _percent;
}



double HICentralityTool::getCentralityEstimator() {
  int _runNumber = 0;
  const xAOD::EventInfo* eventInfo = 0;
  if (evtStore()->retrieve(eventInfo, "EventInfo").isFailure()) {
    ATH_MSG_ERROR( "Could not retrieve eventInfo" );
    return -999;
  } else {
    ATH_MSG_DEBUG( "RunNumber" << eventInfo->runNumber());
    _runNumber = eventInfo->runNumber();
  }
  
  m_is_p_Pb = false;
  // Distinguish PbPb and pPb according to RunNumber
  // Maybe it's better to use MetaData
  // Singal only and overlay MC need to be considered next
  if (m_run.compare("PbPb2015") == 0) {
    if (_runNumber < 286030 || _runNumber > 287932) {
    	ATH_MSG_ERROR( "PbPb2015 is configured, but the runNumber indicate the data belong to a different run!" );
    }
    m_shapeContainerName = "CaloSums";
    ATH_MSG_DEBUG( "Run with 2015 PbPb data");
    ATH_MSG_DEBUG( "FCal sumET will be extracted from " << m_shapeContainerName);
  } else if (m_run.compare("pPb2016") == 0) {
    if (_runNumber < 313060 || _runNumber > 314171) {
    	ATH_MSG_ERROR( "pPb2016 is configured, but the runNumber indicate the data belong to a different run!" );
    }
    m_shapeContainerName = "HIEventShape";
    if (_runNumber < 313500) {
      m_is_p_Pb = true;
    } else {
      m_is_p_Pb = false;
    }
    ATH_MSG_DEBUG( "Run with 2016 pPb data.");
    ATH_MSG_DEBUG( "FCal sumET will be extracted from " << m_shapeContainerName);
  } else {
    ATH_MSG_ERROR( "Run with data not supported");
    return StatusCode::FAILURE;
  }

  const xAOD::HIEventShapeContainer* hies = 0;
  if (evtStore()->retrieve(hies,m_shapeContainerName).isFailure()) {
     ATH_MSG_ERROR( "Could not retrieve HIEventShape " << m_shapeContainerName);
     return -999;
  }

  const xAOD::VertexContainer* primaryVertexCollection = 0;
  if (evtStore()->retrieve(primaryVertexCollection,"PrimaryVertices").isFailure()) {
     ATH_MSG_ERROR( "Could not retrieve PrimaryVertices container!" );
     return -999;
  }
  if (m_run.compare("PbPb2015") == 0) {
  // 2015 PbPb runs 
  // FCal energy in GeV
  // Retrieve CaloSum EventShape
  // The configuration histogram is in TeV !! 
    for (auto ES : *hies) {
      float et = ES->et();
      const std::string name = ES->auxdataConst<std::string>("Summary");
      if (name=="FCal") m_sumFCalEt_tot = et*1e-3;
    }
  } else if (m_run.compare("pPb2016") == 0) {
  // 2016 pPb runs 
  // Retrieve HIEventShape
  // FCal energy in GeV
  // The configuration histogram is in GeV
    double _pv_z = 0;
    const xAOD::Vertex* vertex = (*primaryVertexCollection)[0];
    _pv_z = vertex->z();

    double _sumFCalEt_A = 0;
    double _sumFCalEt_C = 0;
    int size=hies->size();
    for (int ish=0;ish<size;ish++){
      const xAOD::HIEventShape *ES = hies->at(ish);
      int layer = ES->layer();
      if (layer != 21 && layer != 22 && layer != 23) continue;
      double et = ES->et()*1e-3;
      double eta = ES->etaMin();
      if (eta > 0) {
        _sumFCalEt_A += et;
      } else {
        _sumFCalEt_C += et;
      }
    }
    double fcalfactor = 1;
    if (m_is_p_Pb) {
      // p+Pb config using only side A sumET
      ATH_MSG_DEBUG( "Running p+Pb configuration" );
      ATH_MSG_DEBUG( "Uncalibrated FCal sum ET = " << _sumFCalEt_A);

      fcalfactor = 1./(-0.000166*_pv_z + 1);
      m_sumFCalEt_tot = fcalfactor*(_sumFCalEt_A-m_pedestal.at(_runNumber));

    } else {
      // Pb+p config using only side C sumET
      ATH_MSG_DEBUG( "Running Pb+p configuration" );
      ATH_MSG_DEBUG( "Uncalibrated FCal sum ET = " << _sumFCalEt_C);

      fcalfactor = 1./(0.000150*_pv_z + 1);
      m_sumFCalEt_tot = fcalfactor*(_sumFCalEt_C-m_pedestal.at(_runNumber))*0.989;
    }
    ATH_MSG_DEBUG( "fcalfactor = " << fcalfactor);
    ATH_MSG_DEBUG( "pedestal = " << m_pedestal.at(_runNumber));
    ATH_MSG_DEBUG( "Calibrated FCal sum ET = " << m_sumFCalEt_tot);
  } else {
    m_sumFCalEt_tot = -99;
  }
  return m_sumFCalEt_tot;
}



StatusCode HICentralityTool::initialPedestal() {

  m_pedestal.clear();

  if (m_run.compare("PbPb2015")==0) {
    ATH_MSG_WARNING( "Run by run pedestals are not supported for PbPb2015" );
  } else if (m_run.compare("pPb2016")==0) {
    m_pedestal = {
      {313063, -0.100872},
      {313067, -0.142919},
      {313100, -0.154325},
      {313107, -0.056707},
      {313136, 0.0310094},
      {313187, 0.0250888},
      {313259, -0.115792},
      {313285, -0.090617},
      {313295, -0.073289},
      {313333, -0.112103},
      {313435, -0.098324},
      {313572, 0.018611 },
      {313574, 0.0660433},
      {313575, 0.0486774},
      {313603, 0.120227 },
      {313629, 0.096567 },
      {313630, 0.137055 },
      {313688, 0.201423 },
      {313695, 0.219412 },
      {313833, 0.270607 },
      {313878, 0.281708 },
      {313929, 0.244653 },
      {313935, 0.245495 },
      {313984, 0.259454 },
      {314014, 0.227124 },
      {314077, 0.222499 },
      {314105, 0.23403  },  
      {314112, 0.18226  },  
      {314157, 0.185832 },
      {314170, 0.214693 } };
  }
  return StatusCode::SUCCESS;    
}

} // HI
