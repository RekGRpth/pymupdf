#include <mupdf/fitz.h>
#include <string.h>
#include <Python.h>

/*static void pg_mupdf_error_callback(void *user, const char *message) {
    ereport(WARNING, (errmsg("%s", message)));
}

static void pg_mupdf_warning_callback(void *user, const char *message) {
    ereport(WARNING, (errmsg("%s", message)));
}*/

static void runpage(fz_context *ctx, fz_document *doc, int number, fz_document_writer *wri) {
    fz_page *page = fz_load_page(ctx, doc, number - 1);
    fz_try(ctx) {
        fz_rect mediabox = fz_bound_page(ctx, page);
        fz_device *dev = fz_begin_page(ctx, wri, mediabox);
        fz_run_page(ctx, page, dev, fz_identity, NULL);
        fz_end_page(ctx, wri);
    } fz_always(ctx) {
        fz_drop_page(ctx, page);
    } fz_catch(ctx) {
        fz_rethrow(ctx);
    }
}

static void runrange(fz_context *ctx, fz_document *doc, const char *range, fz_document_writer *wri) {
    int start, end, count = fz_count_pages(ctx, doc);
    while ((range = fz_parse_page_range(ctx, range, &start, &end, count))) {
        if (start < end) {
            for (int i = start; i <= end; i++) {
                runpage(ctx, doc, i, wri);
            }
        } else {
            for (int i = start; i >= end; i--) {
                runpage(ctx, doc, i, wri);
            }
        }
    }
}

PyObject *mupdf(PyObject *data, const char *input_type, const char *output_type, const char *options, const char *range) {
//    const char *output_data = NULL;
    PyObject *bytes = PyBytes_FromString("");
    char *input_data;
    Py_ssize_t input_len;
    if (PyBytes_AsStringAndSize(data, &input_data, &input_len)) goto ret;
    fz_context *ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
    if (!ctx) goto ret;
//    fz_set_error_callback(ctx, pg_mupdf_error_callback, r->connection->log);
//    fz_set_warning_callback(ctx, pg_mupdf_warning_callback, r->connection->log);
    fz_buffer *output_buffer = NULL;// fz_var(output_buffer);
    fz_buffer *input_buffer = NULL;// fz_var(input_buffer);
    fz_document *doc = NULL;// fz_var(doc);
    fz_document_writer *wri = NULL;// fz_var(wri);
    fz_try(ctx) {
        fz_register_document_handlers(ctx);
        fz_set_use_document_css(ctx, 1);
        output_buffer = fz_new_buffer(ctx, 0);
        fz_set_user_context(ctx, output_buffer);
        input_buffer = fz_new_buffer_from_data(ctx, (unsigned char *)input_data, input_len);
        fz_stream *input_stream = fz_open_buffer(ctx, input_buffer);
        doc = fz_open_document_with_stream(ctx, input_type, input_stream);
        wri = fz_new_document_writer(ctx, "buf:", output_type, options);
        runrange(ctx, doc, range, wri);
    } fz_always(ctx) {
        if (wri) fz_close_document_writer(ctx, wri);
        if (wri) fz_drop_document_writer(ctx, wri);
        if (doc) fz_drop_document(ctx, doc);
        if (input_buffer) fz_drop_buffer(ctx, input_buffer);
//        if (output_buffer) output_data = fz_string_from_buffer(ctx, output_buffer);
    } fz_catch(ctx) {
        goto fz_drop_context;
    }
    unsigned char *output_data = NULL;
    size_t output_len = fz_buffer_storage(ctx, output_buffer, &output_data);
//    ret = PyByteArray_FromStringAndSize((const char *)output_data, output_len);
    bytes = PyBytes_FromStringAndSize((const char *)output_data, (Py_ssize_t)output_len);
fz_drop_context:
    fz_drop_context(ctx);
ret:
    return bytes;
}

/*int fact(int n) {
    if (n <= 1) return 1;
    else return n*fact(n-1);
}*/