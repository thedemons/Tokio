#include "stdafx.h"
#include "SnowmanDecompiler.h"
#include "Engine/Memory/BaseMemory.h"


//
//#include <nc/common/Branding.h>
//#include <nc/common/Exception.h>
//#include <nc/common/Foreach.h>
//#include <nc/common/SignalLogger.h>
//#include <nc/common/make_unique.h>
//
//#include <nc/core/arch/Instructions.h>
//#include <nc/core/image/Symbol.h>
//#include <nc/core/ir/Program.h>
//
//#include <nc/core/ir/cflow/Graphs.h>
//#include <nc/core/ir/Functions.h>

//
#pragma warning (push, 3)
#include <nc/core/Driver.h>
#include <nc/core/image/Image.h>
#include <nc/core/image/Section.h>
#include <nc/core/image/ByteSource.h>
#include <nc/core/mangling/DefaultDemangler.h>
#include <nc/core/likec/Tree.h>
#include <QTextStream>
#pragma warning (pop)

using namespace nc;
using namespace core;


namespace Engine
{

ByteSize SnowmanDecompiler::MemoryStream::readBytes(ByteAddr addr, void* buf, ByteSize size) const
{
	return m_memory->Read(addr, buf, size);
}

SnowmanDecompiler::SnowmanDecompiler(const std::shared_ptr<ProcessData>& target, const std::shared_ptr<BaseMemory>& memory) :
	BaseDecompiler(target, memory), m_stream(memory)
{

}

_NODISCARD std::string SnowmanDecompiler::Decompile(POINTER address, size_t size) EXCEPT
{
	nc::core::Context m_context;

	std::shared_ptr<image::Image> image = m_context.image();
	image::Platform& platform = image->platform();

	if (m_target->is32bit)
	{
		platform.setArchitecture(QLatin1String("i386"));
	}
	else
	{
		platform.setArchitecture(QLatin1String("x86-64"));
	}

	image->platform().setOperatingSystem(image::Platform::Windows);
	image->setDemangler(std::make_unique<nc::core::mangling::DefaultDemangler>());

	// add the symbols from m_target
	for (ProcessModule& procMod : m_target->modules)
	{
		QString moduleName = QString::fromStdString(procMod.moduleNameA);

		image->addSymbol(
			std::make_unique<image::Symbol>(
				image::SymbolType::SECTION,
				moduleName,
				static_cast<ByteAddr>(procMod.base)
				)
		);

		for (ModuleSymbol& symbol : procMod.exports)
		{
			QString symbolName = QString::fromStdString(symbol.name);

			image->addSymbol(
				std::make_unique<image::Symbol>(
					image::SymbolType::FUNCTION	,
					symbolName,
					static_cast<ByteAddr>(procMod.base + symbol.offset)
					)
			);
		}       
	}

		
	static const auto name = QString::fromUtf8(u8".text");
	ByteAddr maxAddr = (m_target->is32bit) ? 0x7FFFFFFFul : 0x7FFF'FFFFFFFFull;

	auto section = std::make_unique<nc::core::image::Section>(name, 0x000, maxAddr);
	section->setReadable(true);
	section->setWritable(true);
	section->setExecutable(true);
	section->setCode(true);
	section->setData(true);
	section->setAllocated(true);
	section->setExternalByteSource(std::make_unique<MemoryStream>(m_memory));
	image->addSection(std::move(section));

	Driver::disassemble(m_context, &m_stream, static_cast<ByteAddr>(address), static_cast<ByteAddr>(address + size));
	Driver::decompile(m_context);

	QString qResult;
	QTextStream qStream(&qResult);

	m_context.tree()->print(qStream);
	//m_context.tree()->root()
	//m_context.tree()->ge
	QByteArray resultByte = qResult.toLocal8Bit();

	return resultByte.toStdString();
}

}