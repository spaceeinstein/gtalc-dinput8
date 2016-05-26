#include "CRunningScriptHack.h"
#include "CPagerHack.h"
#include "CScrollBarHack.h"
#include "Globals.h"
#include "vcclasses.h"
#include "vcversion.h"

#include <string.h>
#include <Windows.h>

using namespace VCGlobals;

unsigned long *ProcessOneCommandHackAddr = reinterpret_cast<unsigned long *>(vcversion::AdjustOffset(0x0044FDB6));

bool CRunningScriptHack::initialise()
{
	//Memory is protected from write (write protection of .text section removed at startup)
	union { bool (CRunningScriptHack::*func)(); unsigned long offset; } nasty = { &ProcessOneCommandHack };
	*ProcessOneCommandHackAddr = nasty.offset - reinterpret_cast<unsigned long>(ProcessOneCommandHackAddr + 1);
	return true;
}

bool CRunningScriptHack::ProcessOneCommandHack()
{
	CTheScripts::CommandsExecuted++;
	short opCode = MAKEWORD(CTheScripts::ScriptSpace[this->m_dwScriptIP], CTheScripts::ScriptSpace[this->m_dwScriptIP + 1]);
	m_dwScriptIP += 2;
	this->m_bNotFlag = opCode < 0;
	short opCodeNum = opCode & 0x7FFF;

	switch (opCodeNum)
	{
	case 0x014D:
		return this->_014D_text_pager();

	case 0x024C:
		return this->_024C_set_phone_message();
		break;

	case 0x034A:	// set_portland_complete
		CScrollBarHack::ms_PortlandComplete = true;
		break;

	case 0x034B:	// set_staunton_complete
		CScrollBarHack::ms_StauntonComplete = true;
		break;

	case 0x034C:	// set_shoreside_complete
		CScrollBarHack::ms_ShoresideComplete = true;
		break;

	case 0x0351:
		return this->_0351_is_nasty_game();
		break;

	case 0x03C2:
		return this->_03C2_is_phone_displaying_message();
		break;

	case 0x0410:
		return this->_0410_set_gang_ped_model_preference();
		break;

	case 0x0421:
		return this->_0421_force_rain();
		break;

	case 0x0422:
		return this->_0422_does_garage_contain_car();
		break;

	case 0x042A:
		return this->_042A_is_threat_for_ped_type();
		break;

	case 0x0444:
		return this->_0444_set_script_fire_audio();
		break;

	case 0x0447:
		return this->_0447_is_player_lifting_a_phone();
		break;
	}

	CTheScripts::CommandsExecuted--;
	m_dwScriptIP -= 2;
	// Call VC's CRunningScript::ProcessOneCommand
	return ProcessOneCommand();
}

bool CRunningScriptHack::_014D_text_pager()
{
	char pagerText[8];
	
	strcpy_s(pagerText, reinterpret_cast<char *>(&CTheScripts::ScriptSpace[this->m_dwScriptIP]));
	this->m_dwScriptIP += 8;
	
	this->CollectParameters(&this->m_dwScriptIP, 3);

	wchar_t *scmPagerText = TheText.Get(pagerText);

	CPagerHack *pager = reinterpret_cast<CPagerHack *>(&CUserDisplay::Pager); //YUCKY YUCKY!!! but it'll do the job
	pager->AddMessage(scmPagerText, ScriptParams[0].uint16, ScriptParams[1].uint16, ScriptParams[2].uint16);
	
	return 0;
}

bool CRunningScriptHack::_024C_set_phone_message()
{
	this->CollectParameters(&this->m_dwScriptIP, 1);
	char text[8];
	strcpy_s(text, reinterpret_cast<char *>(&CTheScripts::ScriptSpace[this->m_dwScriptIP]));
	this->m_dwScriptIP += 8;
	wchar_t *string = TheText.Get(text);
	auto SetPhoneMessage_JustOnce = (void(__thiscall *)(uintptr_t, DWORD, wchar_t *, wchar_t *, wchar_t *, wchar_t *, wchar_t *, wchar_t *))vcversion::AdjustOffset(0x43C430);
	SetPhoneMessage_JustOnce(vcversion::AdjustOffset(0x817CF0), ScriptParams[0].int32, string, NULL, NULL, NULL, NULL, NULL);
	return 0;
}

bool CRunningScriptHack::_0351_is_nasty_game()
{
	this->UpdateCompareFlag(*(BYTE *)vcversion::AdjustOffset(0x68DD68) != 0);
	return 0;
}

bool CRunningScriptHack::_03C2_is_phone_displaying_message()
{
	this->CollectParameters(&this->m_dwScriptIP, 1);
	this->UpdateCompareFlag((ScriptParams[0].int32 * 0x34 + vcversion::AdjustOffset(0x817CF0) + 8) == *(DWORD *)vcversion::AdjustOffset(0x7030E8));
	return 0;
}

bool CRunningScriptHack::_0410_set_gang_ped_model_preference()
{
	this->CollectParameters(&this->m_dwScriptIP, 2);
	*(BYTE *)(ScriptParams[0].int32 * 0x18 + vcversion::AdjustOffset(0x7D925C)) = (BYTE)ScriptParams[1].int32;
	return 0;
}

bool CRunningScriptHack::_0421_force_rain()
{
	this->CollectParameters(&this->m_dwScriptIP, 1);
	*(BYTE *)vcversion::AdjustOffset(0xA10B38) = (BYTE)(ScriptParams[0].int32 ? 1 : 0);
	return 0;
}

bool CRunningScriptHack::_0422_does_garage_contain_car()
{
	this->CollectParameters(&this->m_dwScriptIP, 2);
	auto IsEntityEntirelyInside3D = (BYTE(__thiscall *)(uintptr_t, uintptr_t, FLOAT))vcversion::AdjustOffset(0x430630);
	auto VehiclePoolGetStruct = (uintptr_t(__thiscall *)(void *, INT))vcversion::AdjustOffset(0x451C70);
	void **carPool = (void **)vcversion::AdjustOffset(0xA0FDE4);
	DWORD cgarage = vcversion::AdjustOffset(0x812668);
	this->UpdateCompareFlag(IsEntityEntirelyInside3D(ScriptParams[0].int32 * 0xA8 + cgarage, VehiclePoolGetStruct(*carPool, ScriptParams[1].int32), 0.0) != 0);
	return 0;
}

bool CRunningScriptHack::_042A_is_threat_for_ped_type()
{
	this->CollectParameters(&this->m_dwScriptIP, 2);
	this->UpdateCompareFlag(ScriptParams[1].int32 == (*(DWORD *)(*(DWORD *)(vcversion::AdjustOffset(0xA0DA64) + ScriptParams[0].int32 * 4) + 0x18) & ScriptParams[1].int32));
	return 0;
}

bool CRunningScriptHack::_0444_set_script_fire_audio()
{
	this->CollectParameters(&this->m_dwScriptIP, 2);
	DWORD fire = vcversion::AdjustOffset(0x97F8A0);
	*(BYTE *)(ScriptParams[0].int32 * 0x30 + fire + 7) = (BYTE)(ScriptParams[1].int32 ? 1 : 0);
	return 0;
}

bool CRunningScriptHack::_0447_is_player_lifting_a_phone()
{
	this->CollectParameters(&this->m_dwScriptIP, 1);
	this->UpdateCompareFlag(*(DWORD *)(*(DWORD *)(CWorld::Players + 0x2E * ScriptParams[0].int32) + 0x244) == 0x13);
	return 0;
}