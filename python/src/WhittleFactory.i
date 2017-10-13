// SWIG file WhittleFactory.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & informationCriteria ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & informationCriteria %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::ARMA OT::WhittleFactory::buildWithCriteria(const OT::TimeSeries & timeSeries,
                                               OT::Point & informationCriteria) const;
OT::ARMA OT::WhittleFactory::buildWithCriteria(const OT::ProcessSample & sample,
                                               OT::Point & informationCriteria) const;

%{
#include "openturns/WhittleFactory.hxx"
%}

%template(WhittleFactoryStateCollection)           OT::Collection<OT::WhittleFactoryState>;
%template(WhittleFactoryStatePersistentCollection) OT::PersistentCollection<OT::WhittleFactoryState>;

%include WhittleFactory_doc.i

%include openturns/WhittleFactory.hxx
namespace OT { %extend WhittleFactory { WhittleFactory(const WhittleFactory & other) { return new OT::WhittleFactory(other); } } }

%clear OT::Point & informationCriteria;
