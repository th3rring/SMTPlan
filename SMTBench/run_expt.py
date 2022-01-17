#!/bin/python3

import subprocess 
import shlex
import os.path
import sys

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

proc = subprocess.Popen([proc_path, domain, problem], stdout=subprocess.PIPE, universal_newlines=True)

outs = proc.stdout.read()


# Prints the standard output from the process!
# print(shlex.split(outs.decode("utf-8")))
print(outs)
# print(shlex.split(outs))

sys.exit(0)

