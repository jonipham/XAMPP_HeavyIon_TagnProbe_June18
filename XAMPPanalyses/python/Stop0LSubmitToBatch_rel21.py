from XAMPPbase.SubmitToBatch import *
from XAMPPanalyses.runStop0Lep import *
    
if __name__ == '__main__':   
    parser = setupStop0LArgParser(setupBatchSubmitArgParser())
    parser.set_defaults(exe='XAMPPanalyses/runStop0LAnalysis.py')
    RunOptions = parser.parse_args()

    AnaOpt = AssembleRemoteRunCmd(RunOptions)
   
    SubmitJob (RunOptions,AnaOpt)
        
    
    
   
    
   
    
   
    
