# Conveyor Restack Experiment
###Author: Thomas Herring
## Description
The problems in this experiment have 3 blocks - A, B, and C - traveling down a conveyor belt and two locations that they can be placed on: goal and temp. The robot is tasked with assembling a tower of blocks with the following order: B, A, C. As the blocks arrive in the same order as listed above, the robot must do the following to achieve the goal:
1. Grab and place A at the temp location
2. Grab and place B at the goal location
3. Before C arrives, pick and place A on top of B
4. Gran and place C on top of B

If the robot does not stack A on B before C arrives, the goal will not be achievable. We label the problems in this experiment with the duration the planner has to perform step 3 or the difference between steps 2 and 4.


## Domain
The domain for these problems models the pick and place domain with transfer actions with a duration of 2.
