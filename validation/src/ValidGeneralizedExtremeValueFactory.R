# sudo apt install -y r-cran-rcpp ...
# R -e 'install.packages(c("mev", "eva"))'
# Rscript ValidGEV.R

# to export .rda to .csv:
# load(file="/tmp/venice.rda")
# write.csv(venice, "/tmp/venice.csv", row.names=FALSE)

# MLE fit
library(mev)
xdat <- evd::rgev(n = 10000, loc=2, scale=1.5, shape=-0.15)
write.csv(xdat,file="/tmp/xdat.csv", row.names=F)
print(xdat)
print(mean(xdat))
print(sd(xdat))
fit.gev(xdat, show = TRUE)

# R maxima fit
library(eva)
data(lowestoft)
print(lowestoft)
write.csv(lowestoft,file="/tmp/lowestoft.csv", row.names=F)
print(mean(lowestoft))
print(sd(lowestoft))
z <- gevrFit(as.matrix(lowestoft[, 1:10]))
print(z)
print()
gevrSeqTests(lowestoft, method = "ed")



