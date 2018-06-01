scripter<-function(Documents=200,ReportFolder=getwd(),
                   Document1=1,Document1_loc,
                   Document2=2,Document2_loc,
                   PhraseLength=8,WordThreshold=6,SkipLength=20,MismatchTolerance=9,
                   MismatchPercentage=75,BriefReport=0,IgnoreCase=1,IgnoreNumber=1,
                   IgnoreOuterPunctuation=1,IgnorePunctuation=1,
                   SkipLongWords=1,Compare1=1,Compare2=2){
  arg1 <- paste("Documents,",Documents, sep="")
  arg2 <- paste("ReportFolder,",ReportFolder,sep="")
  arg3 <- paste("Document,",Document1,",",Document1_loc,sep="")
  arg4 <- paste("Document,",Document2,",",Document2_loc,sep="")
  arg5 <- paste("PhraseLength,",PhraseLength,sep="")
  arg6 <- paste("WordThreshold,",WordThreshold,sep="")
  arg7 <- paste("SkipLength,",SkipLength,sep="")
  arg8 <- paste("MismatchTolerance,",MismatchTolerance,sep="")
  arg9 <- paste("MismatchPercentage,",MismatchPercentage,sep="")
  arg10 <- paste("BriefReport,",BriefReport,sep="")
  arg11 <- paste("IgnoreCase,",IgnoreCase,sep="")
  arg12 <- paste("IgnoreNumber,",IgnoreNumber,sep="")
  arg13 <- paste("IgnoreOuterPunctuation,",IgnoreOuterPunctuation,sep="")
  arg14 <- paste("IgnorePunctuation,",IgnorePunctuation,sep="")
  arg15 <- paste("SkipLongWords,",SkipLongWords,sep="")
  arg16<- paste("Locale,English")
  arg17 <- paste("Compare,",Compare1,",",Compare2,sep="")
  script<-paste(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,
                arg10,arg11,arg12,arg13,arg14,arg15,arg16,
                "PrepareForComparisons",
                arg17,
                "Done",
                sep="\n")
  return(script)
}