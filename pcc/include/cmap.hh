///////////////////////////////////////////////////////////////////////////////
//
// Module      :  records.hh
// Copyright   :  (c) Andy Arvanitis 2015
// License     :  MIT
//
// Maintainer  :  Andy Arvanitis <andy.arvanitis@gmail.com>
// Stability   :  experimental
// Portability :
//
// Types and macros to support PureScript records and row types
//
///////////////////////////////////////////////////////////////////////////////
//
#ifndef PureScript_cmap_HH
#define PureScript_cmap_HH

#include <tuple>
#include "macros.hh"

namespace PureScript {

  //-----------------------------------------------------------------------------
  // For compile-time hashing of cmap key names
  //-----------------------------------------------------------------------------
  namespace Private {
    constexpr auto literalStringHash(const char s[]) -> uint32_t {
      return s[0] ? static_cast<uint32_t>(s[0]) + 33 * literalStringHash(s + 1) : 5381;
    }
  }

  constexpr auto operator "" _key(const char s[], size_t) -> uint32_t {
    return Private::literalStringHash(s);
  }

  //-----------------------------------------------------------------------------
  namespace Private {
    template <typename Derived> // using a bit of CRTP...
    struct cmap_base {};
  }

  // Note: can't use parameter packs because we want to interleave keys (hashes)
  // and value types.
  //
  template <uint32_t K, typename T,
            uint32_t = 0, typename = void,
            uint32_t = 0, typename = void,
            uint32_t = 0, typename = void,
            uint32_t = 0, typename = void,
            uint32_t = 0, typename = void,
            uint32_t = 0, typename = void,
            uint32_t = 0, typename = void,
            uint32_t = 0, typename = void,
            uint32_t = 0, typename = void>
  struct cmap {};

  template <uint32_t H, typename T>
  constexpr auto get(const Private::cmap_base<T>& r) {
    return std::get<T::keyToIndex(H)>(static_cast<const T&>(r).data);
  }

  //-----------------------------------------------------------------------------
  template <uint32_t K, typename T>
  struct cmap<K, T> : Private::cmap_base<cmap<K, T>> {
    const std::tuple<const T> data;
    static constexpr auto keyToIndex(const uint32_t key) -> uint32_t {
      switch (key) {
        case K: return 0;
      }
      return UINT32_MAX;
    }

    cmap(const T value) : data(value) {}
  };

  //-----------------------------------------------------------------------------
  template <uint32_t K0, typename T0,
            uint32_t K1, typename T1>
  struct cmap<K0, T0,
              K1, T1> : Private::cmap_base<cmap<K0, T0,
                                                K1, T1>> {
    const std::tuple<const T0, const T1> data;

    static constexpr auto keyToIndex(const uint32_t key) -> uint32_t {
      switch (key) {
        case K0: return 0;
        case K1: return 1;
      }
      return UINT32_MAX;
    }

    cmap( const T0 value0, const T1 value1 ) : data(value0, value1) {}

    template <uint32_t _K, typename _T>
    constexpr operator cmap<_K, _T>() {
      return cmap<_K, _T>( std::get<keyToIndex(_K)>(data) );
    }
  };

  //-----------------------------------------------------------------------------
  template <uint32_t K0, typename T0,
            uint32_t K1, typename T1,
            uint32_t K2, typename T2>
  struct cmap<K0, T0,
              K1, T1,
              K2, T2> : Private::cmap_base<cmap<K0, T0,
                                                K1, T1,
                                                K2, T2>> {
    const std::tuple<const T0, const T1, const T2> data;

    static constexpr auto keyToIndex(const uint32_t key) -> uint32_t {
      switch (key) {
        case K0: return 0;
        case K1: return 1;
        case K2: return 2;
      }
      return UINT32_MAX;
    }

    cmap( const T0 value0, const T1 value1, const T2 value2 )
      : data(value0, value1, value2) {}

    template <uint32_t _K, typename _T>
    constexpr operator cmap<_K, _T>() {
      return cmap<_K, _T>( std::get<keyToIndex(_K)>(data) );
    }

    template <uint32_t _K0, typename _T0,
              uint32_t _K1, typename _T1>
    constexpr operator cmap<_K0, _T0,
                            _K1, _T1>() {
      return cmap<_K0, _T0,
                  _K1, _T1>( std::get<keyToIndex(_K0)>(data),
                             std::get<keyToIndex(_K1)>(data) );
    }
  };

} // namespace PureScript

//-----------------------------------------------------------------------------
// Main "public" macros to create record type and object
//-----------------------------------------------------------------------------

#define make_cmap(...) MACRO_VARIANT(MAKE_CMAP, __VA_ARGS__)(__VA_ARGS__)

//-----------------------------------------------------------------------------
#define MAKE_CMAP_2(k, v) cmap<k, typename std::add_const<typename std::remove_reference<decltype(v)>::type>::type>(v)
#define MAKE_CMAP_4(k0, v0, \
                    k1, v1) cmap<k0, typename std::add_const<typename std::remove_reference<decltype(v0)>::type>::type, \
                                 k1, typename std::add_const<typename std::remove_reference<decltype(v1)>::type>::type> \
                                (v0, v1)
#define MAKE_CMAP_6(k0, v0, \
                    k1, v1, \
                    k2, v2) cmap<k0, typename std::add_const<typename std::remove_reference<decltype(v0)>::type>::type, \
                                 k1, typename std::add_const<typename std::remove_reference<decltype(v1)>::type>::type, \
                                 k2, typename std::add_const<typename std::remove_reference<decltype(v2)>::type>::type> \
                                (v0, v1, v2)

#endif // PureScript_cmap_HH
