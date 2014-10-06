/************************************************************************/
/*      Copyright (C) Stas Khirman 1998.  All rights reserved.          */
/*      Written by Stas Khirman (staskh@rocketmail.com).                */
/*					  and	                        */
/*                 Raz Galili (razgalili@hotmail.com)		        */
/*                                                                      */
/*      Free software: no warranty; use anywhere is ok; spread the      */
/*      sources; note any modifications; share variations and           */
/*      derivatives (including sending to staskh@rocketmail.com).       */
/*                                                                      */
/*    http://www.cracklab.ru/pro/cpp.php?r=network&d=zdrt9              */
/************************************************************************/

#include <vcl.h>
#include <winsock.h>
#pragma hdrstop


#include "MibAccess.h"

MibExtLoad::MibExtLoad(LPSTR MibDllName)
{
	
	m_Init = NULL;	
	m_InitEx = NULL;
	m_Query = NULL;	
	m_Trap = NULL;
	
	m_hInst = LoadLibrary( MibDllName );
	if(m_hInst < (HINSTANCE) HINSTANCE_ERROR){
		m_hInst = NULL;
		return ;
	}		
	m_Init	=	(pSnmpExtensionInit)GetProcAddress(m_hInst ,"SnmpExtensionInit");
	m_InitEx=	(pSnmpExtensionInitEx)GetProcAddress(m_hInst ,"SnmpExtensionInitEx");
	m_Query	=	(pSnmpExtensionQuery)GetProcAddress(m_hInst ,"SnmpExtensionQuery");
	m_Trap	=	(pSnmpExtensionTrap)GetProcAddress(m_hInst ,"SnmpExtensionTrap");
	
}

MibExtLoad::~MibExtLoad()
{
	if (m_hInst)
		FreeLibrary( m_hInst );
	m_hInst = NULL;
}

BOOL MibExtLoad::Init(DWORD dwTimeZeroReference,HANDLE *hPollForTrapEvent,AsnObjectIdentifier *supportedView)
{
	if(m_hInst && m_Init)
		return m_Init(dwTimeZeroReference,hPollForTrapEvent,supportedView);
	return FALSE;
}
BOOL MibExtLoad::InitEx(AsnObjectIdentifier *supportedView)
{
	if(m_hInst && m_InitEx)
		return m_InitEx(supportedView);
	
	return FALSE;
}

BOOL MibExtLoad::Query(BYTE requestType,OUT RFC1157VarBindList *variableBindings,
					   AsnInteger *errorStatus,AsnInteger *errorIndex)
{
	if(m_hInst && m_Query)
		return m_Query(requestType,variableBindings,errorStatus,errorIndex);
	
	return FALSE;
}

BOOL MibExtLoad::Trap(AsnObjectIdentifier *enterprise, AsnInteger *genericTrap,
					  AsnInteger *specificTrap, AsnTimeticks *timeStamp, 
					  RFC1157VarBindList  *variableBindings)
{
	if(m_hInst && m_Trap)
		return m_Trap(enterprise, genericTrap,specificTrap, timeStamp, variableBindings);
	
	return FALSE;
}

MibII::MibII():MibExtLoad("inetmib1.dll")
{
  WSADATA wsa;
  //int err = WSAStartup( 0x0101, &wsa );
  WSAStartup( 0x0101, &wsa );
}

MibII::~MibII()
{
  WSACleanup();
}

MibII::Init()
{
  HANDLE PollForTrapEvent;
  AsnObjectIdentifier SupportedView;
  return MibExtLoad::Init(GetTickCount(),&PollForTrapEvent,&SupportedView);
}

// iso.org.dod.internet.mgmt.mib-2.ip.ipAddrTable.ipAddrEntry.ipAddress.IPADDRESSREALVALUE
BOOL MibII::GetIPAddress(UINT IpArray[], UINT & IpArraySize)
{
  UINT OID_ipAdEntAddr[] = { 1, 3, 6, 1, 2, 1, 4, 20, 1, 1 };
  AsnObjectIdentifier MIB_ipAdEntAddr = { sizeof(OID_ipAdEntAddr)/sizeof(UINT), OID_ipAdEntAddr };
  RFC1157VarBindList  varBindList;
  RFC1157VarBind      varBind[1];
  AsnInteger          errorStatus;
  AsnInteger          errorIndex;
  AsnObjectIdentifier MIB_NULL = {0,0};
  BOOL                Exit;
  int                 ret;
  int                 IpCount=0;
  DWORD               dtmp;

  varBindList.list = varBind;
  varBindList.len  = 1;
  varBind[0].name  = MIB_NULL;
  SNMP_oidcpy(&varBind[0].name,&MIB_ipAdEntAddr);
  Exit = FALSE;

  while(!Exit){
    ret = Query(ASN_RFC1157_GETNEXTREQUEST,&varBindList,&errorStatus,&errorIndex);

    if(!ret)
      Exit=TRUE;
    else{
      ret = SNMP_oidncmp(&varBind[0].name,&MIB_ipAdEntAddr,MIB_ipAdEntAddr.idLength);
      if(ret!=0){
        Exit=TRUE;
      }
      else{
        dtmp = *((DWORD *)varBind[0].value.asnValue.address.stream);
        IpArray[IpCount] = dtmp;
        IpCount++;
        if((UINT)IpCount>=IpArraySize)
                Exit = TRUE;
      }
    }
  }
  IpArraySize = IpCount;
  SNMP_FreeVarBind(&varBind[0]);
  return Exit;
}

BOOL MibII::GetIPMask(UINT IpArray[],UINT &IpArraySize)
{
  UINT OID_ipAdEntMask[] = { 1, 3, 6, 1, 2, 1, 4 , 20, 1 ,3 };
  AsnObjectIdentifier MIB_ipAdEntMask = { sizeof(OID_ipAdEntMask)/sizeof(UINT), OID_ipAdEntMask };
  RFC1157VarBindList  varBindList; // то-же что и SnmpVarBindList
  RFC1157VarBind      varBind[1];  // SnmpVarBind
  AsnInteger          errorStatus;
  AsnInteger          errorIndex;
  AsnObjectIdentifier MIB_NULL = {0,0};
  BOOL                Exit;
  int                 ret;
  int                 IpCount=0;
  DWORD               dtmp;

  varBindList.list = varBind;
  varBindList.len  = 1;
  varBind[0].name  = MIB_NULL;
  SNMP_oidcpy(&varBind[0].name,&MIB_ipAdEntMask); // то-же что и SnmpUtilOidCpy
  Exit = FALSE;
	
  while(!Exit){
    ret = Query(ASN_RFC1157_GETNEXTREQUEST,&varBindList,&errorStatus,&errorIndex);

    if(!ret)
      Exit=TRUE;
    else{
      ret = SNMP_oidncmp(&varBind[0].name,&MIB_ipAdEntMask,MIB_ipAdEntMask.idLength); // SnmpUtilOidNCmp
      if(ret!=0){
              Exit=TRUE;
      }
      else{
              dtmp = *((DWORD *)varBind[0].value.asnValue.address.stream);
              IpArray[IpCount] = dtmp;
              IpCount++;
              if((UINT)IpCount>=IpArraySize)
                      Exit = TRUE;
      }
    }
  }
	
  IpArraySize = IpCount;

  SNMP_FreeVarBind(&varBind[0]); // то-же что и SnmpUtilVarBindFree

  return Exit;
}

