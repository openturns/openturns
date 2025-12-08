// SWIG file WhittleFactory.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & informationCriteriaOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & informationCriteriaOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"),  SWIG_POINTER_OWN)); %}

%{
#include "openturns/WhittleFactory.hxx"
%}

%template(WhittleFactoryStateCollection)           OT::Collection<OT::WhittleFactoryState>;

%include WhittleFactory_doc.i

%copyctor OT::WhittleFactory;

%include openturns/WhittleFactory.hxx
