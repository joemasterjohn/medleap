#ifndef __MEDLEAP_HISTORY_H__
#define __MEDLEAP_HISTORY_H__

#include <cstddef>

/** Fixed capacity LIFO container. New elements will overwrite the oldest elements when at capacity. */
template <typename T, std::size_t capacity> class History
{
public:
	History() : head_(-1), size_(0) {}

	void push(const T& t)
	{
		head_ = ++head_ % capacity;
		elements_[head_] = t;
		if (size_ < capacity)
			size_++;
	}

	void pop()
	{
		if (size_ > 0) {
			head_ = (head_ == 0) ? capacity - 1 : head_ - 1;
			size_--;
		}
	}

	T& top()
	{
		return elements_[head_];
	}

	std::size_t size() const
	{
		return size_;
	}

private:
	T elements_[capacity];
	std::size_t head_;
	int size_;
};


#endif // __MEDLEAP_HISTORY_H__