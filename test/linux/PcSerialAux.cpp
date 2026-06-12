#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <limits.h>
#include "PcSerialAux.h"

static CPcSerialAux *g_AllSerialAux[10]={0};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CPcSerialAux::EmptyAllSerial()
{
	for(int t=0;t<10;t++)
		g_AllSerialAux[t]=NULL;
}
void CPcSerialAux::AddToGobalPtr()
{
	for(int t=0;t<10;t++)
	{
		if(NULL==g_AllSerialAux[t])
		{
			g_AllSerialAux[t]=this;
            break;
		}
	}
}

CPcSerialAux::CPcSerialAux()
{
	m_pAgent=NULL;
	m_serFd=-1;
}
CPcSerialAux::~CPcSerialAux()
{
	Close();
}
//关闭serial
BOOL CPcSerialAux::Close( void )
{
	if(m_serFd>=0)
		close(m_serFd);
	m_serFd=-1;
	return TRUE;
}

int CPcSerialAux::WritePort(unsigned char *buf,int dwCharToWrite)
{
	if(m_serFd>=0)
		dwCharToWrite=write(m_serFd, buf, dwCharToWrite);
	else
		dwCharToWrite=0;
	return dwCharToWrite;
}

//打开serial
BOOL CPcSerialAux::OpenPort(int iPort,int speed)
{
	int iFd;
	struct termios opt;
	char cSerialName[15];
	if (iPort >= 10)
	{
		TRACE("no this serial:ttySP%d . \n", iPort);     //fixme
		exit(1);
	}

	sprintf(cSerialName, "/dev/ttySP%d", iPort);     //fixme     P
	TRACE("open serila name:%s \n", cSerialName);
	iFd = open(cSerialName, O_RDWR | O_NOCTTY);
	if(iFd < 0)
	{
		perror(cSerialName);
		return -1;
	}
	tcgetattr(iFd, &opt);
	unsigned int burd=B115200;
//	if((speed<115200)&&(speed>9600))
//        burd=B38400;
//	else if(speed<115200)
//        burd=B9600;
	if(speed==9600)
		burd=B9600;
	else if(speed==38400)
		burd=B38400;
	else if(speed==115200)
		burd=B115200;
	else if(speed==230400)
		burd=B230400;
	else
		burd=speed;
		
	cfsetispeed(&opt, burd);
	cfsetospeed(&opt, burd);
	/*     * raw mode     */
	opt.c_lflag   &=   ~(ECHO   |   ICANON   |   IEXTEN   |   ISIG);
	opt.c_iflag   &=   ~(BRKINT   |   ICRNL   |   INPCK   |   ISTRIP   |   IXON);
	opt.c_oflag   &=   ~(OPOST);
	opt.c_cflag   &=   ~(CSIZE   |   PARENB);
	opt.c_cflag   |=   CS8;
	/*     * '8' bytes can be read by serial     */
	opt.c_cc[VMIN]   =   1;
	opt.c_cc[VTIME]  =   0;
	if (tcsetattr(iFd,   TCSANOW,   &opt)<0)
	{
		return   -1;
	}
	m_serFd=iFd;
	AddToGobalPtr();
	m_bOpened=TRUE;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
CReadThread::CReadThread()
{
	Create();
	Run();
}
#define BUFFMAXLEN		(1000)			//最大1K
void* CReadThread::Entry()
{
	int iCount=0;
	DWORD dwBytes;
	BYTE *buf;
	buf = new BYTE[BUFFMAXLEN];
	fd_set fs_read;
	struct timeval tv_timeout;
	int index;
	while (1)
	{
		int maxfd=-1;
		FD_ZERO(&fs_read);
		for(int  index=0;index<10;index++)
		{
			if(g_AllSerialAux[index]&&(g_AllSerialAux[index]->m_serFd>=0))
			{
				FD_SET (g_AllSerialAux[index]->m_serFd, &fs_read);
				maxfd=maxfd>g_AllSerialAux[index]->m_serFd?maxfd:g_AllSerialAux[index]->m_serFd;
			}

		}
		tv_timeout.tv_sec = 0;
		tv_timeout.tv_usec = 2000000;
		int iRet = select(maxfd + 1, &fs_read, NULL, NULL, &tv_timeout);
		if (iRet)
		{
			for(int  index=0;index<10;index++)
			{
				if(g_AllSerialAux[index]&&(g_AllSerialAux[index]->m_serFd>=0))
				{
					/* 先判断一下fs_read这个被监视的句柄是否真的变成可读的了 */
					if(FD_ISSET(g_AllSerialAux[index]->m_serFd, &fs_read))
					{
						iCount = read(g_AllSerialAux[index]->m_serFd, buf, BUFFMAXLEN);
						if(index>0)
						{
						    int aa=0;
						}
						g_AllSerialAux[index]->m_pAgent->OnReceiveMsg(0,buf,iCount);
					}
				}
			}
		}
	}
	delete [] buf;
	return NULL;
}
CReadThread::~CReadThread()
{
}

