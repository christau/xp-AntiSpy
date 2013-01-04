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

//methods:
//		CServicesControl(bool) 
//			tells the class to whether enumerate services to an internal vector or not
//		GetServiceInfo(const UINT i, SELF_SERVICE_INFO& sInfo)
//			gets a SELF_SERVICE_INFO structure wich contains information bout the desired
//			service, only works if class was created using CServicesControl(true)
//		SetStartType(const DWORD ,const LPSTR)
//			changes the starttype of the desired service
//			LPSTR has to be the ServiceName, not the DisplayName
//		GetStartType(const LPSTR) const;
//			returns the startType value
//		StopService(const LPSTR);
//		StartService(const LPSTR);

//---StartTypes---
//BOOT_START	0
//SYSTEM_START	1
//AUTO_START	2
//DEMAND_START	3
//DISABLED		4

#if !defined(AFX_SERVICESCONTROL_H__EB0E6288_F2B1_4F06_A708_E7BD38BA4689__INCLUDED_)
#define AFX_SERVICESCONTROL_H__EB0E6288_F2B1_4F06_A708_E7BD38BA4689__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsvc.h>
#include <vector>


class CServicesControl  
{
public:
	typedef struct
	{
		CString ServiceName;
		CString DisplayName;
		CString BinaryPath;
		CString ServiceStartName;
		DWORD dwServiceType;
		DWORD dwStartType;
		DWORD dwErrorControl;
		DWORD dwCurrentState;
	}SELF_SERVICE_INFO;
private:
	QUERY_SERVICE_CONFIG* m_pQueryService;
	std::vector<SELF_SERVICE_INFO> m_ServiceStatus;
public:
	void GetServiceInfo(const UINT i, SELF_SERVICE_INFO& sInfo);	
	BOOL SetStartType(const DWORD ,LPCTSTR) const;
	DWORD GetStartType(LPCTSTR) const;
	DWORD m_dwServiceCount;
	BOOL StopService(LPCTSTR);
	BOOL StartService(LPCTSTR);
	BOOL IsChangeable(LPCTSTR ServiceName) const;
	CServicesControl(const bool);
	virtual ~CServicesControl();
};

#endif // !defined(AFX_SERVICESCONTROL_H__EB0E6288_F2B1_4F06_A708_E7BD38BA4689__INCLUDED_)
