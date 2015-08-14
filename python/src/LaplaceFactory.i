// SWIG file LaplaceFactory.i

%{
#include "LaplaceFactory.hxx"
%}

%include LaplaceFactory_doc.i

%include LaplaceFactory.hxx
namespace OT { %extend LaplaceFactory { LaplaceFactory(const LaplaceFactory & other) { return new OT::LaplaceFactory(other); } } }
