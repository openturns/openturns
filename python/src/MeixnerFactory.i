// SWIG file MeixnerFactory.i

%{
#include "openturns/MeixnerFactory.hxx"
%}

%include MeixnerFactory_doc.i

%include openturns/MeixnerFactory.hxx
namespace OT { %extend MeixnerFactory { MeixnerFactory(const MeixnerFactory & other) { return new OT::MeixnerFactory(other); } } }
