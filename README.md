# RCopyFind

### Load package:
library(devtools)

install_github("SDeMora/RCopyFind")

library(RCopyFind)

#### Enter the directory ("...") where your HTML (WCopyFind output) is stored (for example download "data").

dir <- "..."

#### Run the following:
match_dat<- (html_to_df(file_name(dir))) ; head(match_dat@data)


#### Plot, Defaults -- Overall Counts, No trim; set cex here
plot(match_dat, cex=.4)

#### Plot, Overall Counts, trimmed #
plot(match_dat, cex=.4, trim=T)

#### Plot, Overall Percent Match
plot(match_dat, cex=.4, count=F)

#### Plot, Overall Percent Match, trimmed
plot(match_dat, cex=.4, count=F, trim=T)
