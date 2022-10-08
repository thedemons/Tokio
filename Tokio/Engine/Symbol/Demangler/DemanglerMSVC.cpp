#include "stdafx.h"
#include "DemanglerMSVC.h"
#include "undname.h"

namespace Engine
{

std::string DemangleSymbolMSVC(const char* symbol)
{
	return __unDName(nullptr, symbol, 0, UNDNAME_NAME_ONLY);
}

}