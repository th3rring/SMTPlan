(define 
  (problem conveyor-high-stack-4)
  (:domain conveyor-transfer-domain)
  (:objects
    a b c d - block
    temp goal - location)
  (:init 
    (conveyor_pickable a)
    (at 2 (not (conveyor_pickable a)))
    (at 10 (conveyor_pickable b))
    (at 12 (not (conveyor_pickable b)))
    (at 20 (conveyor_pickable c))
    (at 22 (not (conveyor_pickable c)))
    (at 30 (conveyor_pickable d))
    (at 32 (not (conveyor_pickable d)))
    (clear temp)
    (clear goal)
    (handempty))
  (:goal (and
    (at d goal)
    (at c d)
    (at b c)
    (at a b)
    (off_conveyor a)
    (off_conveyor b)
    (off_conveyor c)
    (off_conveyor d)
  ))

)
