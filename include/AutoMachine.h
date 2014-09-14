#ifndef _INCLUDE_AUTOMACHINE_H_
#define _INCLUDE_AUTOMACHINE_H_

#define KN       256
#define STATE  int

#define F_TERMINATE 0x80000000
#define FIND_MATCHCASE 0x01

#define MAX_FIND_LEN 		 256						//ÁõÃÍ¼Ó£¬×Ô¶¯»ú²éÕÒ½á¹û×î´ó³¤¶È

#define ID_NOID              -2    
#define ID_SPECIAL1          -3
#define ID_SPECIAL2          -4

typedef STATE (*STATESLIST)[KN+1]; 

typedef struct {
    unsigned char  *word;
    void * ptr;
    int    length;
    int    findnum;
    int    type;
    int    id;
}KEYWORD; 

typedef struct {
    STATESLIST states;
    KEYWORD    *wordlist;
    int       wordlistlength;
    int       wordsnum;
    STATE      wordslength;
    int       totalfind;
    STATE      usedstates;    
}MATCHENTRY;

class AutoMachine
{
	public:
		MATCHENTRY * strMatchInit(int nSize);
		int Addword(const char *word,int len, MATCHENTRY *entry, void *ptr);
		int AddwordWithID(int id, const char *word,int len, MATCHENTRY *entry);
		char *FindUntilNull(const char *p, int *word, STATE *state, MATCHENTRY *entry);
		char *FindUntilLen(int num, const char *p, int *word, STATE state, MATCHENTRY *entry);
		STATE FindAllUntilNull(char *p,STATE state, MATCHENTRY *entry, char * findword);
		int FindAllUntilNullWithID(const char *p,STATE state, MATCHENTRY *entry, char * findword,int& id);
		STATE FindAllUntilLen(int num, const char *p,STATE state, MATCHENTRY *entry);
		int Prepare(int model, MATCHENTRY *entry);
		void RemoveEntry(MATCHENTRY *entry);
		char * Getword(int word, MATCHENTRY *entry);
		int Getwordid(int word, MATCHENTRY *entry);
		int Getwordsnum(MATCHENTRY *entry);
		void Clear(int num, MATCHENTRY *entry);

	private:

};

#endif /* _INCLUDE_AUTOMACHINE_H_ */
