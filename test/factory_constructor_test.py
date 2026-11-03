import gc
import unittest

import factory_constructor as factory


class TestFactoryConstructor(unittest.TestCase):
    def test_overloads_and_defaults(self):
        self.assertEqual(factory.Product().value(), 17)
        self.assertEqual(factory.Product(5).value(), 5)
        self.assertEqual(factory.Product(value=6).value(), 6)
        self.assertEqual(factory.Product("abc").value(), 3)
        self.assertEqual(factory.Product(text="abcd").value(), 4)
        self.assertEqual(factory.Product(number=2.5).value(), 25)
        self.assertEqual(factory.Product(2.5, 4).value(), 10)
        self.assertEqual(factory.Product(multiplier=4, number=2.5).value(), 10)
        # Declaration and definition must contribute only one documented overload.
        self.assertEqual(factory.Product.__doc__.count("(value: int = 17)"), 1)
        self.assertFalse(hasattr(factory, "make_product"))
        with self.assertRaises(TypeError):
            factory.Product(unknown=3)

    def test_aggregate_factory_takes_precedence(self):
        self.assertEqual(factory.Aggregate(seed=4).value, 28)
        self.assertFalse(hasattr(factory, "make_aggregate"))
        with self.assertRaises(TypeError):
            factory.Aggregate(value=4)
        with self.assertRaises(TypeError):
            factory.Aggregate()

    def test_native_constructor_selection(self):
        # A constructor materialized by the factory body is an implementation
        # detail; an explicitly declared native constructor stays public.
        self.assertEqual(factory.ImplicitDefault(value=5).value(), 5)
        with self.assertRaises(TypeError):
            factory.ImplicitDefault()
        self.assertEqual(factory.ExplicitDefault().value(), 0)
        self.assertEqual(factory.ExplicitDefault(value=6).value(), 6)

    def test_lifetime_and_factory_exception(self):
        gc.collect()
        created = factory.construction_count()
        destroyed = factory.destruction_count()
        product = factory.Product(9)
        self.assertEqual(factory.construction_count(), created + 1)
        self.assertEqual(factory.destruction_count(), destroyed)
        del product
        gc.collect()
        self.assertEqual(factory.destruction_count(), destroyed + 1)

        with self.assertRaisesRegex(RuntimeError, "nonnegative value"):
            factory.Product(value=-1)
        gc.collect()
        self.assertEqual(factory.construction_count(), created + 1)
        self.assertEqual(factory.destruction_count(), destroyed + 1)
        # A failed construction must not prevent subsequent valid calls.
        self.assertEqual(factory.Product(3).value(), 3)


if __name__ == "__main__":
    unittest.main()
