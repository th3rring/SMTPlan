(define 
  (domain conveyor-manip-domain)
  (:requirements :strips :typing :durative-actions :negative-preconditions :timed-initial-literals)
  (:types
    block location - object
  )
  (:predicates
    (at ?b - block ?o - object)
    (clear ?o - object)
    (in_hand ?b - block)
    (handempty)
    (conveyor_pickable ?b - block)
    (conveyor_unavailable ?b - block)
    (off_conveyor ?b - block)
  )

  (:durative-action pick-from-conveyor
    :parameters (?b - block)
    :duration (= ?duration 2)
    :condition (and
      ;(at start (clear ?b))
      (at start (conveyor_pickable ?b))
      (at start (not (conveyor_unavailable ?b)))
      (at start (handempty))
      )
    :effect (and
      (at start (not (clear ?b)))
      (at start (not (handempty)))
      (at start (in_hand ?b))
      (at end (off_conveyor ?b))
      )
  )

  (:durative-action pick
    :parameters (?b - block ?o - object)
    :duration (= ?duration 2)
    :condition (and
      (at start (off_conveyor ?b))
      (at start (clear ?b))
      (at start (at ?b ?o))
      (at start (handempty))
      )
    :effect (and 
      (at start (not (clear ?b)))
      (at start (not (handempty)))
      (at start (not (at ?b ?o)))
      (at start (in_hand ?b))
      (at start (clear ?o))
      )
  )


  (:durative-action place
    :parameters (?b - block ?o - object)
    :duration (= ?duration 2)
    :condition (and
      (at start (in_hand ?b))
      (at start (clear ?o))
      (over all (in_hand ?b))
      ;(at end (at ?b ?o))
      )
    :effect (and
      (at start (not (clear ?o)))
      (at end (clear ?b))
      (at end (at ?b ?o))
      (at end (not (in_hand ?b)))
      (at end (handempty))
      )
  )

  

)
