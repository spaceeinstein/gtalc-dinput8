/*******************************************************************************
	vcversion -

	Ported from Delphi source code courtesy of Artem Khassanov (Hammer83)
	Original file: vcversion.dpr
	http://www.artemkh.com/gtalc.php
*******************************************************************************/

#include <Windows.h>

namespace vcversion
{
	enum eGTAVersion
	{
		verUnknown,
		verGTAVC_US_10,
		verGTAVC_US_11,
		verGTAVC_AUS_10,
		verGTAVC_GER_10,
		verGTAVC_GER_11
	};

	unsigned int AdjustOffsetUS11(unsigned int Offset);
	unsigned int AdjustOffsetAUS10(unsigned int Offset);
	unsigned int AdjustOffsetGER10(unsigned int Offset);
	unsigned int AdjustOffsetGER11(unsigned int Offset);

	eGTAVersion gDetectedVersion = verUnknown;

	void DetectVersion()
	{
		unsigned long *p = reinterpret_cast<unsigned long *>(0x006339D0);
		DWORD oldProtect;

		if (VirtualProtect(p, 0x10, PAGE_READWRITE, &oldProtect))
		{
			switch (*p)
			{
			case 0xFFE8874C: gDetectedVersion = verGTAVC_US_10; break;
			case 0x0000CC24: gDetectedVersion = verGTAVC_US_11; break;
			case 0xFFE8885C: gDetectedVersion = verGTAVC_AUS_10; break;
			case 0xFFE8875C: gDetectedVersion = verGTAVC_GER_10; break;
			case 0x0000DC24: gDetectedVersion = verGTAVC_GER_11; break;
			}
			VirtualProtect(p, 0x10, oldProtect, NULL);
		}
	}

	unsigned long AdjustOffset(unsigned long Offset)
	{
		if (gDetectedVersion == verUnknown)
		{
			DetectVersion();
		}

		switch (gDetectedVersion)
		{
		case verGTAVC_US_10: return Offset;
		case verGTAVC_US_11: return AdjustOffsetUS11(Offset);
		case verGTAVC_AUS_10: return AdjustOffsetAUS10(Offset);
		case verGTAVC_GER_10: return AdjustOffsetGER10(Offset);
		case verGTAVC_GER_11: return AdjustOffsetGER11(Offset);
		}

		return Offset;
	}

	const char *GetVersionAsString()
	{
		switch (gDetectedVersion)
		{
		case verGTAVC_US_10: return "GTA-VC US v1.0";
		case verGTAVC_US_11: return "GTA-VC US v1.1";
		case verGTAVC_AUS_10: return "GTA-VC Australian v1.0";
		case verGTAVC_GER_10: return "GTA-VC German v1.0";
		case verGTAVC_GER_11: return "GTA-VC German v1.1";
		}

		return "GTA-VC Unknown Version";
	}
}
