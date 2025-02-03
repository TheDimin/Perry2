#pragma once

namespace Perry
{
	class BufferBase
	{
	public:
		explicit BufferBase(const std::string& Name = "Unnamed Buffer", unsigned int TargetBuffer = 0x90D2);//GL_SHADER_STORAGE_BUFFER
		virtual ~BufferBase();


		BufferBase(BufferBase&& other) noexcept;

		virtual long long SizeOfT() const = 0;
		virtual void Initialize() = 0;
		virtual void GPUInitialize() = 0;
		void Bind(int index);
		unsigned GetGpuHandle() const;

		void* m_CpuDataCopy = nullptr;
		unsigned int GetBufferType()const;
		bool IsGpuInitialized() const { return m_GpuPtr != 0 - 1; }

		virtual void UploadData() = 0;
	protected:
		unsigned m_GpuPtr = 0 - 1;

		bool m_CpuUpdate = false;

		unsigned int m_TargetBuffer = 0;
		std::string m_Name;
	};


	class DataBufferBase :public BufferBase
	{
	public:
		void UploadData() override;
	protected:
		DataBufferBase(const std::string& Name = "Unnamed Buffer", unsigned int Target = 0x90D2) :
			BufferBase(Name, Target) {}
		void SetDataInternal(const void*);
		void Initialize() override;
		void GPUInitialize() override;
	};

	template<typename TYPE>
	class DataBuffer :public DataBufferBase
	{
	public:
		DataBuffer(const std::string& Name = "Unnamed Buffer", unsigned int Target = 0x90D2) :
			DataBufferBase(Name, Target) {}

		long long SizeOfT() const override { return sizeof(TYPE); }
		void SetData(const TYPE& data)
		{
			static_assert(sizeof(TYPE) % 16 == 0, "data does not allign");
			SetDataInternal(static_cast<const void*>(&data));
		}
	};

	class ContainerBufferBase : public BufferBase
	{
	public:

		ContainerBufferBase(const std::string& Name = "Unnamed Buffer", unsigned int Target = 0x90D2); //GL_SHADER_STORAGE_BUFFER
		void Initialize() override;
		void GPUInitialize() override;


		void ResizeCPU(size_t NewSize);
		void ResizeGPU();
		void Clear();

		unsigned GetSize() const;
		unsigned GetCapacity() const;

		void UploadData() override;
		struct HeaderInfo
		{
			unsigned m_Size = 0;
			float padding[3]{ 0 };
		};
	protected:
		unsigned GetNextIndex()const;

		virtual unsigned AddElementPtr(unsigned index, const void* data);
		void RemoveElementPtr(unsigned index);
		void UpdateElementPtr(unsigned index, const void* data);

		void CopyData(const ContainerBufferBase& other);
		HeaderInfo m_Header{};
		size_t m_Capacity = 10;
		bool RecordDeletedElements = true;
		bool CPU_Resize = false;


		std::unordered_map<unsigned, unsigned> m_IndexLookup;
		std::vector<unsigned> m_UnusedElements;
	};

	template <typename Data>
	class ArrayBuffer : public ContainerBufferBase {
	public:
		ArrayBuffer(
			const std::string& Name = "Unnamed Buffer",
			unsigned int BufferType = 0x90D2) // GL_SHADER_STORAGE_BUFFER
			: ContainerBufferBase(Name, BufferType) {}

		long long SizeOfT() const override { return sizeof(Data); }

		unsigned AddElement(const Data& data) {
			static_assert(sizeof(Data) % 16 == 0, "data does not allign");
			return AddElementPtr(GetNextIndex(), static_cast<const void*>(&data));
		}

		void RemoveElement(unsigned id) {
			RemoveElementPtr(id);
		}

		void UpdateElement(unsigned id, const Data& data) {
			UpdateElementPtr(id, static_cast<const void*>(&data));
		}

		// Copy Constructor
		ArrayBuffer(const ArrayBuffer<Data>& other) : ContainerBufferBase(other.m_Name, other.m_TargetBuffer)
		{
			// Perform deep copy of CPU data if it exists
			if (other.m_CpuDataCopy) {
				m_CpuDataCopy = new char[other.m_Header.m_Size * sizeof(Data)];
				std::memcpy(m_CpuDataCopy, other.m_CpuDataCopy, other.m_Header.m_Size * sizeof(Data));
			}

			// Copy other members
			//m_GpuPtr = other.m_GpuPtr; // GPU resource would need to be re-uploaded if necessary
			m_CpuUpdate = other.m_CpuUpdate;
			m_Capacity = other.m_Capacity;
			m_Header = other.m_Header;

			Initialize();
			CopyData(other);
		}

		// Move Constructor
		ArrayBuffer(ArrayBuffer<Data>&& other) noexcept
			: ContainerBufferBase(std::move(other.m_Name),
				other.m_TargetBuffer) {
			// Move CPU data
			m_CpuDataCopy = other.m_CpuDataCopy;
			other.m_CpuDataCopy = nullptr;

			// Move other members
			m_GpuPtr = other.m_GpuPtr;
			other.m_GpuPtr = 0;

			m_CpuUpdate = other.m_CpuUpdate;
			m_Capacity = other.m_Capacity;

			m_Header.m_Size = other.m_Header.m_Size;
		}

		// Assignment operators should be implemented as well if needed
		ArrayBuffer<Data>& operator=(const ArrayBuffer<Data>& other) {
			if (this != &other) {
				// Copy-and-swap idiom can be used for exception safety
				ArrayBuffer<Data> temp(other);
				std::swap(*this, temp);
			}
			return *this;
		}

		ArrayBuffer<Data>& operator=(ArrayBuffer<Data>&& other) noexcept {
			if (this != &other) {
				// Release current resources
				if (m_CpuDataCopy) {
					delete[] static_cast<char*>(m_CpuDataCopy);
				}

				// Move resources
				m_Name = std::move(other.m_Name);
				m_TargetBuffer = other.m_TargetBuffer;
				m_CpuDataCopy = other.m_CpuDataCopy;
				other.m_CpuDataCopy = nullptr;

				m_GpuPtr = other.m_GpuPtr;
				other.m_GpuPtr = 0;

				m_CpuUpdate = other.m_CpuUpdate;
				m_Capacity = other.m_Capacity;
				m_Header = other.m_Header;
			}
			return *this;
		}
	};

	template <typename Data>
	class MapBuffer : public ContainerBufferBase {
	public:
		MapBuffer(
			const std::string& Name = "Unnamed Buffer",
			unsigned int BufferType = 0x90D2) // GL_SHADER_STORAGE_BUFFER
			: ContainerBufferBase(Name, BufferType) {}

		long long SizeOfT() const override { return sizeof(Data); }

		unsigned AddElement(unsigned id, const Data& data) {
			static_assert(sizeof(Data) % 16 == 0, "data does not allign");
			return AddElementPtr(id, static_cast<const void*>(&data));
		}

		bool Contains(unsigned id) const
		{
			return m_IndexLookup.find(id) != m_IndexLookup.end();
		}

		void RemoveElement(unsigned id) {
			RemoveElementPtr(id);
		}

		void AddOrUpdateElement(unsigned id, const Data& data) {
			if (Contains(id))
			{
				UpdateElement(id, data);
			}
			else
			{
				AddElement(id, data);
			}
		}

		void UpdateElement(unsigned id, const Data& data) {
			UpdateElementPtr(id, static_cast<const void*>(&data));
		}

		// Copy Constructor
		MapBuffer(const MapBuffer<Data>& other) : ContainerBufferBase(other.m_Name, other.m_TargetBuffer)
		{
			// Perform deep copy of CPU data if it exists
			if (other.m_CpuDataCopy) {
				m_CpuDataCopy = new char[other.m_Header.m_Size * sizeof(Data)];
				std::memcpy(m_CpuDataCopy, other.m_CpuDataCopy, other.m_Header.m_Size * sizeof(Data));
			}

			// Copy other members
			//m_GpuPtr = other.m_GpuPtr; // GPU resource would need to be re-uploaded if necessary
			m_CpuUpdate = other.m_CpuUpdate;
			m_Capacity = other.m_Capacity;
			m_Header = other.m_Header;

			Initialize();
			CopyData(other);
		}

		// Move Constructor
		MapBuffer(MapBuffer<Data>&& other) noexcept
			: ContainerBufferBase(std::move(other.m_Name),
				other.m_TargetBuffer) {
			// Move CPU data
			m_CpuDataCopy = other.m_CpuDataCopy;
			other.m_CpuDataCopy = nullptr;

			// Move other members
			m_GpuPtr = other.m_GpuPtr;
			other.m_GpuPtr = 0;

			m_CpuUpdate = other.m_CpuUpdate;
			m_Capacity = other.m_Capacity;

			m_Header.m_Size = other.m_Header.m_Size;
		}

		// Assignment operators should be implemented as well if needed
		MapBuffer<Data>& operator=(const MapBuffer<Data>& other) {
			if (this != &other) {
				// Copy-and-swap idiom can be used for exception safety
				MapBuffer<Data> temp(other);
				std::swap(*this, temp);
			}
			return *this;
		}

		MapBuffer<Data>& operator=(MapBuffer<Data>&& other) noexcept {
			if (this != &other) {
				// Release current resources
				if (m_CpuDataCopy) {
					delete[] static_cast<char*>(m_CpuDataCopy);
				}

				// Move resources
				m_Name = std::move(other.m_Name);
				m_TargetBuffer = other.m_TargetBuffer;
				m_CpuDataCopy = other.m_CpuDataCopy;
				other.m_CpuDataCopy = nullptr;

				m_GpuPtr = other.m_GpuPtr;
				other.m_GpuPtr = 0;

				m_CpuUpdate = other.m_CpuUpdate;
				m_Capacity = other.m_Capacity;
				m_Header = other.m_Header;
			}
			return *this;
		}
	};

} // namespace Perry
