#pragma once
#include "stdafx.h"
#include "BaseAnalyzer.h"

#include "Engine/Memory/BaseMemory.h"
#include "Engine/Symbol/BaseSymbol.h"
#include "Engine/Disassembler/BaseDisassembler.h"

namespace Engine
{

BaseAnalyzer::BaseAnalyzer(
	const std::shared_ptr<ProcessData>& target,
	const std::shared_ptr<BaseSymbol>& symbol,
	const std::shared_ptr<BaseMemory>& memory,
	const std::shared_ptr<BaseDisassembler>& disassembler
) : m_target(target), m_symbol(symbol), m_memory(memory), m_disassembler(disassembler)
{

}

void BaseAnalyzer::UpdateSettings()
{

}

}
