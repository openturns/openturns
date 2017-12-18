// SWIG file BernsteinCopula.i

%{
#include "openturns/BernsteinCopula.hxx"
%}

%include BernsteinCopula_doc.i

%include openturns/BernsteinCopula.hxx
namespace OT { %extend BernsteinCopula { BernsteinCopula(const BernsteinCopula & other) { return new OT::BernsteinCopula(other); } } }
