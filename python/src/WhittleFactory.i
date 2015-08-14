// SWIG file WhittleFactory.i

%{
#include "WhittleFactory.hxx"
%}

%template(WhittleFactoryStateCollection)           OT::Collection<OT::WhittleFactoryState>;
%template(WhittleFactoryStatePersistentCollection) OT::PersistentCollection<OT::WhittleFactoryState>;

%include WhittleFactory_doc.i

%include WhittleFactory.hxx
namespace OT { %extend WhittleFactory { WhittleFactory(const WhittleFactory & other) { return new OT::WhittleFactory(other); } } }
