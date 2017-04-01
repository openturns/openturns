// SWIG file GaussLegendre.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::NumericalSample & adaptedNodes ($*ltype temp) %{ temp = OT::NumericalSample(); $1 = &temp; %}
%typemap(argout) OT::NumericalSample & adaptedNodes %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::NumericalSample(*$1), SWIG_TypeQuery("OT::NumericalSample *"), SWIG_POINTER_OWN |  0 )); %}
OT::NumericalPoint OT::GaussLegendre::integrateWithNodes(const OT::NumericalMathFunction & function,
                           const OT::Interval & interval,
                           OT::NumericalSample & adaptedNodes) const;

%{
#include "openturns/GaussLegendre.hxx"
%}

%include GaussLegendre_doc.i

%include openturns/GaussLegendre.hxx
namespace OT { %extend GaussLegendre { GaussLegendre(const GaussLegendre & other) { return new OT::GaussLegendre(other); } } }
