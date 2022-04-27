// SWIG file ResourceMap.i

%{
#include "openturns/ResourceMap.hxx"
%}

%include ResourceMap_doc.i

%template(StringStringMap) std::map<OT::String, OT::String>;

%nodefaultctor ResourceMap;

%ignore OT::ResourceMap_init;

%include openturns/ResourceMap.hxx

namespace OT {

%extend ResourceMap {

static UnsignedInteger __len__()
{
  return OT::ResourceMap::GetSize();
}

} // ResourceMap

} // OT
