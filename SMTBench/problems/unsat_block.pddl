(define (problem restack)
    (:domain block-stack)
    (:objects
        block_a - block
        block_b - block
        loc_one - location
    )
    (:init 
        (at block_a loc_one)
        (on block_b block_a)
        (clear block_b)
        (handempty)
    )
    (:goal 
        (and 
            (at block_b loc_one)
            (on block_a block_b)
            (handempty)
        )
    )
)
