#include "stdafx.h"
#include "BaseDecompiler.h"
#include "Engine/Memory/BaseMemory.h"

namespace Engine
{

BaseDecompiler::BaseDecompiler(const std::shared_ptr<ProcessData>& target, const std::shared_ptr<BaseMemory>& memory) :
	m_target(target), m_memory(memory)
{

}

BaseDecompiler::~BaseDecompiler()
{
	m_target.reset();
	m_memory.reset();
}


}