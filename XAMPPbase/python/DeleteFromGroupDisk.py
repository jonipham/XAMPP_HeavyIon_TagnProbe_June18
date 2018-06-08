#! /usr/bin/env python
from XAMPPbase.ListGroupDisk import *

def ReadListFromFile (File):
    List = []
    if os.path.isfile(File) == True:
        for Line in open(File):
            List.append(Line.strip().replace("\n", ""))
    return List
 
if __name__ == '__main__':
    
    parser = argparse.ArgumentParser(description='This script lists datasets located at a RSE location. Futher patterns to find or exclude can be specified.', prog='ListGroupDisk', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-r', '-R', '--RSE', help='specify RSE storage element (default: %s)' % RSE, default=RSE)
    parser.add_argument('-d', '-l', '-D', '-L', '--list', help='specify a list containing the datasets to be deleted')
    parser.add_argument("--AllRequests" , help="Allows to delete rules from disk not requested by oneself", default=False, action="store_true")
    parser.add_argument("--Rucio" , help="With this option you can set the rucio_account. Default %s" % RUCIO_ACC, default=RUCIO_ACC)
    
    RunOptions = parser.parse_args()
    List = ReadListFromFile(RunOptions.list)
    MyRequests = []
    if not RunOptions.AllRequests:
        MyRequests = ListUserRequests(RunOptions.RSE)
    else:
        MyRequests = ListDisk(RunOptions.RSE)
    Delete = []
    for Item in List:
        if len (Item.strip()) < 2:
            continue
        for DS in MyRequests:
            if Item in DS:
                print "Found rucio rule " + DS + " matching " + Item
                Delete.append(DS)
    
    for Item in Delete:
        print "Delete rule for " + Item
        os.system("rucio delete-rule %s --rse_expression %s --account %s" % (Item, RunOptions.RSE, RunOptions.Rucio))
    
