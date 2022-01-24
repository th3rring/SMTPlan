#!/bin/python3
# Author: Thomas Herring
import sys
from datetime import datetime
from planner_process import PlannerProcess
from database_manager import DatabaseManager
from constants import *
import glob
import yaml

print("--- SMTPlan Experiment Benchmarker ---")
print("Available experiments: ")

# Let user select experiment
dirs = glob.glob(experiment_dir + "*/")
for i in range(len(dirs)):
    print("{}: {}".format(i, dirs[i]))

selected = int(input("Which experiment would you like to run? "))

if selected < 0 or selected >= len(dirs):
    print("Invalid selection {}".format(selected))
    sys.exit(1)
else: 
    print("\nLoading {}".format(dirs[selected]))

expt_dir = dirs[selected]

# Load in yaml config for experiment
config = None
with open(expt_dir + config_filename, 'r') as file:
    config = yaml.safe_load(file)

# Check that all expected values are in the config file
expected_vals = ["name", "domain", "upper_bound", "database", "num_runs_per_expt", "verbose"]
for ev in expected_vals:
    if ev not in config.keys():
        print("Missing config \"{}\" from config.yaml file...".format(ev))
        sys.exit(1)

print("Loaded config...")   

# Check verbose
verbose = bool(config["verbose"])

# Connect to databasedatabase_dir + config["database"] + ".db"
db_name = database_dir + config["database"] + ".db"
db = DatabaseManager(db_name)
print("Connected to database \"{}\"".format(db_name))

# Remove problem pddl file from glob
domain = expt_dir + config["domain"] + ".pddl"
problems = glob.glob(expt_dir + "*.pddl")
problems.remove(domain)

# Get current date and time
date = datetime.now().strftime("%Y_%m_%d")
time = datetime.now().strftime("%H:%M:%S")

# Create process manager
proc = PlannerProcess(date, time)
results = []

# Run actual experiment
print("Running expt...")
for p in problems:
    if verbose:
        print("Running problem {}".format(p))
    for i in range(int(config["num_runs_per_expt"])):
        if verbose:
            print("\tRun {}...".format(i))
        results.append(proc.run_expt(domain, p, int(config["upper_bound"]), bool(config["verbose"])))

# Insert into the database
db.insert_expts(results)

# Save and close database
db.save_and_close()

# Exit!
sys.exit(0)
