// SWIG file UserDefined.i



%{
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/UserDefined.hxx"
%}

%include UserDefined_doc.i

%template(UserDefinedPairCollection) OT::Collection<OT::UserDefinedPair>;

%include openturns/UserDefined.hxx

%extend OT::Collection<OT::UserDefinedPair>
{

OT::Collection<OT::UserDefinedPair> (PyObject * pyObj)
{
  return OT::buildCollectionFromPySequence<OT::UserDefinedPair>( pyObj );
}

}


namespace OT {  

%extend UserDefined {

UserDefined(const UserDefined & other)
{
  return new OT::UserDefined(other);
} 

UserDefined(PyObject * pyObj)
{
  OT::Pointer<OT::Collection<OT::UserDefinedPair> > p_coll =  OT::buildCollectionFromPySequence<OT::UserDefinedPair>( pyObj );
  return new OT::UserDefined( *p_coll );
}


} // UserDefined

} // OT

