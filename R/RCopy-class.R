# Set initial class ( a bit different than what we had done before) #
################
# Class: RCopy #
################

setClass("RCopy", slots=c(data = "data.frame"), contains=c("data.frame"))