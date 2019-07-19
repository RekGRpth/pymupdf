from distutils.core import setup, Extension
from distutils.command.build_py import build_py as build_py_orig
import sys, os

class build_ext_first(build_py_orig):
    def run(self):
        self.run_command("build_ext")
        return super().run()

if sys.platform.startswith("linux"):
    module = Extension(
        "_pymupdf",
        ["pymupdf.i", "pymupdf.c"],
        include_dirs=[
            "/usr/local/include/mupdf",
        ],
        libraries=[
            "mupdf",
            'mupdfthird',
            'freetype',
            'harfbuzz',
            'jbig2dec',
            'jpeg',
            'openjp2',
        ],
    )

setup(
    name="pymupdf",
    url="https://github.com/RekGRpth/pymupdf",
    cmdclass={"build_py": build_ext_first},
    ext_modules=[module],
    py_modules=["pymupdf"],
)
