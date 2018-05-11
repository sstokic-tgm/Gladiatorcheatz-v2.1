#pragma once

#include "../misc/CUserCmd.hpp"

#define MULTIPLAYER_BACKUP 150

class bf_write;
class bf_read;

class CInput
{
public:
    virtual void  Init_All(void);
    virtual void  Shutdown_All(void);
    virtual int   GetButtonBits(int);
    virtual void  CreateMove(int sequence_number, float input_sample_frametime, bool active);
    virtual void  ExtraMouseSample(float frametime, bool active);
    virtual bool  WriteUsercmdDeltaToBuffer(bf_write *buf, int from, int to, bool isnewcommand);
    virtual void  EncodeUserCmdToBuffer(bf_write& buf, int slot);
    virtual void  DecodeUserCmdFromBuffer(bf_read& buf, int slot);

	CUserCmd *GetUserCmd(int nSlot, int sequence_number)
	{
		typedef CUserCmd*(__thiscall *GetUserCmd_t)(void*, int, int);
		return VT::vfunc<GetUserCmd_t>(this, 8)(this, nSlot, sequence_number);
	}

    inline CUserCmd* GetUserCmd(int sequence_number);
    inline CVerifiedUserCmd* GetVerifiedCmd(int sequence_number);

	bool                m_fTrackIRAvailable;
	bool                m_fMouseInitialized;
	bool                m_fMouseActive;
	bool                m_fJoystickAdvancedInit;
	char                pad_0x08[0x2C];
	void*               m_pKeys;
	char                pad_0x38[0x64];
	int                 pad_0x41;
	int                 pad_0x42;
	bool                m_fCameraInterceptingMouse;
	bool                m_fCameraInThirdPerson;
	bool                m_fCameraMovingWithMouse;
	Vector              m_vecCameraOffset;
	bool                m_fCameraDistanceMove;
	int                 m_nCameraOldX;
	int                 m_nCameraOldY;
	int                 m_nCameraX;
	int                 m_nCameraY;
	bool                m_CameraIsOrthographic;
	Vector              m_angPreviousViewAngles;
	Vector              m_angPreviousViewAnglesTilt;
	float               m_flLastForwardMove;
	int                 m_nClearInputState;
	char                pad_0xE4[0x8];
	CUserCmd*           m_pCommands;
	CVerifiedUserCmd*   m_pVerifiedCommands;
};

CUserCmd* CInput::GetUserCmd(int sequence_number)
{
    return &m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
}

CVerifiedUserCmd* CInput::GetVerifiedCmd(int sequence_number)
{
    return &m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP];
}