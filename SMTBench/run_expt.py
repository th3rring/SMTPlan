#!/bin/python3
# Author: Thomas Herring

import subprocess 
import os.path
import sys
import re
import sqlite3
from datetime import datetime

# Path to process executable
proc_path = "../SMTPlan/build/SMTPlanExpt"

# Directories for domain, problem, and experiment files
domain_dir = "domains/"
problem_dir = "problems/"
experiments_dir = "experiments/"

# Print error and exit if executable isn't found
if not os.path.isfile(proc_path):
    sys.stderr.write("Executable %s was not found\n" % proc_path)
    sys.exit(1)

def data_generator(data):
    for d in data:
        yield d
    
# Set an upper bound on how many iterations we're allowing
upper_bound = 20

# Set the domain
# domain = "pick_place_domain.pddl"
# domain = "conveyor_domain.pddl"
domain = "conveyor_domain_transfer.pddl"

# Choose a SAT or UNSAT problem
# problem = "p_3_block.pddl"
# problem = "unsat_block.pddl"
# problem = "conveyor_test_p4.pddl"
problem = "conveyor_p2.pddl"

# Prepend directory on domain and problem
domain = domain_dir + domain
problem = problem_dir + problem

# Set the num of experiments
num_expts = 5

# Set verbose
verbose = True

# Get current date and time
date = datetime.now().strftime("%Y_%m_%d")
time = datetime.now().strftime("%H:%M:%S")

# Compile regexes
grounded_pattern = re.compile(r"(?<=Grounded:\s)\d*\.?\d*")
algebra_pattern = re.compile(r"(?<=Algebra:\s)\d*\.?\d*")
sat_pattern = re.compile(r"(?<!UN)SAT")
sol_time_pattern = re.compile(r"(?<=SAT Solution:\s)\d*\.?\d*")
timeout_pattern = re.compile(r"(?<=Timeout\sat\s)\d*")
total_time_pattern = re.compile(r"(?<=Total time:\s)\d*\.?\d*")
iterations_pattern = re.compile(r"(?<=Iterations:\s)\d*\.?\d*")

# Printout details for this expt
print("--- SMTPlan Benchmark ---")
print("Current time {}-{}".format(date, time))
print("Domain: {}".format(domain))
print("Problem: {}".format(problem))
print("Num expts: {}".format(num_expts))

# Create expt database, currently using unique datetime
con = sqlite3.connect(experiments_dir + "expt_{}.db".format(datetime.now().strftime("%Y_%m_%d-%H:%M:%S")))
cur = con.cursor()

# Create a new table if one doesn't exist
cur.execute("""CREATE TABLE IF NOT EXISTS experiments 
(domain TEXT, problem TEXT, date TEXT, time TEXT, 
sat INTEGER, grounded_time REAL, algebra_time REAL, sol_time REAL, iterations INTEGER, timeout INTEGER, total_time REAL, log TEXT)""")

expt_results = []

for i in range(num_expts):

    # Run process and capture output
    proc = subprocess.Popen([proc_path, domain, problem, "-u", str(upper_bound)], stdout=subprocess.PIPE, universal_newlines=True)
    outs = proc.stdout.read()

    # Capture outputs for all results
    grounded = float(grounded_pattern.search(outs)[0])
    algebra = float(algebra_pattern.search(outs)[0])
    sat = sat_pattern.search(outs) is not None

    if verbose:
        print("\t***Captured Values***")
        print("Grounded time: {}".format(grounded))
        print("Algebra time: {}".format(algebra))
        print("Sat: {}".format(sat))

    # Set default values
    sol_time = -1
    iterations = -1
    timeout = -1

    # Set timeout or sol time depending on expt result
    if (sat):
        iterations = int(iterations_pattern.search(outs)[0])
        sol_time = float(sol_time_pattern.search(outs)[0])
        if verbose:
            print("Solution time: {}".format(sol_time))
    else:
        timeout = int(timeout_pattern.search(outs)[0])
        if verbose:
            print("Max number of iterations: {}".format(timeout))

    total_time = float(total_time_pattern.search(outs)[0])

    if verbose:
        print("Total time: {}".format(total_time))

    expt_results.append((domain, problem, date, time, sat, grounded, algebra, sol_time, iterations, timeout, total_time, outs))

cur.executemany("insert into experiments values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", data_generator(expt_results))

print("Experiment date: {}".format(date))
print("Experiment time: {}".format(time))

print(expt_results[-1][-1])

# Save
con.commit()

# Exit!
con.close()
sys.exit(0)

