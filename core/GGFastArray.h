#ifndef _GG_FASTARRAY_H_
#define _GG_FASTARRAY_H_
#include <assert.h>
namespace Gange {

template <typename T> class FastArray {
    T *mData;
    size_t mSize;
    size_t mCapacity;

    void growToFit(size_t newElements) {
        if (mSize + newElements > mCapacity) {
            mCapacity = std::max<size_t>(mSize + newElements, mCapacity + (mCapacity >> 1u) + 1u);
            T *data = (T *) ::operator new(mCapacity * sizeof(T));
            if (mData) {
                memcpy(data, mData, mSize * sizeof(T));
                ::operator delete(mData);
            }
            mData = data;
        }
    }

public:
    typedef T value_type;

    typedef T *iterator;
    typedef const T *const_iterator;

    FastArray()
        : mData(0)
        , mSize(0)
        , mCapacity(0) {}

    void swap(FastArray<T> &other) {
        std::swap(this->mData, other.mData);
        std::swap(this->mSize, other.mSize);
        std::swap(this->mCapacity, other.mCapacity);
    }

    FastArray(const FastArray<T> &copy)
        : mSize(copy.mSize)
        , mCapacity(copy.mSize) {
        mData = (T *) ::operator new(mSize * sizeof(T));
        for (size_t i = 0; i < mSize; ++i) {
            new (&mData[i]) T(copy.mData[i]);
        }
    }

    void operator=(const FastArray<T> &copy) {
        if (&copy != this) {
            for (size_t i = 0; i < mSize; ++i)
                mData[i].~T();
            ::operator delete(mData);

            mSize = copy.mSize;
            mCapacity = copy.mSize;

            mData = (T *) ::operator new(mSize * sizeof(T));
            for (size_t i = 0; i < mSize; ++i) {
                new (&mData[i]) T(copy.mData[i]);
            }
        }
    }

    /// Creates an array reserving the amount of elements (memory is not initialized)
    FastArray(size_t reserveAmount)
        : mSize(0)
        , mCapacity(reserveAmount) {
        mData = (T *) ::operator new(reserveAmount * sizeof(T));
    }

    /// Creates an array pushing the value N times
    FastArray(size_t count, const T &value)
        : mSize(count)
        , mCapacity(count) {
        mData = (T *) ::operator new(count * sizeof(T));
        for (size_t i = 0; i < count; ++i) {
            new (&mData[i]) T(value);
        }
    }

    ~FastArray() {
        destroy();
    }

    void destroy() {
        for (size_t i = 0; i < mSize; ++i)
            mData[i].~T();
        ::operator delete(mData);
        mSize = 0;
        mCapacity = 0;
        mData = 0;
    }

    size_t size() const {
        return mSize;
    }
    size_t capacity() const {
        return mCapacity;
    }

    const T *data() {
        return mData;
    }

    void push_back(const T &val) {
        growToFit(1);
        new (&mData[mSize]) T(val);
        ++mSize;
    }

    void pop_back() {
        assert(mSize > 0 && "Can't pop a zero-sized array");
        --mSize;
        mData[mSize].~T();
    }

    iterator insert(iterator where, const T &val) {
        size_t idx = (where - mData);

        growToFit(1);

        memmove(mData + idx + 1, mData + idx, (mSize - idx) * sizeof(T));
        new (&mData[idx]) T(val);
        ++mSize;

        return mData + idx;
    }

    /// otherBegin & otherEnd must not overlap with this->begin() and this->end()
    iterator insertPOD(iterator where, const_iterator otherBegin, const_iterator otherEnd) {
        size_t idx = (where - mData);

        const size_t otherSize = otherEnd - otherBegin;

        growToFit(otherSize);

        memmove(mData + idx + otherSize, mData + idx, (mSize - idx) * sizeof(T));

        while (otherBegin != otherEnd)
            *where++ = *otherBegin++;
        mSize += otherSize;

        return mData + idx;
    }

    void append(const_iterator otherBegin, const_iterator otherEnd) {
        const size_t otherSize = otherEnd - otherBegin;

        growToFit(otherSize);

        for (size_t i = mSize; i < mSize + otherSize; ++i)
            new (&mData[i]) T(*otherBegin++);

        mSize += otherSize;
    }

    void appendPOD(const_iterator otherBegin, const_iterator otherEnd) {
        growToFit(otherEnd - otherBegin);

        memcpy(mData + mSize, otherBegin, (otherEnd - otherBegin) * sizeof(T));
        mSize += otherEnd - otherBegin;
    }

    iterator erase(iterator toErase) {
        size_t idx = (toErase - mData);
        toErase->~T();
        memmove(mData + idx, mData + idx + 1, (mSize - idx - 1) * sizeof(T));
        --mSize;

        return mData + idx;
    }

    iterator erase(iterator first, iterator last) {
        assert(first <= last && last <= end());

        size_t idx = (first - mData);
        size_t idxNext = (last - mData);
        if (first != last) {
            while (first != last) {
                first->~T();
                ++first;
            }
            memmove(mData + idx, mData + idxNext, (mSize - idxNext) * sizeof(T));
            mSize -= idxNext - idx;
        }

        return mData + idx;
    }

    iterator erasePOD(iterator first, iterator last) {
        assert(first <= last && last <= end());

        size_t idx = (first - mData);
        size_t idxNext = (last - mData);
        if (first != last) {
            memmove(mData + idx, mData + idxNext, (mSize - idxNext) * sizeof(T));
            mSize -= idxNext - idx;
        }

        return mData + idx;
    }

    void clear() {
        for (size_t i = 0; i < mSize; ++i)
            mData[i].~T();
        mSize = 0;
    }

    bool empty() const {
        return mSize == 0;
    }

    void reserve(size_t reserveAmount) {
        if (reserveAmount > mCapacity) {
            // We don't use growToFit because it will try to increase capacity by 50%,
            // which is not the desire when calling reserve() explicitly
            mCapacity = reserveAmount;
            T *data = (T *) ::operator new(mCapacity * sizeof(T));
            memcpy(data, mData, mSize * sizeof(T));
            ::operator delete(mData);
            mData = data;
        }
    }

    void resize(size_t newSize, const T &value = T()) {
        if (newSize > mSize) {
            growToFit(newSize - mSize);
            for (size_t i = mSize; i < newSize; ++i) {
                new (&mData[i]) T(value);
            }
        } else {
            for (size_t i = newSize; i < mSize; ++i)
                mData[i].~T();
        }

        mSize = newSize;
    }

    void resizePOD(size_t newSize, const T &value = T()) {
        if (newSize > mSize) {
            growToFit(newSize - mSize);
            for (size_t i = mSize; i < newSize; ++i) {
                new (&mData[i]) T(value);
            }
        }

        mSize = newSize;
    }

    T &operator[](size_t idx) {
        assert(idx < mSize && "Index out of bounds");
        return mData[idx];
    }

    const T &operator[](size_t idx) const {
        assert(idx < mSize && "Index out of bounds");
        return mData[idx];
    }

    T &back() {
        assert(mSize > 0 && "Can't call back with no elements");
        return mData[mSize - 1];
    }

    const T &back() const {
        assert(mSize > 0 && "Can't call back with no elements");
        return mData[mSize - 1];
    }

    T &front() {
        assert(mSize > 0 && "Can't call front with no elements");
        return mData[0];
    }

    const T &front() const {
        assert(mSize > 0 && "Can't call front with no elements");
        return mData[0];
    }

    iterator begin() {
        return mData;
    }
    const_iterator begin() const {
        return mData;
    }
    iterator end() {
        return mData + mSize;
    }
    const_iterator end() const {
        return mData + mSize;
    }
};
}  // namespace Gange

#endif
