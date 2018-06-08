from XAMPPbase.SubmitToGrid import *
from XAMPPanalyses.runDMbb import *
    
if __name__ == '__main__':   
    parser = setupDMbbArgParser(setupGridSubmitArgParser())
    parser.set_defaults(exe='XAMPPanalyses/python/runDMbb.py')
    RunOptions = parser.parse_args()   
    
    AnaOpt = AssembleRemoteRunCmd(RunOptions)
    if RunOptions.noRecluster:
        AnaOpt.append("--noRecluster")
    if RunOptions.noTrackMET:
        AnaOpt.append("--noTrackMET")
    if RunOptions.isMC15a:
        AnaOpt.append("--isMC15a")
    SubmitJobs (RunOptions,AnaOpt)