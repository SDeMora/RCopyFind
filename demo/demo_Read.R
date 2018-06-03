######################################
### Send "matches.html" to a .csv ####
######################################
#Set directory to example "match.html" file
dir <- system.file("extdata", package="RCopyFind")

#Send data into dataframe
match_dat<- (html_to_df(file_name(dir)))

#Look at matches
View(match_dat@data)
