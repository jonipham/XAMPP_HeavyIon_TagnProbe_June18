#! /usr/bin/env python
from XAMPPbase.ListGroupDisk import *

def ReadListFromFile (File):
    List = []
    if os.path.isfile(File) == True:
        for Line in open(File):
            List.append(Line.replace("\n", "").replace("/",""))
    return List
 
if __name__ == '__main__':
    
    parser = argparse.ArgumentParser(description='This script lists datasets located at a RSE location. Futher patterns to find or exclude can be specified.', prog='ListGroupDisk', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-r', '-R', '--RSE', help='specify RSE storage element (default: %s)' % RSE, default=RSE)
    parser.add_argument('--list', help='specify a list containing the datasets to be requested', required = True)
    parser.add_argument("--Rucio" , help="With this option you can set the rucio_account. Default %s" % RUCIO_ACC, default=RUCIO_ACC)
    parser.add_argument("--lifetime", help="Defines a lifetime after which the rules are automatically deleted", type=int, default=-1)
    RunOptions = parser.parse_args()
    List = ReadListFromFile(RunOptions.list)
    for Item in List:
        if Item.startswith("#") or len(Item)==0:
            continue
        print "Request new rule for " + Item+" to "+RunOptions.RSE
        if RunOptions.lifetime > -1:
            Cmd=" rucio add-rule --account %s --lifetime %d %s 1 %s"%(RunOptions.Rucio,RunOptions.lifetime, Item, RunOptions.RSE)
        else:
            Cmd=" rucio add-rule --account %s %s 1 %s"%(RunOptions.Rucio, Item, RunOptions.RSE)
        
        os.system(Cmd)
    
