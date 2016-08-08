#include "vcclasses.h"
#include "vcversion.h"

static unsigned long g_CollectParameters = vcversion::AdjustOffset(0x00451010);
static unsigned long g_ProcessOneCommand = vcversion::AdjustOffset(0x0044FBE0);
static unsigned long g_UpdateCompareFlag = vcversion::AdjustOffset(0x00463F00);
static unsigned long g_GetPadState = vcversion::AdjustOffset(0x00460C00);

__declspec(naked) void CRunningScript::CollectParameters(unsigned int *scriptIP, int paramCount)
{
	__asm jmp g_CollectParameters;
}

__declspec(naked) bool CRunningScript::ProcessOneCommand()
{
	__asm jmp g_ProcessOneCommand;
}

__declspec(naked) void CRunningScript::UpdateCompareFlag(bool flag)
{
	__asm jmp g_UpdateCompareFlag;
}

__declspec(naked) short CRunningScript::GetPadState(unsigned short, unsigned short)
{
	__asm jmp g_GetPadState;
}
