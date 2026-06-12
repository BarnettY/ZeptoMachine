// anInterface.cpp: implementation of the CanInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "CanInterface.h"
#include "../trace.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CanInterface::CanInterface()
{

}

CanInterface::~CanInterface()
{

}


BOOL CanInterface::StarCan()
{
    return TRUE;
}

BOOL CanInterface::ResetCan()
{
    return TRUE;
}

BOOL CanInterface::ReceiveCan(CAN_OBJ frameinfo[50],int &Count)
{
    Count=0;
    wxMilliSleep(10);
    return TRUE;
}

BOOL CanInterface::SendCan(CAN_OBJ frameinfo[50],int Count)
{
    return TRUE;
}

BOOL CanInterface::OpenCan(Baud_TYPE baud)
{
    return TRUE;
}

BOOL CanInterface::CloseCan()
{
    return TRUE;
}

ComInterface::ComInterface()
{

}

ComInterface::~ComInterface()
{

}