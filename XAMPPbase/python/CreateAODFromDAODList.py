import sys, os, argparse


if __name__ == '__main__':

    OutDir = os.getcwd()
    
    parser = argparse.ArgumentParser(description='This script converts DAOD filelists to AOD filelists which then can be used for creating pileup reweighting files.', prog='CreateAODFromDAODList', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-d', '-D', '--datasets', help='DAOD filelist to be converted into AOD', default='')
    parser.add_argument('-p', '-P', '--ptag', help='ptag to replace', default='')
    RunOptions = parser.parse_args()

    print 'The following DAODs are converted into ADOs:\n'
    DAODsToConvert = []
    # Do we have one dataset, or a file with a list of them?
    if os.path.exists(RunOptions.datasets):
        dsfile = open(RunOptions.datasets)
        for line in dsfile:
            if line.startswith('#'): continue
            realline = line.strip()
            if not realline: continue # Ignore whitespace
            DAODsToConvert.append(realline)
            print realline
    else:
        print RunOptions.datasets
        print '\nThe ADO is:\n'
        print RunOptions.datasets.replace('DAOD_SUSY1', 'AOD').replace(RunOptions.ptag, '')
        sys.exit(1)
            
    print '\nThe ADOs are:\n'
    for daod in DAODsToConvert:
        print daod.replace('DAOD_SUSY1', 'AOD').replace(RunOptions.ptag, '')
