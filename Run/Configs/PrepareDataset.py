import os
import commands
import fnmatch

def group(iterator, count):
    """
    This function extracts items from a sequence or iterator 'count' at a time:
    >>> list(group([0, 1, 2, 3, 4, 5, 6], 2))
    [(0, 1), (2, 3), (4, 5)]
    Stolen from :
    http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/439095
    """
    itr = iter(iterator)
    while True:
        yield tuple([itr.next() for i in xrange(count)])


def has_wildcard(name) :
    """Return true if the name has a UNIX wildcard (*,?,[,])
    """
    if ( name.count('*') > 0 or name.count('?') > 0 or
         name.count('[') > 0 or name.count(']') > 0 ) :
        return True
    else :
        return False

def nsls(path) :
    """
    lists CASTOR name server directory/file entries.
    If path is a directory, nsls lists the entries in the directory;
    they are sorted alphabetically.

    path specifies the CASTOR pathname. If path does not start  with  /,
    it  is  prefixed  by  the content of the CASTOR_HOME environment
    variable.

    ex:
    >>> nsls( '/castor/cern.ch/atlas/*' )
    >>> nsls( 'mydata' )
    """

    wildcards = False
    tail = "*"
    path = os.path.expandvars(path)

    if path.endswith('/') :
        path = path[0:len(path)-1]
    # Do we detect a wildcard in the path we are given ?
    # if so then we have to parse it to remove them because
    # nsls does not understand them.
    # The ouput of the command will be filtered afterwards
    if has_wildcard(path) :
        wildcards = True

        wholepath = path.split(os.sep)

        # Here we assume the wildcards are located *only* in the filename !!
        tail      = wholepath[len(wholepath)-1]
        if tail == '' :
            if len(wholepath) >= 2 :
                tail = wholepath[len(wholepath)-2]
            else :
                raise Exception, \
                      "Malformed path to files: <"+path+">"

        # Check that the wildcard is not in the path to files
        if tail.count('/') > 0 :
            if tail.endswith('/') :
                # the / is sitting in last position. Can safely remove it
                tail = tail[0:len(tail)-1]
            else :
                raise Exception, \
                      "No wildcard allowed in the path to files: <"+path+">"


        path      = path.split(tail)[0]
        if has_wildcard(path) :
            raise ValueError("No wildcard allowed in the path to files: <"+path+">")
        #print path

    status,output = commands.getstatusoutput('nsls '+path)

    if status != 0 :
        print output
        return []

    output = output.splitlines()

    if wildcards :
        output = fnmatch.filter(output,tail)

    for i in xrange(0,len(output)) :
        if output[i].count(path) < 1:
            output[i] = path+"/"+output[i]
        output[i] = output[i].replace('//','/')
    return output

def stagein( fileListPattern = None, nSlices = 10, verbose = True ) :
    """
    Take a path to a file pattern and stages all the files corresponding
    to this pattern by bunchs of N (default=10) files.
    """
    
    files = nsls( fileListPattern )
    if ( type(files) != type([]) or len(files) < 1 ) :
        raise Exception, \
              "Error, no file to stagein !!"
        return

    slices = list(group(files,nSlices))

    for slice in slices :
        stageList = ' -M '.join( [''] + [ s for s in slice ] )
        cmd = 'stager_get %s' % stageList
        if verbose :
            print ">>> cmd= ",cmd
        status,output = commands.getstatusoutput(cmd)

        if status != 0 :
            print "** PyCastor ERROR **"
            print output
            pass
        else :
            if verbose :
                print output
                pass
            pass
        pass

    return 0

def prepare_ds(dataset,nSlices = 10, verbose = True):
    for ps in dataset:
        if ps.hasParameter("IsCastor"):
            if ps.IsCastor.value():
                print ps.Name.value() + " staging is starting"
                if ps.IsDirectory.value():
                    stagein(ps.File.value() + '*.root' , nSlices , verbose)
                else:
                    stagein(ps.File.value(), nSlices , verbose)
            