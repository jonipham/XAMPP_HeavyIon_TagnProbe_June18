/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//---------------------------------------------------------------
//  Test cross section files
//---------------------------------------------------------------
#include <gtest/gtest.h>

#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "SampleHandler/ToolsMeta.h"
#include "SampleHandler/ScanDir.h"

#include <TSystem.h>
#include <iostream>
#include <string>

int LoadXS(){

  SH::SampleHandler sh;
  
  try {
    readSusyMetaDir(sh, "$ROOTCOREBIN/data/SUSYTools/mc15_13TeV");
    readSusyMetaDir(sh, "$ROOTCOREBIN/data/SUSYTools/mc15_8TeV");
  }
  catch (std::exception e) {
    std::cout << "A problem was encountered when reading the xsec files: "
	      << e.what() << std::endl;
    return 1;
  }

  return 0;
}

TEST(XSECTIONTEST, VALFILES) {
  EXPECT_EQ(0, LoadXS());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
