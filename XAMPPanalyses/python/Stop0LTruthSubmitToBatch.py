from XAMPPbase.SubmitToBatch import *
from XAMPPanalyses.runStop0Lep import *
    
if __name__ == '__main__':   
    parser = setupSUSYArgParser(setupBatchSubmitArgParser())
    parser.set_defaults(exe='XAMPPanalyses/python/runTruthStop0Lep.py')
    RunOptions = parser.parse_args()   
    AnaOpt = AssembleRemoteRunCmd(RunOptions)
    SubmitJob (RunOptions,AnaOpt)
        
    
    
   
    
   
    
   
    
