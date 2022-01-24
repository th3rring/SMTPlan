(define 
  (problem conveyor-high-stack-4)
  (:domain conveyor-transfer-domain)
  (:objects
    a b c d - block
    temp goal - location)
  (:init 
    (conveyor_pickable a)
    (at 2 (conveyor_unavailable a))
    (at 10 (conveyor_pickable b))
    (at 12 (conveyor_unavailable b))
    (at 20 (conveyor_pickable c))
    (at 22 (conveyor_unavailable c))
    (at 30 (conveyor_pickable d))
    (at 32 (conveyor_unavailable d))
    (clear temp)
    (clear goal)
    (handempty))
  (:goal (and
    (at b goal)
    (at a b)
    (at d a)
    (at c d)
    (off_conveyor a)
    (off_conveyor b)
    (off_conveyor c)
    (off_conveyor d)
  ))

)
