#ifndef _GG_SINGLETON_H_
#define _GG_SINGLETON_H_

#include <functional>
#include <mutex>

namespace Gange {

template <typename T> class Singleton final {
public:
    static T *Get() {
        std::call_once(init_flag_, Create);
        return instance_;
    }

private:
    static void Create() {
        instance_ = new T();
    }

    static void Destroy() {
        delete instance_;
        instance_ = nullptr;
    }

private:
    static std::once_flag init_flag_;
    static T *instance_;
};
template <typename T> std::once_flag Singleton<T>::init_flag_;
template <typename T> T *Singleton<T>::instance_ = nullptr;
}  // namespace Gange

#endif  // GG_CORE_SINGLETON_H_
