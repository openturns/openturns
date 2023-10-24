# sudo apt install -y r-cran-rcpp r-cran-gsl ...
# R -e 'install.packages(c("extRemes", "evmix"))'
# Rscript validation/src/ValidGeneralizedParetoFactory.R

data <- as.matrix(read.csv("python/src/usecases/rain.csv"))
# print(data)

# mean residual life
library(extRemes)
png("mlr.png")
mrlplot(data)

library(eva)
png("mlexi.png")
# mle
fit <- gpdFit(data, threshold=30)
print(fit)

# profiled ll
fit2 <- gpdProfShape(fit)
print(fit2)

# parameter stability plots
library(evmix)
png("shape.png")
tshapeplot(data, tlim = c(0, 50))
png("scale.png")
tscaleplot(data, tlim = c(0, 50)) # , try.thresh = c(8, 10, 12)
tscaleplot(data, tlim = c(10, 40))
