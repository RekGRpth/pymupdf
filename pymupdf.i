%module pymupdf
%{
/* Put header files here or function declarations like below */
//extern int fact(int n);
extern PyObject *mupdf(PyObject *data, const char *input_type, const char *output_type, const char *options, const char *range);
%}

//extern int fact(int n);
extern PyObject *mupdf(PyObject *data, const char *input_type, const char *output_type, const char *options, const char *range);
