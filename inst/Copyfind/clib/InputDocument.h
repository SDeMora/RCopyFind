#pragma once

#define CONTENT_TYPE_NONE -1
#define CONTENT_TYPE_UNKNOWN 0
#define CONTENT_TYPE_HTML 1
#define CONTENT_TYPE_TXT 2
#define CONTENT_TYPE_DOCX 3
#define CONTENT_TYPE_DOC  4
#define CONTENT_TYPE_PDF  5
#define CONTENT_TYPE_URL  6

#define ERR_CANNOT_ACCESS_URL 101
#define ERR_NO_FILE_OPEN 102
#define ERR_CANNOT_FIND_FILE 103
#define ERR_CANNOT_FIND_FILE_EXTENSION 104
#define ERR_BAD_DOCX_FILE 105
#define ERR_BAD_PDF_FILE 106
#define ERR_CANNOT_FIND_URL_LINK 107
#define ERR_CANNOT_OPEN_INPUT_FILE 108

#define DEL_TYPE_NONE 0
#define DEL_TYPE_WHITE 1
#define DEL_TYPE_NEWLINE 2
#define DEL_TYPE_EOF 3

#define WORDMAXIMUMLENGTH 255
#define WORDBUFFERLENGTH 256

#define savedCharsLength 15 // number of saved characters for processing PDF streams

class CInputDocument
{
public:
	CInputDocument(void);
	~CInputDocument(void);
	int OpenDocument(CString szFilename);
	void CloseDocument();
	int GetWord(wchar_t *word,int &DelimiterType);

	bool m_bBasic_Characters;
	FILE *m_fLog;
	bool m_debug;

private:
	int m_contentType;
	bool m_haveFile;
	bool m_UTF8;
	bool m_eof;
	bool m_gotWord;
	bool m_gotDelimiter;
	bool m_gotChar;
	int m_char;
	FILE *m_filep;
	size_t m_fileLength;
	bool m_pdftotext;
	bool m_bInternet;

// for DOCX files
	int OpenDocx(wchar_t* filename);
	void CloseDocx();
	int GetCharDocx();
	int GetByteDocx();

	unzFile m_docxZipArchive;
	unsigned char m_docxByteBuffer[1000];
	int m_docxByteBufferLength;
	int m_ByteCountDocx;
	int m_ByteIndexDocx;

// for DOC files
	int OpenDoc(wchar_t* filename);
	void CloseDoc();
//	int GetCharDoc(); // not used
//	int GetByteDoc(); // not used

// for HTML files
	int OpenHtml(wchar_t* filename);
	void CloseHtml();
	int GetCharHtml();
	int GetByteHtml();

// for TXT files
	int OpenTxt(wchar_t* filename);
	void CloseTxt();
	int GetCharTxt();
	int GetByteTxt();

// for Unknown files
	int OpenUnknown(wchar_t* filename);
	void CloseUnknown();
//	int GetCharUnknown(); // not used
//	int GetByteUnknown(); // not used

// for PDF files
	int OpenPdf(wchar_t* filename);
	void ClosePdf();
	int GetCharPdf();
	int GetBytePdf();

	wchar_t m_pdftotextFile[256];
	char* m_ByteBufferPdf;
	char* m_fileBuffer;
	char* m_inflateBuffer;
	int m_ByteBufferLengthPdf;
	int m_ByteCountPdf;
	int m_BytePointerPdf;
	bool m_inTextObject;
	bool m_inLiteral;
	bool m_inParen;
	char m_savedChars[savedCharsLength]; 
	size_t FindStringInBufferPdf(char* haystack, char* needle, size_t haystackLength);
	void GetXPdf(const char * haystack, int lastCharOffset, float &X);
	void GetXYPdf(const char * haystack, int lastCharOffset, float &X, float &Y);
	bool FindTokenPdf(const char* needle, char* haystack);
	void PutBytePdf(char thischar);
	void ProcessStreamPdf(const char* inflateBuffer, size_t inflateLength);

// for URL files
	int OpenUrl(wchar_t* filename);
	CInternetSession* m_pSession;
	CHttpConnection* m_pServer;
	CHttpFile* m_pHttpFile;
};

