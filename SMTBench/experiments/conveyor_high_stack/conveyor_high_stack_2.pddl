(define 
  (problem conveyor-high-stack-2)
  (:domain conveyor-transfer-domain)
  (:objects
    a b - block
    temp goal - location)
  (:init 
    (conveyor_pickable a)
    (at 2 (conveyor_unavailable a))
    (at 10 (conveyor_pickable b))
    (at 12 (conveyor_unavailable b))
    (clear temp)
    (clear goal)
    (handempty))
  (:goal (and
    (at b goal)
    (at a b)
    (off_conveyor a)
    (off_conveyor b)
  ))

)
