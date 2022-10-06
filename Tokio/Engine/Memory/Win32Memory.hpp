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
	~Win32Memory() noexcept;

	_NODISCARD std::shared_ptr<ProcessData>
	Attach(DWORD pid) EXCEPT override;

	void Detach() noexcept override;

	_NODISCARD size_t Read(
		POINTER src,
		void* dest,
		size_t size
	) const noexcept override;

	_NODISCARD size_t Write(
		POINTER dest,
		const void* src, 
		size_t size
	) const noexcept override;

	_NODISCARD VirtualMemoryInfo VirtualQuery(
		POINTER address
	) const EXCEPT override;

};
}
#endif // !TOKIO_ENGINE_MEMORY_WIN32MEMORY_H
