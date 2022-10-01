#ifndef TOKIO_SERIALIZE_HPP
#define TOKIO_SERIALIZE_HPP

class Serializable;

typedef unsigned int dsize;
typedef unsigned char BYTE;

class SerializeStream
{
	friend class Serializable;
private:
	std::vector<BYTE> m_data;
	size_t m_cursor_push = 0;
	size_t m_cursor_pop = 0;

	void push_back(const void* data, size_t size)
	{
		m_data.resize(m_data.size() + size);
		memcpy(m_data.data() + m_cursor_push, data, size);

		m_cursor_push += size;
	}

	void pop_front(void* data, size_t size)
	{
		if (m_cursor_pop + size <= m_data.size())
		{
			memcpy(data, m_data.data() + m_cursor_pop, size);
			m_cursor_pop += size;
		}
		else
		{
			memset(data, 0, size);
		}
	}

	template <typename Type>
	void push_back(const Type& data)
	{
		return push_back(&data, sizeof(Type));
	}

	template <typename Type>
	Type pop_front()
	{
		Type data;
		pop_front(&data, sizeof(Type));
		return data;
	}


public:
	SerializeStream(const std::vector<BYTE>& data)
	{
		m_data = data;
	}

	SerializeStream(const void* buffer, size_t size)
	{
		m_data.resize(size);
		memcpy(m_data.data(), buffer, size);
	}

	SerializeStream()
	{
	}

public:
	// std::string push
	SerializeStream& operator << (const std::string& text)
	{
		dsize stringSize = static_cast<dsize>(text.size());

		push_back(stringSize);
		push_back(text.data(), stringSize);
		return *this;
	}

	// std::string pop
	SerializeStream& operator >> (std::string& text)
	{
		dsize stringSize = pop_front<dsize>();

		text.resize(static_cast<size_t>(stringSize));
		pop_front(text.data(), stringSize);
		return *this;
	}

	// std::wstring push
	SerializeStream& operator << (const std::wstring& text)
	{
		dsize stringSize = static_cast<dsize>(text.size());
		size_t byteSize = text.size() * sizeof(wchar_t);

		push_back(stringSize);
		push_back(text.data(), byteSize);
		return *this;
	}

	// std::wstring pop
	SerializeStream& operator >> (std::wstring& text)
	{
		dsize stringSize = pop_front<dsize>();
		size_t byteSize = static_cast<size_t>(stringSize) * sizeof(wchar_t);

		text.resize(static_cast<size_t>(stringSize));
		pop_front(text.data(), byteSize);
		return *this;
	}

	// push derived Serialiable or dynamic type
	template <typename Type>
	SerializeStream& operator << (const Type& value)
	{
		if (std::is_base_of_v<Serializable, Type>)
		{
			const Serializable* object = reinterpret_cast<const Serializable*>(&value);
			object->Serialize(*this);
		}
		else
		{
			push_back(value);
		}

		return *this;
	}

	// pop derived Serialiable or dynamic type
	template <typename Type>
	SerializeStream& operator >> (Type& value)
	{
		if (std::is_base_of_v<Serializable, Type>)
		{
			Serializable* object = reinterpret_cast<Serializable*>(&value);
			object->Deserialize(*this);
		}
		else
		{
			value = pop_front<Type>();
		}

		return *this;
	}


	// push vector of derived Serialiable or dynamic type
	template <typename Type>
	SerializeStream& operator << (const std::vector<Type>& vector)
	{
		dsize vectorSize = static_cast<dsize>(vector.size());
		push_back(vectorSize);

		if (std::is_base_of_v<Serializable, Type>)
		{
			for (const Type& value : vector)
			{
				const Serializable* object = reinterpret_cast<const Serializable*>(&value);
				object->Serialize(*this);
			}
		}
		else
		{
			size_t byteSize = vector.size() * sizeof(Type);
			push_back(vector.data(), byteSize);
		}

		return *this;
	}

	// pop derived Serialiable or dynamic type
	template <typename Type>
	SerializeStream& operator >> (std::vector<Type>& vector)
	{
		dsize vectorSize = pop_front<dsize>();

		if (std::is_base_of_v<Serializable, Type>)
		{
			vector.reserve(vectorSize);

			for (dsize i = 0; i < vectorSize; i++)
			{
				vector.push_back(Type());

				Serializable* object = reinterpret_cast<Serializable*>(&vector.back());
				object->Deserialize(*this);
			}
		}
		else
		{
			vector.resize(static_cast<size_t>(vectorSize));

			size_t byteSize = vector.size() * sizeof(Type);
			pop_front(vector.data(), byteSize);
		}

		return *this;
	}

	// push array of dynamic type or derived Serializable
	template <typename Type, size_t Size>
	SerializeStream& operator << (const Type(&valArray)[Size])
	{
		// notice that Size is pre-defined, 
		// we don't need to store it

		if (std::is_base_of_v<Serializable, Type>)
		{
			for (dsize i = 0; i < Size; i++)
			{
				const Serializable* object = reinterpret_cast<const Serializable*>(&valArray[i]);
				object->Serialize(*this);
			}
		}
		else
		{
			size_t byteSize = Size * sizeof(Type);
			push_back(valArray, byteSize);
		}

		return *this;
	}

	// pop array of dynamic type or derived Serializable
	template <typename Type, size_t Size>
	SerializeStream& operator >> (Type(&valArray)[Size])
	{
		// notice that Size is pre-defined, 
		// we don't need to get it

		if (std::is_base_of_v<Serializable, Type>)
		{

			for (dsize i = 0; i < Size; i++)
			{
				Serializable* object = reinterpret_cast<Serializable*>(&valArray[i]);
				object->Deserialize(*this);
			}
		}
		else
		{
			size_t byteSize = Size * sizeof(Type);
			pop_front(valArray, byteSize);
		}

		return *this;
	}


public:
	_NODISCARD _CONSTEXPR20 std::vector<BYTE>& vector() noexcept
	{
		return m_data;
	}

	// BYTE* buffer
	_NODISCARD _CONSTEXPR20 BYTE* data() noexcept
	{
		return m_data.data();
	}

	// const BYTE* buffer
	_NODISCARD _CONSTEXPR20 const BYTE* data() const noexcept
	{
		return m_data.data();
	}

	// void* buffer
	_NODISCARD _CONSTEXPR20 void* pdata() noexcept
	{
		return static_cast<void*>(m_data.data());
	}

	// const void* buffer
	_NODISCARD _CONSTEXPR20 const void* pdata() const noexcept
	{
		return static_cast<const void*>(m_data.data());
	}

	// size of the buffer
	_NODISCARD _CONSTEXPR20 size_t size() const noexcept
	{
		return m_data.size();
	}

	// don't think i would ever use this
	_NODISCARD _CONSTEXPR20 operator std::vector<BYTE>& () noexcept
	{
		return m_data;
	}
};

class Serializable
{
	friend class SerializeStream;
protected:
	virtual void Serialize(SerializeStream& stream) const = 0;
	virtual void Deserialize(SerializeStream& stream) = 0;

public:
	// serialize object to a stream
	inline SerializeStream ToStream() const
	{
		SerializeStream stream;
		Serialize(stream);
		return stream;
	}

	// construct object from stream
	inline void FromStream(SerializeStream& stream)
	{
		Deserialize(stream);
	}

	// serialize to file stream
	inline void ToFStream(std::fstream& fstream) const
	{
		SerializeStream stream = ToStream();
		fstream.write(static_cast<const char*>(stream.pdata()), stream.size());
	}

	// construct object from file stream
	inline void FromFStream(std::fstream& fstream)
	{
		std::streampos currentOffset = fstream.tellg();

		fstream.seekg(0, fstream.end);
		size_t length = fstream.tellg();
		fstream.seekg(0, fstream.beg);

		SerializeStream stream;
		stream.vector().resize(length);
		fstream.read(static_cast<char*>(stream.pdata()), length);
		Deserialize(stream);

		fstream.seekg(currentOffset);
	}

	inline bool ToFile(const std::wstring& filePath) const
	{
		std::fstream file(filePath, std::ios::out | std::ios::binary);
		if (!file.is_open()) return false;

		ToFStream(file);
		file.close();

		return true;
	}

	inline bool FromFile(const std::wstring& filePath)
	{
		std::fstream file(filePath, std::ios::in | std::ios::binary);
		if (!file.is_open()) return false;

		FromFStream(file);
		file.close();

		return true;
	}
};
#endif // !TOKIO_SERIALIZE_HPP
