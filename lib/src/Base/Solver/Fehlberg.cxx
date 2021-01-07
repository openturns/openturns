//                                               -*- C++ -*-
/**
 *  @brief This class implements the adaptive Fehlberg method of order p/p+1
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Fehlberg.hxx"
#include "openturns/PiecewiseHermiteEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Fehlberg)

static const Factory<Fehlberg> Factory_Fehlberg;

/* Default constructor */
Fehlberg::Fehlberg()
  : ODESolverImplementation()
  , localPrecision_(ResourceMap::GetAsScalar("Fehlberg-LocalPrecision"))
  , order_(0)
  , alpha_(0)
  , beta_(0)
  , c_(0)
  , cHat_(0)
{
  // Nothing to do
}

/* Default constructor */
Fehlberg::Fehlberg(const Function & transitionFunction,
                   const Scalar localPrecision,
                   const UnsignedInteger order)
  : ODESolverImplementation(transitionFunction)
  , localPrecision_(localPrecision)
  , order_(order)
  , alpha_(order + 1)
  , beta_((order + 1) * (order + 2) / 2)
  , c_(order + 1)
  , cHat_(order + 2)
{
  // Set the coefficients
  // See en.m.wikipedia.org/wiki/List_of_Runge-Kutta_methods for orders 0, 1, 2 and 4
  // and https://fr.mathworks.com/help/symbolic/mupad_ref/numeric-butcher.html for order 3
  switch (order)
  {
    case 0:
      alpha_[0] = 1.0;

      beta_[ 0] = 1.0;

      c_[0] = 1.0;

      cHat_[0] = 1.0 / 2.0;
      cHat_[1] = 1.0 / 2.0;
      break;
    case 1:
      alpha_[0] = 1.0 / 2.0;
      alpha_[1] = 1.0;

      beta_[ 0] = 1.0 / 2.0;
      beta_[ 1] = 1.0 / 256.0;
      beta_[ 2] = 255.0 / 256.0;

      c_[0] = 1.0 / 256.0;
      c_[1] = 255.0 / 256.0;

      cHat_[0] = 1.0 / 512.0;
      cHat_[1] = 255.0 / 256.0;
      cHat_[2] = 1.0 / 512.0;
      break;
    case 2:
      alpha_[0] = 1.0 / 4.0;
      alpha_[1] = 27.0 / 40.0;
      alpha_[2] = 1.0;

      beta_[ 0] = 1.0 / 4.0;
      beta_[ 1] = -189.0 / 800.0;
      beta_[ 2] = 729.0 / 800.0;
      beta_[ 3] = 214.0 / 891.0;
      beta_[ 4] = 1.0 / 35.0;
      beta_[ 5] = 650.0 / 891.0;

      c_[0] = 214.0 / 891.0;
      c_[1] = 1.0 / 33.0;
      c_[2] = 650.0 / 891.0;

      cHat_[0] = 533.0 / 2106.0;
      cHat_[1] = 0.0;
      cHat_[2] = 800.0 / 1053.0;
      cHat_[3] = -1.0 / 78.0;
      break;
    case 3:
      alpha_[0] = 1.0 / 4.0;
      alpha_[1] = 4.0 / 9.0;
      alpha_[2] = 6.0 / 7.0;
      alpha_[3] = 1.0;

      beta_[0] = 1.0 / 4.0;
      beta_[1] = 4.0 / 81.0;
      beta_[2] = 32.0 / 81.0;
      beta_[3] = 57.0 / 98.0;
      beta_[4] = -432.0 / 343.0;
      beta_[5] = 1053.0 / 686.0;
      beta_[6] = 1.0 / 6.0;
      beta_[7] = 0.0;
      beta_[8] = 27.0 / 52.0;
      beta_[9] = 49.0 / 156.0;

      c_[0] = 1.0 / 6.0;
      c_[1] = 0.0;
      c_[2] = 27.0 / 52.0;
      c_[3] = 49.0 / 156.0;

      cHat_[0] = 43.0 / 288.0;
      cHat_[1] = 0.0;
      cHat_[2] = 243.0 / 416.0;
      cHat_[3] = 343.0 / 1872.0;
      cHat_[4] = 1.0 / 12.0;
      break;
    case 4:
      alpha_[0] = 1.0 / 4.0;
      alpha_[1] = 3.0 / 8.0;
      alpha_[2] = 12.0 / 13.0;
      alpha_[3] = 1.0;
      alpha_[4] = 1.0 / 2.0;

      beta_[ 0] = 1.0 / 4.0;
      beta_[ 1] = 3.0 / 32.0;
      beta_[ 2] = 9.0 / 32.0;
      beta_[ 3] = 1932.0 / 2197.0;
      beta_[ 4] = -7200.0 / 2197.0;
      beta_[ 5] = 7296.0 / 2197.0;
      beta_[ 6] = 439.0 / 216.0;
      beta_[ 7] = -8.0;
      beta_[ 8] = 3680.0 / 513.0;
      beta_[ 9] = -845.0 / 4104.0;
      beta_[10] = -8.0 / 27.0;
      beta_[11] = 2.0;
      beta_[12] = -3544.0 / 2565.0;
      beta_[13] = 1859.0 / 4104.0;
      beta_[14] = -11.0 / 40.0;

      c_[0] = 25.0 / 216.0;
      c_[1] = 0.0;
      c_[2] = 1408.0 / 2565.0;
      c_[3] = 2197.0 / 4104.0;
      c_[4] = -1.0 / 5.0;

      cHat_[0] = 16.0 / 135.0;
      cHat_[1] = 0.0;
      cHat_[2] = 6656.0 / 12825.0;
      cHat_[3] = 28561.0 / 56430.0;
      cHat_[4] = -9.0 / 50.0;
      cHat_[5] = 2.0 / 55.0;
      break;
    default:
      throw InvalidArgumentException(HERE) << "Error: no method of order=" << order << " is available for now. Use order=0, 1, 2 or 4;";
  }
}

/* Virtual constructor */
Fehlberg * Fehlberg::clone() const
{
  return new Fehlberg( *this );
}

/* String converter */
String Fehlberg::__repr__() const
{
  return OSS() << "class=" << GetClassName()
         << ", localPrecision=" << localPrecision_
         << ", order=" << order_
         << ", alpha=" << alpha_
         << ", beta=" << beta_
         << ", c=" << c_
         << ", cHat=" << cHat_;
}

/* Perform cross-validation */
Sample Fehlberg::solve(const Point & initialState,
                       const Point & timeGrid) const
{
  if (initialState.getDimension() != transitionFunction_.getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the initial state has a dimension=" << initialState.getDimension() << ", expected dimension=" << transitionFunction_.getInputDimension();
  if (!timeGrid.isMonotonic()) throw InvalidArgumentException(HERE) << "Error: expected a monotonic time grid.";
  // Quick return if the time grid is empty
  const UnsignedInteger steps = timeGrid.getSize();
  const UnsignedInteger dimension = transitionFunction_.getOutputDimension();
  if (steps == 0) return Sample(steps, dimension);
  // First, solve the ODE up to the final time and store all the intermediate values
  Point state(initialState);
  Scalar t = timeGrid[0];
  Sample values(1, state);
  Sample derivatives(0, dimension);
  Point times(1, t);
  Scalar h = ResourceMap::GetAsScalar("Fehlberg-InitialStep");
  const Scalar hMin = ResourceMap::GetAsScalar("Fehlberg-MinimalStep");
  const Scalar tEnd = timeGrid[steps - 1];
  const Bool positiveStep = tEnd > t;
  if (!positiveStep) h = -h;
  Bool done = false;
  Point gradient;
  // Use a pointer to avoid many copies through setParameter
  Pointer<EvaluationImplementation> transitionFunction = transitionFunction_.getEvaluation().getImplementation()->clone();
  while (!done)
  {
    Scalar newT = t + h;
    if ((positiveStep && (newT > tEnd)) || (!positiveStep && (newT < tEnd)))
    {
      done = true;
      h = tEnd - t;
      newT = tEnd;
    }
    state = computeStep(transitionFunction, t, state, gradient, h);
    h = std::max(hMin, h);
    values.add(state);
    derivatives.add(gradient);
    times.add(newT);
    t = newT;
  }
  // Final evaluation of the gradient
  transitionFunction->setParameter(Point(1, t));
  derivatives.add(transitionFunction->operator()(state));
  // Now we interpolate the solution on the expected grid
  PiecewiseHermiteEvaluation hermite(times, values, derivatives);
  Sample result(steps, dimension);
  for (UnsignedInteger i = 0; i < steps; ++i)
    result[i] = hermite(Point(1, timeGrid[i]));
  return result;
}

/* Perform one step of the Fehlberg method
   See J. Stoer, R. Bulirsch, "Introduction to Numerical Analysis 2nd Edition", pp448-458.
 */
Point Fehlberg::computeStep(Pointer<EvaluationImplementation> & transitionFunction,
                            const Scalar t,
                            const Point & state,
                            Point & gradient,
                            Scalar & h) const
{
  const UnsignedInteger dimension = state.getDimension();
  Sample f(order_ + 2, dimension);
  Point parameter(1, t);
  transitionFunction->setParameter(parameter);
  gradient = transitionFunction->operator()(state);
  f[0] = gradient;
  UnsignedInteger index = 0;
  for (UnsignedInteger k = 0; k <= order_; ++k)
  {
    const Scalar tK = t + alpha_[k] * h;
    Point yK(state);
    for (UnsignedInteger l = 0; l <= k; ++l)
    {
      yK += f[l] * (h * beta_[index]);
      ++index;
    }
    parameter[0] = tK;
    transitionFunction->setParameter(parameter);
    f[k + 1] = transitionFunction->operator()(yK);
  }
  Point PhiI(dimension);
  Point PhiII(dimension);
  for (UnsignedInteger k = 0; k <= order_; ++k)
  {
    PhiI  += f[k] * c_[k];
    PhiII += f[k] * cHat_[k];
  }
  PhiII += f[order_ + 1] * cHat_[order_ + 1];
  const Point value(state + PhiII * h);
  // The update formula for h is based on relation 7.2.5.17 in the reference,
  // with the remark that \bar{y}_{i+1}-\hat{y}_{i+1}=h_i(\Phi_I-\Phi_{II})
  const Scalar delta = (PhiI - PhiII).norm();
  if (delta > 0.0)
  {
    const Scalar factor = std::exp(std::log(localPrecision_ / delta) / (order_ + 1.0));
    h *= factor;
  }
  return value;
}

/* Method save() stores the object through the StorageManager */
void Fehlberg::save(Advocate & adv) const
{
  ODESolverImplementation::save(adv);
  adv.saveAttribute("localPrecision_", localPrecision_);
  adv.saveAttribute("order_", order_);
  adv.saveAttribute("alpha_", alpha_);
  adv.saveAttribute("beta_", beta_);
  adv.saveAttribute("c_", c_);
  adv.saveAttribute("cHat_", cHat_);
}

/* Method load() reloads the object from the StorageManager */
void Fehlberg::load(Advocate & adv)
{
  ODESolverImplementation::load(adv);
  adv.loadAttribute("localPrecision_", localPrecision_);
  adv.loadAttribute("order_", order_);
  adv.loadAttribute("alpha_", alpha_);
  adv.loadAttribute("beta_", beta_);
  adv.loadAttribute("c_", c_);
  adv.loadAttribute("cHat_", cHat_);
}

END_NAMESPACE_OPENTURNS
