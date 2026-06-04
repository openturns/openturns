%define OT_GeometricProfile_doc
R"RAW(Geometric temperature profile.

Parameters
----------
T0 : float
    Initial temperature.
c : float, optional, :math:`0 < c < 1`
    The geometric ratio.
iMax : int, optional
    Number of maximal iterations.

Notes
-----
The temperature is defined by:

.. math::

        T(i) = T_0 c^{i} \quad 0 \leq i \leq i_{Max}

Examples
--------
Define the profile: :math:`T(i) = 10 (0.9)^{i}` for :math:`0 \leq i \leq 100` :

>>> import openturns as ot
>>> T0 = 10.0
>>> iMax = 100
>>> c = 0.90
>>> geometricProfile = ot.GeometricProfile(T0, c, iMax)
)RAW"
%enddef
%feature("docstring") OT::GeometricProfile
OT_GeometricProfile_doc
