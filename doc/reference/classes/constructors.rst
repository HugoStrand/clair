.. _class_constructors:

Constructors
============

#. ``clair-c2py`` wraps all public constructors of a class.

#. For C++ aggregates without user-defined constructors
   (i.e., structs with public fields only), 
   ``clair-c2py`` synthesizes a constructor from a Python dict, 
   mimicking in Python the C++ aggregate initialization syntax.

#. An annotated free function can provide a constructor for its wrapped return class.
   Registered factories take precedence over synthesized aggregate constructors.

User-defined constructors
-------------------------

Let us first illustrate user-defined constructors with a simple example:   

.. literalinclude:: ../../examples/classes/constructor_basic.cpp
   :language: cpp
   :caption: constructor_basic.cpp
   :end-before: #include "constructor_basic.wrap.cxx"

In Python, we get:

.. testsetup::

   import sys, os
   sys.path.insert(0, os.path.abspath('examples/classes'))

.. doctest::

   >>> from constructor_basic import Point
   >>> p1 = Point()              # Default constructor
   >>> p2 = Point(3.0, 4.0)      # Constructor with arguments
   >>> p3 = Point(x=1.0, y=2.0)  # Keyword arguments
   >>> print(p1.x(), p1.y())
   0.0 0.0


.. _factory_constructors:

Factory-backed constructors
---------------------------

Use ``C2PY_ANNOTATE("c2py_wrap_as_constructor")`` to construct a wrapped class
through a free function. The function becomes a Python constructor overload for
its return class; it is not also exported as a module function.

.. code-block:: cpp

   #include <c2py/c2py.hpp>

   struct Value {
     int value;
     C2PY_IGNORE explicit Value(int x) : value(x) {}
   };

   C2PY_ANNOTATE("c2py_wrap_as_constructor")
   Value make_value(int value, int scale = 1) {
     return Value{value * scale};
   }

The ignored C++ constructor remains available to the factory. Python calls the
factory using the class name, with the factory's parameter names and defaults:

.. code-block:: python

   x = Value(value=3, scale=2)
   assert x.value == 6

The factory must return the exact class wrapped in this module, by value,
without ``const`` or ``volatile`` qualification. References, pointers, and
unwrapped return types are rejected. It must have a namespace-scope declaration;
member functions and operators cannot be used as factories. Its parameters must be convertible from
Python, as for ordinary wrapped functions.

Other public, nonignored C++ constructors remain available alongside the factory
overloads. No additional default constructor is synthesized when factories are
registered; an explicitly declared, nonignored default constructor is still
wrapped. For aggregates, factories replace the synthesized dictionary
constructor described below.

Factories follow the usual file, namespace, and name selection rules, including
``C2PY_IGNORE``. Function templates require :ref:`explicit instantiation
<function_templates>`. Overloaded factories, keyword arguments, and default
arguments use the usual constructor dispatch.

The binding uses ``c2py::c_maker`` to construct the factory result directly in
Python-owned storage. Returning a prvalue, as in ``return Value{...};`` above,
avoids copying or moving that result through C++ guaranteed copy elision.
Returning a named local instead relies on optional return-value optimization.


Synthesized aggregate constructor
---------------------------------

The second example illustrates the aggregate constructor:

.. literalinclude:: ../../examples/classes/constructor_aggregate.cpp
   :language: cpp
   :caption: constructor_aggregate.cpp
   :end-before: #include "constructor_aggregate.wrap.cxx"

In Python, we get:

.. doctest::

   >>> from constructor_aggregate import Params
   >>> p = Params(width=800, height=600, fullscreen=True, title="My App")

The synthesized constructor accepts only keyword arguments and performs the following checks:

- All fields without a default value must be initialized.
- No unknown fields are allowed.

If any of these checks fail, a ``RuntimeError`` is raised.

.. doctest::

   >>> p = Params(height=600, fullscreen=True)
   Traceback (most recent call last):
     ...
   RuntimeError: 
    Mandatory parameter 'width' is missing.

   >>> p = Params(width=800, height=600, fullscreen=True, title="My App", extra=42)
   Traceback (most recent call last):
     ...
   RuntimeError: 
    The parameter 'extra' was not used.
