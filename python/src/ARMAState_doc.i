%feature("docstring") OT::ARMAState
R"RAW(Last state recorded of an ARMA process.

Parameters
----------
values : :class:`~openturns.Sample`
    Last :math:`p` values of the :math:`ARMA(p,q)` process of dimension :math:`\processDim`.
noises : :class:`~openturns.Sample`
    Last :math:`q` values of the white noise defining the :math:`ARMA(p,q)` process of dimension :math:`\processDim`.

Examples
--------
Define a state of an :math:`ARMA(4,2)` process of dimension 1:

>>> import openturns as ot
>>> myLastValues = ot.Sample([[0.6], [0.7], [0.3], [0.2]])
>>> myLastNoiseValues = ot.Sample([[1.2], [1.8]])
>>> myARMAState = ot.ARMAState(myLastValues, myLastNoiseValues)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ARMAState::getDimension
R"RAW(Accessor to the dimension of coefficients in the list.

Returns
-------
dimension : int
    The dimension :math:`\processDim` of the ARMA process.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ARMAState::getX
R"RAW(Accessor to the last values of the ARMA process.

Returns
-------
values : :class:`~openturns.Sample` of size :math:`p` and dimension :math:`\processDim`
    Last :math:`p` values of the :math:`ARMA(p,q)` process of dimension :math:`\processDim`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::ARMAState::getEpsilon
R"RAW(Accessor to the last values of the noise defining the ARMA process.

Returns
-------
noises : :class:`~openturns.Sample` of size :math:`q` and dimension :math:`\processDim`
    Last :math:`q` values of the white noise of the :math:`ARMA(p,q)` process of dimension :math:`\processDim`.
)RAW"
// ---------------------------------------------------------------------

%feature("docstring") OT::ARMAState::setXEpsilon
R"RAW(Accessor to the last values of the noise defining the ARMA process.

Parameters
----------
values : :class:`~openturns.Sample` of size :math:`p` and dimension :math:`\processDim`
    Last :math:`p` values of the :math:`ARMA(p,q)` process of dimension :math:`\processDim`.
noises : :class:`~openturns.Sample` of size :math:`q` and dimension :math:`\processDim`
    Last :math:`q` values of the white noise of the :math:`ARMA(p,q)` process of dimension :math:`\processDim`.
)RAW"
