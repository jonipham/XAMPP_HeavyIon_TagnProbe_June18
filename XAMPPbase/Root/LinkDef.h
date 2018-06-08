#ifndef XAMPPbase_LINKDEF_H
#define XAMPPbase_LINKDEF_H
#include <XAMPPbase/SUSYAnalysisHelper.h>
#include <XAMPPbase/SUSYTruthAnalysisHelper.h>
#include <XAMPPbase/AnalysisConfig.h>
#include <XAMPPbase/SUSYElectronSelector.h>
#include <XAMPPbase/EventInfo.h>
#include <XAMPPbase/SUSYJetSelector.h>
#include <XAMPPbase/MetaDataTree.h>
#include <XAMPPbase/SUSYMetSelector.h>
#include <XAMPPbase/SUSYMuonSelector.h>
#include <XAMPPbase/SUSYPhotonSelector.h>
#include <XAMPPbase/SUSYSystematics.h>
#include <XAMPPbase/SUSYTauSelector.h>
#include <XAMPPbase/SUSYTriggerTool.h>
#include <XAMPPbase/SUSYTruthSelector.h>
#include <XAMPPbase/xAODLoop.h>
//#include <XAMPPbase/SUSYExecutableWrapper.h>
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#endif

#ifdef __CINT__
#pragma link C++ namespace XAMPP;
#pragma link C++ class XAMPP::SUSYAnalysisHelper+;
#pragma link C++ class XAMPP::SUSYTruthAnalysisHelper+;
#pragma link C++ class XAMPP::AnalysisConfig+;
#pragma link C++ class XAMPP::SUSYElectronSelector;
#pragma link C++ class XAMPP::EventInfo;
#pragma link C++ class XAMPP::SUSYJetSelector;
#pragma link C++ class XAMPP::SUSYMetSelector;
#pragma link C++ class XAMPP::SUSYMuonSelector;
#pragma link C++ class XAMPP::SUSYPhotonSelector;
#pragma link C++ class XAMPP::SUSYSystematics+;
#pragma link C++ class XAMPP::SUSYTauSelector+;
#pragma link C++ class XAMPP::SUSYTriggerTool+;
#pragma link C++ class XAMPP::SUSYTruthSelector+;
#pragma link C++ class XAMPP::xAODLoop+;
#pragma link C++ class XAMPP::MetaDataTree;
#endif

#endif
