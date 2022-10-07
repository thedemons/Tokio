#pragma once
#ifndef TOKIO_ENGINE_DECOMPILER_BASEDECOMPILER_H
#define TOKIO_ENGINE_DECOMPILER_BASEDECOMPILER_H

#include "Engine/EngineDef.h"
#include "Common/Exception.h"

#include <memory>

namespace Engine
{

class BaseMemory;
class BaseDecompiler
{
protected:
	std::shared_ptr<ProcessData> m_target;
	std::shared_ptr<BaseMemory> m_memory;

public:
	BaseDecompiler(
		const std::shared_ptr<ProcessData>& target,
		const std::shared_ptr<BaseMemory>& memory
	);

	~BaseDecompiler();

	_NODISCARD virtual std::string Decompile(POINTER address, size_t size) EXCEPT = 0;
};

}

#endif // !TOKIO_ENGINE_DECOMPILER_BASEDECOMPILER_H