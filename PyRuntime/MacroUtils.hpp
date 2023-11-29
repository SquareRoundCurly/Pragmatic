#pragma once

#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)

#define UNIQUE_NAME(prefix) CONCATENATE(prefix, __COUNTER__)