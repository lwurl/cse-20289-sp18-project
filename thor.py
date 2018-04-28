#!/usr/bin/env python3

import multiprocessing
import os
import requests
import sys
import time

# Globals

PROCESSES = 1
REQUESTS  = 1
VERBOSE   = False
URL       = None

# Functions

def usage(status=0):
    print('''Usage: {} [-p PROCESSES -r REQUESTS -v] URL
    -h              Display help message
    -v              Display verbose output

    -p  PROCESSES   Number of processes to utilize (1)
    -r  REQUESTS    Number of requests per process (1)
    '''.format(os.path.basename(sys.argv[0])))
    sys.exit(status)

def do_request(pid):
    ''' Perform REQUESTS HTTP requests and return the average elapsed time. '''
    totaltime = 0
    # Perform number of requests given
    for i in range(REQUESTS):
        begin = time.time()
        r = requests.get(URL)
        partial_time = time.time() - begin
        totaltime += partial_time
        if VERBOSE:
            print(r.text)
        print("Process: " + str(os.getpid()) + ", Request: " + str(i) + ", Elapsed Time: " + "{0:.2f}".format(partial_time))
    
    print("Process: " + str(os.getpid()) + ", AVERAGE   " + ", Elapsed Time: " + "{0:.2f}".format(totaltime/REQUESTS))
    

    return totaltime/REQUESTS

# Main execution

if __name__ == '__main__':
    # Parse command line arguments
    args = sys.argv[1:]
    if len(args) == 0:
        usage(1)
    while len(args) and len(args[0]) > 1:
        arg = args.pop(0)
        if arg == '-h':
            usage(0)
        elif arg == '-v':
            VERBOSE = True
        elif arg == '-p':
            PROCESSES = int(args.pop(0))
        elif arg == '-r':
            REQUESTS = int(args.pop(0))
        elif arg.startswith('http'):
            URL = arg
        else:
            usage(1)


    # Create pool of workers and perform requests
    pool = multiprocessing.Pool(PROCESSES)

    # Call do_request for all of the workers
    # Returns a list with the ave times for the requests
    l = pool.map(do_request, range(PROCESSES))
    
    # Calculate the total time for all of the processes
    total_time = 0
    for i in l:
        total_time += i
    ave = total_time/PROCESSES
    print("TOTAL AVERAGE ELAPSED TIME: " + "{0:.2f}".format(ave))

    sys.exit(0)

# vim: set sts=4 sw=4 ts=8 expandtab ft=python:
