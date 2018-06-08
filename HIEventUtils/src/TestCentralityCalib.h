#ifndef HIEVENTUTILS_TESTCENTRALITYCALIB_H
#define HIEVENTUTILS_TESTCENTRALITYCALIB_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!
#include "HIEventUtils/HICentralityTool.h"
#include "HIEventUtils/IHICentralityTool.h"



class TestCentralityCalib: public ::AthAlgorithm { 
 public: 
  TestCentralityCalib( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TestCentralityCalib(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 
  ToolHandle<HI::IHICentralityTool> m_centTool;

}; 

#endif //> !HIEVENTUTILS_TESTCENTRALITYCALIB_H
