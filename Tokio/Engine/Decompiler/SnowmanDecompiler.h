#pragma once
#ifndef TOKIO_ENGINE_DECOMPILER_SNOWMANDECOMPILER_H
#define TOKIO_ENGINE_DECOMPILER_SNOWMANDECOMPILER_H

#include "Engine/EngineDef.h"
#include "Engine/Decompiler/BaseDecompiler.h"

#include <nc/common/Types.h>
#include <nc/core/Context.h>
#include <nc/core/image/ByteSource.h>

#pragma warning (push, 1)
#include <qt5/QtCore/qvariant.h> // this must be included here in the header I HAVE NO IDEA WHY BUT IT GIVES A BUG :(
#pragma warning (pop)

#include <memory>

namespace Engine
{

class SnowmanDecompiler : public BaseDecompiler
{
private:

	class MemoryStream : public nc::core::image::ByteSource
	{
	private:
		std::shared_ptr<BaseMemory> m_memory;

	public:

		MemoryStream(const std::shared_ptr<BaseMemory>& memory) : m_memory(memory)
		{

		}

		/**
		* Reads a sequence of bytes.
		*
		* \param[in] addr  Linear address of the first byte to read.
		* \param[out] buf  Valid pointer to the buffer to read into.
		* \param[in] size  Number of bytes to read.
		*
		* \return Number of bytes actually read and copied into the buffer.
		*/
		nc::ByteSize readBytes(nc::ByteAddr addr, void* buf, nc::ByteSize size) const override;
	};

	MemoryStream m_stream;

public:
	SnowmanDecompiler(
		const std::shared_ptr<ProcessData>& target,
		const std::shared_ptr<BaseMemory>& memory
	);

	_NODISCARD std::string Decompile(POINTER address, size_t size) EXCEPT override;
};

}


#endif // !TOKIO_ENGINE_DECOMPILER_SNOWMANDECOMPILER_H