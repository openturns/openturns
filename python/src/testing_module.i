%module(package="openturns", docstring="Test routines.") testing

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OT.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

/* Base/Type */
%import typ_module.i
%import BaseTypTemplateDefs.i

/* Base/Stat */
%import statistics_module.i
%import BaseStatisticsTemplateDefs.i

/* Uncertainty/Model */
%import model_copula_module.i

%{
#include "openturns/OTtestcode.hxx"
%}

%include OTtestcode_doc.i

%ignore OT::Test::assert_equal;
%ignore OT::Test::ExitCode;
%include openturns/OTtestcode.hxx


%pythoncode %{
import contextlib

@contextlib.contextmanager
def assert_raises(exception_class):
    """Checks for an exception to be raised.

    Similar to numpy.testing.assert_raises.

    Parameters
    ----------
    exception_class : Exception
        Exception type

    Examples
    --------
    >>> import openturns as ot
    >>> import openturns.testing as ott
    >>> with ott.assert_raises(ValueError):
    ...     raise ValueError
    """
    try:
        yield
        raise AssertionError(f"{exception_class.__name__} not raised")
    except exception_class:
        pass
%}
