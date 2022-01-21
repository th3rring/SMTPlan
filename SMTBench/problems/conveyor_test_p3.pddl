(define 
  (problem conveyor-test-p1)
  (:domain conveyor-transfer-domain)
  (:objects
    a b c - block
    temp goal - location)
  (:init 
    (conveyor_pickable a)
    (at 2 (conveyor_unavailable a))
    (at 10 (conveyor_pickable b))
    (at 12 (conveyor_unavailable b))
    (at 20 (conveyor_pickable c))
    (at 22 (conveyor_unavailable c))
    (clear temp)
    (clear goal)
    (handempty))
  (:goal (and
    (at a goal)
    (at b a)
    (at c b)
    (off_conveyor a)
    (off_conveyor b)
    (off_conveyor c)
  ))
)
