#pragma once

#ifdef _WIN32
    #define make_unique std::make_unique
#else
    template<typename T, typename... Ts>
    std::unique_ptr<T> make_unique(Ts&&... params)
    {
        return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
    }
#endif
