//
#include "..\stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <afx.h>
#include <afxwin.h>
#include <afxinet.h>
#include <wininet.h>
#include <locale.h>
#include "..\zlib\zlib.h"
#include "..\zlib\zconf.h"
#include "..\zlib\unzip.h"
#include "InputDocument.h"
#include "words.h"

void CWordVocab::AddWord(wchar_t *word)
{
	VocabEntry **ppentry;
	VocabEntry *plastentry;
	unsigned int hash;

	ppentry=&pfirstentry;
	plastentry=NULL;
	hash=WordHash(word);

	while(true)
	{
		if((*ppentry) == NULL)
		{
			(*ppentry) = new VocabEntry;
			(*ppentry)->m_pNextEntry=NULL;
			(*ppentry)->plastentry=plastentry;
			wcscpy_s((*ppentry)->word,word);
			(*ppentry)->usecnt=1;
			(*ppentry)->hash=hash;
			return;
		}
		if( (*ppentry)->hash == hash )
		{
			if( wcscmp((*ppentry)->word,word) == 0 )
			{
				(*ppentry)->usecnt++;

				if( (*ppentry)->plastentry != NULL )	// Do a little sorting by usage to speed things up
				{
					if( (*ppentry)->plastentry->usecnt < (*ppentry)->usecnt )
					{
						VocabEntry *pa,*pb,*pc,*pd;
						VocabEntry **ppan,**ppbl,**ppbn,**ppcl,**ppcn,**ppdl;
						pb=(*ppentry)->plastentry;
						pc=(*ppentry);
						if(pb->plastentry != NULL)
						{
							pa=pb->plastentry;
							ppan=&(pa->m_pNextEntry);
						}
						else
						{
							pa=NULL;
							pfirstentry=pc;
						}
						if(pc->m_pNextEntry != NULL)
						{
							pd=pc->m_pNextEntry;
							ppdl=&(pd->plastentry);
						}
						else
						{
							pd=NULL;
						}
						ppbl=&(pb->plastentry);
						ppbn=&(pb->m_pNextEntry);
						ppcl=&(pc->plastentry);
						ppcn=&(pc->m_pNextEntry);

						if(pa != NULL) (*ppan)=pc;
						if(pd != NULL) (*ppdl)=pb;
						(*ppbl)=pc;
						(*ppbn)=pd;
						(*ppcl)=pa;
						(*ppcn)=pb;
					}
				}
				return;
			}
		}
		plastentry=(*ppentry);
		ppentry=&((*ppentry)->m_pNextEntry);
	}
}

void CWordVocab::ListWords(FILE *fvocab)
{
	VocabEntry *pentry;
	pentry=pfirstentry;
	
	while(pentry != NULL)
	{
		fwprintf(fvocab,L"%d\t%s\n",pentry->usecnt,pentry->word);
		pentry=pentry->m_pNextEntry;
	}
}

void WordRemovePunctuation(wchar_t *word)
{
	int wordlen;
	int ccnt;
	int icnt;
	
	wordlen=wcslen(word);
	for(ccnt=0;ccnt<wordlen;ccnt++)
	{
		if(iswpunct(word[ccnt]))
		{
			for(icnt=ccnt;icnt<wordlen;icnt++) word[icnt]=word[icnt+1]; // move the null, too.
			wordlen--;
			ccnt--;
		}
	}
}

void wordxouterpunct(wchar_t *word)
{
	int wordlen;
	int ccnt;
	int icnt;
	
	wordlen=wcslen(word);
	for(ccnt=0;ccnt<wordlen;ccnt++)
	{
		if(iswpunct(word[ccnt]))
		{
			for(icnt=ccnt;icnt<wordlen;icnt++)	word[icnt]=word[icnt+1]; // move the null, too.
			wordlen--;
			ccnt--;
		}
		else break;
	}
	for(ccnt=wordlen-1;ccnt>=0;ccnt--)
	{
		if(iswpunct(word[ccnt]))
		{
			for(icnt=ccnt;icnt<wordlen;icnt++) word[icnt]=word[icnt+1];	// move the null, too.
			wordlen--;
		}
		else break;
	}
}

void WordRemoveNumbers(wchar_t *word)
{
	int wordlen;
	int ccnt;
	int icnt;
	
	wordlen=wcslen(word);
	for(ccnt=0;ccnt<wordlen;ccnt++)
	{
		if(iswdigit(word[ccnt]))
		{
			for(icnt=ccnt;icnt<wordlen;icnt++) word[icnt]=word[icnt+1];	// move the null, too.
			wordlen--;
			ccnt--;
		}
	}
}

void WordToLowerCase(wchar_t *word)
{
	int wordlen;
	int ccnt;

	wordlen=wcslen(word);
	for(ccnt=0;ccnt<wordlen;ccnt++)
	{
		if(iswupper(word[ccnt])) word[ccnt]=towlower(word[ccnt]);
	}
}

bool WordCheck(wchar_t *word)
{
	int wordlen;
	int ccnt;

	wordlen=wcslen(word);

	if(wordlen < 1) return false;
	if( !iswalpha(word[0]) ) return false;
	if( !iswalpha(word[wordlen-1]) ) return false;

	for(ccnt=1;ccnt<wordlen-1;ccnt++)
	{
		if( iswalpha(word[ccnt]) ) continue;
		if( word[ccnt] == '-' ) continue;
		if( word[ccnt] == '\'' ) continue;
		return false;
	}
	return true;
}

unsigned long WordHash(wchar_t *word)
{
	unsigned long inhash;
	inhash = 0;

	int charcount;
	charcount=0;

	if(word[0] == 0) return 1;	// if word is null, return 1 as hash value
	else while(word[charcount] != 0)
	{
		inhash=	((inhash << 7)|(inhash >> 25)) ^ word[charcount];	// xor into the rotateleft(7) of inhash
		charcount++;							// and increment the count of characters in the word
	}
	return inhash;
}