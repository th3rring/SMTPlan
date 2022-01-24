# Conveyor High Stack Experiment
###Author: Thomas Herring
## Description
The problems in this experiment are designed to strain the planner with increasingly high towers to stack. The robot will be able to grab each block as it travels down a conveyor belt and has two locations they can be placed at: goal and temp. For each pair of blocks, we stack the second and then the first. This reversing of the ordering is intended to force the planner to make use of the temporary location.

## Domain
The domain for these problems models the pick and place domain with transfer actions with a duration of 2.

## Problems
In each problem, each additional blocks arrives 10 seconds after the previous one. 
