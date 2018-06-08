from XAMPPbase.SubmitToGrid import *
from XAMPPanalyses.runStop0LepGamma import *
    
if __name__ == '__main__':   
    parser = setupStop0LArgParser(setupGridSubmitArgParser())
    parser.set_defaults(exe='XAMPPanalyses/python/runStop0LepGamma.py')
    RunOptions = parser.parse_args()   
    
    AnaOpt = AssembleRemoteRunCmd(RunOptions)
    if RunOptions.noRecluster:
        AnaOpt.append("--noRecluster")
    if RunOptions.noTrackMET:
        AnaOpt.append("--noTrackMET")
   
    SubmitJobs (RunOptions,AnaOpt)