/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include <stdlib.h>

/**
 * The fviz C API
 *
 * How to use:
 *  1) Call `fviz_init` to create a new context
 *  2) Call `fviz_configure` with your configuration string
 *  3) Optional: Set parameters using `fviz_setparam_*`
 *  4) Call `fviz_render_*`, e.g. `plotx_render_svg_file`
 *  5) Optional: Repeat steps 2..4
 *  6) Once you're done with the fviz context, free it using `fviz_destroy`
 */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct fviz_s fviz_t;

/**
 * Initialize a new fviz context.
 *
 * @returns: A fviz context that must be free'd using `fviz_destroy`
 */
fviz_t* fviz_init();

/**
 * Free a fviz context
 */
void fviz_destroy(fviz_t* ctx);

/**
 * Set the configuration of the fviz context. Please refer to the documentation
 * for the syntax and available properties in the fviz configuration file.
 *
 * @returns: One (1) on success and zero (0) if an error has occured
 */
int fviz_configure(
    fviz_t* ctx,
    const char* config);

/**
 * Set the configuration of the fviz context. Please refer to the documentation
 * for the syntax and available properties in the fviz configuration file.
 *
 * @returns: One (1) on success and zero (0) if an error has occured
 */
int fviz_configure_file(
    fviz_t* ctx,
    const char* path);

/**
 * Render the context to a file. If format is nullptr, the filetype is inferred
 * from the filename.
 *
 * @returns: One (1) on success and zero (0) if an error has occured
 */
int fviz_render_file(fviz_t* ctx, const char* path, const char* format);

/**
 * Render the context to a SVG file. The result image will
 * be written to the provided filesystem path once you call `fviz_submit`
 *
 * @returns: One (1) on success and zero (0) if an error has occured
 */
int fviz_render_svg_file(fviz_t* ctx, const char* path);

/**
 * Render the context to a PNG file. The result image will
 * be written to the provided filesystem path once you call `fviz_submit`
 *
 * @returns: One (1) on success and zero (0) if an error has occured
 */
int fviz_render_png_file(fviz_t* ctx, const char* path);

/**
 * Render the context to a user-provided backend. This method is only useful
 * if you want to extend fviz. Normal users are not expected to ever call this.
 *
 * @returns: One (1) on success and zero (0) if an error has occured
 */
int fviz_render_to(fviz_t* ctx, void* backend);

/**
 * Retrieve the last error message. The returned pointer is valid until the next
 * `fviz_*` method is called on the context.
 */
const char* fviz_geterror(const fviz_t* ctx);

/**
 * Set a user-defined parameter
 */
void fviz_setparam_f64(
    fviz_t* ctx,
    const char* name,
    size_t name_len,
    double value);

/**
 * Set a user-defined parameter
 */
void fviz_setparam_f64v(
    fviz_t* ctx,
    const char* name,
    size_t name_len,
    const double* values,
    size_t value_count);

/**
 * Set a user-defined parameter
 */
void fviz_setparam_str(
    fviz_t* ctx,
    const char* name,
    size_t name_len,
    const char* value,
    size_t value_len);

/**
 * Set a user-defined parameter
 */
void fviz_setparam_strv(
    fviz_t* ctx,
    const char* name,
    size_t name_len,
    const char** values,
    const size_t* value_lens,
    size_t value_count);

#ifdef __cplusplus
} // extern C
#endif

