#ifndef XAMPPanalyses_LINKDEF_H
#define XAMPPanalyses_LINKDEF_H

#include <XAMPPanalyses/Stop0LAnalysisHelper.h>
#include <XAMPPanalyses/Stop0LAnalysisConfig.h>
#include <XAMPPanalyses/Stop0LMetSelector.h>
#include <XAMPPanalyses/Stop0LTruthAnalysisHelper.h>
#include <XAMPPanalyses/Stop0LTruthSelector.h>
#include <XAMPPanalyses/DMbbAnalysisConfig.h>
#include <XAMPPanalyses/DMbbAnalysisHelper.h>
#include <XAMPPanalyses/HeavyIonAnalysisHelper.h>
#include <XAMPPanalyses/HeavyIonAnalysisConfig.h>

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#endif

#ifdef __CINT__
#pragma link C++ class XAMPP::Stop0LAnalysisConfig;
#pragma link C++ class XAMPP::Stop0LGammaAnalysisConfig;
#pragma link C++ class XAMPP::Stop0LAnalysisHelper;
#pragma link C++ class XAMPP::Stop0LTruthAnalysisHelper;
#pragma link C++ class XAMPP::Stop0LTruthSelector;
#pragma link C++ class XAMPP::Stop0LGammaAnalysisHelper;
#pragma link C++ class XAMPP::Stop0LMetSelector;
#pragma link C++ class XAMPP::DMbbAnalysisConfig;
#pragma link C++ class XAMPP::DMbbAnalysisHelper;
#pragma link C++ class XAMPP::HeavyIonAnalysisHelper;
#pragma link C++ class XAMPP::HeavyIonAnalysisConfig;

#endif

#endif
