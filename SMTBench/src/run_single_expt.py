#!/bin/python3
# Author: Thomas Herring
import sys
from datetime import datetime
from planner_process import PlannerProcess
from database_manager import DatabaseManager
from constants import *

# Set an upper bound on how many iterations we're allowing
upper_bound = 20

# Set the domain
domain = "conveyor_domain_transfer.pddl"

# Choose a SAT or UNSAT problem
# problem = "conveyor_test_p4.pddl"
problem = "conveyor_p2.pddl"

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
print("Single experiment run...")

# Create expt database, currently using unique datetime
db = DatabaseManager(experiments_dir + "expt_{}.db".format(datetime.now().strftime("%Y_%m_%d-%H:%M:%S")))

# Prepend directory on domain and problem
domain = domain_dir + domain
problem = problem_dir + problem

# Create planner process
proc = PlannerProcess(date, time)

# Run the experiment
results = proc.run_expt(domain, problem, upper_bound, verbose)

# Insert into the database
db.insert_expt(results)

print("Experiment date: {}".format(date))
print("Experiment time: {}".format(time))

# Printout the log from this experiment if we're in verbose
if verbose:
    print(results[-1])

# Save and close
db.save_and_close()
sys.exit(0)

