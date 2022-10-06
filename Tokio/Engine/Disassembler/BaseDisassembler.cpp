#include "stdafx.h"
#include "BaseDisassembler.h"

namespace Engine
{

BaseDisassembler::BaseDisassembler(const std::shared_ptr<ProcessData>& target) :
	m_target(target)
{

}

void BaseDisassembler::UpdateSettings() noexcept
{

}

}