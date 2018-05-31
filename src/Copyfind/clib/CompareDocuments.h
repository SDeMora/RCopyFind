#define ERR_ABORT 0
#define ERR_CANNOT_OPEN_FILE 1
#define ERR_CANNOT_ALLOCATE_WORKING_HASH_ARRAY 2
#define ERR_CANNOT_ALLOCATE_HASH_ARRAY 3
#define ERR_CANNOT_ALLOCATE_SORTED_HASH_ARRAY 4
#define ERR_CANNOT_ALLOCATE_SORTED_NUMBER_ARRAY 5
#define ERR_CANNOT_OPEN_LOG_FILE 6
#define ERR_CANNOT_OPEN_COMPARISON_REPORT_TXT_FILE 7
#define ERR_CANNOT_OPEN_COMPARISON_REPORT_HTML_FILE 8
#define ERR_CANNOT_ALLOCATE_LEFT_MATCH_MARKERS 9
#define ERR_CANNOT_ALLOCATE_RIGHT_MATCH_MARKERS 10
#define ERR_CANNOT_ALLOCATE_LEFTA_MATCH_MARKERS 11
#define ERR_CANNOT_ALLOCATE_RIGHTA_MATCH_MARKERS 12
#define ERR_CANNOT_ALLOCATE_LEFTT_MATCH_MARKERS 13
#define ERR_CANNOT_ALLOCATE_RIGHTT_MATCH_MARKERS 14
#define ERR_CANNOT_OPEN_LEFT_HTML_FILE 15
#define ERR_CANNOT_OPEN_LEFT_DOCUMENT_FILE 16
#define ERR_CANNOT_OPEN_RIGHT_HTML_FILE 17
#define ERR_CANNOT_OPEN_RIGHT_DOCUMENT_FILE 18
#define ERR_CANNOT_OPEN_SIDE_BY_SIDE_HTML_FILE 19

#define WORD_UNMATCHED -1
#define WORD_PERFECT 0
#define WORD_FLAW 1
#define WORD_FILTERED 2

#define DOC_TYPE_UNDEFINED 0

class CCompareDocuments
{
public:
	CCompareDocuments(int ndocuments);
	~CCompareDocuments();

	struct Document
	{
		CString m_szDocumentName;					// an entry for the document name
		int m_DocumentType;							// type of document: old, new, etc.
		unsigned long *m_pWordHash;					// a pointer to the hash-coded word list
		unsigned long *m_pSortedWordHash;			// a pointer to the sorted hash-coded word list
		int *m_pSortedWordNumber;					// a pointer to the sorted word number list
		int m_WordsTotal;							// an entry for the number of words in the lists
		int m_FirstHash;							// an entry for the first word with more than 3 chars
	};

private:
	int DocumentToHtml(CInputDocument indoc, int *MatchMark, int *MatchAnchor, long words, wchar_t href[255]);
	int PercentMatching(int FirstL,int FirstR,int LastL,int LastR,int perfectmatches);
	void PrintWCharAsHtmlUTF8(FILE *outputFile,wchar_t wc);
public:
	int LoadDocument(Document *inDocument);
	int ComparePair(Document *DocL,Document *DocR);
	int ComparePairFiltered(Document *DocL,Document *DocR,Document *DocF);
	int SetupReports();
	void FinishReports();
	void SetupLoading();
	void FinishLoading();
	void SetupProgressReports(int Group1,int Group2,int Group3);
	int SetupComparisons();
	int ReportMatchedPair();
	void FinishComparisons();

	struct Document (*m_pDocs);	// the pointer to all the documents
	struct Document (*m_pDocL); // pointer to the Left document
	struct Document (*m_pDocR); // pointer to the right document
	int m_Documents; // number of documents
	CString m_szDocL;		// name of left document
	CString m_szDocR;		// name of right document
private:
	FILE *m_fMatch;							// handle for comparisons that exceed threshold (output)
	FILE *m_fMatchHtml;						// handle for comparisons that exceed threshold (output) - html
	FILE *m_fHtml;							// handle for output html files
	FILE *m_fLog;							// handle for the log file
	bool m_debug;							// flag to include debug output in log file
	int *m_MatchMarkL,*m_MatchMarkR;		// left and right matched word markup list pointers
	int *m_MatchAnchorL,*m_MatchAnchorR;	// left and right matched string anchors for html files
	int *m_MatchMarkTempL,*m_MatchMarkTempR;// left and right matched word  markup list pointers - temporary
	unsigned long *m_pQWordHash;			// a pointer to a working hash-coded word list
	unsigned long *m_pXWordHash;			// a pointer to a temporary hash-coded word list
	int m_WordsAllocated;					// number of words allocated in the many word-related arrays
public:
	int m_MatchingWordsPerfect;				// count of words that are perfect matches
	int m_MatchingWordsTotalL;				// count of words in left document that are in matching phrases, whether each word is a perfect match or not
	int m_MatchingWordsTotalR;				// count of words in right document that are in matching phrases, whether each word is a perfect match or not
	int m_MatchingDocumentPairs;			// count of matching pairs of documents

	long long m_Compares;					// count of comparisons done
	long long m_TotalCompares;				// total number of comparisons to do
	long long m_CompareStep;				// reporting interval
	clock_t m_StartTicks;					// starting tick for CPU time determination
	float m_Time;							// cpu time used

	int m_PhraseLength;
	int m_FilteredPhraseLength;
	int m_FilterPhraseLength;
	int m_WordThreshold;
	bool m_bIgnoreCase;
	bool m_bIgnoreNumbers;
	bool m_bIgnorePunctuation;
	bool m_bIgnoreOuterPunctuation;
	bool m_bSkipLongWords;
	bool m_bSkipNonwords;
	bool m_bBasic_Characters;
	size_t m_SkipLength;
	int m_MismatchTolerance;
	int m_MismatchPercentage;
	bool m_bBriefReport;

	CString m_szReportFolder;
	CString m_szSoftwareName;
};
