// SWIG file ChebychevFactory.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::NumericalPoint & weights ($*ltype temp) %{ temp = OT::NumericalPoint(); $1 = &temp; %}
%typemap(argout) OT::NumericalPoint & weights %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::NumericalPoint(*$1), SWIG_TypeQuery("OT::NumericalPoint *"), SWIG_POINTER_OWN |  0 )); %}
OT::NumericalPoint OT::ChebychevFactory::getNodesAndWeights(OT::NumericalPoint & weights) const;

%{
#include "openturns/ChebychevFactory.hxx"
%}

%include ChebychevFactory_doc.i

%include openturns/ChebychevFactory.hxx
namespace OT { %extend ChebychevFactory { ChebychevFactory(const ChebychevFactory & other) { return new OT::ChebychevFactory(other); } } }
