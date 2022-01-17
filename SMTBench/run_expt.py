#!/bin/python3

import subprocess 
import shlex
import os.path
import sys
import re

proc_path = "../SMTPlan/build/SMTPlanExpt"

# Print error and exit if executable isn't found
if not os.path.isfile(proc_path):
    sys.stderr.write("Executable %s was not found\n" % proc_path)
    sys.exit(1)

print("SMTPlan benchmark")
    

# domain = "../benchmarks/generator_nonlinear/gen_nonlinear_domain.pddl"
# problem = "../benchmarks/generator_nonlinear/gen_nonlinear_prob07.pddl"
domain = "pick_place_domain.pddl"
problem = "p_3_block.pddl"

proc = subprocess.Popen([proc_path, domain, problem, "-v"], stdout=subprocess.PIPE, universal_newlines=True)

outs = proc.stdout.read()

print(outs)

grounded = re.search(r"(?<=Grounded:\s)\d*\.?\d*", outs)
algebra = re.search(r"(?<=Algebra:\s)\d*\.?\d*", outs)
sat_or_not = re.search(r"(?<!UN)SAT", outs)
sol_time = re.search(r"(?<=SAT Solution:\s)\d*\.?\d*", outs)
total_time = re.search(r"(?<=Total time:\s)\d*\.?\d*", outs)

print(m[0])



# Prints the standard output from the process!
# print(shlex.split(outs.decode("utf-8")))
# print(shlex.split(outs))

sys.exit(0)

