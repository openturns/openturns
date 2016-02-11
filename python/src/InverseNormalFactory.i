// SWIG file InverseNormalFactory.i

%{
#include "openturns/InverseNormalFactory.hxx"
%}

%include InverseNormalFactory_doc.i

%include openturns/InverseNormalFactory.hxx
namespace OT { %extend InverseNormalFactory { InverseNormalFactory(const InverseNormalFactory & other) { return new OT::InverseNormalFactory(other); } } }
