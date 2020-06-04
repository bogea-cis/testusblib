// CTimeUtils.h: interface for the CTimeUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CTIMEUTILS_H__1805A278_E066_4E13_BCA0_548D1D94CCD2__INCLUDED_)
#define AFX_CTIMEUTILS_H__1805A278_E066_4E13_BCA0_548D1D94CCD2__INCLUDED_

class CTimeUtils
{
public:
	CTimeUtils ();
	virtual ~CTimeUtils();

	static void msleep(unsigned long milisec);

	void start (int timeout);
	
	bool checkTimeout ();
	
	int getElapsedTime ();

private:
	int m_timeout;
	int m_start;
	

};

#endif // !defined(AFX_CTIMEUTILS_H__1805A278_E066_4E13_BCA0_548D1D94CCD2__INCLUDED_)
