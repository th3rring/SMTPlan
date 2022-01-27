(define 
  (problem conveyor-high-stack-3)
  (:domain conveyor-transfer-domain)
  (:objects
    a b c - block
    temp goal - location)
  (:init 
    (conveyor_pickable a)
    (at 2 (not (conveyor_pickable a)))
    (at 10 (conveyor_pickable b))
    (at 12 (not (conveyor_pickable b)))
    (at 20 (conveyor_pickable c))
    (at 22 (not (conveyor_pickable c)))
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
