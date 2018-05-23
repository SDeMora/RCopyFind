\name{file_name}
\alias{file_name}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
file_name}
\description{
%%  ~~ A concise (1-5 lines) description of what the function does. ~~
}
\usage{
file_name(dir)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{dir}{
%%     ~~Describe \code{dir} here~~
}
}
\details{
%%  ~~ If necessary, more details than the description above ~~
}
\value{
%%  ~Describe the value returned
%%  If it is a LIST, use
%%  \item{comp1 }{Description of 'comp1'}
%%  \item{comp2 }{Description of 'comp2'}
%% ...
}
\references{
%% ~put references to the literature/web site here ~
}
\author{
%%  Stephanie L. DeMora and Loren Collingwood
}
\note{
%%  ~~further notes~~
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
%% ~~objects to See Also as \code{\link{help}}, ~~~
}
\examples{
##Pull in the example data location at set it to "dir":
dir <- system.file("extdata", package="RCopyFind")

##Use the "file_name" function to list the appropraite file path for data conversion:
match_dat<-file_name(dir)

##Use the html_to_df function to convert the HTML code into a dataframe:
match_dat<- html_to_df(match_dat)

##Take a gander!
View(match_dat@data)

#Generate some plots:
plot(match_dat, cex=.4, trim=T)
plot(match_dat, cex=.4, count=F, trim=T)
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ ~kwd1 }% use one of  RShowDoc("KEYWORDS")
\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
