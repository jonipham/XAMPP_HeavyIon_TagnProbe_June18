from XAMPPbase.SubmitToBatch import *
from XAMPPanalyses.runStop0Lep import *
    
if __name__ == '__main__':   
    parser = setupStop0LArgParser(setupBatchSubmitArgParser())
    parser.set_defaults(exe='XAMPPanalyses/python/runStop0Lep.py')
    RunOptions = parser.parse_args()   

    AnaOpt = AssembleRemoteRunCmd(RunOptions)
    if RunOptions.noRecluster:
        AnaOpt.append("--noRecluster")
    if RunOptions.noTrackMET:
        AnaOpt.append("--noTrackMET")
    if RunOptions.doISR:
        AnaOpt.append("--doISR")
   
    SubmitJob (RunOptions,AnaOpt)
        
    
    
   
    
   
    
   
    
