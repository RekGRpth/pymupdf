from distutils.core import setup, Extension
from distutils.command.build_py import build_py as build_py_orig
import sys, os

# custom build_py command which runs build_ext first
# this is necessary because build_py needs the fitz.py which is only generated
# by SWIG in the build_ext step
class build_ext_first(build_py_orig):
    def run(self):
        self.run_command("build_ext")
        return super().run()


# check the platform
if sys.platform.startswith("linux"):
    module = Extension(
        "pymupdf",  # name of the module
        ["pymupdf.i"],
        include_dirs=[  # we need the path of the MuPDF and zlib headers
#            "/usr/include/mupdf",
            "/usr/local/include/mupdf",
#            "/usr/local/thirdparty/zlib",
        ],
        # library_dirs=['<mupdf_and_3rd_party_libraries_dir>'],
        libraries=[
            "mupdf",
            #'crypto', #openssl is required by mupdf on archlinux
            #'jbig2dec', 'openjp2', 'jpeg', 'freetype',
            "mupdf-third",
        ],  # the libraries to link with
    )

pkg_tab = open("PKG-INFO").read().split("\n")
long_dtab = []
classifier = []
for l in pkg_tab:
    if l.startswith("Classifier: "):
        classifier.append(l[12:])
        continue
    if l.startswith(" "):
        long_dtab.append(l.strip())
long_desc = "\n".join(long_dtab)

setup(
#    name="PyMuPDF",
#    version="1.14.17",
#    description="Python bindings for the PDF rendering library MuPDF",
#    long_description=long_desc,
    classifiers=classifier,
    url="https://github.com/RekGRpth/pymupdf",
#    author="Ruikai Liu, Jorj McKie",
#    author_email="jorj.x.mckie@outlook.de",
    cmdclass={"build_py": build_ext_first},
    ext_modules=[module],
    py_modules=["fitz.fitz", "fitz.utils"],
)
