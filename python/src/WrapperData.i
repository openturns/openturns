// SWIG file WrapperData.i

%{
#include "WrapperData.hxx"
#include "WrapperCommon_extern.h"
%}

%typemap(in) WrapperListElementType {
  if (OT::isAPython< OT::_PyInt_ >($input)) {
    $1 = static_cast<$1_ltype>(OT::convert< OT::_PyInt_, OT::UnsignedInteger >($input));
  }
  else if (OT::isAPython< OT::_PyString_ >($input)) {
    OT::String name(OT::convert< OT::_PyString_, OT::String >($input));
    WrapperListElementType res = UNUSED_LISTELEMENT;
    for ( int i = 0 ; i < UNUSED_LISTELEMENT; ++ i ) {
      if (name == wrapper_getListElementTypeAsString( static_cast<WrapperListElementType>(i))) {
        res = static_cast<WrapperListElementType>(i);
        break;
      }
    }
    if(res == UNUSED_LISTELEMENT) 
      throw OT::InvalidArgumentException(HERE) << "Invalid ListElementType keyword: " << name;
    $1 = static_cast<$1_ltype>(res);
  }
  else
    throw OT::InvalidArgumentException(HERE) << "Invalid ListElementType argument.";
}

%typemap(in) WrapperLocationType {
  if (OT::isAPython< OT::_PyInt_ >($input)) {
    $1 = static_cast<$1_ltype>(OT::convert< OT::_PyInt_, OT::UnsignedInteger >($input));
  }
  else if (OT::isAPython< OT::_PyString_ >($input)) {
    OT::String name(OT::convert< OT::_PyString_, OT::String >($input));
    WrapperLocationType res = UNUSED_LOCATION;
    for ( int i = 0 ; i < UNUSED_LOCATION; ++ i ) {
      if (name == wrapper_getLocationAsString( static_cast<WrapperLocationType>(i))) {
        res = static_cast<WrapperLocationType>(i);
        break;
      }
    }
    if(res == UNUSED_LOCATION) 
      throw OT::InvalidArgumentException(HERE) << "Invalid WrapperLocationType keyword: " << name;
    $1 = static_cast<$1_ltype>(res);
  }
  else
    throw OT::InvalidArgumentException(HERE) << "Invalid WrapperLocationType argument.";
}

%typemap(in) WrapperProvided {
  if (OT::isAPython< OT::_PyInt_ >($input)) {
    $1 = static_cast<$1_ltype>(OT::convert< OT::_PyInt_, OT::UnsignedInteger >($input));
  }
  else if (OT::isAPython< OT::_PyString_ >($input)) {
    OT::String name(OT::convert< OT::_PyString_, OT::String >($input));
    WrapperProvided res = UNUSED_PROVIDED;
    for ( int i = 0 ; i < UNUSED_PROVIDED; ++ i ) {
      if (name == wrapper_getProvidedAsString( static_cast<WrapperProvided>(i))) {
        res = static_cast<WrapperProvided>(i);
        break;
      }
    }
    if(res == UNUSED_PROVIDED) 
      throw OT::InvalidArgumentException(HERE) << "Invalid WrapperProvided keyword: " << name;
    $1 = static_cast<$1_ltype>(res);
  }
  else
    throw OT::InvalidArgumentException(HERE) << "Invalid WrapperProvided argument.";
}

%typemap(out) WrapperListElementType, WrapperLocationType, WrapperProvided {
  $result = OT::convert< OT::UnsignedInteger, OT::_PyInt_ >($1);
 }

%include WrapperInterface.h
%include WrapperData.hxx

%template(WrapperDataFileCollection)     OT::Collection<OT::WrapperDataFile>;
%template(WrapperDataVariableCollection) OT::Collection<OT::WrapperDataVariable>;

namespace OT{ %extend WrapperData { WrapperData(const WrapperData & other) { return new OT::WrapperData(other); } } }
