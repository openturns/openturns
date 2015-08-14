//                                               -*- C++ -*-
/**
 *  @brief Implementation of the method AnalyticalFunctionCompiler::CreateSourceCode(),
 *         which produces the C++ implementation of the needed external code.
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <string>
#include <sstream>
#include <iostream>
#include <list>
#include <fstream>
#include <iomanip>
#include "AnalyticalFunctionCompiler.hxx"

namespace AnalyticalFunctionCompiler
{
/** Generate the source code of the plugin */
void AnalyticalFunctionCompiler::createSourceCode()
{
  std::ostringstream oss;
  oss << path_ << name_ << ".cxx";
  std::string fileName(oss.str());
  std::ofstream sourceFile(fileName.c_str(), std::ios::out);

  if(!sourceFile)
  {
    std::cout << "Error creating source file " << fileName << std::endl;
    exit(BAD_SOURCE_CREATION);
  }
  // Headers
  sourceFile << "#include <cmath>" << std::endl
             << "#include <iostream>" << std::endl
             << "#include <cstdlib>" << std::endl
             << "#include \"openturns/OT.hxx\"" << std::endl
             << "#include \"openturns/WrapperInterface.h\"" << std::endl << std::endl
             // Internal part of the wrapper
             << "namespace WrapperInternals {" << std::endl
             << "  struct internalState {" << std::endl
             << "    long numberOfCalls;" << std::endl
             << "  };" << std::endl << std::endl
             << "  void internalStateInitialization(struct internalState * p_internalState)" << std::endl
             << "  {" << std::endl
             << "    if (p_internalState) p_internalState->numberOfCalls = 0;" << std::endl
             << "  }" << std::endl << std::endl
             << "  void internalStateIncrement(struct internalState * p_internalState)" << std::endl
             << "  {" << std::endl
             << "    if (p_internalState) p_internalState->numberOfCalls++;" << std::endl
             << "  }" << std::endl << std::endl
             << "  long internalStateGetNumberOfCalls(struct internalState * p_internalState)" << std::endl
             << "  {" << std::endl
             << "    return (p_internalState ? p_internalState->numberOfCalls : -1);" << std::endl
             << "  }" << std::endl
             << "} /* namespace WrapperInternals */" << std::endl << std::endl
             // End of the internal part
             // External part of the wrapper
             // Function part
             << "extern \"C\" {" << std::endl
             << "  /* Function */" << std::endl
             << "  enum WrapperErrorCode func_createState_" << name_ << "(void ** p_p_state)" << std::endl
             << "  {" << std::endl
             << "    *p_p_state = malloc(sizeof(struct WrapperInternals::internalState));" << std::endl
             << "     internalStateInitialization(static_cast<struct WrapperInternals::internalState *>(*p_p_state));" << std::endl
             << "     return WRAPPER_OK;" << std::endl
             << "   }" << std::endl << std::endl
             << "   enum WrapperErrorCode func_deleteState_" << name_ << "(void * p_state)" << std::endl
             << "   {" << std::endl
             << "     free(p_state);" << std::endl
             << "     return WRAPPER_OK;" << std::endl
             << "   }" << std::endl << std::endl
             << "   static struct WrapperInformation info_" << name_ << " = {/* inSize_  = */ " << variates_.size() << "," << std::endl
             << "     /* outSize_ = */ 1};" << std::endl << std::endl
             << "   enum WrapperErrorCode func_getInfo_" << name_ << "(struct WrapperInformation * p_info)" << std::endl
             << "   {" << std::endl
             << "     *p_info = info_" << name_ << ";" << std::endl
             << "     return WRAPPER_OK;" << std::endl
             << "   }" << std::endl << std::endl
             << "  enum WrapperErrorCode func_init_" << name_ << "(const struct WrapperExchangedData * p_exchangedData)" << std::endl
             << "  {" << std::endl
             << "    return WRAPPER_OK;" << std::endl
             << "  }" << std::endl << std::endl
             << "  enum WrapperErrorCode func_exec_" << name_ << "(void * p_state, const struct point * inPoint, struct point * outPoint)" << std::endl
             << "  {" << std::endl
             << "    internalStateIncrement(static_cast<struct WrapperInternals::internalState *>(p_state));" << std::endl;
  int indexInPoint = 0;
  for (std::list<std::string>::iterator iteratorVariates = variates_.begin();
       iteratorVariates != variates_.end(); iteratorVariates++)
  {
    sourceFile << "    double " << *iteratorVariates << "(inPoint->data_[" << indexInPoint << "]);" << std::endl;
    indexInPoint++;
  }
  sourceFile << std::endl
             << "    outPoint->data_[0] = " << formula_ << ";" << std::endl
             << "    return WRAPPER_OK;" << std::endl
             << "  }" << std::endl << std::endl


             << "  enum WrapperErrorCode func_exec_sample_" << name_ << "(void * p_state, const struct sample * inSample, struct sample * outSample)" << std::endl
             << "  {" << std::endl
             << "    unsigned long inDimension((inSample->data_[0]).size_);" << std::endl
             << "    if (info_" << name_ << ".inSize_ != inDimension) return WRAPPER_WRONG_ARGUMENT;" << std::endl
             << "    unsigned long outDimension((outSample->data_[0]).size_);" << std::endl
             << "    if (info_" << name_ << ".outSize_ != outDimension) return WRAPPER_WRONG_ARGUMENT;" << std::endl
             << "    unsigned long size(inSample->size_);" << std::endl
             << "    for (unsigned long i = 0; i < size; i++)" << std::endl
             << "      {" << std::endl
             << "        if (func_exec_" << name_ << "(p_state, &(inSample->data_[i]),  &(outSample->data_[i])) != WRAPPER_OK) return WRAPPER_EXECUTION_ERROR;" << std::endl
             << "      }" << std::endl
             << "    return WRAPPER_OK;" << std::endl
             << "  }" << std::endl << std::endl
             << "  enum WrapperErrorCode func_finalize_" << name_ << "()" << std::endl
             << "  {" << std::endl
             << "    return WRAPPER_OK;" << std::endl
             << "  }" << std::endl
             // Gradient part
             << "  /* Gradient, which is void at this time */" << std::endl
             << "  enum WrapperErrorCode grad_createState_" << name_ << "(void ** p_p_state)" << std::endl
             << "  {" << std::endl
             << "    *p_p_state = malloc(sizeof(struct WrapperInternals::internalState));" << std::endl
             << "     internalStateInitialization(static_cast<struct WrapperInternals::internalState *>(*p_p_state));" << std::endl
             << "     return WRAPPER_OK;" << std::endl
             << "   }" << std::endl << std::endl
             << "   enum WrapperErrorCode grad_deleteState_" << name_ << "(void * p_state)" << std::endl
             << "   {" << std::endl
             << "     free(p_state);" << std::endl
             << "     return WRAPPER_OK;" << std::endl
             << "   }" << std::endl << std::endl
             << "   enum WrapperErrorCode grad_getInfo_" << name_ << "(struct WrapperInformation * p_info)" << std::endl
             << "   {" << std::endl
             << "     *p_info = info_" << name_ << ";" << std::endl
             << "     return WRAPPER_OK;" << std::endl
             << "   }" << std::endl << std::endl
             << "  enum WrapperErrorCode grad_init_" << name_ << "(const struct WrapperExchangedData * p_exchangedData)" << std::endl
             << "  {" << std::endl
             << "    return WRAPPER_OK;" << std::endl
             << "  }" << std::endl << std::endl
             << "  enum WrapperErrorCode grad_exec_" << name_ << "(void * p_state, const struct point * inPoint, struct matrix * outMatrix)" << std::endl
             << "  {" << std::endl
             << "    return WRAPPER_OK;" << std::endl
             << "  }" << std::endl << std::endl
             << "  enum WrapperErrorCode grad_finalize_" << name_ << "()" << std::endl
             << "  {" << std::endl
             << "    return WRAPPER_OK;" << std::endl
             << "  }" << std::endl
             // Hessian part
             << "  /* Hessian, which is void at this time */" << std::endl
             << "  enum WrapperErrorCode hess_createState_" << name_ << "(void ** p_p_state)" << std::endl
             << "  {" << std::endl
             << "    *p_p_state = malloc(sizeof(struct WrapperInternals::internalState));" << std::endl
             << "     internalStateInitialization(static_cast<struct WrapperInternals::internalState *>(*p_p_state));" << std::endl
             << "     return WRAPPER_OK;" << std::endl
             << "   }" << std::endl << std::endl
             << "   enum WrapperErrorCode hess_deleteState_" << name_ << "(void * p_state)" << std::endl
             << "   {" << std::endl
             << "     free(p_state);" << std::endl
             << "     return WRAPPER_OK;" << std::endl
             << "   }" << std::endl << std::endl
             << "   enum WrapperErrorCode hess_getInfo_" << name_ << "(struct WrapperInformation * p_info)" << std::endl
             << "   {" << std::endl
             << "     *p_info = info_" << name_ << ";" << std::endl
             << "     return WRAPPER_OK;" << std::endl
             << "   }" << std::endl << std::endl
             << "  enum WrapperErrorCode hess_init_" << name_ << "(const struct WrapperExchangedData * p_exchangedData)" << std::endl
             << "  {" << std::endl
             << "    return WRAPPER_OK;" << std::endl
             << "  }" << std::endl << std::endl
             << "  enum WrapperErrorCode hess_exec_" << name_ << "(void * p_state, const struct point * inPoint, struct tensor * outTensor)" << std::endl
             << "  {" << std::endl
             << "    return WRAPPER_OK;" << std::endl
             << "  }" << std::endl << std::endl
             << "  enum WrapperErrorCode hess_finalize_" << name_ << "()" << std::endl
             << "  {" << std::endl
             << "    return WRAPPER_OK;" << std::endl
             << "  }" << std::endl
             << "} /* end extern \"C\" */" << std::endl;
  // End of the external part
  sourceFile.close();
} // AnalyticalFunctionCompiler::CreateSourceCode

} // namespace AnalyticalFunctionCompiler
