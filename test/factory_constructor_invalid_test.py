"""Run valid C++ declarations that clair must reject with specific diagnostics."""

from pathlib import Path
import subprocess
import sys


generator, build_dir, source = sys.argv[1:]
by_value = "Constructor factory must return an unqualified wrapped class by value"
wrapped = "Constructor factory return type is not a class being wrapped"
conflict = "Constructor factory cannot also be a method, property, or module initializer"
free_function = "Constructor factory annotation requires a free function"
cases = {
    "FACTORY_LVALUE_REFERENCE": by_value,
    "FACTORY_RVALUE_REFERENCE": by_value,
    "FACTORY_POINTER": by_value,
    "FACTORY_CONST": by_value,
    "FACTORY_SCALAR": wrapped,
    "FACTORY_VOID": wrapped,
    "FACTORY_UNWRAPPED": wrapped,
    "FACTORY_BAD_ARGUMENT": "Can not convert this argument from python to C++",
    "FACTORY_METHOD_CONFLICT": conflict,
    "FACTORY_GETTER_CONFLICT": conflict,
    "FACTORY_SETTER_CONFLICT": conflict,
    "FACTORY_MODULE_INIT_CONFLICT": conflict,
    "FACTORY_MEMBER": free_function,
    "FACTORY_STATIC_MEMBER": free_function,
    "FACTORY_NATIVE_CONSTRUCTOR": free_function,
    "FACTORY_OPERATOR": "Constructor factory cannot be an operator",
    "FACTORY_HIDDEN_FRIEND": "Constructor factory needs a namespace-scope declaration",
}

for case, diagnostic in cases.items():
    result = subprocess.run(
        [generator, "-p", build_dir, f"--extra-arg=-D{case}", source],
        capture_output=True,
        text=True,
        timeout=60,
    )
    output = result.stdout + result.stderr
    Path(source).with_suffix(f".{case.lower()}.log").write_text(output)
    # A signal/crash must not count as an expected diagnostic failure.
    if result.returncode <= 0 or diagnostic not in output:
        raise RuntimeError(
            f"{case}: expected diagnostic {diagnostic!r}; exit code {result.returncode}\n{output}"
        )
    print(f"{case}: rejected as expected")
