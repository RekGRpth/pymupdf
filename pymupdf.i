%module pymupdf
%{
/* Put header files here or function declarations like below */
extern PyObject *mupdf(PyObject *data, const char *input_type, const char *output_type, const char *options, const char *range);
%}

extern PyObject *mupdf(PyObject *data, const char *input_type, const char *output_type, const char *options, const char *range);
