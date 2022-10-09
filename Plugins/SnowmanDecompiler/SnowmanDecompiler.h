/*
 * Project Tokio
 * Author: thedemons
 * Github: thedemons/Tokio
 * Made:   With love
 *
 * License: MIT License
 *
 * Copyright(c) 2022 thedemons
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#ifndef TOKIO_ENGINE_DECOMPILER_SNOWMANDECOMPILER_H
#define TOKIO_ENGINE_DECOMPILER_SNOWMANDECOMPILER_H

#include "Engine/EngineDef.h"
#include "Engine/Plugins/PluginsDef.h"
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
extern EngineInterface EInterface;

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

	std::unique_ptr<MemoryStream> m_stream;

public:
	SnowmanDecompiler(const std::shared_ptr<ProcessData>& target);

	_NODISCARD std::string Decompile(POINTER address, size_t size) EXCEPT override;
};

}


#endif // !TOKIO_ENGINE_DECOMPILER_SNOWMANDECOMPILER_H