\name{scripter}
\alias{scripter}
scripter
\title{
scripter}
\description{
Creates .txt file to run through CMD line based CopyFind.}
\usage{
scripter(Documents = 200, ReportFolder = getwd(), Document1 = 1, Document1_loc, Document2 = 2, Document2_loc, PhraseLength = 8, WordThreshold = 6, SkipLength = 20, MismatchTolerance = 9, MismatchPercentage = 75, BriefReport = 0, IgnoreCase = 1, IgnoreNumber = 1, IgnoreOuterPunctuation = 1, IgnorePunctuation = 1, SkipLongWords = 1, Compare1 = 1, Compare2 = 2)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{Documents}{
numeric. The number of files to be compared. Default = 200
}
  \item{ReportFolder}{
location. The file location for the reports from WCopyFind to print in. Default = working directory
}
  \item{Document1}{
numeric. The first comparison group number. This should remain at default = 1
}
  \item{Document1_loc}{
The location of the first comparison file or group. No default.
}
  \item{Document2}{
numeric. The second comparison group number. This should remain at default = 2
}
  \item{Document2_loc}{
The location of the second comparison file or group. No deffault.
}
  \item{PhraseLength}{
Set phrase Length. Default = 8
}
  \item{WordThreshold}{
Set Word Threshold. Default = 6
}
  \item{SkipLength}{
Skip Length Setting Default = 20
}
  \item{MismatchTolerance}{
Mismatch Tolerance. Default = 9
}
  \item{MismatchPercentage}{
Mismatch Percentage. Default = 75
}
  \item{BriefReport}{
Produces Brief Report (Yes = 1, No = 0). Default = 0
}
  \item{IgnoreCase}{
Ignore Case (Yes = 1, No = 0). Default = 0
}
  \item{IgnoreNumber}{
Ignore Number (Yes = 1, No = 0). Default = 0
}
  \item{IgnoreOuterPunctuation}{
Ignore Outer Punctuation (Yes = 1, No = 0). Default = 0
}
  \item{IgnorePunctuation}{
Ignore Punctuation (Yes = 1, No = 0). Default = 0
}
  \item{SkipLongWords}{
Skip Long Words (Yes = 1, No = 0). Default = 0
}
  \item{Compare1}{
First group to compare. Should remain at default = 1
}
  \item{Compare2}{
Second group to compare. Should remain at default = 2
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
script<-scripter()
write.table(script,"script.txt", col.names = FALSE, row.names = FALSE, quote = FALSE)
shell("Copyfind64.4.1.5.exe < script.txt")
  }
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ ~kwd1 }% use one of  RShowDoc("KEYWORDS")
\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
