#if !defined(_TRACEH_H_)
#define _TRACEH_H_


#ifdef NOTRACE
#undef TRACE
#define TRACE	NOOP
#endif
//extern void TCPTRACE(wxChar *msg, ... );
//extern void LOGTRACE(wxChar *msg, ... );

#endif
