// https://www.hackerschool.org/HS_Boards/data/Lib_kernel/1160630745/050104.pdf
// www.stsc.co.kr
// https://docs.microsoft.com/en-us/previous-versions/windows/hardware/network/ff562312(v%3Dvs.85)
// https://docs.microsoft.com/en-us/previous-versions/windows/hardware/network/ff548976(v=vs.85)
// https://pastebin.com/tCHqNnJH
// https://www.codeproject.com/Articles/3405/Developing-Firewalls-for-Windows-2000-XP

#include "IPFilter.h"

SINGLE_LIST_ENTRY ipListHead;

struct filterList* first = NULL;
struct filterList* last = NULL;


PF_FORWARD_ACTION match_by_addr(UINT32 srcAddr, UINT32 destAddr) {
	PIP_FILTER_ENTRY entry = CONTAINING_RECORD(&ipListHead, IP_FILTER_ENTRY, SingleListEntry);
	while (entry != NULL) {
		if (entry->srcAddr == srcAddr && entry->destAddr == destAddr) {
			return PF_DROP;
		}
		if (entry->srcAddr == -1 && entry->destAddr == destAddr) {
			return PF_DROP;
		}
		if (entry->destAddr == -1 && entry->srcAddr == srcAddr) {
			return PF_DROP;
		}
		if (entry->destAddr == -1 && entry->srcAddr == -1) {
			return PF_DROP;
		}
		entry = CONTAINING_RECORD(entry->SingleListEntry.Next, IP_FILTER_ENTRY, SingleListEntry);
	}
	return PF_FORWARD;
}


//NTSTATUS MyPacketFilterExtension(
//	// IP 패킷 헤더
//	IN unsigned char* PacketHeader,
//	// 헤더를 포함하지 않는 패킷
//	IN unsigned char* Packet,
//	// IP 패킷 헤더의 길이를 제외한 패킷 길이
//	IN unsigned int PacketLength,
//	// ( ) 장치 인덱스 몇 번째 장치 인지
//	// 받은 패킷에 대해서
//	IN unsigned int RecvInterfaceIndex,
//	// ( ) 장치 인덱스 몇 번째 장치 인지
//	// 보내는 패킷에 대해서
//	IN unsigned int SendInterfaceIndex,
//	// IP 주소 형태
//	// 장치가 받은 주소
//	IN IPAddr RecvLinkNextHop,
//	// IP 주소 형태
//	// 장치가 보낼 주소
//	IN IPAddr SendLinkNextHop
//) {
//
//	UNREFERENCED_PARAMETER(PacketLength);
//	UNREFERENCED_PARAMETER(RecvInterfaceIndex);
//	UNREFERENCED_PARAMETER(SendInterfaceIndex);
//	UNREFERENCED_PARAMETER(RecvLinkNextHop);
//	UNREFERENCED_PARAMETER(SendLinkNextHop);
//
//	PIPV4_HEADER pipv4Header = (PIPV4_HEADER)PacketHeader;
//	int srcAddr = pipv4Header->SourceAddress;
//	int destAddr = pipv4Header->DestinationAddress;
//	int type = pipv4Header->TypeOfService;
//	if (type == 1) { // ICMP
//		PICMP_HEADER picmpHeader = (PICMP_HEADER)Packet;
//		DBG_UNREFERENCED_LOCAL_VARIABLE(picmpHeader);
//
//		// 딱히 파싱 필요 없어보임
//	}
//	else if (type == 6) { // TCP
//		PTCP_HEADER ptcpHeader = (PTCP_HEADER)Packet;
//		int dataOffset = (unsigned char)(ptcpHeader->offset_ns >> 4);
//		char* realData = ((char*)ptcpHeader) + dataOffset;
//		DBG_UNREFERENCED_LOCAL_VARIABLE(realData);
//		// cut until packet length and log it
//	}
//	else if (type == 17) { // UDP
//		PUDP_HEADER pudpHeader = (PUDP_HEADER)Packet;
//		char* realData = ((char*)pudpHeader) + 8;
//		DBG_UNREFERENCED_LOCAL_VARIABLE(realData);
//		// cut until packet length and log it
//	}
//
//	NTSTATUS result;
//	result = match_by_addr(srcAddr, destAddr);
//	if (result!= PF_PASS) {
//		return result;
//	}
//	return PF_FORWARD;
//}

// typedef struct IPHeader { UCHAR iph_verlen; UCHAR iph_tos; USHORT iph_length;
//        USHORT iph_id; USHORT iph_offset; UCHAR iph_ttl; UCHAR iph_protocol; USHORT iph_xsum; ULONG iph_src;
//        ULONG iph_dest; } IPHeader;

// typedef PF_FORWARD_ACTION (*PacketFilterExtensionPtr)(
//// IP 패킷 헤더
//IN unsigned char *PacketHeader,
//// 헤더를 포함하지 않는 패킷
//IN unsigned char *Packet,
//// IP 패킷 헤더의 길이를 제외한 패킷 길이
//IN unsigned int PacketLength,
//// ( ) 장치 인덱스 몇 번째 장치 인지
//// 받은 패킷에 대해서
//IN unsigned int RecvInterfaceIndex,
//// ( ) 장치 인덱스 몇 번째 장치 인지
//// 보내는 패킷에 대해서
//IN unsigned int SendInterfaceIndex,
//// IP 주소 형태
//// 장치가 받은 주소
//IN IPAddr RecvLinkNextHop,
//// IP 주소 형태
//// 장치가 보낼 주소
//IN IPAddr SendLinkNextHop
//);

// PF_FORWARD_ACTION . 은 아래와 같은 결과 값을 가질 수 있게 된다
//• PF_FORWARD
//패킷을 정상적으로 처리 하기 위해 시스템 상의 에 값을 넣는다 넣게 되면 해 IP Stack .
//당 패킷은 처리를 하기 위한 어플리케이션으로 넘어가게 되며 해당 어플리케이션에서는 받
//은 정보를 가지고 적절한 처리를 하게 된다.
//• PF_DROP
//패킷을 드롭 하게 된다 시스템 상의 에 해당 포인터를 넘겨 주지 않고 폐기를
//함으로써 어플리케이션은 해당 패킷을 받지 못하게 된다.
//• PF_PASS
//패킷을 그냥 통과 시킨다 에 넣지는 않지만 시스템 드라이버 내부는 통과 하게 . IP Stack
//된다 하지만 에 값을 넣지 않기 때문에 어플리케이션에서는 정상적인 패킷 데이 . IP Stack
//터를 받지 못하는 것으로 나온다.

// https://www.daniweb.com/programming/software-development/threads/200708/how-to-convert-uint32-to-ip-address-dot-format
// okay, well before you get into the gory details of using winsock for network programming with C, you should understand the basic exercise of converting one number system to another number system, using bitwise operators.
//
//consider the address:
//10.1.10.127
//
//if you convert each octet to a binary group
//
//10         1        10       127
//00001010  00000001  00001010  01111111
//then the 32 bit binary number as one single integer has the decimal value.
//
//1010000000010000101001111111(bin) = 167840383(dec)
//
//you can programmatically convert between these number systems in either direction. consider (and understand) this snippet:
//
//unsigned int  ipAddress = 167840383;
//unsigned char octet[4]  = {0,0,0,0};
//
//for (i=0; i<4; i++)
//{
//    octet[i] = ( ipAddress >> (i*8) ) & 0xFF;
//}


/*++

Routine Description:

	Get a reference to IpFilterDriver so we will be able to install the filter

Arguments:

	pDeviceObject - pointer to a pointer of device object

	pFileObject   - pointer to a pointer of file object

Return Value:

	STATUS_SUCCESS if successful,
	STATUS_UNSUCCESSFUL otherwise

--*/
NTSTATUS SetFilterFunction(PacketFilterExtensionPtr filterFunction)
{
	NTSTATUS status = STATUS_SUCCESS, waitStatus = STATUS_SUCCESS;
	UNICODE_STRING filterName;
	PDEVICE_OBJECT ipDeviceObject = NULL;
	PFILE_OBJECT ipFileObject = NULL;

	PF_SET_EXTENSION_HOOK_INFO filterData;

	KEVENT event;
	IO_STATUS_BLOCK ioStatus;
	PIRP irp;

	DbgPrint("Getting pointer to IpFilterDriver\n");

	//first of all, we have to get a pointer to IpFilterDriver Device
	RtlInitUnicodeString(&filterName, L"\\Device\\IpFilterDriver"); // DD_IPFLTRDRVR_DEVICE_NAME);   L"\\Device\\IpFilterDriver"

	status = IoGetDeviceObjectPointer(&filterName, STANDARD_RIGHTS_ALL, &ipFileObject, &ipDeviceObject);

	if (NT_SUCCESS(status))
	{
		//initialize the struct with functions parameters
		filterData.ExtensionPointer = filterFunction;

		//we need initialize the event used later by the IpFilterDriver to signal us
		//when it finished its work
		KeInitializeEvent(&event, NotificationEvent, FALSE);

		//we build the irp needed to establish fitler function
		irp = IoBuildDeviceIoControlRequest(IOCTL_PF_SET_EXTENSION_POINTER,
			ipDeviceObject,
			(PVOID)&filterData,
			sizeof(PF_SET_EXTENSION_HOOK_INFO),
			NULL,
			0,
			FALSE,
			&event,
			&ioStatus);


		if (irp != NULL)
		{
			// we send the IRP
			status = IoCallDriver(ipDeviceObject, irp);

			//and finally, we wait for "acknowledge" of IpDriverFilter
			if (status == STATUS_PENDING)
			{
				waitStatus = KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);

				if (waitStatus != STATUS_SUCCESS)
					DbgPrint("Error waiting for IpFilterDriver response.");
			}

			status = ioStatus.Status;

			if (!NT_SUCCESS(status))
				DbgPrint("Error, IO error with ipFilterDriver\n");
		}

		else
		{
			//if we cant allocate the space, we return the corresponding code error
			status = STATUS_INSUFFICIENT_RESOURCES;
			DbgPrint("Error building IpFilterDriver IRP\n");
		}

		if (ipFileObject != NULL)
			ObDereferenceObject(ipFileObject);

		ipFileObject = NULL;
		ipDeviceObject = NULL;
	}

	else
		DbgPrint("Error while getting the pointer %u\n", status);

	return status;
}




/*++

Routine Description:

	Add a rule to the filter list

Arguments:

	  pf - pointer to filter rule


Return Value:

	STATUS_SUCCESS if successful,
	STATUS_INSUFFICIENT_RESOURCES otherwise

--*/
NTSTATUS AddFilterToList(IPFilter* pf)
{
	struct filterList* aux = NULL;

	// first, we reserve memory (non paged) to the new filter
	aux = (struct filterList*)ExAllocatePoolZero(NonPagedPool, sizeof(struct filterList), 'helo');

	if (aux == NULL)
	{
		DbgPrint("Problem reserving memory\n");

		return STATUS_INSUFFICIENT_RESOURCES;
	}

	//fill the new structure
	aux->ipf.destinationIp = pf->destinationIp;
	aux->ipf.sourceIp = pf->sourceIp;

	aux->ipf.destinationMask = pf->destinationMask;
	aux->ipf.sourceMask = pf->sourceMask;

	aux->ipf.destinationPort = pf->destinationPort;
	aux->ipf.sourcePort = pf->sourcePort;

	aux->ipf.protocol = pf->protocol;

	aux->ipf.drop = pf->drop;

	//Add the new filter to the filter list
	if (first == NULL)
	{
		first = last = aux;

		first->next = NULL;
	}

	else
	{
		last->next = aux;
		last = aux;
		last->next = NULL;
	}

	DbgPrint("Rule Added\n\t%x %x\n\t%x %x\n\t%x\n\t%x", aux->ipf.sourceIp
		, aux->ipf.sourceMask
		, aux->ipf.destinationIp
		, aux->ipf.destinationMask
		, aux->ipf.sourcePort
		, aux->ipf.destinationPort);

	return STATUS_SUCCESS;
}



/*++

Routine Description:

	Remove the linked list where the rules were saved.

Arguments:


Return Value:


--*/
void ClearFilterList(void)
{
	struct filterList* aux = NULL;

	//free the linked list
	DbgPrint("Removing the filter List...");

	while (first != NULL)
	{
		aux = first;
		first = first->next;
		ExFreePool(aux);

		DbgPrint("One Rule removed");
	}

	first = last = NULL;

	DbgPrint("Removed is complete.");
}


/*++

Routine Description:

	Filter each packet is received or sended

	To see parameters and return you can read it in MSDN
--*/

PF_FORWARD_ACTION cbFilterFunction(
	IN unsigned char* PacketHeader,
	IN unsigned char* Packet,
	IN unsigned int PacketLength,
	IN unsigned int RecvInterfaceIndex,
	IN unsigned int SendInterfaceIndex,
	IN unsigned long RecvLinkNextHop,
	IN unsigned long SendLinkNextHop)
{
	UNREFERENCED_PARAMETER(SendLinkNextHop);
	UNREFERENCED_PARAMETER(RecvLinkNextHop);
	UNREFERENCED_PARAMETER(SendInterfaceIndex);
	IPPacket* ipp = NULL;
	TCPHeader* tcph = NULL;
	UDPHeader* udph = NULL;

	int countRule = 0;

	struct filterList* aux = first;

	// we "extract" the ip Header 
	ipp = (IPPacket*)PacketHeader;

	DbgPrint("Destination: %x, %d", PacketLength, RecvInterfaceIndex);
	DbgPrint("Source: %x\nDestination: %x\nProtocol: %d", ipp->ipSource, ipp->ipDestination, ipp->ipProtocol);

	// TCP -> protocol = 6
	// we accept all packets of established connections
	if (ipp->ipProtocol == 6)
	{
		tcph = (TCPHeader*)Packet;

		DbgPrint("FLAGS: %x\n", tcph->flags);

		//if we havent the bit SYN activate, we pass the packets
		if (!(tcph->flags & 0x02))
			return PF_FORWARD;
	}

	//otherwise, we compare the packet with our rules
	while (aux != NULL)
	{
		DbgPrint("Comparing with Rule %d", countRule);

		//if protocol is the same....
		if (aux->ipf.protocol == 0 || ipp->ipProtocol == aux->ipf.protocol)
		{
			//we look in source Address
			if (aux->ipf.sourceIp != 0 && (ipp->ipSource & aux->ipf.sourceMask) != aux->ipf.sourceIp)
			{
				aux = aux->next;

				countRule++;
				continue;
			}

			// we look in destination address
			if (aux->ipf.destinationIp != 0 && (ipp->ipDestination & aux->ipf.destinationMask) != aux->ipf.destinationIp)
			{
				aux = aux->next;

				countRule++;
				continue;
			}

			//if we have a tcp packet, we look in ports
			//tcp, protocol = 6
			if (ipp->ipProtocol == 6)
			{
				if (aux->ipf.sourcePort == 0 || tcph->sourcePort == aux->ipf.sourcePort)
				{
					if (aux->ipf.destinationPort == 0 || tcph->destinationPort == aux->ipf.destinationPort) //puerto tcp destino
					{
						//now we decided what to do with the packet
						if (aux->ipf.drop)
							return  PF_DROP;
						else
							return PF_FORWARD;
					}
				}
			}

			//udp, protocol = 17
			else if (ipp->ipProtocol == 17)
			{
				udph = (UDPHeader*)Packet;

				if (aux->ipf.sourcePort == 0 || udph->sourcePort == aux->ipf.sourcePort)
				{
					if (aux->ipf.destinationPort == 0 || udph->destinationPort == aux->ipf.destinationPort)
					{
						//now we decided what to do with the packet
						if (aux->ipf.drop)
							return  PF_DROP;

						else
							return PF_FORWARD;
					}
				}
			}

			else
			{
				//for other packet we dont look more and ....
				//now we decided what to do with the packet
				if (aux->ipf.drop)
					return  PF_DROP;
				else
					return PF_FORWARD;
			}
		}

		//compare with the next rule
		countRule++;
		aux = aux->next;
	}

	//we accept all not registered
	return PF_FORWARD;
}
