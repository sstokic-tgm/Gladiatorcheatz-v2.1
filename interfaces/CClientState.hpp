#pragma once

#include <cstdint>

typedef enum
{
	NA_NULL = 0,
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
} netadrtype_t;

typedef struct netadr_s
{
public:
	netadrtype_t	type;
	unsigned char	ip[4];
	unsigned short	port;
} netadr_t;

class INetMessageBinder;
class IDemoRecorder;
class INetMessage;
class INetChannelHandler;

class INetChannel : public INetChannelInfo
{
public:

	virtual	~INetChannel(void) {};

	virtual void	SetDataRate(float rate) = 0;
	virtual bool	RegisterMessage(INetMessageBinder *msg) = 0;
	virtual bool	UnregisterMessage(INetMessageBinder *msg) = 0;
	virtual bool	StartStreaming(unsigned int challengeNr) = 0;
	virtual void	ResetStreaming(void) = 0;
	virtual void	SetTimeout(float seconds, bool bForceExact = false) = 0;
	virtual void	SetDemoRecorder(IDemoRecorder *recorder) = 0;
	virtual void	SetChallengeNr(unsigned int chnr) = 0;

	virtual void	Reset(void) = 0;
	virtual void	Clear(void) = 0;
	virtual void	Shutdown(const char *reason) = 0;

	virtual void	ProcessPlayback(void) = 0;
	virtual bool	ProcessStream(void) = 0;
	virtual void	ProcessPacket(struct netpacket_s* packet, bool bHasHeader) = 0;

	virtual bool	SendNetMsg(INetMessage &msg, bool bForceReliable = false, bool bVoice = false) = 0;

	virtual bool	SendData(bf_write &msg, bool bReliable = true) = 0;
	virtual bool	SendFile(const char *filename, unsigned int transferID, bool isReplayDemo) = 0;
	virtual void	DenyFile(const char *filename, unsigned int transferID, bool isReplayDemo) = 0;
	virtual void	RequestFile_OLD(const char *filename, unsigned int transferID) = 0;	// get rid of this function when we version the 
	virtual void	SetChoked(void) = 0;
	virtual int		SendDatagram(bf_write *data) = 0;
	virtual bool	Transmit(bool onlyReliable = false) = 0;

	virtual const netadr_t	&GetRemoteAddress(void) const = 0;
	virtual INetChannelHandler *GetMsgHandler(void) const = 0;
	virtual int				GetDropNumber(void) const = 0;
	virtual int				GetSocket(void) const = 0;
	virtual unsigned int	GetChallengeNr(void) const = 0;
	virtual void			GetSequenceData(int &nOutSequenceNr, int &nInSequenceNr, int &nOutSequenceNrAck) = 0;
	virtual void			SetSequenceData(int nOutSequenceNr, int nInSequenceNr, int nOutSequenceNrAck) = 0;

	virtual void	UpdateMessageStats(int msggroup, int bits) = 0;
	virtual bool	CanPacket(void) const = 0;
	virtual bool	IsOverflowed(void) const = 0;
	virtual bool	IsTimedOut(void) const = 0;
	virtual bool	HasPendingReliableData(void) = 0;

	virtual void	SetFileTransmissionMode(bool bBackgroundMode) = 0;
	virtual void	SetCompressionMode(bool bUseCompression) = 0;
	virtual unsigned int RequestFile(const char *filename, bool isReplayDemoFile) = 0;
	virtual float	GetTimeSinceLastReceived(void) const = 0;	// get time since last received packet in seconds

	virtual void	SetMaxBufferSize(bool bReliable, int nBytes, bool bVoice = false) = 0;

	virtual bool	IsNull() const = 0;
	virtual int		GetNumBitsWritten(bool bReliable) = 0;
	virtual void	SetInterpolationAmount(float flInterpolationAmount) = 0;
	virtual void	SetRemoteFramerate(float flFrameTime, float flFrameTimeStdDeviation) = 0;

	// Max # of payload bytes before we must split/fragment the packet
	virtual void	SetMaxRoutablePayloadSize(int nSplitSize) = 0;
	virtual int		GetMaxRoutablePayloadSize() = 0;

	// For routing messages to a different handler
	virtual bool	SetActiveChannel(INetChannel *pNewChannel) = 0;
	virtual void	AttachSplitPlayer(int nSplitPlayerSlot, INetChannel *pChannel) = 0;
	virtual void	DetachSplitPlayer(int nSplitPlayerSlot) = 0;

	virtual bool	IsRemoteDisconnected() const = 0;

	virtual bool	WasLastMessageReliable() const = 0;

	char pad_0000[20]; //0x0000
	bool m_bProcessingMessages; //0x0014
	bool m_bShouldDelete; //0x0015
	char pad_0016[2]; //0x0016
	int32_t m_nOutSequenceNr;    //0x0018 last send outgoing sequence number
	int32_t m_nInSequenceNr;     //0x001C last received incoming sequnec number
	int32_t m_nOutSequenceNrAck; //0x0020 last received acknowledge outgoing sequnce number
	int32_t m_nOutReliableState; //0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
	int32_t m_nInReliableState;  //0x0028 state of incoming reliable data
	int32_t m_nChokedPackets;    //0x002C number of choked packets
	char pad_0030[1044]; //0x0030
}; //Size: 0x0444

class CEventInfo
{
public:

	uint16_t classID; //0x0000 0 implies not in use
	char pad_0002[2]; //0x0002 
	float fire_delay; //0x0004 If non-zero, the delay time when the event should be fired ( fixed up on the client )
	char pad_0008[4]; //0x0008
	ClientClass *pClientClass; //0x000C
	void *pData; //0x0010 Raw event data
	char pad_0014[48]; //0x0014
}; //Size: 0x0044

class CClientState
{

public:

    void ForceFullUpdate()
    {
		m_nDeltaTick = -1;
    }

	char pad_0000[148]; //0x0000
	INetChannel *m_NetChannel; //0x0094
	char pad_0098[8]; //0x0098
	uint32_t m_nChallengeNr; //0x00A0
	char pad_00A4[100]; //0x00A4
	uint32_t m_nSignonState; //0x0108
	char pad_010C[8]; //0x010C
	float m_flNextCmdTime; //0x0114
	uint32_t m_nServerCount; //0x0118
	uint32_t m_nCurrentSequence; //0x011C
	char pad_0120[84]; //0x0120
	uint32_t m_nDeltaTick; //0x0174
	bool m_bPaused; //0x0178
	char pad_0179[7]; //0x0179
	uint32_t m_nViewEntity; //0x0180
	uint32_t m_nPlayerSlot; //0x0184
	char m_szLevelName[260]; //0x0188
	char m_szLevelNameShort[40]; //0x028C
	char m_szGroupName[40]; //0x02B4
	char pad_02DC[52]; //0x02DC
	uint32_t m_nMaxClients; //0x0310
	char pad_0314[18820]; //0x0314
	float m_flLastServerTickTime; //0x4C98
	bool insimulation; //0x4C9C
	char pad_4C9D[3]; //0x4C9D
	uint32_t oldtickcount; //0x4CA0
	float m_tickRemainder; //0x4CA4
	float m_frameTime; //0x4CA8
	uint32_t lastoutgoingcommand; //0x4CAC
	uint32_t chokedcommands; //0x4CB0
	uint32_t last_command_ack; //0x4CB4
	uint32_t command_ack; //0x4CB8
	uint32_t m_nSoundSequence; //0x4CBC
	char pad_4CC0[80]; //0x4CC0
	Vector viewangles; //0x4D10
	char pad_4D1C[208]; //0x4D1C
	CEventInfo *events; //0x4DEC
}; //Size: 0x4E1C

class INetMessage
{

public:

	virtual ~INetMessage();
};

template<typename T>
class CNetMessagePB : public INetMessage, public T {};

class CCLCMsg_Move
{

private:

	char __PAD0[0x8];

public:

	int numBackupCommands;
	int numNewCommands;
};

using CCLCMsg_Move_t = CNetMessagePB<CCLCMsg_Move>;

static_assert(FIELD_OFFSET(CClientState, m_NetChannel) == 0x0094, "Wrong struct offset");
static_assert(FIELD_OFFSET(CClientState, viewangles) == 0x4D10, "Wrong struct offset");