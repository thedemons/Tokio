#pragma once
#ifndef TOKIO_ENGINE_MEMORY_WIN32MEMORY_H
#define TOKIO_ENGINE_MEMORY_WIN32MEMORY_H

#include "BaseMemory.h"

namespace Engine
{
class Win32Memory : public BaseMemory
{
private:
public:

	~Win32Memory();
	_NODISCARD auto Attach(DWORD pid)->SafeResult(std::shared_ptr<ProcessData>) override;
	void Detach() override;
	_NODISCARD auto Read(POINTER src, void* dest, size_t size)->SafeResult(void) override;
	_NODISCARD auto Write(POINTER dest, const void* src, size_t size)->SafeResult(void) override;
	_NODISCARD auto VirtualQuery(POINTER address)->SafeResult(VirtualMemoryInfo) override;

};
}
#endif // !TOKIO_ENGINE_MEMORY_WIN32MEMORY_H
