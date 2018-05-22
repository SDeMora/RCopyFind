# Set initial class #
################
# Class: RCopy #
################

setClass("RCopy", slots=c(data = "data.frame"), contains=c("data.frame"))