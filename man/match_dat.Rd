\name{match_dat}
\alias{match_dat}
\docType{data}
\title{
match_dat}
\description{
%%  ~~ A concise (1-5 lines) description of the dataset. ~~
}
\usage{data("match_dat")}
\format{
  A data frame with 0 observations on the following 0 variables.
  \describe{
  }
}
\details{
%%  ~~ If necessary, more details than the __description__ above ~~
}
\source{
%%  ~~ reference to a publication or URL from which the data were obtained ~~
}
\references{
%%  ~~ possibly secondary sources and usages ~~
}
\examples{
dir <- system.file("extdata", package="RCopyFind")
match_dat<- (html_to_df(file_name(dir)))
View(match_dat@data)
}
\keyword{datasets}
