#include "EnginePCH.h"
#pragma hdrstop

#include "pch.h"
#include "Graphics/Buffer.h"

#include "Engine.h"
#include "Log.h"

using namespace Perry;

bool IsOpenGLThread()
{
	return GetEngine().GetRenderer().RenderThread == std::this_thread::get_id();
}

BufferBase::BufferBase(const std::string& Name, unsigned TargetBuffer) :
	m_TargetBuffer(TargetBuffer), m_Name(Name)
{
}

ContainerBufferBase::ContainerBufferBase(const std::string& Name, unsigned TargetBuffer) :BufferBase(Name, TargetBuffer)
{}

BufferBase::~BufferBase()
{
	if (m_GpuPtr == 0)
		return;

	Perry::GetEngine().GetRenderer().commandBackLog.releasableBuffers.push(m_GpuPtr);

	free(m_CpuDataCopy);
	m_CpuDataCopy = nullptr;
	m_CpuUpdate = true;
}

BufferBase::BufferBase(BufferBase&& other) noexcept :
	m_CpuDataCopy(std::move(other.m_CpuDataCopy)), m_GpuPtr(std::move(other.m_GpuPtr)),
	m_Name(std::move(other.m_Name)), m_TargetBuffer(std::move(other.m_TargetBuffer)), m_CpuUpdate(std::move(other.m_CpuUpdate))
{
}

void BufferBase::Bind(int index)
{
	ASSERT_MSG(LOG_GRAPHICS,IsOpenGLThread(),"OPENGL FUNC called from non opengl Thread");

	if(!IsGpuInitialized())
	{
		GPUInitialize();
	}

	glBindBufferBase(m_TargetBuffer, index, m_GpuPtr);
	if (m_CpuUpdate)
	{
		
		UploadData();
	}
}

unsigned BufferBase::GetGpuHandle() const
{
	return m_GpuPtr;
}
unsigned int BufferBase::GetBufferType() const
{
	return m_TargetBuffer;
}

void DataBufferBase::Initialize()
{
	if (m_CpuDataCopy)
	{
		printf("Buffer already initialized !!! \n");
		return;
	}
	m_CpuDataCopy = malloc(SizeOfT());
	m_CpuUpdate = false;
}

void DataBufferBase::GPUInitialize()
{
	ASSERT_MSG(LOG_GRAPHICS,IsOpenGLThread(),"OPENGL FUNC called from non opengl Thread");
	if (m_GpuPtr != 0)
	{
		printf("GPUBuffer already initialized !!! \n");
		return;
	}

	glCreateBuffers(1, &m_GpuPtr);
	glObjectLabel(GL_BUFFER, m_GpuPtr, (GLsizei)m_Name.length(), m_Name.c_str());

	glBindBuffer(m_TargetBuffer, m_GpuPtr);
	glBufferData(m_TargetBuffer, SizeOfT(), m_CpuDataCopy, GL_DYNAMIC_DRAW);
}

void DataBufferBase::UploadData()
{
	ASSERT_MSG(LOG_GRAPHICS,IsOpenGLThread(),"OPENGL FUNC called from non opengl Thread");
	if (m_GpuPtr == 0 - 1)
	{
		GPUInitialize();
	}

	glBufferSubData(m_TargetBuffer, 0, SizeOfT(), m_CpuDataCopy);
	m_CpuUpdate = false;
}

void DataBufferBase::SetDataInternal(const void* data)
{
	ASSERT_MSG(LOG_GRAPHICS,IsOpenGLThread(),"OPENGL FUNC called from non opengl Thread");
	if (!m_CpuDataCopy)
		Initialize();

	memcpy(m_CpuDataCopy, data, SizeOfT());
	m_CpuUpdate = true;
	glBindBuffer(m_TargetBuffer, m_GpuPtr);
	UploadData();
}

void ContainerBufferBase::Initialize()
{
	if (m_CpuDataCopy)
	{
		printf("Buffer already initialized !!! \n");
		return;
	}

	m_CpuDataCopy = malloc(SizeOfT() * m_Capacity);
	m_CpuUpdate = true;
}

void ContainerBufferBase::GPUInitialize()
{
	ASSERT_MSG(LOG_GRAPHICS,IsOpenGLThread(),"OPENGL FUNC called from non opengl Thread");
	if (m_GpuPtr != 0 - 1)
	{
		printf("GPUBuffer already initialized !!! \n");
		return;
	}

	unsigned nb = 0;
	glCreateBuffers(1, &nb);
	glBindBuffer(m_TargetBuffer, nb);
	glBufferData(m_TargetBuffer, (SizeOfT() * m_Capacity) + sizeof(m_Header), nullptr, GL_DYNAMIC_DRAW);

	m_GpuPtr = nb;
	m_CpuUpdate = true;

	glObjectLabel(GL_BUFFER, m_GpuPtr, (GLsizei)m_Name.length(), m_Name.c_str());
}

void ContainerBufferBase::ResizeCPU(size_t NewSize)
{
	size_t copySize = NewSize > m_Capacity ? m_Capacity * SizeOfT() : NewSize * SizeOfT();

	void* newCpuData = malloc(NewSize * SizeOfT());
	memcpy(newCpuData, m_CpuDataCopy, copySize);

	free(m_CpuDataCopy);

	m_CpuDataCopy = newCpuData;
	m_Capacity = NewSize;
	m_CpuUpdate = true;
	CPU_Resize= true;
}
void ContainerBufferBase::ResizeGPU()
{
	ASSERT_MSG(LOG_GRAPHICS,IsOpenGLThread(),"OPENGL FUNC called from non opengl Thread");
	if(!CPU_Resize)
		return;

	CPU_Resize = false;
	
	glBufferData(m_TargetBuffer, (SizeOfT() * m_Capacity) + sizeof(m_Header), nullptr, GL_DYNAMIC_DRAW);
}

void ContainerBufferBase::UploadData()
{
	ASSERT_MSG(LOG_GRAPHICS,IsOpenGLThread(),"OPENGL FUNC called from non opengl Thread");
	ResizeGPU();
	
	glBindBuffer(m_TargetBuffer, m_GpuPtr);
	// when we bind we upload the header data first..
	glBufferSubData(m_TargetBuffer, 0, sizeof(m_Header), &m_Header);
	if(m_Header.m_Size != 0)
		glBufferSubData(m_TargetBuffer, sizeof(m_Header), (SizeOfT() * m_Header.m_Size), m_CpuDataCopy);
	m_CpuUpdate = false;
}


void ContainerBufferBase::Clear()
{
	m_Header.m_Size = 0;
	m_CpuUpdate = true;

	m_IndexLookup.clear();
	m_UnusedElements.clear();
}

unsigned ContainerBufferBase::GetSize() const
{
	return m_Header.m_Size;
}

unsigned ContainerBufferBase::GetCapacity() const
{
	return (unsigned)m_Capacity;
}

unsigned ContainerBufferBase::GetNextIndex() const
{
	unsigned index = m_Header.m_Size;
	if (!m_UnusedElements.empty())
	{
		index = m_UnusedElements[0];
		ASSERT_MSG(LOG_GRAPHICS, index != (unsigned)-1, "Invalid Batch ID !");
	}

	return index;
}

unsigned ContainerBufferBase::AddElementPtr(unsigned index, const void* data) {

	//ASSERT_MSG(LOG_GRAPHICS, m_IndexLookup.find(index) == m_IndexLookup.end(), "Attempted to insert a index that already exist, update if needed !");

	if (!m_CpuDataCopy) {
		Initialize();
	}

	if (m_Capacity <= m_Header.m_Size) {
		ResizeCPU(m_Capacity * 2);
	}

	void* startAddress = static_cast<char*>(m_CpuDataCopy) + (SizeOfT() * m_Header.m_Size);
	memcpy(startAddress, data, SizeOfT());

	m_CpuUpdate = true;
	m_Header.m_Size++;

	if (!m_UnusedElements.empty())
	{
		if (index == m_UnusedElements[0])
		{
			m_UnusedElements.erase(std::begin(m_UnusedElements));
		}
	}

	//Yes these can match but no garuantee that stays the way !
	m_IndexLookup.insert({ index,m_Header.m_Size - 1 });

	return index;
}

void ContainerBufferBase::RemoveElementPtr(unsigned index)
{
	//First swap last element to index we removed.
	auto localIndex = m_IndexLookup[index];


	// TODO make this actually fast... lol
	unsigned highestLocalIndex = 0;
	unsigned globalIndex = 0;
	for (auto element : m_IndexLookup) {
		if (element.second > highestLocalIndex || element.second == highestLocalIndex) {
			highestLocalIndex = element.second;
			globalIndex = element.first;
		}
	}


	//Actual memswap
	void* oldData = static_cast<char*>(m_CpuDataCopy) + (SizeOfT() * localIndex);
	void* NewData = static_cast<char*>(m_CpuDataCopy) + (SizeOfT() * highestLocalIndex);
	memcpy(oldData, NewData, SizeOfT());


	//Second fixup lookup map
	auto elm = m_IndexLookup.find(globalIndex);
	elm->second = index;


	//third delete 
	m_IndexLookup.erase(m_IndexLookup.find(index));
	if (RecordDeletedElements)
		m_UnusedElements.emplace_back(localIndex);

	m_Header.m_Size--;
	m_CpuUpdate = true;
}

void ContainerBufferBase::UpdateElementPtr(unsigned index, const void* data) {

	//First swap last element to index we removed.
	auto localIndex = m_IndexLookup[index];

	//Actual memswap
	void* oldData = static_cast<char*>(m_CpuDataCopy) + (SizeOfT() * localIndex);

	memcpy(oldData, data, SizeOfT());

	m_CpuUpdate = true;
}

void ContainerBufferBase::CopyData(const ContainerBufferBase& other) {

	ASSERT_MSG(LOG_GRAPHICS,IsOpenGLThread(),"OPENGL FUNC called from non opengl Thread");
	glBindBuffer(GL_COPY_READ_BUFFER, other.m_GpuPtr);
	glBindBuffer(GL_COPY_WRITE_BUFFER, m_GpuPtr);


	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, (SizeOfT() * m_Capacity) + sizeof(m_Header));
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	glBindBuffer(GL_COPY_READ_BUFFER, 0);

}
