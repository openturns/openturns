//                                               -*- C++ -*-
/**
 *  @brief The validation of a chaos expansion
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_FUNCTIONALCHAOSVALIDATION_HXX
#define OPENTURNS_FUNCTIONALCHAOSVALIDATION_HXX

#include "openturns/Sample.hxx"
#include "openturns/FunctionalChaosResult.hxx"
#include "openturns/MetaModelValidation.hxx"
#include "openturns/SplitterImplementation.hxx"
#include "openturns/LeaveOneOutSplitter.hxx"
#include "openturns/KFoldSplitter.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FunctionalChaosValidation
 *
 * The validation of a chaos expansion
 */

class OT_API FunctionalChaosValidation
  : public MetaModelValidation
{
  CLASSNAME

public:
  typedef Collection<Function>           FunctionCollection;

  /** Default constructor */
  FunctionalChaosValidation();

  /** Parameter constructor */
  explicit FunctionalChaosValidation(const FunctionalChaosResult & chaosResult);

  /** Parameter constructor */
  FunctionalChaosValidation(const FunctionalChaosResult & chaosResult,
                            const LeaveOneOutSplitter & splitter);

  /** Parameter constructor */
  FunctionalChaosValidation(const FunctionalChaosResult & chaosResult,
                            const KFoldSplitter & splitter);

  /** Virtual constructor */
  FunctionalChaosValidation * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Result accessor */
  FunctionalChaosResult getFunctionalChaosResult() const;

  /** Get the splitter */
  SplitterImplementation getSplitter() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

private:

  /** The functional chaos result */
  FunctionalChaosResult functionalChaosResult_;

  /** Splitter parameter */
  SplitterImplementation splitter_;

  /* Compute cross-validation metamodel predictions */
  static Sample ComputeMetamodelLeaveOneOutPredictions(
    const FunctionalChaosResult & functionalChaosResult, const LeaveOneOutSplitter & splitter);

  /* Compute cross-validation metamodel predictions */
  static Sample ComputeMetamodelKFoldPredictions(
    const FunctionalChaosResult & functionalChaosResult, const KFoldSplitter & splitter);

} ; /* class FunctionalChaosValidation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FUNCTIONALCHAOSVALIDATION_HXX */
