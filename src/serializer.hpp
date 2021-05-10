#pragma once

#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <vector>

#define SERIALIZE_CLASS(args...)                                               \
  void serialize(std::ostream &os) const {                                     \
    serializer::serialize_fields(os, args);                                    \
  }                                                                            \
  void deserialize(std::istream &is) {                                         \
    serializer::deserialize_fields(is, args);                                  \
  }

namespace serializer {

    namespace serializer_concepts {
        template<typename T> concept Arithmetic = std::is_arithmetic_v<T>;
        template<typename T> concept Sizeable = requires(T const &t) {
            {
            t.size()
            }
            ->std::same_as<std::size_t>;
        };
        template<typename T>
        concept IterableContainer = requires(T &cont, T const &const_cont) {
            typename T::value_type;
            typename T::iterator;
            typename T::const_iterator;
            {
            cont.begin()
            }
            ->std::same_as<typename T::iterator>;
            {
            cont.end()
            }
            ->std::same_as<typename T::iterator>;
            {
            const_cont.begin()
            }
            ->std::same_as<typename T::const_iterator>;
            {
            const_cont.end()
            }
            ->std::same_as<typename T::const_iterator>;
        };
        template<typename T>
        concept HasTwoIteratorConstructor = requires(typename T::iterator it) {
            T{it, it};
        };
        template<typename T>
        concept Container =
        Sizeable<T> && IterableContainer<T> && HasTwoIteratorConstructor<T>;
    } // namespace serializer_concepts

    template<typename T>
    struct serialization_traits {
        static void serialize(std::ostream &os, const T &x) { x.serialize(os); }

        static T deserialize(std::istream &is) {
            T x;
            x.deserialize(is);
            return x;
        }
    };

    template<serializer_concepts::Arithmetic T>
    struct serialization_traits<T> {
        static void serialize(std::ostream &s, T value) {
            std::uint8_t buf[sizeof(T)];
            std::memcpy(buf, &value, sizeof(T));
            for (std::size_t i = 0; i < sizeof(T); ++i) {
                s << buf[i];
            }
        }

        static T deserialize(std::istream &is) {
            std::uint8_t buf[sizeof(T)];
            for (std::size_t i = 0; i < sizeof(T); ++i) {
                buf[i] = is.get();
            }
            T result;
            std::memcpy(&result, buf, sizeof(T));
            return result;
        }
    };

    template<serializer_concepts::Container T>
    struct serialization_traits<T> {
        static void serialize(std::ostream &os, const T &data) {
            serialization_traits<std::size_t>::serialize(os, data.size());
            for (const auto &item : data) {
                serialization_traits<typename T::value_type>::serialize(os, item);
            }
        }

        static T deserialize(std::istream &is) {
            std::size_t size = serialization_traits<std::size_t>::deserialize(is);
            std::vector<typename T::value_type> data(size);
            for (auto &item : data) {
                item = serialization_traits<typename T::value_type>::deserialize(is);
            }
            return T(data.begin(), data.end());
        }
    };

    template<typename T>
    void save_to_file(const std::string &fileName, T data) {
        std::ofstream os(fileName, std::ios::binary);
        serialization_traits<T>::serialize(os, std::move(data));
    }

    template<typename T>
    T read_from_file(const std::string &fileName) {
        std::ifstream is(fileName, std::ios::binary);
        return serialization_traits<T>::deserialize(is);
    }

    void inline serialize_fields(std::ostream &os) {}

    template<typename Arg, typename... Args>
    void serialize_fields(std::ostream &os, const Arg &arg, const Args &...args) {
        serialization_traits<Arg>::serialize(os, arg);
        serialize_fields(os, args...);
    }

    void inline deserialize_fields(std::istream &is) {}

    template<typename Arg, typename... Args>
    void deserialize_fields(std::istream &is, Arg &arg, Args &...args) {
        arg = serialization_traits<Arg>::deserialize(is);
        deserialize_fields(is, args...);
    }

} // namespace serializer
