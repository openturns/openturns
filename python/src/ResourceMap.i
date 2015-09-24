// SWIG file ResourceMap.i

%{
#include "ResourceMap.hxx"
%}

%include ResourceMap_doc.i

%template(StringStringMap) std::map<OT::String, OT::String>;

%nodefaultctor ResourceMap;

%include ResourceMap.hxx

namespace OT {

%extend ResourceMap {

UnsignedInteger __len__() const
{
  return OT::ResourceMap::GetSize();
}

} // ResourceMap

} // OT
