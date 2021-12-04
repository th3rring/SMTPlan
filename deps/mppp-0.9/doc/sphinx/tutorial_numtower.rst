.. _tutorial_numtower:

The numerical tower and type coercion
=====================================

Before proceeding to show how mp++'s classes can be used to perform arithmetic computations,
it is useful to introduce first the hierarchy on top of which mp++'s type coercion
is built.

In a broad sense, mp++ aims to extend C++'s type hierarchy with multiprecision
numerical types. In C++, when numerical operands of different types are involved
in an arithmetic operation, all operands are converted (or *coerced*) into a
common type determined from the types of the operands. The converted operands
are then used in place of the original operands to perform the operation,
and the type of the result will be the common type. For instance:

.. code-block:: c++

   auto a = 4 + 5l;   // '4' is int, '5l' is long: a will be long.
   auto b = 4.f + 5l; // '4.f' is float, '5l' is long: b will be float.
   auto c = 4.f + 5.; // '4.f' is float, '5.' is double: c will be double.

In order to determine the common type, C++ assigns a rank to each fundamental type.
In an operation involving operands of different types, the type of the result will be
the type with the highest rank among the types of the operands. Although there are
a few complications and caveats, the general rule in C++ is that integral types have
a lower rank than floating-point types, and that, within the integral and floating-point
types, a higher range or bit width translates to a higher rank. The underlying
idea is that automatic type coercion should not change the value of an operand [#f1]_.

mp++ extends C++'s type hierarchy in a (hopefully) natural way:

* :cpp:class:`~mppp::integer` has a rank higher than any C++ integral type, but lower
  than any C++ floating-point type;
* :cpp:class:`~mppp::rational` has a rank higher than :cpp:class:`~mppp::integer`, but lower
  than any C++ floating-point type;
* :cpp:class:`~mppp::real128` has a rank higher than any C++ floating-point type;
* :cpp:class:`~mppp::real` has a rank higher than :cpp:class:`~mppp::real128`.

In other words, mp++'s type hierarchy (or *numerical tower*) from the lowest rank to the highest is the following:

* C++ integral types,
* :cpp:class:`~mppp::integer`,
* :cpp:class:`~mppp::rational`,
* C++ floating-point types,
* :cpp:class:`~mppp::real128`,
* :cpp:class:`~mppp::real`.

Note that up to and including :cpp:class:`~mppp::rational`, types with higher rank can represent exactly all values
of any type with a lower rank. The floating-point types, however, cannot represent exactly all values representable
by :cpp:class:`~mppp::rational`, :cpp:class:`~mppp::integer` or even the C++ integral types. It should also be noted
that :cpp:class:`~mppp::real`'s precision is set at runtime, and it is thus possible to create :cpp:class:`~mppp::real`
objects with a precision lower than :cpp:class:`~mppp::real128` or any of the C++ floating-point types. Regardless,
when it comes to type coercion, :cpp:class:`~mppp::real` is always assigned a rank higher than any other type.

mp++'s type coercion rules extend beyond arithmetic operators. The exponentiation functions ``pow()``, for instance,
also use the type hierarchy to determine the type of the result. Type coercion is also applied in the comparison operators,
where arguments of different types are promoted to the common type before actually carrying out the comparison.

.. rubric:: Footnotes

.. [#f1] Strictly speaking, this is of course not true. On modern architectures, a large enough
   64-bit ``long long`` will be subject to a lossy conversion to, e.g., ``float``
   during type coercion.
