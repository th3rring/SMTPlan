(define 
  (domain conveyor-transfer-domain)
  (:requirements :strips :typing :durative-actions :equality :timed-initial-literals)
  (:types
    block location - object
  )
  (:predicates
    (at ?b - block ?o - object)
    (clear ?o - object)
    (in_hand ?b - block)
    (handempty)
    (conveyor_pickable ?b - block)
    (off_conveyor ?b - block)
  )
  (:durative-action transfer-from-conveyor
    :parameters (?b - block ?o - object)
    :duration (= ?duration 2)
    :condition (and
      (at start (clear ?o))
      (at start (conveyor_pickable ?b))
      (at start (handempty))
    )
    :effect (and
      (at start (not (clear ?o)))
      (at start (in_hand ?b))
      (at start (not (handempty)))
      (at start (off_conveyor ?b))
      (at end (not (in_hand ?b)))
      (at end (handempty))
      (at end (at ?b ?o))
      (at end (clear ?b))
    )
  )

  (:durative-action transfer
    :parameters (?b - block ?f - object ?t - object)
    :duration (= ?duration 2)
    :condition (and
      (at start (clear ?t))
      (at start (clear ?b))
      (at start (off_conveyor ?b))
      (at start (at ?b ?f))
      (at start (handempty))
    )
    :effect (and
      (at start (not (at ?b ?f)))
      (at start (clear ?f))
      (at start (not (clear ?t)))
      (at start (in_hand ?b))
      (at start (not (handempty)))
      (at end (at ?b ?t))
      (at end (not (in_hand ?b)))
      (at end (handempty))
    )
  )
)
