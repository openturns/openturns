lines(0);
Sample=...
 [ 3.92916 , 3.89263 , 0.446781;
  2.9039  , 1.80452 , 0.566517;
  0.962047, 1.73582 , 1.87565 ;
  1.49738 ,-1.97567 , 1.54773 ;
  0.202148,-0.822598, 0.905646;
 -1.80895 ,-0.587343, 2.24034 ;
  2.1505  , 1.4477  , 2.3731  ;
  2.90242 , 4.33725 , 2.11401 ;
  1.26622 ,-0.60984 ,-0.363285;
  0.900494,-1.77254 , 1.39818 ];
SampleSize = size(Sample, 1);
SampleDimension = size(Sample, 2);
ratio = SampleSize / (SampleSize - 1);
SampleMin = min(Sample, 'r')
SampleMax = max(Sample, 'r')
SampleMean = mean(Sample, 'r')
SampleCovariance = mvvacov(Sample) * ratio
SampleStandardDeviation = chol(SampleCovariance)'
SampleStandardDeviationPerComponent = stdev(Sample, 'r')
Pearson = zeros(SampleDimension, SampleDimension);
Id = diag(ones(SampleSize, 1));
for i=1:SampleDimension
  for j=1:SampleDimension
    Pearson(i, j) = correl(Sample(:, i),Sample(:, j), Id);
  end
end
Pearson
Spearman = zeros(SampleDimension, SampleDimension)
Range = zeros(SampleDimension, SampleDimension);
RangePerComponent = strange(Sample, 'r')
MedianPerComponent = median(Sample, 'r')
Sd = SampleStandardDeviationPerComponent;
Mode = SampleMin
Variance = diag(SampleCovariance)'
Skewness = cmoment(Sample, 3, 'r') ./ (SampleSize * Sd .^ 3)
Kurtosis = cmoment(Sample, 4, 'r') ./ (SampleSize * Sd .^ 4) - 3
Sorted = -sort(-Sample, 'r');
QuantilePerComponent = (1 - g) * Sorted(floor(0.25 * SampleSize), :) + g * Sorted(ceil(0.25 * SampleSize), :)


//(j - m) / n <= p < (j - m + 1) / n
//g = np + m - j
//p(k) = (k - 1) / (n - 1) = (k - alpha) / (n - alpha - beta + 1)
//m = alpha + p(1 - alpha - beta)
