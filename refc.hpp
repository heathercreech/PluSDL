#pragma once

#include <functional>

//unique_ptr requires classes have a defined destructor. This solves the problem of classes that are destroyed with a function (i.e. SDL_DestroyRenderer(SDL_Renderer*))
template <typename T>
class ReferenceCounter {
public:
	class Pointer {
	public:
		Pointer(ReferenceCounter<T>* ref_counter) { counter = ref_counter; ++counter->count; };
		Pointer(const Pointer& p) : counter(p.counter) { ++counter->count; };
		~Pointer() { if (--counter->count == 0) { delete counter; } };

		operator T*() { return counter->reference; };
		T* operator-> () { return counter->reference; }

		int count() { return counter->count; };
		const bool good() { return counter->reference != NULL ? true : false; };
	private:
		ReferenceCounter<T>* counter;
	};

	static Pointer create(T* r, std::function<void(T*)> d) { return new ReferenceCounter<T>(r, d); };

private:
	ReferenceCounter(T* r, std::function<void(T*)> d) : count(0), destructor(d) { reference = r; };
	~ReferenceCounter() { destructor(reference); };
	
	T* reference;
	std::function<void(T*)> destructor;
	unsigned int count;
};