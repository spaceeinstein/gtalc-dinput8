#pragma once

#include "vcclasses.h"

class CGaragesHack : public CGarages
{
public:
	static bool initialise();
	static bool HasImportExportGarageCollectedThisCar(short index, int car);
};

class CGarageHack : public CGarage
{
public:
	void MarkThisCarAsCollectedForCraig(int model);
	bool HasThisCarBeenCollected(int model);
	bool HasThisCarNotBeenCollected(int model);
};