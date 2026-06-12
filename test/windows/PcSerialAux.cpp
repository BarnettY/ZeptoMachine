#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "PcSerialAux.h"
#include "../trace.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPcSerialAux::CPcSerialAux()
{
	memset( &m_OverlappedRead, 0, sizeof( OVERLAPPED ) );
	memset( &m_OverlappedWrite, 0, sizeof( OVERLAPPED ) );
	m_hIDComDev = NULL;
	m_bOpened = FALSE;
	m_pAgent=NULL;
}
CPcSerialAux::~CPcSerialAux()
{
	Close();
} 
//关闭serial
BOOL CPcSerialAux::Close( void )
{
	if( !m_bOpened || m_hIDComDev == NULL ) 
		return TRUE;
	
	if( m_OverlappedRead.hEvent != NULL )
		CloseHandle( m_OverlappedRead.hEvent );
	if( m_OverlappedWrite.hEvent != NULL ) 
		CloseHandle( m_OverlappedWrite.hEvent );
	CloseHandle( m_hIDComDev );
	m_bOpened = FALSE;
	m_hIDComDev = NULL;
	return TRUE;
}

int CPcSerialAux::WritePort(unsigned char *buf,int dwCharToWrite)
{
	return SendData(buf,dwCharToWrite);
}

int CPcSerialAux::SendData(unsigned char *buffer, int size )
{
	if( !m_bOpened || m_hIDComDev == NULL )
		return 0;
	TRACE(_T("CPcSerialAux::SendData PC AD Len=%d\n"),size);

	BOOL bWriteStat;
	DWORD dwBytesWritten;
	
	bWriteStat = WriteFile( m_hIDComDev, buffer, size, &dwBytesWritten, &m_OverlappedWrite );
	if( !bWriteStat && ( GetLastError() == ERROR_IO_PENDING ) )
	{
		if( WaitForSingleObject( m_OverlappedWrite.hEvent, 1000 ) ) 
			dwBytesWritten = 0;
		else
		{
			GetOverlappedResult( m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, FALSE );
			m_OverlappedWrite.Offset += dwBytesWritten;
		}
	}
	return (int) dwBytesWritten;  
}

int CPcSerialAux::ReadDataWaiting( void )
{
	if( !m_bOpened || m_hIDComDev == NULL ) return( 0 );
	DWORD dwErrorFlags;
	COMSTAT ComStat;
	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	return (int)ComStat.cbInQue;
}

int CPcSerialAux::ReadData( void *buffer, int limit )
{
	if( !m_bOpened || m_hIDComDev == NULL ) return( 0 );
	
	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;
	int t;
	for(t=0;t<2;t++)
	{
		ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );   //当通讯错误时，清除通讯错误标识以执行输入输出操作
		if(ComStat.cbInQue )
			break;
		wxMilliSleep(50);
	}
	if(t>=2)
		return 0;
	dwBytesRead = (DWORD) ComStat.cbInQue;
	if( limit < (int) dwBytesRead ) 
		dwBytesRead = (DWORD) limit;
	
	bReadStatus = ReadFile( m_hIDComDev, buffer, dwBytesRead, &dwBytesRead, &m_OverlappedRead );
	if( !bReadStatus )
	{
		if( GetLastError() == ERROR_IO_PENDING )
		{
			UINT waitTm=200;
			WaitForSingleObject( m_OverlappedRead.hEvent, waitTm );
			return (int) dwBytesRead;
		}
		return 0;
	}
	
	return (int) dwBytesRead;
	
}
 
//打开serial
BOOL CPcSerialAux::OpenPort(int port,int speed)
{
	if(!Open(port,speed))//115200
		return FALSE;
	new CReadThread(this);
	return TRUE;
}

BOOL CPcSerialAux::Open( int nPort, int nBaud )
{
	if( m_bOpened ) 
		return TRUE;
	TCHAR  szPort[15];
	TCHAR  szComParams[50];
	DCB dcb;
	wsprintf( szPort, _T("COM%d"), nPort );
	m_hIDComDev = CreateFile( szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL );
	if( m_hIDComDev == NULL )
		return	FALSE;
	
	memset( &m_OverlappedRead, 0, sizeof( OVERLAPPED ) );
	memset( &m_OverlappedWrite, 0, sizeof( OVERLAPPED ) );
	
	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;
	SetCommTimeouts( m_hIDComDev, &CommTimeOuts );
	
	wsprintf( szComParams, _T("COM%d:%d,n,8,1"), nPort, nBaud );
	
	m_OverlappedRead.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_OverlappedWrite.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	
	dcb.DCBlength = sizeof( DCB );
	GetCommState( m_hIDComDev, &dcb );
	dcb.BaudRate = nBaud;
	dcb.ByteSize = 8;
	unsigned char ucSet;
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_DTRDSR ) != 0 );
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_RTSCTS ) != 0 );
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_XONXOFF ) != 0 );
	if( !SetCommState( m_hIDComDev, &dcb ) ||
		!SetupComm( m_hIDComDev, 10000, 10000 ) ||
		m_OverlappedRead.hEvent == NULL ||
		m_OverlappedWrite.hEvent == NULL )
	{
		DWORD dwError = GetLastError();
		if( m_OverlappedRead.hEvent != NULL ) CloseHandle( m_OverlappedRead.hEvent );
		if( m_OverlappedWrite.hEvent != NULL ) CloseHandle( m_OverlappedWrite.hEvent );
		CloseHandle( m_hIDComDev );
		return	FALSE;
	}
	m_bOpened = TRUE;
	return	m_bOpened;
}

//////////////////////////////////////////////////////////////////////
CReadThread::CReadThread(CPcSerialAux* pComm)
{
	m_pserial=pComm;
	Create();
	Run();
}
#define BUFFMAXLEN		(1*1000*1000)			//最大1M
void* CReadThread::Entry()
{
	int iCount=0;
	DWORD dwBytes;
	BYTE *buf;
	buf = new BYTE[BUFFMAXLEN];
	while (1)
	{
		if(m_pserial->ReadDataWaiting()==0)
		{
			wxMilliSleep(10);
			continue;
		}
		iCount=0;
		do
		{
			dwBytes=m_pserial->ReadData(&buf[iCount],1100);
			if(dwBytes>0)
			{
				iCount+=dwBytes;
				TRACE(_T("CPcSerialAux::ReadModuleThread dwBytes=%d\n"),dwBytes);
				if(iCount>=BUFFMAXLEN-1024)
				{
					TRACE(_T("++++++++++Module resv Over MaxLen!\n"));
					break;
				}
			}
			wxMilliSleep(5);
		}while(dwBytes>0);
		
		if(iCount>0)
		{
			TRACE(_T("CPcSerialAux::ReadModuleThread AD resv!Len=%d\n"),iCount);
			m_pserial->m_pAgent->OnReceiveMsg(0,buf,iCount);
		}
	}
	delete [] buf;    
	return NULL;
}
CReadThread::~CReadThread()
{
}

