#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "errcode.h"
/****************************************************************************/
/*						 ³ö´í´úÂëµÄºê																						*/
/****************************************************************************/


wxString InTRACE_BYTE(wxString name,BYTE DATE)
{
	wxString ret;
	ret.Printf(_T("%s=%d ,"),name,DATE);
	return ret;
}
wxString InTRACE_BOOL(wxString name,BOOL DATE)
{
	wxString ret;
	ret.Printf(_T("%s=%s ,"),name,DATE?"TRUE":"FALSE");
	return ret;
}
wxString InTRACE_UINT(wxString name,UINT DATE)
{
	wxString ret;
	ret.Printf(_T("%s=%d ,"),name,DATE);
	return ret;
}
wxString InTRACE_int(wxString name,int DATE)
{
	wxString ret;
	ret.Printf(_T("%s=%d ,"),name,DATE);
	return ret;
}
wxString InTRACE_WORD(wxString name,WORD DATE)
{
	wxString ret;
	ret.Printf(_T("%s=%d ,"),name,DATE);
	return ret;
}
wxString InTRACE_DWORD(wxString name,DWORD DATE)
{
	wxString ret;
	ret.Printf(_T("%s=%ld ,"),name,DATE);
	return ret;
}

wxString InTRACE_DWORD_ARY(wxString name,DWORD DATE[],UINT LEN)
{
	wxString ret;
	for(UINT i=0;i<LEN;i++)
	{
		wxString strtemp;
		strtemp.Printf(_T("%s[%d]"),name,i);
		ret+=InTRACE_DWORD(strtemp,DATE[i]);
	}
	return ret;
}
