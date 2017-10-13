// SWIG file MonteCarloExperiment.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Sample OT::MonteCarloExperiment::generateWithWeights(OT::Point & weightsOut) const;

%{
#include "openturns/MonteCarloExperiment.hxx"
%}

%include MonteCarloExperiment_doc.i

%include openturns/MonteCarloExperiment.hxx
namespace OT { %extend MonteCarloExperiment { MonteCarloExperiment(const MonteCarloExperiment & other) { return new OT::MonteCarloExperiment(other); } } }

%clear OT::Point & weightsOut;
