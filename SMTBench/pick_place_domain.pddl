(define (domain block-stack)
  (:requirements :strips :typing :negative-preconditions)
  (:types block location)
  (:predicates (at ?x - block ?y - location)
               (clear ?x - block)
               (clear_loc ?x - location)
               (in_hand ?x - block)
               (handempty)
               (on ?x - block ?y - block))

  (:action pick-up
           :parameters (?x - block ?y - location)
           :precondition (and (clear ?x)
                              (at ?x ?y)
                              (handempty))
           :effect (and (not (clear ?x))
                        (not (at ?x ?y))
                        (in_hand ?x)
                        (not (handempty))
                        (clear_loc ?y)))
  (:action put-down
           :parameters (?x - block ?y - location)
           :precondition (and (clear_loc ?y)
                              (in_hand ?x))
           :effect (and (clear ?x)
                        (at ?x ?y)
                        (not (in_hand ?x))
                        (handempty)
                        (not (clear_loc ?y))))
  (:action stack
           :parameters (?x - block ?y - block)
           :precondition (and (clear ?y)
                              (in_hand ?x)
                              (not (handempty)))
           :effect (and (not (clear ?y))
                        (not (in_hand ?x))
                        (clear ?x)
                        (handempty)
                        (on ?x ?y)))
  (:action unstack
           :parameters (?x - block ?y - block)
           :precondition (and (on ?x ?y)
                              (clear ?x)
                              (handempty))
           :effect (and (clear ?y)
                        (in_hand ?x)
                        (not (clear ?x))
                        (not (handempty))
                        (not (on ?x ?y)))))
