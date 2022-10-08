#include "stdafx.h"
#include "BaseDecompiler.h"
#include "Engine/Memory/BaseMemory.h"

namespace Engine
{

BaseDecompiler::BaseDecompiler(const std::shared_ptr<ProcessData>& target) :
	m_target(target)
{

}

BaseDecompiler::~BaseDecompiler()
{
	m_target.reset();
	m_memory.reset();
}


}