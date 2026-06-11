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
  convert<_PySequence_,OT::PointWithDescription>(PyObject * pyObj)
  {
    check<_PySequence_>(pyObj);
    const OT::UnsignedInteger size = PySequence_Length(pyObj);
    OT::PointWithDescription coll(size);
    OT::Description desc = OT::Description(size);

    for (OT::UnsignedInteger i = 0; i < size; ++ i)
    {
      ScopedPyObjectPointer elt(PySequence_GetItem(pyObj, i));
      check<_PySequence_>(elt.get());
      if (PySequence_Length(elt.get()) == 2) {
        ScopedPyObjectPointer item_0(PySequence_GetItem(elt.get(), 0));
        check<_PyString_>(item_0.get());
        desc[i] = convert<_PyString_, OT::String>(item_0.get());
        ScopedPyObjectPointer item_1(PySequence_GetItem(elt.get(), 1));
        check<_PyFloat_>(item_1.get());
        coll[i] = convert<_PyFloat_, OT::Scalar>(item_1.get());
      }
      else
        throw OT::InvalidArgumentException(HERE) << "Sequence item " << i << " passed as argument is not a pair (String, Scalar)";
    }

    coll.setDescription(desc);
    return coll;
  }


} /* namespace OT */

%}

%include PointWithDescription_doc.i

%rename(PointWithDescription_operator___eq__) OT::operator ==(const PointWithDescription & lhs, const PointWithDescription & rhs);
%ignore OT::PointWithDescription::PointWithDescription(std::initializer_list<Scalar> initList);

%include openturns/PointWithDescription.hxx
namespace OT{

%extend PointWithDescription {

PointWithDescription(const PointWithDescription & other)
{
  return new OT::PointWithDescription(other);
}


PointWithDescription(PyObject * pyObj)
{
  return new OT::PointWithDescription( OT::convert<OT::_PySequence_,OT::PointWithDescription>(pyObj) );
}

}
}

%template(_PointWithDescriptionCollection) OT::Collection<OT::PointWithDescription>;
%template(_PointWithDescriptionPersistentCollection) OT::PersistentCollection<OT::PointWithDescription>;

