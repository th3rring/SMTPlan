import subprocess 
import os.path
import sys
import re
from constants import * # Import all constants

class PlannerProcess:
    def __init__(self, date, time) -> None:
        if not os.path.isfile(proc_path):
            sys.stderr.write("Executable %s was not found\n" % proc_path)
            sys.exit(1)

        self.date = date
        self.time = time

        # Define regexes for capturing process output.
        self.grounded_pattern = re.compile(r"(?<=Grounded:\s)\d*\.?\d*")
        self.algebra_pattern = re.compile(r"(?<=Algebra:\s)\d*\.?\d*")
        self.sat_pattern = re.compile(r"(?<!UN)SAT")
        self.sol_time_pattern = re.compile(r"(?<=SAT Solution:\s)\d*\.?\d*")
        self.timeout_pattern = re.compile(r"(?<=Timeout\sat\s)\d*")
        self.total_time_pattern = re.compile(r"(?<=Total time:\s)\d*\.?\d*")
        self.iterations_pattern = re.compile(r"(?<=Iterations:\s)\d*\.?\d*")
     

    def run_expt(self, domain, problem, verbose):

        proc = subprocess.Popen([proc_path, domain, problem, "-u", str(upper_bound)], stdout=subprocess.PIPE, universal_newlines=True)
        outs = proc.stdout.read()

        # Capture outputs for all results
        grounded = float(self.grounded_pattern.search(outs)[0])
        algebra = float(self.algebra_pattern.search(outs)[0])
        sat = self.sat_pattern.search(outs) is not None

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
            iterations = int(self.iterations_pattern.search(outs)[0])
            sol_time = float(self.sol_time_pattern.search(outs)[0])
            if verbose:
                print("Solution time: {}".format(sol_time))
        else:
            timeout = int(self.timeout_pattern.search(outs)[0])
            if verbose:
                print("Max number of iterations: {}".format(timeout))

        total_time = float(self.total_time_pattern.search(outs)[0])

        if verbose:
            print("Total time: {}".format(total_time))

        return (domain, problem, self.date, self.time, sat, grounded, algebra, sol_time, iterations, timeout, total_time, outs)


