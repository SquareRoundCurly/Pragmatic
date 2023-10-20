#pragma once

#define PY_VERSION_AT_LEAST(major, minor) ( \
    (major) < PY_MAJOR_VERSION || \
    ((major) == PY_MAJOR_VERSION && (minor) <= PY_MINOR_VERSION) \
)
