#ifndef TOOLKIT_MACRO_H
#define TOOLKIT_MACRO_H
#endif

#ifndef ALIGN_BIT_SIZE
#define ALIGN_BIT_SIZE(size) (((size) + static_cast<std::size_t>(7u)) & ~static_cast<std::size_t>(7u))
#endif
