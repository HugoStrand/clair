#include <c2py/c2py.hpp>
#include <stdexcept>
#include <string>

namespace factory_test {

  namespace detail {
    int created   = 0;
    int destroyed = 0;
  } // namespace detail

  class Immovable;
  using Product = Immovable;

  // Defaults appear on the forward declaration; the factory annotation comes
  // later, on the definition. Both declarations must be classified together.
  Product make_product(int value = 17);

  class Immovable {
    int value_;

    public:
    C2PY_IGNORE explicit Immovable(int value) : value_(value) {
      if (value < 0) throw std::invalid_argument("expected a nonnegative value");
      ++detail::created;
    }
    Immovable(Immovable const &) = delete;
    Immovable(Immovable &&)      = delete;
    ~Immovable() { ++detail::destroyed; }
    int value() const { return value_; }
  };

  C2PY_ANNOTATE("c2py_wrap_as_constructor") Product make_product(int value) { return Product{value}; }

  C2PY_ANNOTATE("c2py_wrap_as_constructor") Product make_product(std::string text) { return Product{int(text.size())}; }

  // Ordinary overloads and several explicitly instantiated template overloads
  // share one name. Code generation must disambiguate their exact signatures.
  template <typename T> C2PY_ANNOTATE("c2py_wrap_as_constructor") Product make_product(T number) { return Product{int(number * 10)}; }
  template Product make_product<double>(double);

  template <typename T> C2PY_ANNOTATE("c2py_wrap_as_constructor") Product make_product(T number, int multiplier) {
    return Product{int(number * multiplier)};
  }
  template Product make_product<double>(double, int);

  int construction_count() { return detail::created; }
  int destruction_count() { return detail::destroyed; }

  struct Aggregate {
    int value = 1;
  };

  C2PY_ANNOTATE("c2py_wrap_as_constructor") Aggregate make_aggregate(int seed) { return Aggregate{7 * seed}; }

  class ImplicitDefault {
    int value_ = 0;

    public:
    int value() const { return value_; }
    C2PY_IGNORE void set_value(int value) { value_ = value; }
  };

  C2PY_ANNOTATE("c2py_wrap_as_constructor") ImplicitDefault make_implicit(int value) {
    ImplicitDefault result;
    result.set_value(value);
    return result;
  }

  class ExplicitDefault {
    int value_ = 0;

    public:
    ExplicitDefault() = default;
    int value() const { return value_; }
    C2PY_IGNORE void set_value(int value) { value_ = value; }
  };

  C2PY_ANNOTATE("c2py_wrap_as_constructor") ExplicitDefault make_explicit(int value) {
    ExplicitDefault result;
    result.set_value(value);
    return result;
  }

} // namespace factory_test

namespace c2py_module {
  using Product         = factory_test::Product;
  using Aggregate       = factory_test::Aggregate;
  using ImplicitDefault = factory_test::ImplicitDefault;
  using ExplicitDefault = factory_test::ExplicitDefault;
} // namespace c2py_module

#include "factory_constructor.wrap.cxx"
