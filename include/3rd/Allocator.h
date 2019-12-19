#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef> // size_t

class Allocator {
protected:
    std::size_t m_totalSize;
    std::size_t m_used;   
    std::size_t m_peak;
public:
    Allocator(const std::size_t totalSize);

    virtual ~Allocator();

    virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) = 0;

	/*
		liangshenge
		���һ�� ���ж������ allocate �汾
	*/
	template<typename T, typename... TaskArgs>
	T* Allocate(const std::size_t alignment, TaskArgs&&... args)
	{
		// ����һ���ַ
		auto address = Allocate(sizeof(T), alignment);
		if (!address)
			return nullptr;

		// ��ָ���ĵ�ַ�Ϲ������
		return new (address) T(std::forward<TaskArgs>(args)...);
	}

    virtual void Free(void* ptr) = 0;

    virtual void Init() = 0;

    friend class Benchmark;
};

#endif /* ALLOCATOR_H */

