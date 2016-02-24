// SWIG file WelchFactory.i

%{
#include "openturns/WelchFactory.hxx"
%}

%include  WelchFactory_doc.i

%include openturns/WelchFactory.hxx
namespace OT { 

  %extend WelchFactory {

     WelchFactory(const WelchFactory & other)
       {
         return new OT::WelchFactory(other);
       }

   }
 }
