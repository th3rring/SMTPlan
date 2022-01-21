(define 
  (problem conveyor-p1)
  (:domain conveyor-transfer-domain)
  (:objects
    a b c - block
    temp goal - location)
  (:init 
    (conveyor_pickable a)
    (at 2 (conveyor_unavailable a))
    (at 10 (conveyor_pickable b))
    (at 12 (conveyor_unavailable b))
    (at 14.8 (conveyor_pickable c))
    (at 16.8 (conveyor_unavailable c))
    (clear temp)
    (clear goal)
    (handempty))
  (:goal (and
    (at b goal)
    (at a b)
    (at c a)
    (off_conveyor a)
    (off_conveyor b)
    (off_conveyor c)
  ))

)
