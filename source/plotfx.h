/**
 * This file is part of the "plotfx" project
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
 * The PlotFX C API
 *
 * How to use:
 *  1) Call `plotfx_init_*` to create a new context, for example `plotfx_init_svgfile`
 *  2) Call `plotfx_configure` and pass the configuration string
 *  3) Optional: Call `plotfx_setvar` to override/set dynamic variables
 *  4) Call `plotfx_submit`
 *  5) Optional: Retrieve the result using `plotfx_getimage`
 *  6) Optional: Repeat steps 2..6
 *  7) Once you're done with the PlotFX context, free it using `plotfx_destroy`
 */
#ifdef __cplusplus
extern "C" {
#endif

typedef void plotfx_t;

/**
 * Initialize a new PlotFX context.
 *
 * @returns: A plotfx context that must be free'd using `plotfx_destroy`
 */
plotfx_t* plotfx_init();

/**
 * Render the context to an file. If format is nullptr, the filetype is inferred
 * from the filename.
 *
 * @returns: One (1) on success and zero (0) if an error has occured
 */
int plotfx_render_file(plotfx_t* ctx, const char* path, const char* format);

/**
 * Render the context to an SVG file. The result image will
 * be written to the provided filesystem path once you call `plotfx_Submit`
 *
 * @returns: One (1) on success and zero (0) if an error has occured
 */
int plotfx_render_svgfile(plotfx_t* ctx, const char* path);

/**
 * Free a PlotFX context
 */
void plotfx_destroy(plotfx_t* ctx);

/**
 * Set the configuration of the PlotFX context. Please refer to the documentation
 * for the syntax and available properties in the PlotFX configuration file.
 *
 * @returns: One (1) on success and zero (0) if an error has occured
 */
int plotfx_configure(
    plotfx_t* ctx,
    const char* config);

/**
 * Set the configuration of the PlotFX context. Please refer to the documentation
 * for the syntax and available properties in the PlotFX configuration file.
 *
 * @returns: One (1) on success and zero (0) if an error has occured
 */
int plotfx_configure_file(
    plotfx_t* ctx,
    const char* path);

/**
 * Retrieve the last error message. The returned pointer is valid until the next
 * `plotfx_*` method is called on the context.
 */
const char* plotfx_geterror(const plotfx_t* ctx);

/**
 * Set a variable in the given PlotFX context.
 *
 * NOTE: If you use this to override a variable that is also set in the
 * configuration file, the order in which you call the `plotfx_configure` and
 * `plotfx_setvar` methods is significant.
 */
void plotfx_setvar_f64(
    plotfx_t* ctx,
    const char* name,
    size_t name_len,
    double value);

/**
 * Set a variable in the given PlotFX context.
 */
void plotfx_setvar_f64v(
    plotfx_t* ctx,
    const char* name,
    size_t name_len,
    const double* values,
    size_t value_count);

/**
 * Set a variable in the given PlotFX context.
 */
void plotfx_setvar_str(
    plotfx_t* ctx,
    const char* name,
    size_t name_len,
    const char* value,
    size_t value_len);

/**
 * Set a variable in the given PlotFX context.
 */
void plotfx_setvar_strv(
    plotfx_t* ctx,
    const char* name,
    size_t name_len,
    const char** values,
    const size_t* value_lens,
    size_t value_count);

#ifdef __cplusplus
} // extern C
#endif

