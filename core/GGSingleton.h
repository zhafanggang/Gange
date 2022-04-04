#ifndef GG_SINGLETON_H_
#define GG_SINGLETON_H_

#include <functional>
#include <mutex>

namespace Gange {

	template <typename T> class Singleton final {
	public:
		static T *Get() {
			std::call_once(mInitFlag, Alloc);
			return mInstance;
		}

	private:
		static void Alloc() {
			mInstance = new T();
		}

		static void Destroy() {
			delete mInstance;
			mInstance = nullptr;
		}

	private:
		static std::once_flag mInitFlag;
		static T *mInstance;
	};

	template <typename T> std::once_flag Singleton<T>::mInitFlag;
	template <typename T> T *Singleton<T>::mInstance = nullptr;
}  // namespace Gange

#endif  // GG_SINGLETON_H_
