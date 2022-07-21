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
    check<_PySequence_>( pyObj );
    ScopedPyObjectPointer newPyObj(PySequence_Fast( pyObj, "" ));

    const OT::UnsignedInteger size = PySequence_Fast_GET_SIZE( newPyObj.get() );
    OT::PointWithDescription coll( size );
    OT::Description desc = OT::Description( size );

    for(OT::UnsignedInteger i=0; i<size; ++i) {
      PyObject * elt = PySequence_Fast_GET_ITEM( newPyObj.get(), i );
      check<_PySequence_>( elt );
      if (PySequence_Fast_GET_SIZE( elt ) == 2) {
        PyObject * item_0 = PySequence_Fast_GET_ITEM( elt, 0 );
        check<_PyString_>( item_0 );
        desc[i] = convert<_PyString_,OT::String>( item_0 );
        PyObject * item_1 = PySequence_Fast_GET_ITEM( elt, 1 );
        check<_PyFloat_>( item_1 );
        coll[i] = convert<_PyFloat_,OT::Scalar>( item_1 );
      } else {
        throw OT::InvalidArgumentException(HERE) << "Sequence item " << i << " passed as argument is not a pair (String, Scalar)";
      }
    }

    coll.setDescription( desc );
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

%template(PointWithDescriptionCollection) OT::Collection<OT::PointWithDescription>;
%template(PointWithDescriptionPersistentCollection) OT::PersistentCollection<OT::PointWithDescription>;

