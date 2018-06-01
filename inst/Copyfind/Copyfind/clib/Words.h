

class CWordVocab
{
	private:
		struct VocabEntry
		{
			wchar_t word[WORDBUFFERLENGTH];
			int usecnt;
			unsigned long hash;
			VocabEntry *m_pNextEntry;
			VocabEntry *plastentry;
		};
		VocabEntry *pfirstentry;
	public:
		CWordVocab()
		{
			pfirstentry=NULL;
		}
		~CWordVocab()
		{
			VocabEntry *pentry;
			VocabEntry *pentrynext;

			pentry=pfirstentry;

			while( pentry != NULL)
			{
				pentrynext=pentry->m_pNextEntry;
				delete pentry;
				pentry=pentrynext;
			}
		}
		void AddWord(wchar_t *word);
		void ListWords(FILE *fvocab);
};

void WordRemovePunctuation(wchar_t *word);
void wordxouterpunct(wchar_t *word);
void WordRemoveNumbers(wchar_t *word);
void WordToLowerCase(wchar_t *word);
bool WordCheck(wchar_t *word);
unsigned long WordHash(wchar_t *word);