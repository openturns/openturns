// SWIG file WelchFactory.i

%{
#include "WelchFactory.hxx"
%}

%include  WelchFactory_doc.i

%include WelchFactory.hxx
namespace OT { 

  %extend WelchFactory {

     WelchFactory(const WelchFactory & other)
       {
         return new OT::WelchFactory(other);
       }

   }
 }
