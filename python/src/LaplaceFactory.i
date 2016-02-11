// SWIG file LaplaceFactory.i

%{
#include "openturns/LaplaceFactory.hxx"
%}

%include LaplaceFactory_doc.i

%include openturns/LaplaceFactory.hxx
namespace OT { %extend LaplaceFactory { LaplaceFactory(const LaplaceFactory & other) { return new OT::LaplaceFactory(other); } } }
