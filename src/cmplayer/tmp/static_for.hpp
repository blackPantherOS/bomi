#ifndef STATIC_FOR_HPP
#define STATIC_FOR_HPP

#ifndef SCIA
#define SCIA static constexpr inline auto
#define SIA static inline auto
#define SCA static constexpr auto
#endif

namespace tmp {

namespace detail {
template<int idx, int size, bool go = idx < size>
struct static_for { };

template<int idx, int size>
struct static_for<idx, size, true> {
    template<class... Args, class F>
    SIA run(const std::tuple<Args...> &tuple, const F &func) -> void
    {
        func(std::get<idx>(tuple));
        static_for<idx+1, size>::run(tuple, func);
    }
};

template<int idx, int size>
struct static_for<idx, size, false> {
    template<class T, class F>
    static inline auto run(const T &, const F &) -> void { }
    template<class T, class F>
    static inline auto run(T &&, const F &) -> void { }
};

template<int idx, int size, bool go = idx < size>
struct static_for_breakable { };

template<int idx, int size>
struct static_for_breakable<idx, size, true> {
    template<class... Args, class F>
    SIA run(const std::tuple<Args...> &tuple, const F &func) -> bool
    {
        if (!func(std::get<idx>(tuple)))
            return false;
        return static_for_breakable<idx+1, size>::run(tuple, func);
    }
};

template<int idx, int size>
struct static_for_breakable<idx, size, false> {
    template<class T, class F>
    static inline auto run(const T &, const F &) -> bool { return true; }
    template<class T, class F>
    static inline auto run(T &&, const F &) -> bool { return true; }
};
}

template<int idx, int size, class... Args, class F>
SIA static_for(const std::tuple<Args...> &tuple, const F &func) -> void
{ detail::static_for<idx, size>::run(tuple, func); }


template<int idx, int size, class... Args, class F>
SIA static_for_breakable(const std::tuple<Args...> &tuple, const F &func) -> bool
{ return detail::static_for_breakable<idx, size>::run(tuple, func); }

}

#endif // STATIC_FOR_HPP
