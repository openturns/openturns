# sudo apt install -y r-cran-rcpp r-cran-gsl ...
# R -e 'install.packages(c("extRemes"))'
# Rscript validation/src/ValidSamplePartition.R

library(extRemes)

data <- as.data.frame(read.csv("python/src/usecases/wooster.csv"))
data$Temperature <- as.numeric(data$Temperature) * -1
data$month <- as.numeric(format(as.Date(data$Date, format="%Y-%m-%d"), "%m"))
data <- data[data$month %in% c(1,2,12), ]

ydc <- decluster(data$Temperature, threshold=-20, r=3, method="runs", which.plot="scatter")
print(ydc)

png("decluster.png")
plot(ydc)
