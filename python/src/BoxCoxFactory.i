// SWIG file BoxCoxFactory.i

%{
#include "openturns/BoxCoxFactory.hxx"
%}


%include  BoxCoxFactory_doc.i

%include openturns/BoxCoxFactory.hxx
namespace OT { %extend BoxCoxFactory { BoxCoxFactory(const BoxCoxFactory & other) { return new OT::BoxCoxFactory(other); } } }
