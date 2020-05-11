

#include "openturns/IterativeQuantile.hxx"
#include "openturns/IterativeAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
// #include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeQuantile)

static const Factory<IterativeQuantile> Factory_IterativeQuantile;

IterativeQuantile::IterativeQuantile(const UnsignedInteger dimension,
                                     const Scalar order,
                                     const UnsignedInteger nmax
                                    )
  : IterativeAlgorithmImplementation(dimension)
  , order_(order)
  , nmax_(nmax)
  , data_(dimension_, 0.0)
{
  // Nothing to do
}

/* Virtual constructor */
IterativeQuantile * IterativeQuantile::clone() const
{
  return new IterativeQuantile(*this);
}

// IterativeAlgorithm * IterativeQuantile::create(const int dimension)
// {
//   return new IterativeQuantile(dimension);
// }

/* String converter */
String IterativeQuantile::__repr__() const
{
  OSS oss(true);
  oss << "class=" << IterativeQuantile::GetClassName()
      << " iteration=" << iteration_
      << " dimension=" << dimension_
      << " values=" << data_.__repr__();
  return oss;
}

String IterativeQuantile::__str__(const String & offset) const
{
  return data_.__str__(offset);
}

UnsignedInteger IterativeQuantile::getOrder() const
{
  return order_;
}

UnsignedInteger IterativeQuantile::getNmax() const
{
  return nmax_;
}

Point IterativeQuantile::getQuantile() const
{
  return data_;
}

void IterativeQuantile::increment(const Point & newData)
{
  if (newData.getSize() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Point is not compatible with the dimension of the iterative quantile.";
  iteration_ += 1;
  Scalar temp;

  if (iteration_ > 1)
  {
      for (UnsignedInteger i = 0; i < dimension_; ++i)
      {
          Scalar gamma = (dimension_ - 1) * 0.9 / (nmax_ - 1) + 0.1;
          if (data_[i] >= newData[i])
          {
              temp = 1 - order_;
          }
          else
          {
              temp = 0 - order_;
          }
          data_[i] -= temp/pow(iteration_, gamma);
      }
  }
  else
  {
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      data_[i] = newData[i];
    }
  }
}

void IterativeQuantile::increment(const Sample & newData)
{
  if (newData.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given Sample is not compatible with the dimension of the iterative quantile.";

  Scalar temp;
  for (UnsignedInteger j = 0; j < newData.getSize(); ++j)
  {
    Point tempData = newData[j];
    iteration_ += 1;

    if (iteration_ > 1)
    {
        for (UnsignedInteger i = 0; i < dimension_; ++i)
        {
            Scalar gamma = (dimension_ - 1) * 0.9 / (nmax_ - 1) + 0.1;
            if (data_[i] >= tempData[i])
            {
                temp = 1 - order_;
            }
            else
            {
                temp = 0 - order_;
            }
            data_[i] -= temp/pow(iteration_, gamma);
        }
    }
    else
    {
      for (UnsignedInteger i = 0; i < dimension_; ++i)
      {
        data_[i] = tempData[i];
      }
    }
  }
}

/* Method save() stores the object through the StorageManager */
void IterativeQuantile::save(Advocate & adv) const
{
  IterativeAlgorithmImplementation::save(adv);
  adv.saveAttribute( "order_", order_);
  adv.saveAttribute( "nmax_", nmax_);
  adv.saveAttribute( "data_", data_);
}


/* Method load() reloads the object from the StorageManager */
void IterativeQuantile::load(Advocate & adv)
{
  IterativeAlgorithmImplementation::load(adv);
  adv.loadAttribute( "order_", order_);
  adv.loadAttribute( "nmax_", nmax_);
  adv.loadAttribute( "data_", data_);
}

END_NAMESPACE_OPENTURNS
