// SWIG file NumericalPointWithDescription.i

%{
#include "NumericalPointWithDescription.hxx"

namespace OT { 

  template <>
  struct traitsPythonType<OT::NumericalPointWithDescription>
  {
    typedef _PySequence_ Type;
  };

  template <>
  inline
  OT::NumericalPointWithDescription
  convert<_PySequence_,OT::NumericalPointWithDescription>(PyObject * pyObj)
  {
    check<_PySequence_>( pyObj );
    ScopedPyObjectPointer newPyObj(PySequence_Fast( pyObj, "" ));

    const OT::UnsignedInteger size = PySequence_Fast_GET_SIZE( newPyObj.get() );
    OT::NumericalPointWithDescription coll( size );
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
        coll[i] = convert<_PyFloat_,OT::NumericalScalar>( item_1 );
      } else {
        throw OT::InvalidArgumentException(HERE) << "Sequence item " << i << " passed as argument is not a pair (String, NumericalScalar)";
      }
    }

    coll.setDescription( desc );
    return coll;
  }


} /* namespace OT */

%}

%include NumericalPointWithDescription_doc.i

%rename(NumericalPointWithDescription_operator___eq__) OT::operator ==(const NumericalPointWithDescription & lhs, const NumericalPointWithDescription & rhs);

%include NumericalPointWithDescription.hxx
namespace OT{  

%extend NumericalPointWithDescription {

NumericalPointWithDescription(const NumericalPointWithDescription & other)
{
  return new OT::NumericalPointWithDescription(other);
}


NumericalPointWithDescription(PyObject * pyObj)
{
  return new OT::NumericalPointWithDescription( OT::convert<OT::_PySequence_,OT::NumericalPointWithDescription>(pyObj) );
}

}
}

%template(NumericalPointWithDescriptionCollection) OT::Collection<OT::NumericalPointWithDescription>;
%template(NumericalPointWithDescriptionPersistentCollection) OT::PersistentCollection<OT::NumericalPointWithDescription>;
