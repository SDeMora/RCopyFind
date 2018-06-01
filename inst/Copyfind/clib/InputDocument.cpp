#include "..\stdafx.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <afxinet.h>
#include <wininet.h>
#include "..\zlib\zlib.h"
#include "..\zlib\zconf.h"
#include "..\zlib\unzip.h"
#include "inputDocument.h"
#include "Words.h"
using namespace std;

CInputDocument::CInputDocument(void)
{
// check if the pdftotext.exe conversion program is present in the same directory as WCopyfind.exe
	wchar_t directoryName[256];
	GetCurrentDirectory(256,directoryName);
	wcscpy_s(m_pdftotextFile,256,directoryName);
	wcscat_s(m_pdftotextFile,256,L"\\pdftotext.exe");

	// Check if pdftotext.exe exists and get its length
	struct _stat statBuffer;

	if( _wstat(m_pdftotextFile,&statBuffer) != 0 ) m_pdftotext = false;
	else m_pdftotext = true;

	m_docxZipArchive=NULL;
	m_docxByteBufferLength=1000;

	m_fileBuffer=NULL;
	m_inflateBuffer=NULL;
	m_ByteBufferPdf=NULL;

	m_contentType=CONTENT_TYPE_NONE;
	m_haveFile = false;
	m_UTF8 = false;
	m_eof = false;
	m_gotChar = false;
	m_gotWord = false;
	m_gotDelimiter = false;
	m_bInternet = false;

	m_filep=NULL;
}

CInputDocument::~CInputDocument(void)
{
}

int CInputDocument::OpenDocument(CString szFilename)
{
	// Check that file exists and get its length
	struct _stat statBuffer;

	wchar_t* pFilename = szFilename.GetBuffer(0);
//	szFilename.ReleaseBuffer(); // should be release when this method returns

	if( _wstat(pFilename,&statBuffer) != 0 ) return ERR_CANNOT_FIND_FILE; // open fails if file is not found
	m_fileLength = statBuffer.st_size;

	wchar_t *pstr;
	pstr=wcsrchr(pFilename,0x2E); // find filename extention
	if(pstr == NULL) return ERR_CANNOT_FIND_FILE_EXTENSION; // open fails if there is no file extension
	pstr++;

	if( (_wcsnicmp(pstr,L"htm",3) == 0 ) || (_wcsnicmp(pstr,L"html",4) == 0 ) ) m_contentType=CONTENT_TYPE_HTML;
	else if ( _wcsnicmp(pstr,L"docx",4) == 0 ) m_contentType=CONTENT_TYPE_DOCX;
	else if ( _wcsnicmp(pstr,L"doc",3) == 0 ) m_contentType=CONTENT_TYPE_DOC;
	else if ( _wcsnicmp(pstr,L"txt",3) == 0 ) m_contentType=CONTENT_TYPE_TXT;
	else if ( _wcsnicmp(pstr,L"pdf",3) == 0 ) m_contentType=CONTENT_TYPE_PDF;
	else if ( _wcsnicmp(pstr,L"url",3) == 0 ) m_contentType=CONTENT_TYPE_URL;
	else m_contentType=CONTENT_TYPE_UNKNOWN;

	if(m_contentType == CONTENT_TYPE_DOCX) return OpenDocx(pFilename);
	else if( m_contentType == CONTENT_TYPE_HTML ) return OpenHtml(pFilename);
	else if( m_contentType == CONTENT_TYPE_TXT ) return OpenTxt(pFilename);
	else if( m_contentType == CONTENT_TYPE_DOC ) return OpenDoc(pFilename);
	else if( m_contentType == CONTENT_TYPE_PDF ) return OpenPdf(pFilename);
	else if( m_contentType == CONTENT_TYPE_URL ) return OpenUrl(pFilename);
	else return OpenUnknown(pFilename);;
}

void CInputDocument::CloseDocument()
{
	if(m_filep != NULL) { // don't try to close a file that was never successfully opened
		if(m_contentType == CONTENT_TYPE_DOCX) CloseDocx();
		else if(m_contentType == CONTENT_TYPE_HTML) CloseHtml();
		else if(m_contentType == CONTENT_TYPE_DOC) CloseDoc();
		else if( m_contentType == CONTENT_TYPE_PDF) ClosePdf();
		else if( m_contentType == CONTENT_TYPE_TXT) CloseTxt();
		else if( m_contentType == CONTENT_TYPE_URL) {}	// should never be called, since a URL should become something else
	}

	m_contentType=CONTENT_TYPE_NONE;
	m_haveFile = false;
	m_UTF8 = false;
	m_eof = false;
	m_gotChar = false;
	m_gotWord = false;
	m_gotDelimiter = false;

	m_filep=NULL;
}

int CInputDocument::GetWord(wchar_t *word,int &DelimiterType)
{
	if(!m_haveFile) return ERR_NO_FILE_OPEN; // if no file is open, failure

	word[0]=0; // start with empty word, in case we encounter EOF before we encounter a word
	int wordLength = 0;
	m_gotWord = false;
	m_gotDelimiter = false;
	DelimiterType = DEL_TYPE_NONE;

	if(m_contentType == CONTENT_TYPE_DOCX)
	{
		while(true)
		{
			if(m_gotChar) m_gotChar = false; // check to see if we already have the next character
			else m_char=GetCharDocx();
			
			if(m_char < 0) // check for EOF encountered
			{
				word[wordLength]=0; // finish the word off
				DelimiterType = DEL_TYPE_EOF;
				return -1;
			}
			else if(m_char == '<') // check for '<' character
			{
				CStringW tagName = L"";
				bool bneedTagName = true;

				while(true) // read in the entire tag, saving the tag name
				{
					m_char=GetCharDocx();
					if(m_char < 0) // check for EOF encountered
					{
						word[wordLength]=0; // finsh the word off;
						DelimiterType = DEL_TYPE_EOF;
						return -1;
					}
					else if(m_char == '>') break; // found '>' (end of tag), so stop scanning it
					else if(m_char == ' ') bneedTagName = false; // found ' ' (end of tag name), so stop gathering name
					else if(bneedTagName) tagName += (wchar_t)m_char;
				}
				if( tagName.CompareNoCase(L"w:p" ) == 0) // check for a paragraph tag
				{
					DelimiterType=DEL_TYPE_NEWLINE;
					m_gotDelimiter=true;
				}
				else if( tagName.CompareNoCase(L"w:tab/" ) == 0) // check for a tab tag
				{
					DelimiterType=max(DelimiterType,DEL_TYPE_WHITE); // if delimiter isn't already at NEWLINE, set it to WHITE
					m_gotDelimiter=true;
				}
				continue; // otherwise keep reading
			}
			else if(iswspace(m_char)) // check for white space
			{
				DelimiterType=max(DelimiterType,DEL_TYPE_WHITE); // if delimiter isn't already at NEWLINE, set it to WHITE
				m_gotDelimiter=true;
			}
			else if(iswcntrl(m_char)) continue; // skip other control characters
			else if(m_gotDelimiter) // have we just reached the end of one or more delimiters?
			{
				if(m_gotWord) // make sure that we have a word
				{
					word[wordLength]=0; // finish the word off
					m_gotChar=true;
					return -1;
				}
				else // these were preliminary delimiters and we will ignore them
				{
					DelimiterType=DEL_TYPE_NONE;
					m_gotDelimiter=false;
					m_gotChar=true;
				}
			}
			else if(m_char == '&') // skip and ignore &xxx; codes.
			{
				wchar_t ampBuffer[256];
				int ampBufferCount = 0;
				while(true)
				{
					m_char=GetCharDocx(); // read the next character
					if( (!iswalnum(m_char)) && (m_char != '#')) break; // keep reading until we hit a non-alphanumeric, which should be a ';')
					if (m_char < 0) break; // we encountered an EOF before the end of the & code
					if(ampBufferCount < 255)
					{
						ampBuffer[ampBufferCount] = m_char;
						ampBufferCount++;
					}
				}
				ampBuffer[ampBufferCount] = 0; // finish off the string
				if(ampBuffer[0] == '#')
				{
					if(ampBuffer[1] == 'x') swscanf_s(ampBuffer+2,L"%x",&m_char); // read in the hexidecial number directly
					else swscanf_s(ampBuffer+1,L"%d",&m_char); // read in the character number directly
				}

				else if(wcscmp(ampBuffer,L"trade") == 0) m_char = 153;
				else if(wcscmp(ampBuffer,L"nbsp") == 0) m_char = 160;
				else if(wcscmp(ampBuffer,L"iexcl") == 0) m_char = 161;
				else if(wcscmp(ampBuffer,L"cent") == 0) m_char = 162;
				else if(wcscmp(ampBuffer,L"pound") == 0) m_char = 163;
				else if(wcscmp(ampBuffer,L"yen") == 0) m_char = 165;
				else if(wcscmp(ampBuffer,L"sect") == 0) m_char = 167;
				else if(wcscmp(ampBuffer,L"copy") == 0) m_char = 169;
				else if(wcscmp(ampBuffer,L"laquo ") == 0) m_char = 171;
				else if(wcscmp(ampBuffer,L"reg") == 0) m_char = 174;
				else if(wcscmp(ampBuffer,L"plusmn") == 0) m_char = 177;
				else if(wcscmp(ampBuffer,L"micro") == 0) m_char = 181;
				else if(wcscmp(ampBuffer,L"para") == 0) m_char = 182;
				else if(wcscmp(ampBuffer,L"middot") == 0) m_char = 183;
				else if(wcscmp(ampBuffer,L"raquo") == 0) m_char = 187;
				else if(wcscmp(ampBuffer,L"iquest") == 0) m_char = 191;
				else if(wcscmp(ampBuffer,L"Agrave") == 0) m_char = 192;
				else if(wcscmp(ampBuffer,L"Aacute") == 0) m_char = 193;
				else if(wcscmp(ampBuffer,L"Acirc") == 0) m_char = 194;
				else if(wcscmp(ampBuffer,L"Atilde") == 0) m_char = 195;
				else if(wcscmp(ampBuffer,L"Auml") == 0) m_char = 196;
				else if(wcscmp(ampBuffer,L"Aring") == 0) m_char = 197;
				else if(wcscmp(ampBuffer,L"AElig") == 0) m_char = 198;
				else if(wcscmp(ampBuffer,L"Ccedil") == 0) m_char = 199;
				else if(wcscmp(ampBuffer,L"Egrave") == 0) m_char = 200;
				else if(wcscmp(ampBuffer,L"Eacute") == 0) m_char = 201;
				else if(wcscmp(ampBuffer,L"Ecirc") == 0) m_char = 202;
				else if(wcscmp(ampBuffer,L"Euml") == 0) m_char = 203;
				else if(wcscmp(ampBuffer,L"Igrave") == 0) m_char = 204;
				else if(wcscmp(ampBuffer,L"Iacute") == 0) m_char = 205;
				else if(wcscmp(ampBuffer,L"Icirc") == 0) m_char = 206;
				else if(wcscmp(ampBuffer,L"Iuml") == 0) m_char = 207;
				else if(wcscmp(ampBuffer,L"Ntilde") == 0) m_char = 209;
				else if(wcscmp(ampBuffer,L"Ograve") == 0) m_char = 210;
				else if(wcscmp(ampBuffer,L"Oacute") == 0) m_char = 211;
				else if(wcscmp(ampBuffer,L"Ocirc") == 0) m_char = 212;
				else if(wcscmp(ampBuffer,L"Otilde") == 0) m_char = 213;
				else if(wcscmp(ampBuffer,L"Ouml") == 0) m_char = 214;
				else if(wcscmp(ampBuffer,L"Oslash") == 0) m_char = 216;
				else if(wcscmp(ampBuffer,L"Ugrave") == 0) m_char = 217;
				else if(wcscmp(ampBuffer,L"Uacute") == 0) m_char = 218;
				else if(wcscmp(ampBuffer,L"Ucirc") == 0) m_char = 219;
				else if(wcscmp(ampBuffer,L"Uuml") == 0) m_char = 220;
				else if(wcscmp(ampBuffer,L"szlig") == 0) m_char = 223;
				else if(wcscmp(ampBuffer,L"agrave ") == 0) m_char = 224;
				else if(wcscmp(ampBuffer,L"aacute ") == 0) m_char = 225;
				else if(wcscmp(ampBuffer,L"acirc ") == 0) m_char = 226;
				else if(wcscmp(ampBuffer,L"atilde ") == 0) m_char = 227;
				else if(wcscmp(ampBuffer,L"auml ") == 0) m_char = 228;
				else if(wcscmp(ampBuffer,L"aring ") == 0) m_char = 229;
				else if(wcscmp(ampBuffer,L"aelig ") == 0) m_char = 230;
				else if(wcscmp(ampBuffer,L"ccedil ") == 0) m_char = 231;
				else if(wcscmp(ampBuffer,L"egrave ") == 0) m_char = 232;
				else if(wcscmp(ampBuffer,L"eacute ") == 0) m_char = 233;
				else if(wcscmp(ampBuffer,L"ecirc ") == 0) m_char = 234;
				else if(wcscmp(ampBuffer,L"euml ") == 0) m_char = 235;
				else if(wcscmp(ampBuffer,L"igrave ") == 0) m_char = 236;
				else if(wcscmp(ampBuffer,L"iacute ") == 0) m_char = 237;
				else if(wcscmp(ampBuffer,L"icirc ") == 0) m_char = 238;
				else if(wcscmp(ampBuffer,L"iuml ") == 0) m_char = 239;
				else if(wcscmp(ampBuffer,L"ntilde ") == 0) m_char = 241;
				else if(wcscmp(ampBuffer,L"ograve ") == 0) m_char = 242;
				else if(wcscmp(ampBuffer,L"oacute ") == 0) m_char = 243;
				else if(wcscmp(ampBuffer,L"ocirc ") == 0) m_char = 244;
				else if(wcscmp(ampBuffer,L"otilde ") == 0) m_char = 245;
				else if(wcscmp(ampBuffer,L"ouml ") == 0) m_char = 246;
				else if(wcscmp(ampBuffer,L"divide") == 0) m_char = 247;
				else if(wcscmp(ampBuffer,L"oslash ") == 0) m_char = 248;
				else if(wcscmp(ampBuffer,L"ugrave ") == 0) m_char = 249;
				else if(wcscmp(ampBuffer,L"uacute ") == 0) m_char = 250;
				else if(wcscmp(ampBuffer,L"ucirc ") == 0) m_char = 251;
				else if(wcscmp(ampBuffer,L"uuml ") == 0) m_char = 252;
				else if(wcscmp(ampBuffer,L"yuml") == 0) m_char = 255;
				else if(wcscmp(ampBuffer,L"quot") == 0) m_char = 34;
				else if(wcscmp(ampBuffer,L"amp") == 0) m_char = 38;
				else if(wcscmp(ampBuffer,L"lt") == 0) m_char = 60;
				else if(wcscmp(ampBuffer,L"gt") == 0) m_char = 62;
				else if(wcscmp(ampBuffer,L"ndash") == 0) m_char = 8211;
				else if(wcscmp(ampBuffer,L"mdash") == 0) m_char = 8212;
				else if(wcscmp(ampBuffer,L"lsquo") == 0) m_char = 8216;
				else if(wcscmp(ampBuffer,L"rsquo") == 0) m_char = 8217;
				else if(wcscmp(ampBuffer,L"ldquo") == 0) m_char = 8220;
				else if(wcscmp(ampBuffer,L"rdquo") == 0) m_char = 8221;
				else if(wcscmp(ampBuffer,L"euro") == 0) m_char = 8364;
				else continue; // if we couldn't figure out what this character is, skip it

				if(wordLength < WORDMAXIMUMLENGTH)
				{
					word[wordLength]=m_char; // add this character to the word
					wordLength++;
					m_gotWord=true;
				}
			}			
			else if(wordLength < WORDMAXIMUMLENGTH)
			{
				word[wordLength]=m_char; // add this character to the word
				wordLength++;
				m_gotWord=true;
			}
		}
	}

	else if(m_contentType == CONTENT_TYPE_HTML)
	{
		bool binScript = false;
		bool binStyle = false;
		bool binHeader = false;
		
		while(true)
		{
			if(m_gotChar) m_gotChar = false; // check to see if we already have the next character
			else
			{
				m_char=GetCharHtml(); // get a character
			}

			if(m_char < 0) // check for EOF encountered
			{
				word[wordLength]=0; // finish the word off
				DelimiterType = DEL_TYPE_EOF;
				return -1;
			}
			if(m_char == '<') // check for "<" character
			{
				CStringW tagName = L"";
				bool bneedTagName = true;
				CStringW tagBody = L"";

				while(true) // read in the entire tag, saving the tag name
				{
					m_char=GetCharHtml();
					if(m_char < 0) // check for EOF encountered
					{
						word[wordLength]=0; // finsh the word off;
						DelimiterType = DEL_TYPE_EOF;
						return -1;
					}
					else if(m_char == '>') break; // found ">" (end of tag), so stop scanning it
					else if(m_char == ' ') bneedTagName = false; // found " " (end of tag name), so stop gathering name
					else if(bneedTagName) tagName += (wchar_t)m_char; // save tag name
					else tagBody += (wchar_t)m_char; // save tag body
					if( tagName.Compare(L"!--") == 0 ) // are we starting a comment?
					{
						int dashes=0;
						while(true)
						{
							m_char=GetCharHtml(); // read the next character
							if( (m_char == '>') && (dashes > 1) ) break; // we found the end of the comment
							if( m_char < 0) break; // file ended without a completion of the comment
							else if( m_char == '-') dashes++; // we found a dash, so increment the count
							else dashes = 0; // not a dash, so return the dash count to zero
						}
						break;
					}
				}
				if( ( tagName.CompareNoCase(L"P") == 0 ) || // look for any tag that should trigger a new line
					( tagName.CompareNoCase(L"BR") == 0 ) ||
					( tagName.CompareNoCase(L"BR/") == 0 ) ||
					( tagName.CompareNoCase(L"UL") == 0 ) ||
					( tagName.CompareNoCase(L"TD") == 0 ) )
				{
					DelimiterType=DEL_TYPE_NEWLINE;
					m_gotDelimiter=true;
				}
				else if( tagName.CompareNoCase(L"SCRIPT") == 0 ) binScript = true; // starting a script, which we need to ignore
				else if( tagName.CompareNoCase(L"/SCRIPT") == 0 ) binScript = false; // ending a script
				else if( tagName.CompareNoCase(L"STYLE") == 0 ) binStyle = true; // starting a style, which we need to ignore
				else if( tagName.CompareNoCase(L"/STYLE") == 0 ) binStyle = false; // ending a style
				else if( tagName.CompareNoCase(L"HEAD") == 0 ) binHeader = true; // starting a style, which we need to ignore
				else if( tagName.CompareNoCase(L"/HEAD") == 0 ) binHeader = false; // ending a style
				else if( tagName.CompareNoCase(L"META") == 0 )
				{
					// we have a meta tag, which might change our character set
					tagBody.MakeLower(); // make the tag body all lower case
					if(tagBody.Find(L"charset") > 0) // look for the word charset
					{
						if(tagBody.Find(L"utf-8") > 0) m_UTF8 = true; // look for utf-8
						else m_UTF8 = false; // otherwise we're in another charset and can't use the utf-8 decoding
					}
				}
				continue; // otherwise keep reading
			}
			if(binScript || binStyle || binHeader) continue; // skip if we're in the middle of script or style or header
			else if(iswspace(m_char)) // check for white space
			{
				DelimiterType=max(DelimiterType,DEL_TYPE_WHITE); // if delimiter isn't already at NEWLINE, set it to WHITE
				m_gotDelimiter=true;
			}
			else if(iswcntrl(m_char)) continue; // skip other control characters
			else if(m_gotDelimiter) // have we just reached the end of one or more delimiters?
			{
				if(m_gotWord) // make sure that we have a word
				{
					word[wordLength]=0; // finish the word off
					m_gotChar=true;
					return -1;
				}
				else // these were preliminary delimiters and we will ignore them
				{
					DelimiterType=DEL_TYPE_NONE;
					m_gotDelimiter=false;
					m_gotChar=true;
				}
			}
			else if(m_char == '&') // skip and ignore &xxx; codes.
			{
				wchar_t ampBuffer[256];
				int ampBufferCount = 0;
				while(true)
				{
					m_char=GetCharHtml(); // read the next character
					if( (!iswalnum(m_char)) && (m_char != '#')) break; // keep reading until we hit a non-alphanumeric, which should be a ';')
					if( m_char < 0) break; // we hit an EOF before the end of the & code
					if(ampBufferCount < 255)
					{
						ampBuffer[ampBufferCount] = m_char;
						ampBufferCount++;
					}
				}
				ampBuffer[ampBufferCount] = 0; // finish off the string
				if(ampBuffer[0] == '#')
				{
					if(ampBuffer[1] == 'x') swscanf_s(ampBuffer+2,L"%x",&m_char); // read in the hexidecial number directly
					else swscanf_s(ampBuffer+1,L"%d",&m_char); // read in the character number directly
				}

				else if(wcscmp(ampBuffer,L"trade") == 0) m_char = 153;
				else if(wcscmp(ampBuffer,L"nbsp") == 0) m_char = 160;
				else if(wcscmp(ampBuffer,L"iexcl") == 0) m_char = 161;
				else if(wcscmp(ampBuffer,L"cent") == 0) m_char = 162;
				else if(wcscmp(ampBuffer,L"pound") == 0) m_char = 163;
				else if(wcscmp(ampBuffer,L"yen") == 0) m_char = 165;
				else if(wcscmp(ampBuffer,L"sect") == 0) m_char = 167;
				else if(wcscmp(ampBuffer,L"copy") == 0) m_char = 169;
				else if(wcscmp(ampBuffer,L"laquo ") == 0) m_char = 171;
				else if(wcscmp(ampBuffer,L"reg") == 0) m_char = 174;
				else if(wcscmp(ampBuffer,L"plusmn") == 0) m_char = 177;
				else if(wcscmp(ampBuffer,L"micro") == 0) m_char = 181;
				else if(wcscmp(ampBuffer,L"para") == 0) m_char = 182;
				else if(wcscmp(ampBuffer,L"middot") == 0) m_char = 183;
				else if(wcscmp(ampBuffer,L"raquo") == 0) m_char = 187;
				else if(wcscmp(ampBuffer,L"iquest") == 0) m_char = 191;
				else if(wcscmp(ampBuffer,L"Agrave") == 0) m_char = 192;
				else if(wcscmp(ampBuffer,L"Aacute") == 0) m_char = 193;
				else if(wcscmp(ampBuffer,L"Acirc") == 0) m_char = 194;
				else if(wcscmp(ampBuffer,L"Atilde") == 0) m_char = 195;
				else if(wcscmp(ampBuffer,L"Auml") == 0) m_char = 196;
				else if(wcscmp(ampBuffer,L"Aring") == 0) m_char = 197;
				else if(wcscmp(ampBuffer,L"AElig") == 0) m_char = 198;
				else if(wcscmp(ampBuffer,L"Ccedil") == 0) m_char = 199;
				else if(wcscmp(ampBuffer,L"Egrave") == 0) m_char = 200;
				else if(wcscmp(ampBuffer,L"Eacute") == 0) m_char = 201;
				else if(wcscmp(ampBuffer,L"Ecirc") == 0) m_char = 202;
				else if(wcscmp(ampBuffer,L"Euml") == 0) m_char = 203;
				else if(wcscmp(ampBuffer,L"Igrave") == 0) m_char = 204;
				else if(wcscmp(ampBuffer,L"Iacute") == 0) m_char = 205;
				else if(wcscmp(ampBuffer,L"Icirc") == 0) m_char = 206;
				else if(wcscmp(ampBuffer,L"Iuml") == 0) m_char = 207;
				else if(wcscmp(ampBuffer,L"Ntilde") == 0) m_char = 209;
				else if(wcscmp(ampBuffer,L"Ograve") == 0) m_char = 210;
				else if(wcscmp(ampBuffer,L"Oacute") == 0) m_char = 211;
				else if(wcscmp(ampBuffer,L"Ocirc") == 0) m_char = 212;
				else if(wcscmp(ampBuffer,L"Otilde") == 0) m_char = 213;
				else if(wcscmp(ampBuffer,L"Ouml") == 0) m_char = 214;
				else if(wcscmp(ampBuffer,L"Oslash") == 0) m_char = 216;
				else if(wcscmp(ampBuffer,L"Ugrave") == 0) m_char = 217;
				else if(wcscmp(ampBuffer,L"Uacute") == 0) m_char = 218;
				else if(wcscmp(ampBuffer,L"Ucirc") == 0) m_char = 219;
				else if(wcscmp(ampBuffer,L"Uuml") == 0) m_char = 220;
				else if(wcscmp(ampBuffer,L"szlig") == 0) m_char = 223;
				else if(wcscmp(ampBuffer,L"agrave ") == 0) m_char = 224;
				else if(wcscmp(ampBuffer,L"aacute ") == 0) m_char = 225;
				else if(wcscmp(ampBuffer,L"acirc ") == 0) m_char = 226;
				else if(wcscmp(ampBuffer,L"atilde ") == 0) m_char = 227;
				else if(wcscmp(ampBuffer,L"auml ") == 0) m_char = 228;
				else if(wcscmp(ampBuffer,L"aring ") == 0) m_char = 229;
				else if(wcscmp(ampBuffer,L"aelig ") == 0) m_char = 230;
				else if(wcscmp(ampBuffer,L"ccedil ") == 0) m_char = 231;
				else if(wcscmp(ampBuffer,L"egrave ") == 0) m_char = 232;
				else if(wcscmp(ampBuffer,L"eacute ") == 0) m_char = 233;
				else if(wcscmp(ampBuffer,L"ecirc ") == 0) m_char = 234;
				else if(wcscmp(ampBuffer,L"euml ") == 0) m_char = 235;
				else if(wcscmp(ampBuffer,L"igrave ") == 0) m_char = 236;
				else if(wcscmp(ampBuffer,L"iacute ") == 0) m_char = 237;
				else if(wcscmp(ampBuffer,L"icirc ") == 0) m_char = 238;
				else if(wcscmp(ampBuffer,L"iuml ") == 0) m_char = 239;
				else if(wcscmp(ampBuffer,L"ntilde ") == 0) m_char = 241;
				else if(wcscmp(ampBuffer,L"ograve ") == 0) m_char = 242;
				else if(wcscmp(ampBuffer,L"oacute ") == 0) m_char = 243;
				else if(wcscmp(ampBuffer,L"ocirc ") == 0) m_char = 244;
				else if(wcscmp(ampBuffer,L"otilde ") == 0) m_char = 245;
				else if(wcscmp(ampBuffer,L"ouml ") == 0) m_char = 246;
				else if(wcscmp(ampBuffer,L"divide") == 0) m_char = 247;
				else if(wcscmp(ampBuffer,L"oslash ") == 0) m_char = 248;
				else if(wcscmp(ampBuffer,L"ugrave ") == 0) m_char = 249;
				else if(wcscmp(ampBuffer,L"uacute ") == 0) m_char = 250;
				else if(wcscmp(ampBuffer,L"ucirc ") == 0) m_char = 251;
				else if(wcscmp(ampBuffer,L"uuml ") == 0) m_char = 252;
				else if(wcscmp(ampBuffer,L"yuml") == 0) m_char = 255;
				else if(wcscmp(ampBuffer,L"quot") == 0) m_char = 34;
				else if(wcscmp(ampBuffer,L"amp") == 0) m_char = 38;
				else if(wcscmp(ampBuffer,L"lt") == 0) m_char = 60;
				else if(wcscmp(ampBuffer,L"gt") == 0) m_char = 62;
				else if(wcscmp(ampBuffer,L"ndash") == 0) m_char = 8211;
				else if(wcscmp(ampBuffer,L"mdash") == 0) m_char = 8212;
				else if(wcscmp(ampBuffer,L"lsquo") == 0) m_char = 8216;
				else if(wcscmp(ampBuffer,L"rsquo") == 0) m_char = 8217;
				else if(wcscmp(ampBuffer,L"ldquo") == 0) m_char = 8220;
				else if(wcscmp(ampBuffer,L"rdquo") == 0) m_char = 8221;
				else if(wcscmp(ampBuffer,L"euro") == 0) m_char = 8364;
				else continue; // if we couldn't figure out what this character is, skip it

				if(wordLength < WORDMAXIMUMLENGTH)
				{
					word[wordLength]=m_char; // add this character to the word
					wordLength++;
					m_gotWord=true;
				}
			}
			else if(wordLength < WORDMAXIMUMLENGTH)
			{
				word[wordLength]=m_char; // add this character to the word
				wordLength++;
				m_gotWord=true;
			}
		}
	}
	else if( m_contentType == CONTENT_TYPE_TXT )
	{
		while(true)
		{
			if(m_gotChar) m_gotChar = false; // check to see if we already have the next character
			else m_char=GetCharTxt(); // otherwise, get the next character (normal or UTF-8)
			
			if(m_char < 0) // check for EOF encountered
			{
				word[wordLength]=0; // finish the word off
				DelimiterType = DEL_TYPE_EOF;
				return -1;
			}
			else if( (m_char == '\n') || (m_char == '\r') ) // check for newline characters
			{
				DelimiterType=DEL_TYPE_NEWLINE;
				m_gotDelimiter=true;
			}
			else if(iswspace(m_char)) // check for white space
			{
				DelimiterType=max(DelimiterType,DEL_TYPE_WHITE); // if delimiter isn't already at NEWLINE, set it to WHITE
				m_gotDelimiter=true;
			}
			else if( (iswcntrl(m_char) && m_char < 0x80) || (m_char == 0xff) ) continue; // skip any other control characters
			else if(m_gotDelimiter) // have we just reached the end of one or more delimiters?
			{
				if(m_gotWord) // make sure that we have a word
				{
					word[wordLength]=0; // finish the word off
					m_gotChar=true;
					return -1;
				}
				else // these were preliminary delimiters and we will ignore them
				{
					DelimiterType=DEL_TYPE_NONE;
					m_gotDelimiter=false;
					m_gotChar=true;
				}
			}
			else if(wordLength < WORDMAXIMUMLENGTH)
			{
				word[wordLength]=m_char; // add this character to the word
				wordLength++;
				m_gotWord=true;
			}
		}
	}
	else if(m_contentType == CONTENT_TYPE_DOC)
	{
		while(true)
		{
			if(m_gotChar) m_gotChar = false; // check to see if we already have the next character
			else m_char=fgetc(m_filep);
			
			if(m_char == 0) m_char=fgetc(m_filep); // skip a single null character (but not multiple nulls)
			
			if(m_char >= 0x80)	// convert extended ISO8559-1 characters into appropriate unicode characters
			{
				switch( m_char )
				{
				case 128: m_char = 8364; break;
				case 129: m_char = 32; break;
				case 130: m_char = 8218; break;
				case 131: m_char = 402; break;
				case 132: m_char = 8222; break;
				case 133: m_char = 8230; break;
				case 134: m_char = 8224; break;
				case 135: m_char = 8225; break;
				case 136: m_char = 170; break;
				case 137: m_char = 8240; break;
				case 138: m_char = 353; break;
				case 139: m_char = 8249; break;
				case 140: m_char = 339; break;
				case 141: m_char = 32; break;
				case 142: m_char = 381; break;
				case 143: m_char = 32; break;
				case 144: m_char = 32; break;
				case 145: m_char = 8216; break;
				case 146: m_char = 8217; break;
				case 147: m_char = 8220; break;
				case 148: m_char = 8221; break;
				case 149: m_char = 8226; break;
				case 150: m_char = 8211; break;
				case 151: m_char = 8212; break;
				case 152: m_char = 732; break;
				case 153: m_char = 8482; break;
				case 154: m_char = 353; break;
				case 155: m_char = 8250; break;
				case 156: m_char = 339; break;
				case 157: m_char = 32; break;
				case 158: m_char = 382; break;
				case 159: m_char = 376; break;
				}
			}
			else if(m_char < 0) // check for EOF encountered
			{
				word[wordLength]=0; // finish the word off
				DelimiterType = DEL_TYPE_EOF;
				return -1;
			}
			else if( (m_char == '\n') || (m_char == '\r')) // check for newline characters
			{
				DelimiterType=DEL_TYPE_NEWLINE;
				m_gotDelimiter=true;
			}
			else if( (m_char == '\t') || (m_char == ' ')) // check for tab or space characters
			{
				DelimiterType=max(DelimiterType,DEL_TYPE_WHITE); // if delimiter isn't already at NEWLINE, set it to WHITE
				m_gotDelimiter=true;
			}
			else if ( iswcntrl(m_char) && (m_char < 0x80) ) // if we encounter a control character, restart the word search
			{
				word[0]=0; // start with empty word, in case we encounter EOF before we encounter a word
				wordLength = 0;
				m_gotWord = false;
				m_gotDelimiter = false;
				DelimiterType = DEL_TYPE_NONE;
			}
			else if(m_bBasic_Characters && (m_char >= 0x80 )) // if we're using basic characters only and this is a non-basic character, restart the word search
			{
				word[0]=0; // start with empty word, in case we encounter EOF before we encounter a word
				wordLength = 0;
				m_gotWord = false;
				m_gotDelimiter = false;
				DelimiterType = DEL_TYPE_NONE;
			}
			else if(m_gotDelimiter) // have we just reached the end of one or more delimiters?
			{
				if(m_gotWord) // make sure that we have a word
				{
					word[wordLength]=0; // finish the word off
					m_gotChar=true;
					return -1;
				}
				else // these were preliminary delimiters and we will ignore them
				{
					DelimiterType=DEL_TYPE_NONE;
					m_gotDelimiter=false;
					m_gotChar=true;
				}
			}
			else if( wordLength < WORDMAXIMUMLENGTH )
			{
				word[wordLength]=m_char; // add this character to the word
				wordLength++;
				m_gotWord=true;
			}
		}
	}
	else if( m_contentType == CONTENT_TYPE_PDF )
	{
		while(true)
		{
			if(m_gotChar) m_gotChar = false; // check to see if we already have the next character
			else
			{
				m_char=GetCharPdf();
			}
			
			if(m_char < 0) // check for EOF encountered
			{
				word[wordLength]=0; // finish the word off
				DelimiterType = DEL_TYPE_EOF;
				return -1;
			}
			else if( (m_char == '\n') || (m_char == '\r') ) // check for newline characters
			{
				DelimiterType=DEL_TYPE_NEWLINE;
				m_gotDelimiter=true;
			}
			else if(iswspace(m_char)) // check for white space
			{
				DelimiterType=max(DelimiterType,DEL_TYPE_WHITE); // if delimiter isn't already at NEWLINE, set it to WHITE
				m_gotDelimiter=true;
			}
			else if( (iswcntrl(m_char) && m_char < 0x80) || (m_char == 0xff) ) continue; // skip any other control characters
			else if(m_gotDelimiter) // have we just reached the end of one or more delimiters?
			{
				if(m_gotWord) // make sure that we have a word
				{
					word[wordLength]=0; // finish the word off
					m_gotChar=true;
					return -1;
				}
				else // these were preliminary delimiters and we will ignore them
				{
					DelimiterType=DEL_TYPE_NONE;
					m_gotDelimiter=false;
					m_gotChar=true;
				}
			}
			else if( (m_char >= 0xfb00) && (m_char <= 0xfb06) )	// check for ligatured charactures
			{
				if(wordLength < WORDMAXIMUMLENGTH-3)
				{
					switch( m_char )
					{
					case 0xfb00:
							word[wordLength]='f';
							wordLength++;
							word[wordLength]='f';
							wordLength++;
							break;
					case 0xfb01:
							word[wordLength]='f';
							wordLength++;
							word[wordLength]='i';
							wordLength++;
							break;
					case 0xfb02:
							word[wordLength]='f';
							wordLength++;
							word[wordLength]='l';
							wordLength++;
							break;
					case 0xfb03:
							word[wordLength]='f';
							wordLength++;
							word[wordLength]='f';
							wordLength++;
							word[wordLength]='i';
							wordLength++;
							break;
					case 0xfb04:
							word[wordLength]='f';
							wordLength++;
							word[wordLength]='f';
							wordLength++;
							word[wordLength]='l';
							wordLength++;
							break;
					case 0xfb05:
							word[wordLength]='f';
							wordLength++;
							word[wordLength]='t';
							wordLength++;
							break;
					case 0xfb06:
							word[wordLength]='s';
							wordLength++;
							word[wordLength]='t';
							wordLength++;
							break;
					}
					m_gotWord=true;
				}
			}
			else if(wordLength < WORDMAXIMUMLENGTH)
			{
				word[wordLength]=m_char; // add this character to the word
				wordLength++;
				m_gotWord=true;
			}
		}
	}
	else // unknown type, so read bytes
	{
		while(true)
		{
			if(m_gotChar) m_gotChar = false; // check to see if we already have the next character
			else m_char=fgetc(m_filep);
			
			if(m_char == 0) m_char=fgetc(m_filep); // skip a single null character (but not multiple nulls)
			if(m_char < 0) // check for EOF encountered
			{
				word[wordLength]=0; // finish the word off
				DelimiterType = DEL_TYPE_EOF;
				return -1;
			}
			else if( (m_char == '\n') || (m_char == '\r')) // check for newline characters
			{
				DelimiterType=DEL_TYPE_NEWLINE;
				m_gotDelimiter=true;
			}
			else if( (m_char == '\t') || (m_char == ' ')) // check for tab or space characters
			{
				DelimiterType=max(DelimiterType,DEL_TYPE_WHITE); // if delimiter isn't already at NEWLINE, set it to WHITE
				m_gotDelimiter=true;
			}
			else if ( (iswcntrl(m_char) && m_char < 0x80) || (m_char == 0xff) ) // if we encounter a control character, restart the word search
			{
				word[0]=0; // start with empty word, in case we encounter EOF before we encounter a word
				wordLength = 0;
				m_gotWord = false;
				m_gotDelimiter = false;
				DelimiterType = DEL_TYPE_NONE;
			}
			else if(m_gotDelimiter) // have we just reached the end of one or more delimiters?
			{
				if(m_gotWord) // make sure that we have a word
				{
					word[wordLength]=0; // finish the word off
					m_gotChar=true;
					return -1;
				}
				else // these were preliminary delimiters and we will ignore them
				{
					DelimiterType=DEL_TYPE_NONE;
					m_gotDelimiter=false;
					m_gotChar=true;
				}
			}
			else if( wordLength < WORDMAXIMUMLENGTH )
			{
				word[wordLength]=m_char; // add this character to the word
				wordLength++;
				m_gotWord=true;
			}
		}
	}
	return -1;
}

int CInputDocument::OpenHtml(wchar_t* filename)
{
	_wfopen_s(&m_filep,filename,L"r");
	if(m_filep == NULL) return ERR_CANNOT_OPEN_INPUT_FILE;
	m_haveFile = true;
	m_UTF8 = true; // assume that the html file is encoded in UTF-8
	return -1;
}

int CInputDocument::OpenDoc(wchar_t* filename)
{
	_wfopen_s(&m_filep,filename,L"rb"); // open in binary read mode
	if(m_filep == NULL) return ERR_CANNOT_OPEN_INPUT_FILE;
	m_haveFile = true;
	m_UTF8 = false;
	return -1;
}

int CInputDocument::OpenTxt(wchar_t* filename)
{
	_wfopen_s(&m_filep,filename,L"r");
	if(m_filep == NULL) return ERR_CANNOT_OPEN_INPUT_FILE;
	m_haveFile = true;
	if(fgetc(m_filep) == 0xEF) // check for the BOM to indicate a utf-8 text file
	{
		if(fgetc(m_filep) == 0xBB)
		{
			if(fgetc(m_filep) == 0xBF)
			{
				m_UTF8 = true;  // found BOM; leave input pointing after the BOM
				return -1;
			}
		}
	}
	m_UTF8 = false;
	fseek(m_filep,0,SEEK_SET); // not a utf-8 text file, so rewind to the beginning
	return -1;
}

int CInputDocument::OpenUnknown(wchar_t* filename)
{
	_wfopen_s(&m_filep,filename,L"r");
	if(m_filep == NULL) return ERR_CANNOT_OPEN_INPUT_FILE;
	m_haveFile = true;
	m_UTF8 = false;
	return -1;
}

int CInputDocument::OpenDocx(wchar_t* filename)
{
	char cfilename[256];
	size_t cfilenameLength;
	wcstombs_s(&cfilenameLength, cfilename, 256, filename, _TRUNCATE); // convert wide-character filename to byte filename
	m_docxZipArchive = unzOpen(cfilename);
	if (m_docxZipArchive == NULL)
	{
		m_filep = NULL;
		return ERR_CANNOT_OPEN_INPUT_FILE;
	}

	int rv = unzLocateFile(m_docxZipArchive, "word/document.xml", NULL);
	if(rv != UNZ_OK)
	{
		unzClose(m_docxZipArchive);
		m_docxZipArchive=NULL;
		m_filep = NULL;
		return ERR_BAD_DOCX_FILE;
	}
	unzOpenCurrentFile(m_docxZipArchive);
	m_haveFile = true;
	m_UTF8 = true;
	m_ByteIndexDocx=0; // start at first byte
	m_ByteCountDocx=0; // there are currently zero bytes
	m_filep = (FILE*)1; // indicate that the file was found
	return -1;
}

int CInputDocument::OpenPdf(wchar_t* filename)
{
	if(m_pdftotext) // pdftotext program exists, so use it to pipe text to this program
	{
		// assemble command line, e.g.: ""C:\\fullpath\\pdftotext.exe" -enc UTF-8 "C:\\anotherpath\\xin.pdf" - "
		// where the final '-' indicates that the output should be piped to a readable input pipe
		// both filenames (pdftotext.exe and xxx.pdf) need to be in quotes, in case they contain spaces, and the entire command must also be in quotes.
		wchar_t commandLine[1024];
		wcscpy_s(commandLine,1024,L"\"\"");
		wcscat_s(commandLine,1024,m_pdftotextFile);
		wcscat_s(commandLine,1024,L"\" -enc UTF-8 \"");
		wcscat_s(commandLine,1024,filename);
		wcscat_s(commandLine,1024,L"\" - \"");

		if((m_filep = _wpopen(commandLine,L"r")) == NULL) return ERR_CANNOT_OPEN_INPUT_FILE; // if the pipe don't form, command filed.
		m_haveFile = true;
		m_UTF8 = true;

		return -1; // we should now have the text portion of the PDF file as a readable file attached to m_filep
	}
	else // don't have pdftotext program avialable, so use my simple-minded pdf decoding functions
	{

		_wfopen_s(&m_filep,filename,L"rb");
		if(m_filep == NULL) return ERR_CANNOT_OPEN_INPUT_FILE; // open fails if file didn't actually open

		int fileBufferLength = m_fileLength;
		m_fileBuffer = new char [fileBufferLength]; // allocate buffer for entire file

		//Read the entire file into memory and close the actual file
		int itemsRead = fread(m_fileBuffer, fileBufferLength, 1 , m_filep);
		if(itemsRead < 1) // check that the read worked out properly
		{
			fclose(m_filep); m_filep=NULL;
			delete[] m_fileBuffer; m_fileBuffer=NULL;
			return ERR_BAD_PDF_FILE;
		}

		fclose(m_filep); m_filep = NULL;

		char* fileBufferPointer = m_fileBuffer; // get moveable copy of fileBuffer pointer
		int bytesRemaining = m_fileLength; // get decrementable copy of fileLength
		
		m_ByteBufferLengthPdf = m_fileLength*10; // make text buffer 10 times as large as file (should be enough space)
		m_ByteBufferPdf = new char [m_ByteBufferLengthPdf]; // allocate buffer for all the text in that file
		m_ByteCountPdf = 0; // start at the beginning of the buffer

		m_inTextObject = false; // flag to indicate we're in a text object
		m_inLiteral = false; // flag to indicate we're in a literal character
		m_inParen = false; // flag to indicate we're in a parenthesized region
		for (int j=0; j<savedCharsLength; j++) m_savedChars[j]=' ';

		//Now search the buffer repeatedly for streams of data:
		while (true)
		{
			//Search for stream, endstream. We ought to first check the filter
			//of the object to make sure it if FlateDecode, but skip that for now!
			size_t streamStart = FindStringInBufferPdf (fileBufferPointer, "stream", bytesRemaining);
			size_t streamEnd   = FindStringInBufferPdf(fileBufferPointer, "endstream", bytesRemaining);
			if (streamStart>0 && streamEnd>streamStart)
			{
				//Skip to beginning and end of the data stream:
				streamStart += 6;

				if (fileBufferPointer[streamStart]==0x0d && fileBufferPointer[streamStart+1]==0x0a) streamStart += 2;
				else if (fileBufferPointer[streamStart]==0x0a) streamStart++;

				if (fileBufferPointer[streamEnd-2]==0x0d && fileBufferPointer[streamEnd-1]==0x0a) streamEnd -= 2;
				else if (fileBufferPointer[streamEnd-1]==0x0a) streamEnd--;

				//Assume output will fit into 10 times input buffer:
				size_t inflateBufferSize = (streamEnd - streamStart) * 10;
				m_inflateBuffer = new char [inflateBufferSize];

				//Now use zlib to inflate:
				z_stream zstrm;
				//ZeroMemory(&zstrm, sizeof(zstrm));

				zstrm.avail_in = streamEnd - streamStart + 1;
				zstrm.avail_out = inflateBufferSize;
				zstrm.next_in = (Bytef*)(fileBufferPointer + streamStart);
				zstrm.next_out = (Bytef*)m_inflateBuffer;
				zstrm.zalloc = Z_NULL;
				zstrm.zfree = Z_NULL;
				zstrm.opaque = Z_NULL;

				int rsti = inflateInit(&zstrm);
				if (rsti == Z_OK)
				{
					int rst2 = inflate(&zstrm, Z_FINISH);
					if (rst2 >= 0)
					{
						//Ok, got something, extract the text:
						size_t inflateLength = zstrm.total_out;
						ProcessStreamPdf(m_inflateBuffer, inflateLength);
					}
				}
				inflateEnd(&zstrm);
				delete[] m_inflateBuffer; m_inflateBuffer=NULL;
				fileBufferPointer += streamEnd + 7;
				bytesRemaining -= streamEnd + 7;
			}
			else
			{
				break;
			}
		}
		delete[] m_fileBuffer; m_fileBuffer=NULL; // deallocate file buffer
		m_haveFile = true;
		m_UTF8 = false;
		m_BytePointerPdf = 0;

		return -1;
	}
}

void CInputDocument::CloseDocx()
{
	if(m_docxZipArchive!=NULL)
	{
		unzCloseCurrentFile(m_docxZipArchive);
		unzClose(m_docxZipArchive);
	}
	m_docxZipArchive=NULL;
	m_haveFile = false;
}

void CInputDocument::ClosePdf()
{
	if(m_pdftotext)
	{
		fclose(m_filep); m_filep = NULL;
	}
	else
	{
		delete[] m_ByteBufferPdf; m_ByteBufferPdf=NULL; // deallocate text buffer
	}
	m_haveFile = false;
}

void CInputDocument::CloseHtml()
{
	if(m_bInternet) {m_pHttpFile->Close(); m_pHttpFile = NULL;}
	else {fclose(m_filep); m_filep = NULL;}
	m_haveFile = false;
}

void CInputDocument::CloseTxt()
{
	if(m_bInternet) {m_pHttpFile->Close(); m_pHttpFile = NULL;}
	else {fclose(m_filep); m_filep = NULL;}
	m_haveFile = false;
}

void CInputDocument::CloseUnknown()
{
	fclose(m_filep); m_filep = NULL;
	m_haveFile = false;
}

void CInputDocument::CloseDoc()
{
	fclose(m_filep); m_filep = NULL;
	m_haveFile = false;
}

int CInputDocument::GetCharDocx()
{
	int thisChar;
	int thisByte;
	int bytes;
	int i;

	if(!m_UTF8) return GetByteDocx(); // will never happen, but included for completeness

	while(true)
	{
		thisChar = GetByteDocx();
		if(thisChar < 0) return -1; // end of file reached?
		else if((thisChar & 0x80) == 0) // one-byte character?
		{
			return thisChar;
		}
		else if((thisChar & 0x20) == 0) // two-byte character?
		{
			bytes=2;
			thisChar = (thisChar & 0x1F);
		}
		else if((thisChar & 0x10) == 0) // three-byte character?
		{
			bytes=3;
			thisChar = (thisChar & 0x0F);
		}
		else if((thisChar & 0x08) == 0) // four-byte character?
		{
			bytes=4;
			thisChar = (thisChar & 0x07);
		}
		else return -1; // either bad unicode or character is more than four bytes long
		for(i=1;i<bytes;i++)
		{
			thisByte=GetByteDocx();
			if(thisByte < 0) return -1; // end of file reached prematurely?
			thisChar = (thisChar << 6) | (thisByte & 0x3F); // incorporate additional 6 bits
		}
		return thisChar;
	}
}

int CInputDocument::GetCharTxt()
{
	int thisChar;
	int thisByte;
	int bytes;
	int i;

	if(!m_UTF8) return GetByteTxt();

	while(true)
	{
		thisChar = GetByteTxt();
		if(thisChar < 0) return -1; // end of file reached?
		else if((thisChar & 0x80) == 0) // one-byte character?
		{
			return thisChar;
		}
		else if((thisChar & 0x20) == 0) // two-byte character?
		{
			bytes=2;
			thisChar = (thisChar & 0x1F);
		}
		else if((thisChar & 0x10) == 0) // three-byte character?
		{
			bytes=3;
			thisChar = (thisChar & 0x0F);
		}
		else if((thisChar & 0x08) == 0) // four-byte character?
		{
			bytes=4;
			thisChar = (thisChar & 0x07);
		}
		else return -1; // either bad unicode or character is more than four bytes long
		for(i=1;i<bytes;i++)
		{
			thisByte=GetByteTxt();
			if(thisByte < 0) return -1; // end of file reached prematurely?
			thisChar = (thisChar << 6) | (thisByte & 0x3F); // incorporate additional 6 bits
		}
		return thisChar;
	}
}

int CInputDocument::GetCharHtml()
{
	int thisChar;
	int thisByte;
	int bytes;
	int i;

	if(!m_UTF8) return GetByteHtml();

	while(true)
	{
		thisChar = GetByteHtml();
		if(thisChar < 0) return -1; // end of file reached?
		else if((thisChar & 0x80) == 0) // one-byte character?
		{
			return thisChar;
		}
		else if((thisChar & 0x20) == 0) // two-byte character?
		{
			bytes=2;
			thisChar = (thisChar & 0x1F);
		}
		else if((thisChar & 0x10) == 0) // three-byte character?
		{
			bytes=3;
			thisChar = (thisChar & 0x0F);
		}
		else if((thisChar & 0x08) == 0) // four-byte character?
		{
			bytes=4;
			thisChar = (thisChar & 0x07);
		}
		else return -1; // either bad unicode or character is more than four bytes long
		for(i=1;i<bytes;i++)
		{
			thisByte=GetByteHtml();
			if(thisByte < 0) return -1; // end of file reached prematurely?
			thisChar = (thisChar << 6) | (thisByte & 0x3F); // incorporate additional 6 bits
		}
		return thisChar;
	}
}

int CInputDocument::GetCharPdf()
{
	if(m_pdftotext)  // did pdftotext do the conversion to a UTF-8 piped input file?
	{
		int thisChar;
		int thisByte;
		int bytes;
		int i;

		if(!m_UTF8) return GetByteHtml(); // will never happen, but included for completeness

		while(true)
		{
			thisChar = GetBytePdf();
			if(thisChar < 0) return -1; // end of file reached?
			else if((thisChar & 0x80) == 0) // one-byte character?
			{
				return thisChar;
			}
			else if((thisChar & 0x20) == 0) // two-byte character?
			{
				bytes=2;
				thisChar = (thisChar & 0x1F);
			}
			else if((thisChar & 0x10) == 0) // three-byte character?
			{
				bytes=3;
				thisChar = (thisChar & 0x0F);
			}
			else if((thisChar & 0x08) == 0) // four-byte character?
			{
				bytes=4;
				thisChar = (thisChar & 0x07);
			}
			else return -1; // either bad unicode or character is more than four bytes long
			for(i=1;i<bytes;i++)
			{
				thisByte=GetBytePdf();
				if(thisByte < 0) return -1; // end of file reached prematurely?
				thisChar = (thisChar << 6) | (thisByte & 0x3F); // incorporate additional 6 bits
			}
			return thisChar;
		}
	}
	else // no pdftotext, so we're using the simple conversion and reading characters from a memory buffer
	{
		if( m_BytePointerPdf < m_ByteCountPdf )
		{
			m_BytePointerPdf++;
			return m_ByteBufferPdf[m_BytePointerPdf-1];
		}
		else return -1;
	}
}

int CInputDocument::GetByteDocx()
{
	unsigned char thisByte;
	while(true)
	{
		if(m_ByteIndexDocx < m_ByteCountDocx)
		{
			thisByte=m_docxByteBuffer[m_ByteIndexDocx];
			m_ByteIndexDocx++;
			return thisByte;
		}
		else if(m_eof) return -1;
		else
		{
			m_ByteCountDocx = unzReadCurrentFile(m_docxZipArchive,m_docxByteBuffer,m_docxByteBufferLength);
			m_ByteIndexDocx = 0;
			if(m_ByteCountDocx==0)
			{
				m_eof=true;
				return -1;
			}
		}
	}
}

int CInputDocument::GetByteTxt()
{
	if(m_bInternet)
	{
		unsigned char byte;
		if(m_pHttpFile->Read(&byte,1) > 0) return byte;
		else return -1;
	}
	return fgetc(m_filep);
}

int CInputDocument::GetByteHtml()
{
	if(m_bInternet)
	{
		unsigned char byte;
		if(m_pHttpFile->Read(&byte,1) > 0) return byte;
		else return -1;
	}
	else return fgetc(m_filep);
}

int CInputDocument::GetBytePdf()
{
	return fgetc(m_filep);
}

//Find a string (needle) in a buffer (haystack):
size_t CInputDocument::FindStringInBufferPdf(char* haystack, char* needle, size_t haystackLength)
{
	char* haystack0 = haystack;
	size_t needleLength = strlen(needle);

	size_t position;
	for(position=0;position<=haystackLength-needleLength;position++)
	{
		if(strncmp(haystack0,needle,needleLength) == 0) return position;
		haystack0++;
	}
	return -1;
}


//Convert a recent set of characters into an X value if it exists
void CInputDocument::GetXPdf(const char * haystack, int lastCharOffset, float &X)
{
	char string[savedCharsLength+1];
	int offset;
	int count;

	offset=lastCharOffset;

	count=0;
	while (offset>0 && haystack[offset]==' ') offset--; // skip back over any extra blanks
	while (offset>0 && (iswdigit(haystack[offset]) || haystack[offset]=='.' || haystack[offset]=='-')) // look backward for X value
	{
		offset--;
		count++;
	}
	if(count==0) // didn't find a number, so we failed
	{
		X=0.0;
		return;
	}
	strncpy_s(string, savedCharsLength+1, haystack+offset+1, count);
	string[count]=0; // null terminate the string
	sscanf_s(string, "%f", &X);

	return;
}

//Convert a recent set of characters into an X value and a Y value if they exist
void CInputDocument::GetXYPdf(const char * haystack, int lastCharOffset, float &X, float &Y)
{
	char string[savedCharsLength+1];
	int offset;
	int count;

	offset=lastCharOffset;

	count=0;
	while (offset>0 && haystack[offset]==' ') offset--; // skip back over any extra blanks
	while (offset>0 && (iswdigit(haystack[offset]) || haystack[offset]=='.' || haystack[offset]=='-')) // look backward for Y value
	{
		offset--;
		count++;
	}
	if(count==0) // didn't find a number, so we failed
	{
		X=0.0;
		Y=0.0;
		return;
	}
	strncpy_s(string, savedCharsLength+1, haystack+offset+1, count);
	string[count]=0; // null terminate the string
	sscanf_s(string, "%f", &Y);

	count=0;
	while (offset>0 && haystack[offset]==' ') offset--; // skip back over any extra blanks
	while (offset>0 && (iswdigit(haystack[offset]) || haystack[offset]=='.' || haystack[offset]=='-')) // look backward for X value
	{
		offset--;
		count++;
	}
	if(count==0) // didn't find a number, so we failed
	{
		X=0.0;
		Y=0.0;
		return;
	}
	strncpy_s(string,savedCharsLength+1, haystack+offset+1, count);
	string[count]=0; // null terminate the string
	sscanf_s(string, "%f", &X);
	return;
}

//Check for a recent 2 character token (e.g. BT):
bool CInputDocument::FindTokenPdf(const char* needle, char* haystack)
{
	if ( (haystack[savedCharsLength-4] == ' ' || haystack[savedCharsLength-4]=='\n' || haystack[savedCharsLength-4]=='\r') &&
		 (haystack[savedCharsLength-3] == needle[0]) &&
		 (haystack[savedCharsLength-2] == needle[1]) &&
		 (haystack[savedCharsLength-1] == ' ' || haystack[savedCharsLength-1]=='\n' || haystack[savedCharsLength-1]=='\r') ) return true;
	else return false;
}

// Put a byte into the text buffer
void CInputDocument::PutBytePdf(char thischar)
{
	if ( m_ByteCountPdf < m_ByteBufferLengthPdf )
	{
		m_ByteBufferPdf[m_ByteCountPdf]=thischar;
		m_ByteCountPdf++;
	}
	return;
}

//This method processes an uncompressed Adobe (text) object and extracts text.
void CInputDocument::ProcessStreamPdf(const char* inflateBuffer, size_t inflateLength)
{
	float X,Y; // positioning values

	for (size_t i=0; i<inflateLength; i++)
	{
		char thisChar = inflateBuffer[i];

		if (m_inTextObject)
		{
			if (m_inParen)
			{
				if (thisChar==')' && !m_inLiteral) m_inParen=false;
				else 
				{
					//Just a normal text character:
					if (!m_inLiteral && thisChar=='\\') m_inLiteral = true;
					else
					{
						if(thisChar=='H')
						{
							thisChar='H';
						}
						m_inLiteral = false;
						if ( ((thisChar>=' ') && (thisChar<='~')) || ((thisChar>=128) && (thisChar<255)) ) PutBytePdf(thisChar);
					}
				}
			}
			else
			{
				if(FindTokenPdf("TD", m_savedChars)) // positioning; have we moved enough to warrent a space?
				{
					GetXYPdf(m_savedChars, savedCharsLength-5, X, Y);
					if( (X>1.0) || (Y!=0.0) ) PutBytePdf(' ');
				}
				else if(FindTokenPdf("Tm", m_savedChars)) // positioning; have we moved enough to warrent a space?
				{
					PutBytePdf(' ');
				}
				else if (FindTokenPdf("ET", m_savedChars))
				{
					//End of a text object, also go to a new line.
					m_inTextObject = false;
					PutBytePdf('\n');
					PutBytePdf('\r');
				}
				if (thisChar=='(') // check for start of text
				{
					//Start outputting text!
					m_inParen=true;
					//See if a space is called for by looking at the number in front of the (
					GetXPdf(m_savedChars,savedCharsLength-1,X);
					if (X>500) PutBytePdf(' ');
				}
			}
		}
		else
		{
			if (FindTokenPdf("BT", m_savedChars))
			{
				//Start of a text object:
				m_inTextObject = true;
				PutBytePdf('\r');
				PutBytePdf('\n');
			}
		}
		//Store the recent characters for when we have to go back for a number:
		for (int j=0; j<savedCharsLength-1; j++) m_savedChars[j] = m_savedChars[j+1];
		m_savedChars[savedCharsLength-1] = thisChar;
	}
}

int CInputDocument::OpenUrl(wchar_t* filename)
{
	_wfopen_s(&m_filep,filename,L"r");
	if(m_filep == NULL) return ERR_CANNOT_FIND_URL_LINK;

	wchar_t string[256];
	CString szmessage;

	while(fgetws(string,255,m_filep) != NULL)
	{
		if(_wcsnicmp(&string[0],L"url=",4) == 0)
		{
			fclose(m_filep); m_filep = NULL;
			DWORD dwHttpRequestFlags = INTERNET_FLAG_EXISTING_CONNECT;
			const wchar_t szHeaders[] = L"Accept: text/*\r\nUser-Agent: WCopyfind\r\n";

			DWORD dwServiceType;
			CString strServerName;
			CString strObject;
			INTERNET_PORT nPort;

			if (!AfxParseURL(&string[4], dwServiceType, strServerName, strObject, nPort) || dwServiceType != INTERNET_SERVICE_HTTP)
			{
				fclose(m_filep); m_filep = NULL;
				return ERR_CANNOT_ACCESS_URL;
			}

			m_pSession = new CInternetSession;
			m_pServer = m_pSession->GetHttpConnection(strServerName, nPort);
			m_pHttpFile = m_pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET,strObject, NULL, 1, NULL, NULL, dwHttpRequestFlags);
			m_pHttpFile->AddRequestHeaders(szHeaders);
			m_pHttpFile->SendRequest();
			DWORD dwRet;
			m_pHttpFile->QueryInfoStatusCode(dwRet);

			if (dwRet >= 300)
			{
				fclose(m_filep); m_filep = NULL;
				return ERR_CANNOT_ACCESS_URL;
			}
			m_bInternet = true;

			DWORD dwQuery;
			CString szreturn;
			dwQuery=HTTP_QUERY_CONTENT_TYPE;
			m_pHttpFile->QueryInfo(dwQuery,szreturn);

			if(wcsstr(szreturn,L"text/html") != NULL) m_contentType=CONTENT_TYPE_HTML;
			else if(wcsstr(szreturn,L"text/plain") != NULL) m_contentType=CONTENT_TYPE_TXT;
			if(wcsstr(szreturn,L"UTF-8") != NULL) m_UTF8 = true;
			else m_UTF8 = false;

			m_haveFile = true;
			return -1;
		}
	}
	return ERR_CANNOT_ACCESS_URL;
}