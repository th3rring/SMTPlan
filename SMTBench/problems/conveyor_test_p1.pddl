(define 
  (problem conveyor-test-p1)
  (:domain conveyor-transfer-domain)
  (:objects
    a - block
    temp goal - location)
  (:init 
    (conveyor_pickable a)
    (at 10 (conveyor_unavailable a))
    (clear temp)
    (clear goal)
    (handempty))
  (:goal (and
    (at a goal)
    (off_conveyor a)
  ))



)
