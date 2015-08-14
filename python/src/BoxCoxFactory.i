// SWIG file BoxCoxFactory.i

%{
#include "BoxCoxFactory.hxx"
%}


%include  BoxCoxFactory_doc.i

%include BoxCoxFactory.hxx
namespace OT { %extend BoxCoxFactory { BoxCoxFactory(const BoxCoxFactory & other) { return new OT::BoxCoxFactory(other); } } }
