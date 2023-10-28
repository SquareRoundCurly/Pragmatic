#pragma once

// External
#include "pytypedefs.h"

#define PY_VERSION_AT_LEAST(major, minor) (                       \
    (major) < PY_MAJOR_VERSION ||                                 \
    ((major) == PY_MAJOR_VERSION && (minor) <= PY_MINOR_VERSION))

namespace Pragmatic::auto_graph
{
	PyObject* ensure_tuple(PyObject* obj);
	PyObject* merge_tuples(PyObject* args1, PyObject* args2);
} // namespace Pragmatic::auto_graph
