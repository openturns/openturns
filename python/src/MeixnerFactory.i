// SWIG file MeixnerFactory.i

%{
#include "MeixnerFactory.hxx"
%}

%include MeixnerFactory_doc.i

%include MeixnerFactory.hxx
namespace OT { %extend MeixnerFactory { MeixnerFactory(const MeixnerFactory & other) { return new OT::MeixnerFactory(other); } } }
