#ifndef TOKIO_SERIALIZE_HPP
#define TOKIO_SERIALIZE_HPP
//
//
//class SerializeStream
//{
//private:
//	std::vector<BYTE> m_data;
//	size_t m_cursor_push = 0;
//	size_t m_cursor_pop = 0;
//
//	void push_back(void* data, size_t size)
//	{
//		m_data.resize(m_data.size() + size);
//		memcpy(m_data.data() + m_cursor_push, data, size);
//
//		m_cursor_push += size;
//	}
//
//	void pop_front(void* data, size_t size)
//	{
//		if (m_cursor_pop + size <= m_data.size())
//		{
//			memcpy(data, m_data.data() + m_cursor_pop, size);
//			m_cursor_pop += size;
//		}
//		else
//		{
//			memset(data, 0, size);
//		}
//	}
//
//	template <typename T>
//	void push_back(T* data)
//	{
//		return push_back(data, sizeof(T));
//	}
//
//	template <typename T>
//	T pop_front()
//	{
//		T data;
//		pop_front(&data, sizeof(T));
//		return data;
//	}
//	friend class Serializable;
//
//public:
//	SerializeStream(std::vector<BYTE>& data)
//	{
//		m_data = data;
//	}
//
//	SerializeStream(void* buffer, size_t size)
//	{
//		m_data.resize(size);
//		memcpy(m_data.data(), buffer, size);
//	}
//
//	SerializeStream()
//	{
//	}
//
//	friend SerializeStream& operator<< (SerializeStream& stream, std::string& text)
//	{
//		int stringSize = text.size();
//		stream.push_back<int>(&stringSize);
//		stream.push_back((void*)text.data(), text.size());
//		return stream;
//	}
//
//	friend SerializeStream& operator>> (SerializeStream& stream, std::string& text)
//	{
//		int stringSize = stream.pop_front<int>();
//		text.resize(stringSize);
//		stream.pop_front((void*)text.data(), stringSize);
//		return stream;
//	}
//
//	friend SerializeStream& operator<< (SerializeStream& stream, std::wstring& text)
//	{
//		int stringSize = text.size();
//		stream.push_back<int>(&stringSize);
//		stream.push_back((void*)text.data(), stringSize * 2);
//		return stream;
//	}
//
//	friend SerializeStream& operator>> (SerializeStream& stream, std::wstring& text)
//	{
//		int stringSize = stream.pop_front<int>();
//		text.resize(stringSize);
//		stream.pop_front((void*)text.data(), stringSize * 2);
//		return stream;
//	}
//
//	// BYTE[]
//	//friend SerializeStream& operator<< (SerializeStream& stream, std::vector<BYTE>& buffer)
//	//{
//	//	int bufferSize = buffer.size();
//	//	stream.push_back<int>(&bufferSize);
//	//	stream.push_back((void*)buffer.data(), bufferSize);
//	//	return stream;
//	//}
//
//	//friend SerializeStream& operator<< (SerializeStream& stream, std::vector<std::vector<BYTE>>& arrayBuffer)
//	//{
//	//	int arraySize = arrayBuffer.size();
//	//	stream.push_back<int>(&arraySize);
//	//	for (std::vector<BYTE>& buffer : arrayBuffer)
//	//		stream << buffer;
//	//	return stream;
//	//}
//
//	// BYTE[]
//	//friend SerializeStream& operator>> (SerializeStream& stream, std::vector<BYTE>& buffer)
//	//{
//	//	int bufferSize = stream.pop_front<int>();
//	//	buffer.clear();
//	//	buffer.resize(bufferSize);
//	//	stream.pop_front((void*)buffer.data(), bufferSize);
//	//	return stream;
//	//}
//
//	//friend SerializeStream& operator>> (SerializeStream& stream, std::vector<std::vector<BYTE>>& arrayBuffer)
//	//{
//	//	int arraySize = stream.pop_front<int>();
//	//	arrayBuffer.clear();
//	//	arrayBuffer.resize(arraySize);
//	//	for (std::vector<BYTE>& buffer : arrayBuffer)
//	//		stream >> buffer;
//	//	return stream;
//	//}
//
//
//
//
//	// serialize object
//	//template <
//	//	typename T,
//	//	typename std::enable_if_t<std::is_base_of_v<Serializable, T>>* = nullptr
//	//>
//	//	friend SerializeStream& operator<< (SerializeStream& stream, T& object)
//	//{
//	//	object.Serialize(stream);
//	//	return stream;
//	//}
//
//	// deserialize object
//	//template <
//	//	typename T,
//	//	typename std::enable_if_t<std::is_base_of_v<Serializable, T>>* = nullptr
//	//>
//	//	friend SerializeStream& operator>> (SerializeStream& stream, T& object)
//	//{
//	//	object.Deserialize(stream);
//	//	return stream;
//	//}
//
//	// serialize array of object
//	//template <
//	//	typename T,
//	//	typename std::enable_if_t<std::is_base_of_v<Serializable, T>>* = nullptr
//	//>
//	//	friend SerializeStream& operator<< (SerializeStream& stream, std::vector<T>& arrayObject)
//	//{
//	//	int arraySize = arrayObject.size();
//	//	stream.push_back<int>(&arraySize);
//	//	for (auto& obj : arrayObject)
//	//	{
//	//		stream << obj;
//	//	}
//
//	//	return stream;
//	//}
//
//	// deserialize array of object
//	//template <
//	//	typename T,
//	//	typename std::enable_if_t<std::is_base_of_v<Serializable, T>>* = nullptr
//	//>
//	//	friend SerializeStream& operator>> (SerializeStream& stream, std::vector<T>& arrayObject)
//	//{
//	//	int arraySize = stream.pop_front<int>();
//	//	arrayObject.clear();
//	//	arrayObject.resize(arraySize);
//
//	//	for (auto& obj : arrayObject)
//	//	{
//	//		stream >> obj;
//	//	}
//
//	//	return stream;
//	//}
//
//
//
//
//
//
//	// serialize array of value
//	//template <
//	//	typename T,
//	//	typename std::enable_if_t<!std::is_base_of_v<Serializable, T>>* = nullptr
//	//>
//	//	friend SerializeStream& operator<< (SerializeStream& stream, std::vector<T>& arrayData)
//	//{
//	//	int arraySize = arrayData.size();
//	//	stream.push_back<int>(&arraySize);
//
//	//	for (auto& data : arrayData)
//	//	{
//	//		stream << data;
//	//	}
//
//	//	return stream;
//	//}
//
//	// deserialize array of value
//	//template <
//	//	typename T,
//	//	typename std::enable_if_t<!std::is_base_of_v<Serializable, T>>* = nullptr
//	//>
//	//	friend SerializeStream& operator>> (SerializeStream& stream, std::vector<T>& arrayData)
//	//{
//	//	int arraySize = stream.pop_front<int>();
//	//	arrayData.clear();
//	//	arrayData.resize(arraySize);
//
//	//	for (auto& data : arrayData)
//	//	{
//	//		stream >> data;
//	//	}
//
//	//	return stream;
//	//}
//
//
//
//
//
//	template <typename T,
//		typename = std::enable_if_t<!std::is_base_of<Serializable, T>::value> >
//		friend SerializeStream& operator<< (SerializeStream& stream, T& value)
//	{
//		stream.push_back<T>(&value);
//		return stream;
//	}
//
//	template <
//		typename T,
//		typename std::enable_if_t<!std::is_base_of_v<Serializable, T>>* = nullptr
//	>
//		friend SerializeStream& operator>> (SerializeStream& stream, T& value)
//	{
//		value = stream.pop_front<T>();
//		return stream;
//	}
//
//
//	std::vector<BYTE>& data()
//	{
//		return m_data;
//	}
//
//	BYTE* buffer()
//	{
//		return m_data.data();
//	}
//
//	size_t size()
//	{
//		return m_data.size();
//	}
//
//	operator std::vector<BYTE>() const { return m_data; }
//};
//
//
//class Serializable
//{
//private:
//protected:
//	virtual void Deserialize(SerializeStream& stream)
//	{
//
//	}
//
//	virtual void Serialize(SerializeStream& stream)
//	{
//
//	}
//
//public:
//	Serializable() {}
//
//
//	template <typename T>
//	static T FromStream(SerializeStream& stream)
//	{
//		T object;
//		object.Deserialize(stream);
//		return object;
//	}
//
//	template <typename T>
//	static T FromStream(std::vector<BYTE>& byteStream)
//	{
//		SerializeStream stream(byteStream);
//		return FromStream<T>(stream);
//	}
//
//	template <typename T>
//	static T FromStream(void* data, size_t size)
//	{
//		SerializeStream stream;
//		stream.data().resize(size);
//		memcpy(stream.buffer(), data, size);
//		return FromStream<T>(stream);
//	}
//
//	SerializeStream ToStream()
//	{
//		SerializeStream stream;
//		Serialize(stream);
//
//		return stream;
//	}
//
//	template <typename T>
//	static std::vector<T> ArrayFromStream(SerializeStream& stream)
//	{
//		int arrayLen = stream.pop_front<int>();
//		std::vector<T> results;
//		results.reserve(arrayLen);
//
//		for (int i = 0; i < arrayLen; i++)
//		{
//			T object;
//			object.Deserialize(stream);
//			results.push_back(object);
//		}
//
//		return results;
//	}
//	template <typename T>
//	static std::vector<T> ArrayFromStream(std::vector<BYTE>& byteStream)
//	{
//		SerializeStream stream(byteStream);
//		return ArrayFromStream<T>(stream);
//	}
//
//	template <typename T>
//	static std::vector<T> ArrayFromStream(void* data, size_t size)
//	{
//		SerializeStream stream;
//		stream.data().resize(size);
//		memcpy(stream.buffer(), data, size);
//		return ArrayFromStream<T>(stream);
//	}
//
//	template <typename T>
//	static SerializeStream ToStream(std::vector<T> dataArray)
//	{
//		int arrayLen = dataArray.size();
//		SerializeStream stream;
//		stream.push_back<int>(&arrayLen);
//
//		for (auto& data : dataArray)
//		{
//			data.Serialize(stream);
//		}
//
//		return stream;
//	}
//};
#endif // !__SERIALIZE__H__
