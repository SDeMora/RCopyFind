\name{plot.RCopy}
\alias{plot.RCopy}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
plot.RCopy}
\description{
%%  ~~ A concise (1-5 lines) description of what the function does. ~~
}
\usage{
plot.RCopy(x, count = TRUE, trim = FALSE, ...)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{x}{
%%     ~~Describe \code{x} here~~
}
  \item{count}{
%%     ~~Describe \code{count} here~~
}
  \item{trim}{
%%     ~~Describe \code{trim} here~~
}
  \item{\dots}{
%%     ~~Describe \code{\dots} here~~
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
Stephanie L. DeMora and Loren Collingwood}
\note{
%%  ~~further notes~~
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
%% ~~objects to See Also as \code{\link{help}}, ~~~
}
\examples{
dir <- system.file("extdata", package="RCopyFind")

match_dat<- (html_to_df(file_name(dir)))

plot(match_dat, cex=.4)

plot(match_dat, cex=.4, trim=T)

plot(match_dat, cex=.4, count=F)

plot(match_dat, cex=.4, count=F, trim=T)
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ ~kwd1 }% use one of  RShowDoc("KEYWORDS")
\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
