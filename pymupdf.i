%module pymupdf
%{
extern PyObject *mupdf(PyObject *data, const char *input_type, const char *output_type, const char *options, const char *range);
%}

extern PyObject *mupdf(PyObject *data, const char *input_type, const char *output_type, const char *options, const char *range);
