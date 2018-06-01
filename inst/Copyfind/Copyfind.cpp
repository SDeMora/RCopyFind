// Copyfind.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <afxinet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <direct.h>
#include <locale.h>
#include "zlib\zlib.h"
#include "zlib\zconf.h"
#include "zlib\unzip.h"
#include "clib\InputDocument.h"
#include "clib\Words.h"
#include "clib\CompareDocuments.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	CCompareDocuments* m_pDoc;
	wchar_t InputLine[1000];
	wchar_t *InputPointer;
	wchar_t *CommaPosition;
	wchar_t *PoundPosition;
	wchar_t *SlashSlashPosition;
	CString Command;
	CString Parameter[3];

	int StringLength;
	int ParameterCount;
	CString szfolder;
	int Documents;
	int Document;
	int FilterDocument;
	int Group;
	int iReturn;
	int i;
	int DocL,DocR;
	int Group1,Group2;
	int Group1Count,Group2Count;
	struct _stat statBuffer;
	CString FileName;
	CString DirectoryName;
	CString SearchName;
	WIN32_FIND_DATA ffd;
	HANDLE hFind;
	LARGE_INTEGER filesize;
	DWORD dwError;

	bool bScriptFile;
	wifstream ScriptFile;

	m_pDoc = NULL;

	bScriptFile = false;
	if( (argc >= 3) && (wcscmp(argv[1],L"-f") == 0) )
	{
		wprintf(L"ScriptFile Found\n");	
		bScriptFile = true;
		wprintf(L"Opening file '%s'\n",argv[2]);
		ScriptFile.open(argv[2],wifstream::in);
		if(ScriptFile.fail())
		{
			wprintf(L"Open Failed\n");
		}
	}
	
	szfolder=L"";
	Document=-1;

	while(true)
	{
		Parameter[0]=L"";
		Parameter[1]=L"";
		Parameter[2]=L"";
		Command=L"";

		if(bScriptFile) ScriptFile.getline(InputLine,1000);
		else wcin.getline(InputLine,1000);

		if(wcslen(InputLine)<1) break;
		PoundPosition=wcschr(InputLine,L'#');
		if(PoundPosition==InputLine) continue;
		if(PoundPosition!=NULL) PoundPosition[0]=NULL;	// terminate line at a # sign, the beginning of a comment
		SlashSlashPosition=wcsstr(InputLine,L"//");
		if(SlashSlashPosition==InputLine) continue;
		if(SlashSlashPosition!=NULL) SlashSlashPosition[0]=NULL;	// terminate line at a # sign, the beginning of a comment

		InputPointer=InputLine;
		CommaPosition=wcschr(InputPointer,L',');
		if(CommaPosition == NULL)
		{
			Command = InputPointer;
			Command = Command.Trim();	// trim off all leading or trailing whitespace
		}
		else
		{
			StringLength=CommaPosition-InputPointer;

			Command = InputPointer;
			Command = Command.Left(StringLength);
			Command = Command.Trim();	// trim off all leading or trailing whitespace

			InputPointer=CommaPosition+1;

			for(ParameterCount=0;ParameterCount<3;ParameterCount++)
			{
				CommaPosition=wcschr(InputPointer,L',');
				if(CommaPosition == NULL)
				{
					Parameter[ParameterCount]=InputPointer;
					Parameter[ParameterCount] = Parameter[ParameterCount].Trim();	// trim off all leading or trailing whitespace
					break;
				}
				else {
					StringLength=CommaPosition-InputPointer;
					Parameter[ParameterCount] = InputPointer;
					Parameter[ParameterCount] = Parameter[ParameterCount].Left(StringLength);
					Parameter[ParameterCount] = Parameter[ParameterCount].Trim();	// trim off all leading or trailing whitespace
					InputPointer=CommaPosition+1;
				}
			}
		}

		if(_wcsicmp(Command,L"Documents") == 0)
		{
			Documents=_wtoi(Parameter[0]);
			if( (Documents<1) || (Documents>10000000) )
			{
				wprintf(L"Document Count %d is Invalid. It must be between 1 and 10,000,000.\n",Documents);
				return 1;
			}
			if( ((m_pDoc = new CCompareDocuments(Documents)) == NULL) || (m_pDoc->m_pDocs == NULL) )
			{
				wprintf(L"Could not allocate enough memory for the documents.\n");
				if (m_pDoc != NULL) {delete m_pDoc; m_pDoc=NULL;}
				return 2;
			}

			FilterDocument=-1;

			m_pDoc->m_PhraseLength = 6;
			m_pDoc->m_FilterPhraseLength = 6;
			m_pDoc->m_WordThreshold = 100;
			m_pDoc->m_SkipLength = 20;
			m_pDoc->m_MismatchTolerance = 2;
			m_pDoc->m_MismatchPercentage = 80;

			m_pDoc->m_bBriefReport = false;
			m_pDoc->m_bIgnoreCase = false;
			m_pDoc->m_bIgnoreNumbers = false;
			m_pDoc->m_bIgnoreOuterPunctuation = false;
			m_pDoc->m_bIgnorePunctuation = false;
			m_pDoc->m_bSkipLongWords = false;
			m_pDoc->m_bSkipNonwords = false;
			m_pDoc->m_bBasic_Characters = false;
			m_pDoc->m_szSoftwareName.Format(L"Copyfind.4.1.5");
			_wsetlocale(LC_ALL, L"English");

			wprintf(L"Documents Created Successfully\n");
		}

		else if(_wcsicmp(Command,L"ReportFolder") == 0)
		{
			struct _stat buf;
			int result = _tstat( Parameter[0], &buf );
			if( (result != 0) || ((buf.st_mode & _S_IFDIR) != _S_IFDIR))
			{
				wprintf(L"Report Folder %s cannot be found. You must make sure a Report Folder exists before running WCopyfind.\n",Parameter[0]);
				return 1;
			}
			szfolder.SetString(Parameter[0]);
			wprintf(L"About the set report folder\n");
			m_pDoc->m_szReportFolder = szfolder;
			wprintf(L"Report Folder Set Successfully\n");
		}

		else if(_wcsicmp(Command,L"Document") == 0)
		{
			Group = _wtoi(Parameter[0]);
			if( Group < 1 )
			{
				wprintf(L"Group %d is out of range (it must be 1 or greater)\n",Group);
				return 1;
			}
			FileName = Parameter[1];
						
			if( _wstat(FileName,&statBuffer) != 0 )
			{
				wprintf(L"File or Directory %s Not Found\n",FileName);
				return 1;
			}
			if (statBuffer.st_mode & _S_IFDIR)	// is it a directory?
			{
				// handle directory
				DirectoryName=Parameter[1];
				SearchName = DirectoryName + L"\\*";
				hFind = INVALID_HANDLE_VALUE;
				hFind = FindFirstFile(SearchName, &ffd);
				if( hFind == INVALID_HANDLE_VALUE)
				{
					wprintf(L"Directory %s Not Available\n",DirectoryName);
					return 1;
				}
				do
				{
					if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
					filesize.LowPart = ffd.nFileSizeLow;
					filesize.HighPart = ffd.nFileSizeHigh;
					FileName = DirectoryName + L"\\" + ffd.cFileName;
					Document++;
					if(Document >= Documents)
					{
						wprintf(L"Document Space Exceeded. You Have Allocated Space for Only %d Documents.\n",Documents);
						return 1;
					}
					(m_pDoc->m_pDocs+Document)->m_szDocumentName=FileName;
					(m_pDoc->m_pDocs+Document)->m_DocumentType = Group;
				}
				while (FindNextFile(hFind, &ffd) != 0);
 
				dwError = GetLastError();
				if (dwError != ERROR_NO_MORE_FILES) 
				{
					wprintf(L"Unexpected End of Directory %s\n",DirectoryName);
					return 1;
				}

				FindClose(hFind);
			}
			else
			{
				// handle regular file
				Document++;
				if(Document >= Documents)
				{
					wprintf(L"Document Space Exceeded. You Have Allocated Space for Only %d Documents.\n",Documents);
					return 1;
				}
				(m_pDoc->m_pDocs+Document)->m_szDocumentName=FileName;
				(m_pDoc->m_pDocs+Document)->m_DocumentType = Group;
			}
		}
		else if(_wcsicmp(Command,L"FilterDocument") == 0)
		{
			FileName = Parameter[0];
						
			if( _wstat(FileName,&statBuffer) != 0 )
			{
				wprintf(L"File or Directory %s Not Found\n",FileName);
				return 1;
			}
			if (statBuffer.st_mode & _S_IFDIR)	// is it a directory?
			{
				wprintf(L"FilterDocument cannot be a directory\n");
				return 1;
			}
			else
			{
				// handle regular file
				Document++;
				FilterDocument=Document;	// this is the filter document
				if(Document >= Documents)
				{
					wprintf(L"Document Space Exceeded. You Have Allocated Space for Only %d Documents.\n",Documents);
					return 1;
				}
				(m_pDoc->m_pDocs+Document)->m_szDocumentName=FileName;
				(m_pDoc->m_pDocs+Document)->m_DocumentType = -1;	// put the filterdocument in group -1, where it cannot be matched with anything
				wprintf(L"FilterDocument = %d\n",FilterDocument);
			}
		}
		else if(_wcsicmp(Command,L"PhraseLength") == 0)
		{
			m_pDoc->m_PhraseLength = _wtoi(Parameter[0]);
			wprintf(L"PhraseLength = %d\n",m_pDoc->m_PhraseLength);
		}

		else if(_wcsicmp(Command,L"FilterPhraseLength") == 0)
		{
			m_pDoc->m_FilterPhraseLength = _wtoi(Parameter[0]);
			wprintf(L"FilterPhraseLength = %d\n",m_pDoc->m_FilterPhraseLength);
		}

		else if(_wcsicmp(Command,L"WordThreshold") == 0)
		{
			m_pDoc->m_WordThreshold = _wtoi(Parameter[0]);
			wprintf(L"WordThreshold = %d\n",m_pDoc->m_WordThreshold);
		}

		else if(_wcsicmp(Command,L"SkipLength") == 0)
		{
			m_pDoc->m_SkipLength = _wtoi(Parameter[0]);
			wprintf(L"SkipLength = %d\n",m_pDoc->m_SkipLength);
		}

		else if(_wcsicmp(Command,L"MismatchTolerance") == 0)
		{
			m_pDoc->m_MismatchTolerance = _wtoi(Parameter[0]);
			wprintf(L"MismatchTolerance = %d\n",m_pDoc->m_MismatchTolerance);
		}

		else if(_wcsicmp(Command,L"MismatchPercentage") == 0)
		{
			m_pDoc->m_MismatchPercentage = _wtoi(Parameter[0]);
			wprintf(L"MismatchPercentage = %d\n",m_pDoc->m_MismatchPercentage);
		}

		else if(_wcsicmp(Command,L"BriefReport") == 0)
		{
			m_pDoc->m_bBriefReport = (_wtoi(Parameter[0])==1);
			wprintf(L"BriefReport = %d\n",m_pDoc->m_bBriefReport);
		}

		else if(_wcsicmp(Command,L"IgnoreCase") == 0)
		{
			m_pDoc->m_bIgnoreCase = (_wtoi(Parameter[0])==1);
			wprintf(L"IgnoreCase = %d\n",m_pDoc->m_bIgnoreCase);
		}

		else if(_wcsicmp(Command,L"IgnoreNumbers") == 0)
		{
			m_pDoc->m_bIgnoreNumbers = (_wtoi(Parameter[0])==1);
			wprintf(L"IgnoreNumbers = %d\n",m_pDoc->m_bIgnoreNumbers);
		}

		else if(_wcsicmp(Command,L"IgnoreOuterPunctuation") == 0)
		{
			m_pDoc->m_bIgnoreOuterPunctuation = (_wtoi(Parameter[0])==1);
			wprintf(L"IgnoreOuterPunctuation = %d\n",m_pDoc->m_bIgnoreOuterPunctuation);
		}

		else if(_wcsicmp(Command,L"IgnorePunctuation") == 0)
		{
			m_pDoc->m_bIgnorePunctuation = (_wtoi(Parameter[0])==1);
			wprintf(L"IgnorePunctuation = %d\n",m_pDoc->m_bIgnorePunctuation);
		}

		else if(_wcsicmp(Command,L"SkipLongWords") == 0)
		{
			m_pDoc->m_bSkipLongWords = (_wtoi(Parameter[0])==1);
			wprintf(L"SkipLongWords = %d\n",m_pDoc->m_bSkipLongWords);
		}

		else if(_wcsicmp(Command,L"SkipNonwords") == 0)
		{
			m_pDoc->m_bSkipNonwords = (_wtoi(Parameter[0])==1);
			wprintf(L"SkipNonwords = %d\n",m_pDoc->m_bSkipNonwords);
		}

		else if(_wcsicmp(Command,L"BasicCharacters") == 0)
		{
			m_pDoc->m_bBasic_Characters = (_wtoi(Parameter[0])==1);
			wprintf(L"BasicCharacters = %d\n",m_pDoc->m_bBasic_Characters);
		}

		else if(_wcsicmp(Command,L"Locale") == 0)
		{
			_wsetlocale(LC_ALL, Parameter[0]);
			wprintf(L"Locale = %s\n",Parameter[0]);
		}

		else if(_wcsicmp(Command,L"PrepareForComparisons") == 0)
		{
			if(szfolder=="")
			{
				wprintf(L"Report Folder Must be Set before Beginning the Comparison Process\n");
				return 2;
			}
			wprintf(L"Initializing Reports\n");
			iReturn = m_pDoc->SetupReports();		// setup reporting files
			if(iReturn > -1)
			{
				wprintf(L"Error %d Occurred During Report Initialization\n",iReturn);
				return iReturn;
			}
			wprintf(L"Reports Initialized Successfully\n");
			wprintf(L"Starting Document Loading and Hashing\n");
			m_pDoc->SetupLoading();								// setup document loading step
			for(i=0;i<m_pDoc->m_Documents;i++)			// loop for all document entries
			{
				if((m_pDoc->m_pDocs+i)->m_DocumentType == 0) continue;			// skip any document that hasn't been assigned a group
				iReturn = m_pDoc->LoadDocument((m_pDoc->m_pDocs+i));			// load this document
				if(iReturn > -1)
				{
					wprintf(L"Error %d Occurred While Loading Document %d",iReturn,i);
					return iReturn;
				}
			}
			m_pDoc->FinishLoading();									// Finish document loading step
			wprintf(L"Document Loaded and Hashed Successfully\n");
			wprintf(L"Initializing Comparison Process\n");
			iReturn = m_pDoc->SetupComparisons();
			if(iReturn > -1)
			{
				wprintf(L"Error %d Occurred While Initializing Comparison Process\n",iReturn);
				return iReturn;
			}
			wprintf(L"Comparison Process Initialized Successfully\n");
		}

		else if(_wcsicmp(Command,L"Compare") == 0)
		{
			Group1=_wtoi(Parameter[0]);
			Group2=_wtoi(Parameter[1]);
			Group1Count=0;
			Group2Count=0;

			if(Group1 < 1)
			{
				wprintf(L"Group %d is Invalid. Groups must be 1 or greater\n",Group1);
				return 2;
			}
			if(Group2 < 1)
			{
				wprintf(L"Group %d is Invalid. Groups must be 1 or greater\n",Group2);
				return 2;
			}

			if(Group1 == Group2)
			{
				wprintf(L"Beginning to Compare Group %d Documents with One Another (Internal Comparison)\n",Group1);
				for(i=0;i<m_pDoc->m_Documents;i++)
				{
					if((m_pDoc->m_pDocs+i)->m_DocumentType == Group1) Group1Count++;
				}
				m_pDoc->SetupProgressReports(0,0,Group1);
				m_pDoc->m_Compares=0;
			}
			else
			{
				wprintf(L"Beginning to Compare Group %d Documents with Group %d Documents\n",Group1,Group2);
				m_pDoc->SetupProgressReports(Group1,Group2,0);
				m_pDoc->m_Compares=0;
			}

			for(DocL=0;DocL<m_pDoc->m_Documents;DocL++)			// for all possible left documents
			{
				m_pDoc->m_pDocL = m_pDoc->m_pDocs + DocL;	// obtain a quick pointer to the left document
				if((m_pDoc->m_pDocL)->m_DocumentType == 0) continue;			// skip any document that hasn't been assigned a group
				for(DocR=0;DocR<DocL;DocR++)					// for all possible right documents
				{
					m_pDoc->m_pDocR = m_pDoc->m_pDocs + DocR;	// obtain a quick pointer to the right document
					if((m_pDoc->m_pDocR)->m_DocumentType == 0) continue;			// skip any document that hasn't been assigned a group

					if( Group1 == Group2)						// is this an internal comparison within one group?
					{
						if( (m_pDoc->m_pDocL->m_DocumentType != Group1) || (m_pDoc->m_pDocR->m_DocumentType != Group1) ) continue;	// skip if either document isn't in that group
					}
					else
					{
						if( ((m_pDoc->m_pDocL->m_DocumentType != Group1) || (m_pDoc->m_pDocR->m_DocumentType != Group2)) &&
							((m_pDoc->m_pDocL->m_DocumentType != Group2) || (m_pDoc->m_pDocR->m_DocumentType != Group1)) ) continue; // skip if the documents aren't in those two groups
					}

					iReturn = m_pDoc->ComparePair(m_pDoc->m_pDocL,m_pDoc->m_pDocR); 		// compare the two documents
					if(iReturn > 0)
					{
						wprintf(L"Error %d Occurred While Comparing Document %d and Document %d\n",iReturn,DocL,DocR);
						return iReturn;	
					}
					
					if( (m_pDoc->m_Compares%m_pDoc->m_CompareStep)	== 0 )				// if count is divisible by 1000,
					{
						wprintf(L"Completed %d of %d Comparisons\n",m_pDoc->m_Compares,m_pDoc->m_TotalCompares);
					}
					
					if(m_pDoc->m_MatchingWordsPerfect>=m_pDoc->m_WordThreshold)		// if there are enough matches to report,
					{
						m_pDoc->m_MatchingDocumentPairs++;				// increment count of matched pairs of documents
						m_pDoc->ReportMatchedPair();
					}
				}
			}
			wprintf(L"Compared Group %d to Group %d Successfully\n",Group1,Group2);

		}

		else if(_wcsicmp(Command,L"CompareFiltered") == 0)
		{
			Group1=_wtoi(Parameter[0]);
			Group2=_wtoi(Parameter[1]);
			Group1Count=0;
			Group2Count=0;

			if(Group1 < 1)
			{
				wprintf(L"Group %d is Invalid. Groups must be 1 or greater\n",Group1);
				return 2;
			}
			if(Group2 < 1)
			{
				wprintf(L"Group %d is Invalid. Groups must be 1 or greater\n",Group2);
				return 2;
			}
			if(FilterDocument < 0)
			{
				wprintf(L"FilterDocument not specified\n",Group1);
				return 2;
			}

			if(Group1 == Group2)
			{
				wprintf(L"Beginning to Compare Group %d Documents with One Another (Internal Comparison)\n",Group1);
				for(i=0;i<m_pDoc->m_Documents;i++)
				{
					if((m_pDoc->m_pDocs+i)->m_DocumentType == Group1) Group1Count++;
				}
				m_pDoc->SetupProgressReports(0,0,Group1);
				m_pDoc->m_Compares=0;
			}
			else
			{
				wprintf(L"Beginning to Compare Group %d Documents with Group %d Documents\n",Group1,Group2);
				m_pDoc->SetupProgressReports(Group1,Group2,0);
				m_pDoc->m_Compares=0;
			}

			for(DocL=0;DocL<m_pDoc->m_Documents;DocL++)			// for all possible left documents
			{
				m_pDoc->m_pDocL = m_pDoc->m_pDocs + DocL;	// obtain a quick pointer to the left document
				if((m_pDoc->m_pDocL)->m_DocumentType == 0) continue;			// skip any document that hasn't been assigned a group
				for(DocR=0;DocR<DocL;DocR++)					// for all possible right documents
				{
					m_pDoc->m_pDocR = m_pDoc->m_pDocs + DocR;	// obtain a quick pointer to the right document
					if((m_pDoc->m_pDocR)->m_DocumentType == 0) continue;			// skip any document that hasn't been assigned a group

					if( Group1 == Group2)						// is this an internal comparison within one group?
					{
						if( (m_pDoc->m_pDocL->m_DocumentType != Group1) || (m_pDoc->m_pDocR->m_DocumentType != Group1) ) continue;	// skip if either document isn't in that group
					}
					else
					{
						if( ((m_pDoc->m_pDocL->m_DocumentType != Group1) || (m_pDoc->m_pDocR->m_DocumentType != Group2)) &&
							((m_pDoc->m_pDocL->m_DocumentType != Group2) || (m_pDoc->m_pDocR->m_DocumentType != Group1)) ) continue; // skip if the documents aren't in those two groups
					}

					iReturn = m_pDoc->ComparePairFiltered(m_pDoc->m_pDocL,m_pDoc->m_pDocR, m_pDoc->m_pDocs + FilterDocument); 		// compare the two documents, with filtering
					if(iReturn > 0)
					{
						wprintf(L"Error %d Occurred While Comparing Document %d and Document %d\n",iReturn,DocL,DocR);
						return iReturn;	
					}
					
					if( (m_pDoc->m_Compares%m_pDoc->m_CompareStep)	== 0 )				// if count is divisible by 1000,
					{
						wprintf(L"Completed %d of %d Comparisons\n",m_pDoc->m_Compares,m_pDoc->m_TotalCompares);
					}
					
					if(m_pDoc->m_MatchingWordsPerfect>=m_pDoc->m_WordThreshold)		// if there are enough matches to report,
					{
						m_pDoc->m_MatchingDocumentPairs++;				// increment count of matched pairs of documents
						m_pDoc->ReportMatchedPair();
					}
				}
			}
			wprintf(L"Compared Group %d to Group %d with Filtering Successfully\n",Group1,Group2);

		}

		else if(_wcsicmp(Command,L"Done") == 0)
		{
			if (m_pDoc != NULL)
			{
				wprintf(L"Finishing Comparison Process\n");
				m_pDoc->FinishComparisons();
				wprintf(L"Finished Comparison Process Successfully\n");
				wprintf(L"Finishing Reports\n");
				m_pDoc->FinishReports();
				wprintf(L"Finished Reports Successfully\n");
				wprintf(L"Unloading Documents\n");
				if (m_pDoc != NULL) { delete m_pDoc; m_pDoc = NULL; }
				szfolder = L"";
				Document = -1;
				wprintf(L"Documents Unloaded Successfully\n");
			}
		}

		else
		{
			wprintf(L"Error: Command '%s' Not Recognized\n",Command);
		}

	}

	ScriptFile.close();
	return 1;
}
