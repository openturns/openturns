%define OT_LinearProfile_doc
R"RAW(Linear temperature profile.

Parameters
----------
T0 : float
    Initial temperature
iMax : int
    Number of maximal iterations

Notes
-----
The temperature write:

.. math::
    :nowrap:

    \begin{equation*}
        T(i) = T_0*(1 - \frac{i}{iMax}), i \in [0,iMax]
    \end{equation*}

Examples
--------
>>> import openturns as ot
>>> # Define a profile
>>> T0 = 10.0
>>> iMax = 100
>>> linearProfile = ot.LinearProfile(T0, iMax)
)RAW"
%enddef
%feature("docstring") OT::LinearProfile
OT_LinearProfile_doc
