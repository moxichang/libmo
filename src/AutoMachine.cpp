#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AutoMachine.h"

/*
 *  Function Name: StrMatchInit(int nSize)
 *  Purpose      : 初始化一个用来完成多模字符串匹配的MATCHENTRY
                   并完成其它的一些初始化工作
                   
 *  Parameter    : nSize : 需要匹配的关键字表容量
 */
MATCHENTRY* AutoMachine::strMatchInit(int nSize)
{
	MATCHENTRY *entry;
	int size;
	int hi;
	entry = (MATCHENTRY *)malloc(sizeof(MATCHENTRY));

	if (!entry)
		return NULL;

	size = (nSize > 2) ? nSize:2;

	//printf("nSize = %d\n", size);
	hi = sizeof(KEYWORD) * size;

	entry->wordlist = (KEYWORD *)malloc( hi);
	entry->wordlistlength=size;
	entry->wordsnum=0;
	entry->totalfind=0;
	entry->wordslength=0;
	entry->states=NULL;
	entry->usedstates=0;

	return entry;
}

/*
 *  Function Name: Addword
 *  Purpose      :向自动机的关键字列表中填加关键字
                   
 *  Parameter    :word指向关键字内容指针，len关键字长度，entry所属的状态机
 */
int AutoMachine::Addword(const char *word,int len, MATCHENTRY *entry, void *ptr)
{ 
	int length;
	int i;

	length = (len > 0)?len:strlen(word);

	if (!length) return -1;
	if (entry->wordsnum == entry->wordlistlength)
	{
		KEYWORD *templist;
		entry->wordlistlength += entry->wordlistlength;
		templist=(KEYWORD *)malloc(sizeof(KEYWORD)*(entry->wordlistlength));

		for (i = 0; i < entry->wordsnum; i++)
			templist[i] = entry->wordlist[i];

		if (entry->wordlist!=NULL)
		{
			free(entry->wordlist);
		} 
		entry->wordlist=templist;
	}

	entry->wordlist[entry->wordsnum].word=(unsigned char *)malloc(length+2);
	memcpy(entry->wordlist[entry->wordsnum].word, word, length);
	entry->wordlist[entry->wordsnum].word[length]=0;
	entry->wordlist[entry->wordsnum].length=length;
	entry->wordlist[entry->wordsnum].findnum=0;
	entry->wordlist[entry->wordsnum].ptr = ptr;
	entry->wordsnum++; 
	entry->wordslength+=length;
	return entry->wordsnum;
}

int AutoMachine::AddwordWithID(int id,const char *word,int len, MATCHENTRY *entry)
{ 
	int length,i;
	if (len>0) length=len;
	else  length=strlen(word);
	if (!length) return -1;
	if (entry->wordsnum==entry->wordlistlength)
	{
		KEYWORD *templist;
		entry->wordlistlength+=entry->wordlistlength;
		templist=(KEYWORD *)malloc(sizeof(KEYWORD)*(entry->wordlistlength));
		for (i=0;i<entry->wordsnum;i++)
			templist[i]=entry->wordlist[i];
		if (entry->wordlist!=NULL)
		{

			free(entry->wordlist);
		} 
		entry->wordlist=templist;
	}
	entry->wordlist[entry->wordsnum].word=(unsigned char *)malloc(length+2);
	memcpy(entry->wordlist[entry->wordsnum].word,word,length);
	entry->wordlist[entry->wordsnum].word[length]=0;
	entry->wordlist[entry->wordsnum].length=length;
	entry->wordlist[entry->wordsnum].findnum=0;
	entry->wordlist[entry->wordsnum].id=id;
	entry->wordsnum++; 
	entry->wordslength+=length;
	return entry->wordsnum;
}

int AutoMachine::Prepare(int model, MATCHENTRY *entry)
{
	int i,j;
	STATE  k,l,n;
	unsigned char c = 0;
	STATE *from,*to;
	int   first=0,last=0, is_prefix;    

	if (entry->wordsnum < 1) 
		return 0;
	if (entry->states != NULL) 
		free(entry->states);

	/* 第一阶段：生成状态转换图 */

	entry->states = (STATESLIST)malloc(sizeof(STATE)*(entry->wordslength+2)*(KN+1)); 
	memset((void *)entry->states,0,sizeof(STATE)*(entry->wordslength+2)*(KN+1));
	k = entry->wordsnum + 1;
	for (i = 0; i < entry->wordsnum; i++)
	{
		n = l = 0;
		is_prefix = 1;
		for (j = 0; j < entry->wordlist[i].length; j++)
		{
			l = n;
			c = entry->wordlist[i].word[j];

			if (!(model & FIND_MATCHCASE))
				if (c >= 'a' && c<= 'z')
					c = c - 32;

			if (entry->states[l][c] == 0) {
				is_prefix = 0;
				entry->states[l][c] = n = k++;
			}
			else 
				n = entry->states[l][c];
		}
		entry->states[l][c] = i + 1;

		if (n == k - 1) {
			/* 出现n == k - 1，有两种可能。
			 * 1。该关键字没有出现过：如else，递减k
			 *    最后一次entry->states[l][c] = n = k++
			 *    使得n指向结束的状态，k指向下一个空闲的状态
			 *    又因为循环外面的entry->states[l][c] = i + 1
			 *    使得结束状态由n变为i + 1，所以此时n也指向
			 *    一个空闲的状态，而且n = k - 1，所以递减k
			 *    保证不浪费空间。
			 * 2。可能是因为本关键字是上一个关键字的前缀，
			 *    而且上一个关键字的长度比本关键字大1，使得
			 *    n = entry->states[l][c]得到的n正好等于k-1，
			 *    例如abc和ab的情况。这里有个疑问：hhhh, xyz, abcd, efg会出现什么样的情况呢？
			 *    这也满足if语句，而且如果它们是全部的关键字的时候可以造成SEGMENT FAULT
			 *    entry->states[n]的内存没有分配。'hhhh, xyz'造成状态泄漏一次（k没有减1）
			 *    'abcd, efg'又造成泄漏一次，于是此时需要的状态是n = 1 + 4 + 3 + 1 + 4 + 3 = 16
			 *    entry->states[n]泄漏，因为entry->states只分配了16个（0-15）。
			 * 个人观点：
			 * 1。设局部变量is_prefix
			 * 2。for循环之前置is_prefix = 1;
			 * 3。if (entry->states[l][c] == 0)
			 entry->states[l][c] = n = k++;
			 * 改成
			 *    if (entry->states[l][c] == 0) {
			 entry->states[l][c] = n = k++;
			 is_prefix = 0;
			 }
			 * 3。if( (i>=1)&& ( entry->wordlist[i-1].length==(entry->wordlist[i].length+1) ))
			 *改成if(is_prefix == 1)
			 */

			if (is_prefix == 1) 
				memcpy(entry->states[i+1], entry->states[n], KN*sizeof(STATE));     	
			else
				k--; 
		}
		/* 表明第i个关键字是以前某个关键字的前缀，
		 * 所以需要拷贝长关键字的当前状态到当前关键字的结束状态
		 * 以保证状态转移可以从该关键字的技术状态平滑地继续下去 
		 */
		else 
			memcpy(entry->states[i+1], entry->states[n], KN*sizeof(STATE)); 

		entry->states[i+1][KN] = i + 1;
	}
	/* 指出本状态图最终使用的状态数目 */
	entry->usedstates = k;
	/* 重新分配状态图空间，以前分配的空间太大 */
	entry->states = (STATESLIST)realloc(entry->states,sizeof(STATE)*(k+3)*(KN+1)); 

	/* 第二阶段：用失败函数修改状态转换表。
	 * 失败函数在状态转换失败，即states[i][ch] = fail的时候发挥作用
	 * 此时无法找到下一个状态，所以必须填充
	 * 这样的states[i][ch]，以保证状态转换可以平滑地继续下去。 
	 */

	/* from和to组成失败函数，f(to[i]) = from[i]。
	 * 假设状态s用串u表示，状态t用串v表示，
	 * 那么f(s) = t意味着在所有状态中，v是u的最长的真后缀，而且是某个关键字的前缀。
	 */
	/* 初始大小是entry->wordsnum + 1是因为任一时刻的状态最多有entry->wordsnum个 
	 * 因为在任一时刻表示一个关键字的n个状态最多只有一个会出现在to队列中，
	 * from和to意义对应，所以也不会超过entry->wordsnum
	 */
	from = (STATE *)malloc(sizeof(STATE)*(entry->wordsnum+1));
	/* 自动机的状态图形成一个有向树的结构，开始状态0就是根节点。
	 * 程序使用一个深度优先的树查找算法，按照深度依次将状态插入
	 * to这个循环队列中。对每个关键字而言，都是先插入深度小状态的，
	 * 再插入深度大的状态，后者只有在前者出列的情况下才可能被插入，
	 * 所以任一时刻表示一个关键字的n个状态最多只有一个会出现在to队列中。
	 *
	 * 状态A的深度就是从开始状态到状态A的最短路径的长度
	 */
	to = (STATE *)malloc(sizeof(STATE)*(entry->wordsnum+1));
	for (i = 0; i < KN; i++)
		if (entry->states[0][i])
		{
			/* 对所有深度为1的状态s，都有f(s) = 0 */
			from[last] = 0;
			/* 深度1的状态入队列 */
			to[last++] = entry->states[0][i];
		}

	/* 只要to队列不为空 */
	while(first != last)
	{
		for (i = 0; i < KN; i++)
		{

			if (entry->states[to[first]][i])
			{ 
				l = entry->states[to[first]][i]; 
				from[last] = entry->states[from[first]][i];
				to[last]=l;
				/* 关键字s和t，s = utv，u对应的状态是A，ut对应的状态是B，s对应的状态是C
				 * 那么匹配的内容为s的时候，状态转换过程应该是
				 * （...A...B...C，.代表中间状态），这就出现一个问题当到达状态B的时候，
				 * 自动机必须知道现在有一个关键字可以匹配（失败函数的定义表明它一定表示
				 * 某个关键字的前缀，所以可以根据它达到目的），
				 * 所以在这里更改该状态的KN字节使其表示被匹配的关键字t。
				 */
				if (entry->states[from[last]][KN])
				{

					/* 这里的if语句也是为了得到最长的匹配关键字 
					 * 对大多数from[last] <= entry->wordsnum的情况，
					 * entry->states[from[last]][KN] == from[last];
					 * 但如果两个关键字后缀相同那么，长的关键字的
					 * entry->states[state][KN] != state，而是等于短的关键字的state，
					 * 所以当if (from[last]>=1 && from[last] <= entry->wordsnum)成立的时候
					 * 用from[last]而不是entry->states[from[last]][KN]
					 */

					/* 例如关键字'ab', 'ttabcd' */
					if (from[last]>=1 && from[last] <= entry->wordsnum)
						entry->states[to[last]][KN] = from[last];
					/* 例如关键字'ab', 'ttabcd', 'xttabzz' */
					else
						entry->states[to[last]][KN] = 
							entry->states[from[last]][KN];
				}
				last = (last+1) % (entry->wordsnum+1);  
			}
			else 
			{
				l=entry->states[from[first]][i];
				entry->states[to[first]][i]=l;
			}
		}
		first=(first+1) % (entry->wordsnum+1); 
	}

	if (!(model & FIND_MATCHCASE))
	{
		for(i=0; i<entry->usedstates; i++)
			for (j='a';j<='z';j++)
				entry->states[i][j] = entry->states[i][j-32];
	}

	free(from);
	free(to);
	return 1;
}

/*
 *  Function Name: FindUntilNull
 *  Purpose      :在指定文本中查找关键字，查找一个关键字即返回，
 返回出现关键字的位置，文本以NULL结束
 *  Parameter    :    p--输入的文本，
 word--返回查找到的关键字在该自动机关键字列表的序号，
 state--自动机当前停留的状态，
 entry--自动机
 */
char* AutoMachine::FindUntilNull(const char *p, int *word, STATE *state, MATCHENTRY *entry)
{  
	if (p == NULL) {
		return NULL;
	}
	if (!entry->states) 
		return NULL;

	if (*state < 0 || *state > entry->usedstates) 
	{
		*word=0;
		return NULL;
	}
	while(*p)
	{
		*state = entry->states[*state][(unsigned char)*p];
		if ( entry->states[*state][KN] )
		{
			*word = entry->states[*state][KN];
			/* 这里应该判断state而不是*word
			 * 根据Prepare函数，
			 * *word总是小于entry->wordsnum的，
			 * 但是因为失败函数的关系，此时的
			 * state不一定小于entry->wordsnum，
			 * 当state小于entry->wordsnum的时候，
			 * 为了防止后缀重复的情况，采用*word=state-1;
			 */
			if (*state <= entry->wordsnum)
				/* 这条语句保证返回的是最长的关键字
				 * 例如两个关键字'abc'和'abcd'，当
				 * 内容是'hiabcdhi'的时候返回abcd，
				 * 如果把*word=state-1;改成*word = *word - 1;
				 * 那么返回abc，这是由Prepare的第二个阶段决定的。
				 */
				*word=*state-1;
			else
				*word = *word - 1;
			return (char *)p; 
		}
		p++;
	}

	*word=*state;
	return NULL;
}

/*
 *  Function Name: FindAllUntilNull 
 *  Purpose      :查找出文本中所有出现的关键字,文本以NULL结束

 *  Parameter    :p--输入的文本，state--当前自动机停留的状态，entry--自动机
 */
int AutoMachine::FindAllUntilNull(char *p,STATE state, MATCHENTRY *entry, char * findword)
{
	int w,Find=0;


	// 	printf("i am in the function findalluntinull\n");
	memset(findword,0,strlen(findword));
	while(*p)
	{
		state=entry->states[state][(unsigned char)*p];
		if (entry->states[state][KN])
		{
			if (state<=entry->wordsnum)
			{
				entry->wordlist[state-1].findnum++;
				//printf("%s\n",entry->wordlist[state-1].word);
				//刘猛加，判断是否溢出
				if(strlen(findword) + strlen((char *)entry->wordlist[state-1].word) > MAX_FIND_LEN - 2)
					return 0;
				//刘猛加，结束

				strcat( findword , (const char *)entry->wordlist[state-1].word);
				Find=1;
				strcat( findword , " " );  //zby do  070702
				entry->totalfind++;     
			}

			w=state;    
			while (w!=(entry->states[w][KN]))
			{
				w=entry->states[w][KN];
				if(w<=entry->wordsnum)
				{   entry->wordlist[w-1].findnum++;
					//printf("%s\n",entry->wordlist[w-1].word);
					//刘猛加，判断是否溢出
					if(strlen(findword) + strlen((char *)entry->wordlist[state-1].word) > MAX_FIND_LEN - 2)
						return 0;
					//刘猛加，结束
					strcat( findword , (const char *)entry->wordlist[state-1].word);
					Find=1;
					strcat( findword , " " );    //zby do  070702
					entry->totalfind++;
				}

			}
		}
		p++;
	}
	/*if(ISSPAM==1)
	  printf("自动机检查为垃圾邮件\n");
	  printf("findword=%s\n",findword);*/
	return Find;
}

int AutoMachine::FindAllUntilNullWithID(const char *p,STATE state, MATCHENTRY *entry, char * findword,int& id)
{
	int w,Find=0;


	//  printf("i am in the function findalluntinullwithid\n");
	memset(findword,0,strlen(findword));
	while(*p)
	{
		state=entry->states[state][(unsigned char)*p];
		if (entry->states[state][KN])
		{
			if (state<=entry->wordsnum)
			{
				entry->wordlist[state-1].findnum++;
				//printf("1:%s\n",entry->wordlist[state-1].word);
				strcat( findword , (const char *)entry->wordlist[state-1].word);
				Find=1;

				id = entry->wordlist[state-1].id;      //zby add this 

				strcat( findword , "," );
				entry->totalfind++;     
			}

			w=state;    
			while (w!=(entry->states[w][KN]))
			{
				w=entry->states[w][KN];
				if(w<=entry->wordsnum)
				{   entry->wordlist[w-1].findnum++;
					//printf("2:%s\n",entry->wordlist[w-1].word);
					strcat( findword , (const char *)entry->wordlist[state-1].word);
					Find=1;

					id = entry->wordlist[state-1].id;    //zby add this 

					strcat( findword , "," );
					entry->totalfind++;
				}

			}
		}
		p++;
	}
	/*if(ISSPAM==1)
	  printf("自动机检查为垃圾邮件\n");
	  printf("findword=%s\n",findword);*/
	return Find;
}


/*
 *  Function Name: FindUntilLen
 *  Purpose      :在指定文本中查找关键字，查找一个关键字即返回，
 返回出现关键字的位置，直到文本长度LEN
 *  Parameter    :    p--输入的文本，
 word--返回查找到的关键字在该自动机关键字列表的序号，
 state--自动机当前停留的状态，
 entry--自动机
 */
char* AutoMachine::FindUntilLen(int num, const char *p, int *word, STATE state, MATCHENTRY *entry)
{  
	if (!entry->states) return NULL;
	if (state<0 || state>entry->usedstates) 
	{
		*word=0;
		return NULL;
	}
	while(num>0)
	{
		state=entry->states[state][(unsigned char)*p];
		if (entry->states[state][KN])
		{
			*word=entry->states[state][KN];
			if (*word & F_TERMINATE)
				*word=state-1;
			else
				*word = *word - 1;
			return (char *)p; 
		}

		p++;
		num--;
	}
	*word=state;
	return NULL;
}


/*
 *  Function Name: FindAllUntilLen 
 *  Purpose      :查找出文本中所有出现的关键字,直到规定的文本长度

 *  Parameter    :p--输入的文本，state--当前自动机停留的状态，entry--自动机
 */
STATE AutoMachine::FindAllUntilLen(int num, const char *p, STATE state, MATCHENTRY *entry)
{
	unsigned int w;
	while(num)
	{
		state=entry->states[state][(unsigned char)*p];
		if (entry->states[state][KN])
		{
			w=entry->states[state][KN];
			if (w & F_TERMINATE)
			{
				w=w&~F_TERMINATE;
				entry->wordlist[state-1].findnum++;
				entry->totalfind++;
			}
			entry->wordlist[w-1].findnum++;
			entry->totalfind++;
			while (w!=(entry->states[w][KN]&~F_TERMINATE))
			{
				w=entry->states[w][KN];
				entry->wordlist[w-1].findnum++;
				entry->totalfind++;
			}
		}

		num--;
		p++;
	}
	return state;
}


void AutoMachine::RemoveEntry(MATCHENTRY *entry)
{
	int i;
	if (entry->wordlist!=NULL)
	{
		for (i=0;i<entry->wordsnum;i++)
			free(entry->wordlist[i].word);
		free(entry->wordlist);
	}
	if (entry->states!=NULL) free(entry->states);

}

char * AutoMachine::Getword(int word, MATCHENTRY *entry)
{
	if (word >= 0 && word < entry->wordsnum)
		return (char *)(entry->wordlist[word].word);
	return NULL;
}

int AutoMachine::Getwordid(int word, MATCHENTRY *entry)
{
	if (word >= 0 && word < entry->wordsnum)
		return (entry->wordlist[word].id);
	return -1;
}

int AutoMachine::Getwordsnum(MATCHENTRY *entry)
{
	return entry->wordsnum;
}

void AutoMachine::Clear(int num, MATCHENTRY *entry)
{
	int i;
	if (num<2) num=2;
	if (entry->wordlist!=NULL)
	{
		for (i=0;i<entry->wordsnum;i++)
			free(entry->wordlist[i].word);
		free(entry->wordlist);
	} 
	if (entry->states!=NULL) free(entry->states);
	entry->wordlist=(KEYWORD *)malloc(sizeof(KEYWORD)*(num));
	entry->wordlistlength=num;
	entry->wordsnum=0;
	entry->wordslength=0;
	entry->totalfind=0;
	entry->states=NULL;
	entry->usedstates=0;
}
