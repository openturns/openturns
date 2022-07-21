// SWIG file VonMisesFactory.i

%{
#include "openturns/VonMisesFactory.hxx"
%}

%include VonMisesFactory_doc.i

%include openturns/VonMisesFactory.hxx
namespace OT { %extend VonMisesFactory { VonMisesFactory(const VonMisesFactory & other) { return new OT::VonMisesFactory(other); } } }
