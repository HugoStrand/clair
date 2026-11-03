#include <c2py/c2py.hpp>

namespace factory_invalid {

  struct Product {
    int value = 0;
#if defined(FACTORY_MEMBER)
    C2PY_ANNOTATE("c2py_wrap_as_constructor") Product make();
#elif defined(FACTORY_STATIC_MEMBER)
    C2PY_ANNOTATE("c2py_wrap_as_constructor") static Product make();
#elif defined(FACTORY_NATIVE_CONSTRUCTOR)
    C2PY_ANNOTATE("c2py_wrap_as_constructor") Product(int);
#elif defined(FACTORY_HIDDEN_FRIEND)
    friend C2PY_ANNOTATE("c2py_wrap_as_constructor") Product make(int) { return Product{}; }
#endif
  };

  struct Hidden {};

#if defined(FACTORY_LVALUE_REFERENCE)
  C2PY_ANNOTATE("c2py_wrap_as_constructor") Product &make();
#elif defined(FACTORY_RVALUE_REFERENCE)
  C2PY_ANNOTATE("c2py_wrap_as_constructor") Product &&make();
#elif defined(FACTORY_POINTER)
  C2PY_ANNOTATE("c2py_wrap_as_constructor") Product *make();
#elif defined(FACTORY_CONST)
  C2PY_ANNOTATE("c2py_wrap_as_constructor") Product const make();
#elif defined(FACTORY_SCALAR)
  C2PY_ANNOTATE("c2py_wrap_as_constructor") int make();
#elif defined(FACTORY_VOID)
  C2PY_ANNOTATE("c2py_wrap_as_constructor") void make();
#elif defined(FACTORY_UNWRAPPED)
  C2PY_ANNOTATE("c2py_wrap_as_constructor") Hidden make();
#elif defined(FACTORY_BAD_ARGUMENT)
  C2PY_ANNOTATE("c2py_wrap_as_constructor") Product make(Hidden const &);
#elif defined(FACTORY_METHOD_CONFLICT)
  C2PY_ANNOTATE("c2py_wrap_as_constructor") C2PY_WRAP_AS_METHOD Product make(Product const &);
#elif defined(FACTORY_GETTER_CONFLICT)
  C2PY_ANNOTATE("c2py_wrap_as_constructor") C2PY_PROPERTY_GET(value) Product make(Product const &);
#elif defined(FACTORY_SETTER_CONFLICT)
  C2PY_ANNOTATE("c2py_wrap_as_constructor") C2PY_PROPERTY_SET(value) Product make(Product &, int);
#elif defined(FACTORY_MODULE_INIT_CONFLICT)
  C2PY_ANNOTATE("c2py_wrap_as_constructor") C2PY_MODULE_INIT Product make();
#elif defined(FACTORY_OPERATOR)
  C2PY_ANNOTATE("c2py_wrap_as_constructor") Product operator+(Product const &, int);
#endif

} // namespace factory_invalid

namespace c2py_module {
  using Product = factory_invalid::Product;
}
