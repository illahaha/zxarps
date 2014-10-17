//////////////////////////////////////////////////
// protoinfo.h�ļ�

/*

����Э���ʽ
����Э����ʹ�õĺ�

*/


#ifndef __PROTOINFO_H__
#define __PROTOINFO_H__


#define ETHERTYPE_IP    0x0800
#define ETHERTYPE_ARP   0x0806
#define	ARP_REPLY	 0x0002			/* ARP reply */
#define ARPHRD_ETHER 	1
#define ARP_LEN		 48

#define HEAD_LEN           54
#define TCP_MAXLEN       1460
#define PACKET_MAXLEN    1514
// Э��
#define PROTO_TCP     0x6
#define PROTO_UDP     0x11

typedef BYTE u_char;
typedef BYTE UCHAR;

#pragma pack(push, 1)//ȡ���ڴ��С�Զ�����

typedef struct ip_address
{
    u_char byte1;
    u_char byte2;
    u_char byte3;
    u_char byte4;
}ip_address;


typedef struct _ETHeader         // 14�ֽڵ���̫ͷ
{
	UCHAR	dhost[6];			// Ŀ��MAC��ַdestination mac address
	UCHAR	shost[6];			// ԴMAC��ַsource mac address
	USHORT	type;				// �²�Э�����ͣ���IP��ETHERTYPE_IP����ARP��ETHERTYPE_ARP����
} ETHeader, *PETHeader;

typedef struct _ARPHeader		// 28�ֽڵ�ARPͷ
{
	USHORT	hrd;				//	Ӳ����ַ�ռ䣬��̫����ΪARPHRD_ETHER
	USHORT	eth_type;			//  ��̫�����ͣ�ETHERTYPE_IP ����
	UCHAR	maclen;				//	MAC��ַ�ĳ��ȣ�Ϊ6
	UCHAR	iplen;				//	IP��ַ�ĳ��ȣ�Ϊ4
	USHORT	opcode;				//	�������룬ARPOP_REQUESTΪ����ARPOP_REPLYΪ��Ӧ
	UCHAR	smac[6];			//	ԴMAC��ַ
	ULONG	saddr;			//	ԴIP��ַ
	UCHAR	dmac[6];			//	Ŀ��MAC��ַ
	ULONG	daddr;			//	Ŀ��IP��ַ
} ARPHeader, *PARPHeader;

typedef struct _IPHeader		// 20�ֽڵ�IPͷ
{
    UCHAR     iphVerLen;      // �汾�ź�ͷ���ȣ���ռ4λ��
    UCHAR     ipTOS;          // �������� 
    USHORT    ipLength;       // ����ܳ��ȣ�������IP���ĳ���
    USHORT    ipID;			  // �����ʶ��Ωһ��ʶ���͵�ÿһ�����ݱ�
    USHORT    ipFlags;	      // ��־
    UCHAR     ipTTL;	      // ����ʱ�䣬����TTL
    UCHAR     ipProtocol;     // Э�飬������TCP��UDP��ICMP��
    USHORT    ipChecksum;     // У���
	union {
		unsigned int   ipSource;
		ip_address ipSourceByte;
	};
	union {
		unsigned int   ipDestination;
		ip_address ipDestinationByte;
	};
} IPHeader, *PIPHeader; 

typedef struct _TCPHeader		// 20�ֽڵ�TCPͷ
{
	USHORT	sourcePort;			// 16λԴ�˿ں�
	USHORT	destinationPort;	// 16λĿ�Ķ˿ں�
	ULONG	sequenceNumber;		// 32λ���к�
	ULONG	acknowledgeNumber;	// 32λȷ�Ϻ�
	UCHAR	dataoffset;			// ��4λ��ʾ����ƫ��
	UCHAR	flags;				// 6λ��־λ
								//FIN - 0x01
								//SYN - 0x02
								//RST - 0x04 
								//PUSH- 0x08
								//ACK- 0x10
								//URG- 0x20
								//ACE- 0x40
								//CWR- 0x80

	USHORT	windows;			// 16λ���ڴ�С
	USHORT	checksum;			// 16λУ���
	USHORT	urgentPointer;		// 16λ��������ƫ���� 
} TCPHeader, *PTCPHeader;

typedef struct _udphdr	//����UDP�ײ� 
{ 
	unsigned short uh_sport;	//16λԴ�˿� 
	unsigned short uh_dport;	//16λĿ�Ķ˿� 
	unsigned short uh_len;	//16λ���� 
	unsigned short uh_sum;	//16λУ��� 
}UDPHEADER, *PUDPHeader;

typedef struct _ACKPacket
{
	ETHeader	eh;
	IPHeader	ih;
	TCPHeader	th;
}ACKPacket;
/*
typedef struct _psd
{
	unsigned int ipSource;
	unsigned int ipDestination;
	unsigned char zero;
	unsigned char ipProtocol;
	unsigned short tcp_len;
}PSD,*PPSD;
*/
typedef struct _psd
{
    unsigned int   saddr;
    unsigned int   daddr;
    char           mbz;
    char           ptcl;
    unsigned short udpl;
}PSD,*PPSD;

typedef struct _dns
{
  unsigned short id;  //��ʶ��ͨ�����ͻ��˿��Խ�DNS��������Ӧ����ƥ�䣻

  unsigned short flags;  //��־��[QR | opcode | AA| TC| RD| RA | zero | rcode ]
                         //1 & htons(0x8000)
                         //4 & htons(0x7800)
                         //1 & htons(0x400)
                         //1 & htons(0x200)
                         //1 & htons(0x100)
                         //1 & htons(0x80)
                         //3
                         //4 & htons(0xF)

  unsigned short quests;  //������Ŀ��

  unsigned short answers;  //��Դ��¼��Ŀ��

  unsigned short author;  //��Ȩ��Դ��¼��Ŀ��

  unsigned short addition;  //������Դ��¼��Ŀ��

}TCPIP_DNS,*PDNS;
//��16λ�ı�־�У�QRλ�ж��ǲ�ѯ/��Ӧ���ģ�opcode�����ѯ���ͣ�AA�ж��Ƿ�Ϊ��Ȩ�ش�TC�ж��Ƿ�ɽضϣ�RD�ж��Ƿ������ݹ��ѯ��RA�ж��Ƿ�Ϊ���õݹ飬zero����Ϊ0��rcodeΪ�������ֶΡ�

//DNS��ѯ���ݱ���
typedef struct query
{
  //unsigned char  *name;  //��ѯ������,������,����һ����С��0��63֮����ַ�����

  unsigned short type;  //��ѯ���ͣ���Լ��20����ͬ������

  unsigned short classes;  //��ѯ��,ͨ����A��Ȳ�ѯIP��ַ��

}QUERY,*PQUERY;

//DNS��Ӧ���ݱ���
typedef struct response
{
  unsigned short name;   //��ѯ������

  unsigned short type;  //��ѯ����

  unsigned short classes;  //������

  unsigned int   ttl;  //����ʱ��

  unsigned short length;  //��Դ���ݳ���

  unsigned int   addr;  //��Դ����

}RESPONSE,*PRESPONSE;


#pragma pack(pop)

#endif // __PROTOINFO_H__

