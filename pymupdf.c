#include <Python.h>
#include <mupdf/fitz.h>

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
    PyObject *bytes = PyBytes_FromString("");
    char *input_data;
    Py_ssize_t input_len;
    if (PyBytes_AsStringAndSize(data, &input_data, &input_len)) goto ret;
    fz_context *ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
    if (!ctx) goto ret;
    fz_stream *stm = NULL;// fz_var(stm);
    fz_buffer *buf = NULL;// fz_var(buf);
    fz_document *doc = NULL;// fz_var(doc);
    fz_document_writer *wri = NULL;// fz_var(wri);
    fz_try(ctx) {
        fz_register_document_handlers(ctx);
        fz_set_use_document_css(ctx, 1);
        buf = fz_new_buffer(ctx, 0);
        fz_set_user_context(ctx, buf);
        stm = fz_open_memory(ctx, (unsigned char *)input_data, input_len);
        doc = fz_open_document_with_stream(ctx, input_type, stm);
        wri = fz_new_document_writer(ctx, "buf:", output_type, options);
        runrange(ctx, doc, range, wri);
    } fz_always(ctx) {
        if (wri) fz_close_document_writer(ctx, wri);
        if (wri) fz_drop_document_writer(ctx, wri);
        if (doc) fz_drop_document(ctx, doc);
        if (stm) fz_drop_stream(ctx, stm);
    } fz_catch(ctx) {
        goto fz_drop_context;
    }
    unsigned char *output_data = NULL;
    size_t output_len = fz_buffer_storage(ctx, buf, &output_data);
    bytes = PyBytes_FromStringAndSize((const char *)output_data, (Py_ssize_t)output_len);
fz_drop_context:
    if (buf) fz_drop_buffer(ctx, buf);
    fz_drop_context(ctx);
ret:
    return bytes;
}
