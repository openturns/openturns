// SWIG file WhittleFactoryState.i

%{
#include "WhittleFactoryState.hxx"
%}

%include WhittleFactoryState_doc.i

%include WhittleFactoryState.hxx
namespace OT { %extend WhittleFactoryState { WhittleFactoryState(const WhittleFactoryState & other) { return new OT::WhittleFactoryState(other); } } }
