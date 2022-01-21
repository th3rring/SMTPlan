(define (problem restack)
    (:domain block-stack)
    (:objects
        block_a - block
        block_b - block
        block_c - block
        loc_one - location
        loc_two - location
        loc_three - location
    )
    (:init 
        (at block_a loc_one)
        (at block_c loc_two)
        (on block_b block_a)
        (clear block_b)
        (clear block_c)
        (handempty)
        (clear_loc loc_three)
    )
    (:goal 
        (and 
            (at block_b loc_two)
            (at block_a loc_three)
            (on block_c block_a)
            (handempty)
        )
    )
)
