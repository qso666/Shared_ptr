#include <atomic>
#include <iostream>

class Control
{
	std::atomic_uint counter;
public:
	Control()
	{
		counter = 0;
	}
	void increase()
	{
		counter++;
	}
	void decrease()
	{
		counter--;
	}
	size_t size()
	{
		return static_cast<size_t>(counter);
	}
};

template <typename T>
class SharedPtr
{
	Control* counter;
	T* data;
public:
	SharedPtr()
	{
		data = nullptr;
		counter = nullptr;
	}
	SharedPtr(T* a)
	{
		data = a;
		counter = new Control();
		counter->increase();
	}
	SharedPtr(const SharedPtr& a)
	{
		counter = a.counter;
		data = a.data;
		if (counter != nullptr)
		{
			counter->increase();
		}
	}
	SharedPtr(SharedPtr&& a)
	{
		counter = a.counter;
		data = a.data;
		counter->increase();
		a.counter = nullptr;
		a.data = nullptr;
	}
	~SharedPtr()
	{
		if (data == nullptr || counter == nullptr)
			return;
		if (*this)
		{
			counter->decrease();
			if (counter->size() == 0)
			{
				delete data;
				delete counter;
			}
		}
	}
	auto operator=(const SharedPtr& a) -> SharedPtr &
	{
		if (data != nullptr)
		{
			counter->decrease();
			if (counter->size() == 0)
			{
				delete counter;
				delete data;
			}
		}

		counter = a.counter;
		if (counter != nullptr) counter->increase();
		data = a.data;
		return (*this);
	}

	auto operator=(SharedPtr&& a) -> SharedPtr &
	{
		if (data != nullptr)
		{
			counter->decrease();
			if (counter->size() == 0)
			{
				delete counter;
				delete data;
			}
		}
		counter = a.counter;
		data = a.data;
		if (a.data != nullptr)
		{
			delete a.counter;
			delete a.data;
		}
		return (*this);
	}
	operator bool() const
	{
		if (data == nullptr)
			return false;
		if (counter->size() == 0)
			return false;
		return true;
	}
	auto operator*() const -> T &
	{
		return *data;
	}
	auto operator-> () const -> T*
	{
		return data;
	}
	auto get() -> T*
	{
		return data;
	}
	void reset() {
		if (counter != nullptr)
		{
			counter->decrease();
			if (counter->size() == 0)
			{
				delete data;
				delete counter;
			}
		}
		data = nullptr;
		counter = nullptr;
	}
	void reset(T* ptr)
	{
		reset();
		data = ptr;
		counter = new Control();
		counter->increase();
	}
	void swap(SharedPtr& a)
	{
		auto buf1 = a.data;
		a.data = data;
		data = buf1;
		auto buf2 = a.counter;
		a.counter = counter;
		counter = buf2;
	}
	auto use_count() const -> size_t
	{
		if (!*this)
			return 0;
		return counter->size();
	}
};
int main() {

	int* p = new int(5);
	SharedPtr<int> ptr1(p);
	SharedPtr<int> ptr2(ptr1);
	SharedPtr<int> ptr3(ptr2);
	SharedPtr<int> ptr4(ptr3);
	std::cout << ptr1.use_count() << std::endl;
	ptr3.reset();
	ptr4.reset();

	{
		SharedPtr<int> ptr5(ptr4);
		std::cout << ptr1.use_count() << std::endl;
		std::cout << ptr5.use_count() << std::endl;

	}
	std::cout << ptr2.use_count() << std::endl;

	system("pause");
	return 0;
}