#pragma once

// __PRETTY_FUNCTION__
#ifdef _MSC_VER
#	define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

// Concatenate
#define CONCAT_IMPL(x, y) x##y // Helper macro to concatenate two tokens
#define CONCAT(x, y) CONCAT_IMPL(x, y) // Macro to concatenate two tokens