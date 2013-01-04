/*
	Copyright (C) 2001 by Christian Taubenheim <mail at xp-antispy.org>
	This file is part of xp-AntiSpy http://www.xp-AntiSpy.org.

    xp-AntiSpy is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    xp-AntiSpy is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with xp-AntiSpy. If not, see <http://www.gnu.org/licenses/
*/
#include "stdafx.h"
#include "ServicesControl.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CServicesControl::CServicesControl(const bool doEnum)
{
    if(doEnum)
	{
		SC_HANDLE scMan = ::OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
		if (scMan) 
		{
			ENUM_SERVICE_STATUS service;
			ENUM_SERVICE_STATUS* lpService;
			DWORD bytesNeeded;
			DWORD servicesReturned;
			DWORD resumeHandle = 0;
			BOOL rc = ::EnumServicesStatus(scMan,SERVICE_WIN32,SERVICE_STATE_ALL,&service,sizeof(service),
									  &bytesNeeded,&servicesReturned,&resumeHandle);
			if ((rc == FALSE) && (::GetLastError() == ERROR_MORE_DATA)) {
				DWORD bytes = bytesNeeded + sizeof(ENUM_SERVICE_STATUS);
				lpService = new ENUM_SERVICE_STATUS [bytes];//no good solution!
				::EnumServicesStatus(scMan,SERVICE_WIN32,SERVICE_STATE_ALL,lpService,bytes,
					&bytesNeeded,&servicesReturned,&resumeHandle);
				m_dwServiceCount = servicesReturned; 

				TCHAR Buffer[1024];
	
				QUERY_SERVICE_CONFIG *lpqch = (QUERY_SERVICE_CONFIG*)Buffer;
				
				for (DWORD ndx = 0; ndx < servicesReturned; ndx++) {
					
					SC_HANDLE sh = ::OpenService(scMan,lpService[ndx].lpServiceName,SERVICE_ALL_ACCESS);
					
					if (::QueryServiceConfig(sh,lpqch,sizeof(Buffer),&bytesNeeded))
					{
						SELF_SERVICE_INFO tmpInfo;
						tmpInfo.BinaryPath=		  lpqch->lpBinaryPathName;
						tmpInfo.DisplayName=	  lpqch->lpDisplayName;
						tmpInfo.dwCurrentState=	  lpService[ndx].ServiceStatus.dwCurrentState;
						tmpInfo.ServiceName=	  lpService[ndx].lpServiceName;
						tmpInfo.dwErrorControl=   lpqch->dwErrorControl;
						tmpInfo.dwServiceType=	  lpqch->dwServiceType;
						tmpInfo.dwStartType=	  lpqch->dwStartType;
						tmpInfo.ServiceStartName= lpqch->lpServiceStartName;
						m_ServiceStatus.push_back(tmpInfo);	
					}
					::CloseServiceHandle(sh);
				}
			}
			::CloseServiceHandle(scMan);
			delete [] lpService;
		}
	}
	else
	{
		m_dwServiceCount=0;

	}
}

CServicesControl::~CServicesControl()
{
}

BOOL CServicesControl::StartService(LPCTSTR ServiceName)
{
	SC_HANDLE scMan = ::OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if (scMan) 
	{
		SC_HANDLE sh = ::OpenService(scMan,ServiceName,SERVICE_ALL_ACCESS);
		BOOL ret=::StartService(sh,0,0);
		::CloseServiceHandle(sh);
		::CloseServiceHandle(scMan);
		return ret;
	}
	else return false;
}

BOOL CServicesControl::StopService(LPCTSTR ServiceName)
{
	SC_HANDLE scMan = ::OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if (scMan) 
	{
		SERVICE_STATUS* pSS= new SERVICE_STATUS;
		SC_HANDLE sh = ::OpenService(scMan,ServiceName,SERVICE_ALL_ACCESS);
		BOOL ret=::ControlService(sh,SERVICE_CONTROL_STOP,pSS);
		if(!ret)
		{
			DWORD errMsg=GetLastError();
			switch(errMsg)
			{
				case ERROR_ACCESS_DENIED:
					ret;
				break;
				case ERROR_DEPENDENT_SERVICES_RUNNING:
					ret;
				break;
				case ERROR_INVALID_SERVICE_CONTROL:
					ret;
				break;
				case ERROR_SERVICE_CANNOT_ACCEPT_CTRL:
					ret;
				break;
				case ERROR_SERVICE_NOT_ACTIVE:
					ret;
				break;
				case ERROR_SERVICE_REQUEST_TIMEOUT:
					ret;
				break;

			}
		}
		::CloseServiceHandle(sh);
		::CloseServiceHandle(scMan);
		delete pSS;
		return ret;
	}
	else return false;
}

BOOL CServicesControl::SetStartType(const DWORD dwStartType,LPCTSTR ServiceName) const
{
	SC_HANDLE scMan = ::OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if (scMan) 
	{
        TCHAR Buffer[1024];
		DWORD bytesNeeded;
		
		QUERY_SERVICE_CONFIG *lpqch = reinterpret_cast<QUERY_SERVICE_CONFIG*>(Buffer);
		SC_HANDLE sh = ::OpenService(scMan,ServiceName,SERVICE_ALL_ACCESS);
		BOOL ret;
		if(::QueryServiceConfig(sh,lpqch,sizeof(Buffer),&bytesNeeded))
		{
			lpqch->dwStartType=dwStartType;
			ret=::ChangeServiceConfig(sh,lpqch->dwServiceType,lpqch->dwStartType,
								  lpqch->dwErrorControl,lpqch->lpBinaryPathName,
								  lpqch->lpLoadOrderGroup,NULL,lpqch->lpDependencies,
								  lpqch->lpServiceStartName,NULL,lpqch->lpDisplayName);

		}
	::CloseServiceHandle(sh);
	::CloseServiceHandle(scMan);
	return ret;
	}

	return false;
}

DWORD CServicesControl::GetStartType(LPCTSTR ServiceName) const
{	
	SC_HANDLE scMan = ::OpenSCManager(NULL,NULL,STANDARD_RIGHTS_REQUIRED || SC_MANAGER_CONNECT);
    if (scMan) 
	{
        TCHAR Buffer[1024];
		DWORD bytesNeeded;
		
		QUERY_SERVICE_CONFIG *lpqch = reinterpret_cast<QUERY_SERVICE_CONFIG*>(Buffer);
		SC_HANDLE sh = ::OpenService(scMan,ServiceName,SERVICE_QUERY_CONFIG || SERVICE_QUERY_STATUS);
		if(::QueryServiceConfig(sh,lpqch,sizeof(Buffer),&bytesNeeded))
		{
			return lpqch->dwStartType;
		}
	::CloseServiceHandle(sh);
	::CloseServiceHandle(scMan);
	}

	return 5;//<--means an error has occured, can't be 0
}

BOOL CServicesControl::IsChangeable(LPCTSTR ServiceName) const
{	
	SC_HANDLE scMan = ::OpenSCManager(NULL,NULL,STANDARD_RIGHTS_REQUIRED || SC_MANAGER_CONNECT);
    if (scMan) 
	{
		SC_HANDLE sh = ::OpenService(scMan,ServiceName,SERVICE_ALL_ACCESS);
		if(sh != 0)
		{
			::CloseServiceHandle(sh);
			::CloseServiceHandle(scMan);
			return true;
		}
	::CloseServiceHandle(scMan);
	}

	return false;
}

void CServicesControl::GetServiceInfo(const UINT i, SELF_SERVICE_INFO &sInfo)
{
	sInfo.BinaryPath=		m_ServiceStatus[i].BinaryPath;
	sInfo.DisplayName=		m_ServiceStatus[i].DisplayName;
	sInfo.dwCurrentState=	m_ServiceStatus[i].dwCurrentState;
	sInfo.ServiceName=		m_ServiceStatus[i].ServiceName;
	sInfo.dwErrorControl=	m_ServiceStatus[i].dwErrorControl;
	sInfo.dwServiceType=	m_ServiceStatus[i].dwServiceType;
	sInfo.dwStartType=		m_ServiceStatus[i].dwStartType;
	sInfo.ServiceStartName= m_ServiceStatus[i].ServiceStartName;
}
