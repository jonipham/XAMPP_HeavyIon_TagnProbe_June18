Welcome to XAMPP!
===
    
Introduction
---

The XAMPPbase package can be used to run an analysis on ATLAS xAODs (original and derived). An analysis and its selections have to be defined. The standard output is a root file with cutflow histograms and trees for each systematic uncertainty affecting the event kinematics and each analysis. Additional analysis specific classes can be implemented in your own analysis package inheriting from XAMPPbase. SUSYTools is exploited for the implementation of the CP recommendations in all classes containing *SUSY* in their name. You can also implement your own CP tools independent of SUSYTools if needed. For further information please visit our Twiki https://twiki.cern.ch/twiki/bin/view/AtlasProtected/XAMPPSoftwareFramework.

Setup the package
---

We recommend working on your own branch an using Merge Requests through the gitlab web interface to push your changes to the master branch. You can create your branch by clicking on 'Branches' and choosing 'New branch'. If you prefer working in your own fork, feel free to create one from this repository.

a) Using RootCore (RC) in release 20.7:

```
rcSetup Base,2.4.30 (or higher!)
git clone ssh://git@gitlab.cern.ch:7999/atlas-mpp-xampp/XAMPPbase.git <your branch>
rc build
```
    
b) Using cmake in release 21:

```
mkdir source build run
cd source && asetup AthAnalysis,21.2.0,here
(cd source && asetup AnalysisBase,21.2.0,here)
git clone ssh://git@gitlab.cern.ch:7999/atlas-mpp-xampp/XAMPPbase.git <your branch>
cd ../build && cmake ../source && make
cd ../run && source ../build/x86_64-slc6-gcc49-opt/setup.sh
```

Running the code
---

In order to run on the first 1000 events of a single input file without systematics, please use

a) Using AnalysisBase:

```
python XAMPPbase/python/runSUSYAnalysis.py -i <file name> --nevents 1000 --noSyst 
```

For more possible options, use:

```
python XAMPPbase/python/runSUSYAnalysis.py -h
```

b) Using AthAnalysisBase / Athena:

```
athena -c "noSyst=True;inputDS=<file name>;nevents=1000" XAMPPbase/runXAMPPbase.py
```

Submitting to the grid
---

In order to submit your code to the grid, use:

a) Using AnalysisBase:

```
source XAMPPbase/scripts/PrepareGridSubmit.sh
python XAMPPbase/python/SubmitToGrid.py --exe XAMPPbase/python/runSUSYAnalysis.py -i <dataset name> --outDS <name of grid output container>
```

b) Using AthAnalysisBase / Athena:

```
python XAMPPbase/python/SubmitToGrid.py --jobOptions XAMPPbase/runXAMPPbase.py -i <dataset name> --outDS <name of grid output container>
```

Using the latest GRL
---

The latest GRL and ilumicalc file are present in
```
XAMPPbase/data/GRL
```

```
**********************************************
**********************************************
***                                        ***
***  XAMPPbase - (c) 2015-2017             ***
***  Developers:                           ***
***      jojungge         @ cern.ch        ***
***      nicolas.koehler  @ cern.ch        ***
***      francesca.ungaro @ cern.ch        ***
***                                        ***
**********************************************
**********************************************
```
