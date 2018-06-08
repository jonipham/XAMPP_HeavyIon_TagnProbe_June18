// HICentralityTool.h

#ifndef HICENTRALITYTOOL_H
#define HICENTRALITYTOOL_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "HIEventUtils/IHICentralityTool.h"

#include "TFile.h"
#include "TH1.h"

namespace HI {

  class HICentralityTool : 
  public asg::AsgTool, virtual public HI::IHICentralityTool {
    ASG_TOOL_CLASS(HICentralityTool, IHICentralityTool)
  public:

    HICentralityTool( const std::string& myname="HICentralityTool" );
    virtual ~HICentralityTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    StatusCode getHist( TFile* file, const char* histName, TH1D*& hist );
    StatusCode initialPedestal();

    // return the centrality percentile 
    virtual double getCentralityPercentile();
    // percentile varaition accessor for particular analysis
    virtual double getCentralityPercentileUp();
    virtual double getCentralityPercentileDown();

    // return the sumFCalET values used to classify centrality 
    // may be useful for p+Pb
    virtual double getCentralityEstimator();

  private:

    // Property members
    std::string m_run;

    // private data members
    bool m_is_p_Pb;
    double m_sumFCalEt_tot;
    std::string m_shapeContainerName;
    std::map<int, double> m_pedestal; // run by run pedestal

    // 1D histograms with x = estimator and y = percentile
    TH1D* h_FCalSumEt;
    TH1D* h_FCalSumEt_Up;
    TH1D* h_FCalSumEt_Down;
  };
}
#endif
