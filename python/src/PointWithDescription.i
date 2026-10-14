// SWIG file PointWithDescription.i

%{
#include "openturns/PointWithDescription.hxx"

namespace OT { 
template <>
struct traitsPythonType<OT::PointWithDescription>
{
  typedef _PySequence_ Type;
};

template <>
inline
OT::PointWithDescription
convert<_PySequence_, OT::PointWithDescription>(PyObject * pyObj)
{
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::PointWithDescription *"), SWIG_POINTER_NO_NULL)))
  {
    return *reinterpret_cast<OT::PointWithDescription *>(ptr);
  }
  const OT::Point values(convert<_PySequence_, OT::Point>(pyObj));
  return OT::PointWithDescription(values);
}

} // namespace OT
%}

%include PointWithDescription_doc.i

%ignore OT::PointWithDescription::PointWithDescription(std::initializer_list<Scalar> initList);

%copyctor OT::PointWithDescription;

%include openturns/PointWithDescription.hxx

namespace OT {

%extend PointWithDescription {

Bool __eq__(const PointWithDescription & other) { return (*self) == other; }
Bool __ne__(const PointWithDescription & other) { return (*self) != other; }

} // %extend

} // OT

%template(_PointWithDescriptionCollection) OT::Collection<OT::PointWithDescription>;
%template(_PointWithDescriptionPersistentCollection) OT::PersistentCollection<OT::PointWithDescription>;

