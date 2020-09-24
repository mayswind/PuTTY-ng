#include "native_putty_common.h"
#include "window_interface.h"


#include "putty.h"

void ErrorExit(char * str) 
{ 
    LPVOID lpMsgBuf;
    char* buf;
    DWORD dw = GetLastError(); 

    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR) &lpMsgBuf,
        0, NULL );
    if (dw)
        buf = dupprintf("fatal error:%s failed with error (%d: %s)", str, dw, lpMsgBuf);
    else
        buf = dupprintf("fatal error:%s failed", str);

	MessageBoxA(WindowInterface::GetInstance()->getNativeTopWnd(), (LPCSTR)buf, "Error", MB_OK | MB_TOPMOST);

    sfree(buf);
    LocalFree(lpMsgBuf);
    ExitProcess(dw); 
}

//-----------------------------------------------------------------------

void win_bind_data(HWND hwnd, void *data)
{
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)data);
}

//-----------------------------------------------------------------------

void* win_get_data(HWND hwnd)
{
    return (void *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
}

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

void ltrim(char* str)
{
    int space = strspn(str, " \t");
    memmove(str, str+space, 1+strlen(str)-space);
}

//-----------------------------------------------------------------------
/* 
 * See if host is of the form user@host
 */
void takeout_username_from_host(Conf *cfg)
{
	char* host = conf_get_str(cfg, CONF_host);
	if (host[0] != '\0') {
	    char *atsign = strrchr(host, '@');
	    /* Make sure we're not overflowing the user field */
	    if (atsign) {
			char username[512] = {0};
    		if (atsign - host < sizeof username) {
    		    strncpy(username, host, atsign - host);
    		    username[atsign - host] = '\0';
				conf_set_str(cfg, CONF_username, username);
    		}
    		memmove(host, atsign + 1, 1 + strlen(atsign + 1));
			conf_set_str(cfg, CONF_host, host);
	    }
	}
}

//-----------------------------------------------------------------------
/*
 * Trim a colon suffix off the hostname if it's there. In
 * order to protect IPv6 address literals against this
 * treatment, we do not do this if there's _more_ than one
 * colon.
 */
void handle_host_colon(char *host)
{
    char *c = strchr(host, ':');

    if (c) {
	char *d = strchr(c+1, ':');
	if (!d)
	    *c = '\0';
    }

}

//-----------------------------------------------------------------------
/*
 * Remove any remaining whitespace from the hostname.
 */
void rm_whitespace(char *host)
{
    int p1 = 0, p2 = 0;
    while (host[p2] != '\0') {
    	if (host[p2] != ' ' && host[p2] != '\t') {
    	    host[p1] = host[p2];
    	    p1++;
    	}
    	p2++;
    }
    host[p1] = '\0';
}

//-----------------------------------------------------------------------

void adjust_host(Conf *cfg)
{
    /*
	 * Trim leading whitespace off the hostname if it's there.
	 */
	char* host = conf_get_str(cfg, CONF_host);
    ltrim(host);
	conf_set_str(cfg, CONF_host, host);
	
	/* See if host is of the form user@host */
	takeout_username_from_host(cfg);

	/*
	 * Trim a colon suffix off the hostname if it's there. In
	 * order to protect IPv6 address literals against this
	 * treatment, we do not do this if there's _more_ than one
	 * colon.
	 */
	host = conf_get_str(cfg, CONF_host);
	handle_host_colon(host);

	/*
	 * Remove any remaining whitespace from the hostname.
	 */
	rm_whitespace(host);
	conf_set_str(cfg, CONF_host, host);
}

void bringToForeground(HWND hwnd)
{
	if (GetForegroundWindow() != hwnd){
		//relation time of SetForegroundWindow lock
		DWORD lockTimeOut = 0;
		HWND  hCurrWnd = GetForegroundWindow();
		DWORD dwThisTID = GetCurrentThreadId(),
			dwCurrTID = GetWindowThreadProcessId(hCurrWnd, 0);

		//we need to bypass some limitations from Microsoft :)
		if (dwThisTID != dwCurrTID)
		{
			AttachThreadInput(dwThisTID, dwCurrTID, TRUE);

			SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &lockTimeOut, 0);
			SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, 0, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);

			AllowSetForegroundWindow(ASFW_ANY);
		}

		SetForegroundWindow(hwnd);
	}
}

