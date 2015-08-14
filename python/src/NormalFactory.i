// SWIG file NormalFactory.i

%{
#include "NormalFactory.hxx"
%}

%include NormalFactory_doc.i

%include NormalFactory.hxx
namespace OT { %extend NormalFactory { NormalFactory(const NormalFactory & other) { return new OT::NormalFactory(other); } } }
