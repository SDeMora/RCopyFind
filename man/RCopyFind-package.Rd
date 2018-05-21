\name{RCopyFind-package}
\alias{RCopyFind-package}
\alias{RCopyFind}
\docType{package}
\title{RCopyFind
\packageTitle{RCopyFind}
}
\description{
\packageDescription{RCopyFind}
}
\details{
\packageDESCRIPTION{RCopyFind}
\packageIndices{RCopyFind}
}
\author{
\packageAuthor{RCopyFind}

Maintainer: \packageMaintainer{RCopyFind}
}
\references{"Shoot First in ALEC's Castle: The Diffusion of Stand Your Ground Laws" (Collingwood, DeMora, and Ninci 2018)}

\keyword{ package }
\seealso{
www.StephanieDeMora.com, www.CollingwoodResearch.com
}
\examples{

match_dat <- html_to_df(file_name(dir)); head(match_dat@data)


# Plot, Defaults -- Overall Counts, No trim; set cex here
plot.RCopy(match_dat, cex=.4)

# Plot, Overall Counts, trimmed #
plot.RCopy(match_dat, cex=.4, trim=T)

# Plot, Overall Percent Match
plot.RCopy(match_dat, cex=.4, count=F)

# Plot, Overall Percent Match, trimmed
plot.RCopy(match_dat, cex=.4, count=F, trim=T)}

