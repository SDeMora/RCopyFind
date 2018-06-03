####################################################
### Create Directions Script File For Copy Find ####
####################################################

##1.) Create a folder in your working directory called "CReport".

##2.) Save directory as "OldWD":
OldWD<-paste(getwd(),"/CReport", sep="")

##3.) Get path to .exe in R library examples:
#CopyFind Program
DIRECT<-system.file("CopyFind", package="RCopyFind")
#Example Text for "Document1_loc"
Doc1<-system.file("extdata/ALEC Model Leg.txt", package="RCopyFind")
#Example Text for "Document2_loc"
Doc2<-system.file("extdata/txt files", package="RCopyFind")

##4.) Re-set your directory (LEAVE "CReport" Wherever your initial directory (oldWD) was.).
setwd(DIRECT)

#Double check:
getwd()


##5.) Save a script to THAT (above) folder:
script<-scripter(ReportFolder = OldWD, Document1_loc = Doc1, Document2_loc = Doc2)
#Write it out:
write.table(script,"script.txt", col.names = FALSE, row.names = FALSE, quote = FALSE)

####################################
### Compare files with CopyFind ####
####################################

#Run it.
run_copyfind(cleanup = T,"script")


######################################
### Send "matches.html" to a .csv ####
######################################
#Set directory to example "match.html" file
dir <- system.file("extdata", package="RCopyFind")

#Send data into dataframe
match_dat<- (html_to_df(file_name(dir)))

#Look at matches
View(match_dat@data)
