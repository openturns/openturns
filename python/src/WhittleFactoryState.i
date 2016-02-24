// SWIG file WhittleFactoryState.i

%{
#include "openturns/WhittleFactoryState.hxx"
%}

%include WhittleFactoryState_doc.i

%include openturns/WhittleFactoryState.hxx
namespace OT { %extend WhittleFactoryState { WhittleFactoryState(const WhittleFactoryState & other) { return new OT::WhittleFactoryState(other); } } }
