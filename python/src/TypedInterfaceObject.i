// SWIG file TypedInterfaceObject.i

%{
#include "openturns/TypedInterfaceObject.hxx"
%}

%define TypedInterfaceObjectImplementationHelper(Namespace, Interface, Implementation)

%template(Implementation ## TypedInterfaceObject) OT::TypedInterfaceObject<Namespace::Implementation>;

%typemap(in) const Namespace::Interface & ($1_basetype temp) {
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    // From interface class, ok
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, $descriptor(Namespace ## __ ## Implementation *), SWIG_POINTER_NO_NULL))) {
    // From Implementation*
    Namespace::Implementation * p_impl = reinterpret_cast< Namespace::Implementation * >(ptr);
    temp = Namespace::Interface(*p_impl);
    $1 = &temp;
  } else {
    SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a " # Interface);
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const Namespace::Interface & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $descriptor(Namespace ## __ ## Implementation *), SWIG_POINTER_NO_NULL));
}

%enddef


%define OTTypedInterfaceObjectImplementationHelper(Interface, Implementation)
TypedInterfaceObjectImplementationHelper(OT, Interface, Implementation)
%enddef

%define OTTypedInterfaceObjectHelper(Interface)
OTTypedInterfaceObjectImplementationHelper(Interface,Interface ## Implementation)
%enddef


/***************************************************************************
 * This section contains the parts of TypedInterfaceObject.hxx
 * that SWIG needs to generate a wrapper.
 *
 * Not all public methods are interfaced in Python.
 * In particular, swap is not interfaced.
 * The accessor getImplementation is also not interfaced and is replaced by
 * a SWIG implementation provided under %extend (see below).
 ***************************************************************************/

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TypedInterfaceObject
 * @brief Implements InterfaceObject for a specific class
 * @internal
 * @tparam T The class bound to the interface object
 * @see PersistentObject
 */
template <class T>
class TypedInterfaceObject
  : public InterfaceObject
{
public:

  typedef T                                                   ImplementationType;
  typedef Pointer<ImplementationType>                         Implementation;

  /** Constructor */
  TypedInterfaceObject();
  TypedInterfaceObject(const Implementation & impl);

  /** Return a pointer to the underlying implementation object viewed as a PersistentObject */
  inline virtual ImplementationAsPersistentObject getImplementationAsPersistentObject() const;

  /** Set the pointer to the underlying implementation object */
  inline virtual void setImplementationAsPersistentObject(const ImplementationAsPersistentObject & obj);

  /** Name accessor */
  inline virtual void setName(const String & name);

  /** Name accessor */
  inline virtual String getName() const;

  /** Comparison Operator */
  inline virtual
  Bool operator == (const TypedInterfaceObject & other) const;

  /** Comparison Operator */
  inline virtual
  Bool operator != (const TypedInterfaceObject & other) const;

}; /* class TypedInterfaceObject */

END_NAMESPACE_OPENTURNS



/***************************************************************************
 * TypedInterfaceObject class extension
 *
 * Provides a version of getImplementation that performs dynamic casting.
 ***************************************************************************/
namespace OT
{
  %extend TypedInterfaceObject
  {
    PyObject * getImplementation()
    {
      // Get child info
      OT::String childname = "OT::";
      childname.append($self->getImplementation()->getClassName());
      childname.append("*");
      swig_type_info * childinfo = SWIG_TypeQuery(childname.c_str());

      // Return child pointer
      return SWIG_NewPointerObj(SWIG_as_voidptr($self->getImplementation().get()), childinfo, 0 | 0);
    }
  }
}
