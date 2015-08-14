// SWIG file InverseNormalFactory.i

%{
#include "InverseNormalFactory.hxx"
%}

%include InverseNormalFactory_doc.i

%include InverseNormalFactory.hxx
namespace OT { %extend InverseNormalFactory { InverseNormalFactory(const InverseNormalFactory & other) { return new OT::InverseNormalFactory(other); } } }
