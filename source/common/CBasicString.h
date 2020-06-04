// CBasicString.h: interface for the CBasicString class.
// Luis Gustavo de Brito
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CBasicString_H__F78A089F_05C5_42EA_BF72_DC87CD6FE2E3__INCLUDED_)
#define AFX_CBasicString_H__F78A089F_05C5_42EA_BF72_DC87CD6FE2E3__INCLUDED_

#include "CBasicStringList.h"

const int DEFAULT_STRING_LEN = 1;
const int DEFAULT_MEMORY_BLOCK_SIZE = 16;

class CBasicString
{
public:
	CBasicString ();
	CBasicString (const char* str);
	CBasicString (const CBasicString& otherString);
	virtual ~CBasicString();

	void setDefaultMemoryBlockLength (int len);
	int getDefaultMemoryBlockLength ();
	void clearAndFreeMemory ();

	bool equals (const CBasicString& otherString, bool caseSensitive = false);
	bool equals (const char* otherString, bool caseSensitive = false);
	void append (const CBasicString& otherString);
	void append (const char* otherString);
	void append (int count, const char character);
	void append (const char character);
	void append (const char* otherString, unsigned int len);

	int compare (const CBasicString& otherString, bool caseSensitive = false);
	int compare (const char* otherString, bool caseSensitive = false);

	int nCompare (const CBasicString& otherString, int len, bool caseSensitive = false);
	int nCompare (const char* otherString, int len, bool caseSensitive = false);
	
	bool startWith (const CBasicString& otherString, bool caseSensitive = false);
	bool startWith (const char* otherString, bool caseSensitive = false);

	bool endWith (const CBasicString& otherString, bool caseSensitive = false);
	bool endWith (const char* otherString, bool caseSensitive = false);

	int firstIndexOf (const char character);
	int firstIndexOfAny (const CBasicString& characteres);
	int firstIndexOfAny (const char* characteres);

	int lastIndexOf (const char character);
	int lastIndexOfAny (const CBasicString& characteres);
	int lastIndexOfAny (const char* characteres);

	void subString (int startIndex, CBasicString& outStr);
	void subString (int startIndex, int len, CBasicString& outStr);

	void setValue (const CBasicString& otherString);
	void setValue (const char* otherString);
	void setValue (int count, const char character);

	void padLeft (int totalLen, const char character = ' ');
	void padRight (int totalLen, const char character = ' ');

	static void toUpper (char* str);
	static void toLower (char* str);

	void toUpper ();
	void toLower ();

	int countChar (const char character, bool caseSensitive = false);
	int countStr(const CBasicString& str, bool caseSensitive = false);
	int countStr(const char* str, bool caseSensitive = false);

	void centralize (int len, char padCharacter = ' ');

	void replace (const char oldCharacter, const char newCharacter);
	void replace (const CBasicString& oldString, const CBasicString& newString);
	void replace (const char* oldString, const char* newString);

	void split (const char character, CBasicStringList& outList);
	void split (const char* characteres, CBasicStringList& outList);

	void trim (const char character = ' ');
	void lTrim (const char character = ' ');
	void rTrim (const char character = ' ');

	int length();
	char at (int index);
	void clear ();
	const char* cStr ();
	void show();


	CBasicString& operator=(const CBasicString& otherString);
	CBasicString& operator=(const char* str);

	bool operator==(const CBasicString& otherString);
	bool operator==(const char* otherString);

	bool operator!=(const CBasicString& otherString);
	bool operator!=(const char* otherString);

	CBasicString& operator+(const CBasicString& otherString);
	CBasicString& operator+(const char* str);

	CBasicString& operator+=(const CBasicString& otherString);
	CBasicString& operator+=(const char* str);

	char operator[] (int index);


private:
	int nCompare (const char* otherString, int startIndex, int len, bool caseSensitive = false);
	void setValue (const char* otherString, int len);
	void adjustInternalAllocation (int size);
	void freeInternalAllocation ();


private:
	char* m_str;
	int m_length;
	int m_allocatedLen;
	int m_memoryBlockLen;
		
};

#endif // !defined(AFX_CBasicString_H__F78A089F_05C5_42EA_BF72_DC87CD6FE2E3__INCLUDED_)
