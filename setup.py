from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import setuptools

class get_pybind_include(object):
    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        import pybind11
        return pybind11.get_include(self.user)

ext_modules = [
    Extension(
        'cl_sdk_cpp',
        ['python_bindings.cpp', 'src/CorticalLabs.cpp', 'src/libclsdk.cpp', 'third_party/cJSON.c'],
        include_dirs=[
            get_pybind_include(),
            get_pybind_include(user=True),
            'include',
            'third_party'
        ],
        language='c++'
    ),
]

def has_flag(compiler, flagname):
    import tempfile
    with tempfile.NamedTemporaryFile('w', suffix='.cpp') as f:
        f.write('int main (int argc, char **argv) { return 0; }')
        try:
            compiler.compile([f.name], extra_postargs=[flagname])
        except setuptools.distutils.errors.CompileError:
            return False
    return True

class BuildExt(build_ext):
    c_opts = {
        'msvc': ['/EHsc'],
        'unix': [],
    }

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])
        if ct == 'unix':
            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            opts.append('-std=c++17')
            # Assuming Linux with Boost installed globally
            if sys.platform.startswith("linux"):
                opts.append('-DUSE_BOOST_BEAST')
                for ext in self.extensions:
                    ext.libraries.extend(['boost_system', 'boost_thread', 'ssl', 'crypto'])
        for ext in self.extensions:
            ext.extra_compile_args = opts
        build_ext.build_extensions(self)

setup(
    name='cl_sdk_cpp',
    version='1.2.0',
    author='Roe Defense',
    description='Cortical Labs C++ SDK Python Bindings',
    ext_modules=ext_modules,
    install_requires=['pybind11>=2.4'],
    cmdclass={'build_ext': BuildExt},
    zip_safe=False,
)
