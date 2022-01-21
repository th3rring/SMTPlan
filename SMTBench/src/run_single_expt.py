#!/bin/python3
# Author: Thomas Herring
import sqlite3
import sys
from datetime import datetime
from planner_process import PlannerProcess
from constants import *

def data_generator(data):
    for d in data:
        yield d
    
# Set an upper bound on how many iterations we're allowing
upper_bound = 20

# Set the domain
domain = "conveyor_domain_transfer.pddl"

# Choose a SAT or UNSAT problem
# problem = "conveyor_test_p4.pddl"
problem = "conveyor_p2.pddl"

# Set the num of experiments
num_expts = 5

# Set verbose
verbose = True

# Get current date and time
date = datetime.now().strftime("%Y_%m_%d")
time = datetime.now().strftime("%H:%M:%S")

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

# Prepend directory on domain and problem
domain = domain_dir + domain
problem = problem_dir + problem

# Create planner process
proc = PlannerProcess(date, time)
expt_results = []

for i in range(num_expts):
    results = proc.run_expt(domain, problem, upper_bound, verbose)
    expt_results.append(results)

cur.executemany("insert into experiments values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", data_generator(expt_results))

print("Experiment date: {}".format(date))
print("Experiment time: {}".format(time))

print(expt_results[-1][-1])

# Save
con.commit()

# Exit!
con.close()
sys.exit(0)

