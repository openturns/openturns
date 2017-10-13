// SWIG file ChebychevFactory.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weights ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weights %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Point OT::ChebychevFactory::getNodesAndWeights(OT::Point & weights) const;

%{
#include "openturns/ChebychevFactory.hxx"
%}

%include ChebychevFactory_doc.i

%include openturns/ChebychevFactory.hxx
namespace OT { %extend ChebychevFactory { ChebychevFactory(const ChebychevFactory & other) { return new OT::ChebychevFactory(other); } } }

%clear OT::Point & weights;
