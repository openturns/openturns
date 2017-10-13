// SWIG file

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Sample OT::MonteCarloLHS::generateWithWeights(OT::Point & weightsOut) const;

%{
#include "openturns/MonteCarloLHS.hxx"
%}

%include MonteCarloLHS_doc.i

%include openturns/MonteCarloLHS.hxx
namespace OT { %extend MonteCarloLHS { MonteCarloLHS(const MonteCarloLHS & other) { return new OT::MonteCarloLHS(other); } } }

%clear OT::Point & weightsOut;
