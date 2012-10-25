/*
 * This file is part of libkern.
 *
 * libkern is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libkern is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libkern.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COMPILER_H_
#define COMPILER_H_

#ifdef __GNUC__

/* Optimization barrier */
#define barrier() __asm__ __volatile__("": : :"memory")

/*
 * This macro obfuscates arithmetic on a variable address so that gcc
 * shouldn't recognize the original var, and make assumptions about it.
 *
 * This is needed because the C standard makes it undefined to do pointer
 * arithmetic on "objects" outside their boundaries and the gcc optimizers
 * assume this is the case. In particular they assume such arithmetic does not
 * wrap.
 */
#define RELOC_HIDE(ptr, off)          \
  ({ unsigned long __ptr;         \
    __asm__ ("" : "=r"(__ptr) : "0"(ptr));    \
    (typeof(ptr)) (__ptr + (off)); })

#define __must_be_array(a) __same_type((a), &(a)[0])

#define __deprecated      __attribute__((deprecated))
#define __packed      __attribute__((packed))
#define __weak        __attribute__((weak))

#define __noreturn __attribute__((noreturn))

/*
 * From the GCC manual:
 *
 * Many functions have no effects except the return value and their return
 * value depends only on the parameters and/or global variables.  Such a
 * function can be subject to common subexpression elimination and loop
 * optimization just as an arithmetic operator would be.  [...]
 */
#define __pure __attribute__((pure))
#define __aligned(x) __attribute__((aligned(x)))
#define __printf(a,b) __attribute__((format(printf,a,b)))
#define noinline __attribute__((noinline))
#define __attribute_const__ __attribute__((__const__))
#define __maybe_unused __attribute__((unused))
#define __always_unused __attribute__((unused))

#define __constructor __attribute__((constructor))
#define __destructor __attribute__((destructor))
#define __cleanup(f) __attribute__((cleanup(f)))

#define __hidden __attribute__((visibility("hidden")))
#define __internal __attribute__((visibility("internal")))
#define __protected __attribute__((visibility("protected")))

/*
 * A trick to suppress uninitialized variable warning without generating any
 * code
 */
#define uninitialized_var(x) x = x

#define __always_inline inline __attribute__((always_inline))

#if __GNUC__ == 4

#define __used __attribute__((__used__))
#define __must_check __attribute__((warn_unused_result))
#define __compiler_offsetof(a,b) __builtin_offsetof(a,b)

#if __GNUC_MINOR__ >= 3
/* Mark functions as cold. gcc will assume any path leading to a call
   to them will be unlikely.  This means a lot of manual unlikely()s
   are unnecessary now for any paths leading to the usual suspects
   like BUG(), printk(), panic() etc. [but let's keep them for now for
   older compilers]

   Early snapshots of gcc 4.3 don't support this and we can't detect this
   in the preprocessor, but we can live with this because they're unreleased.
   Maketime probing would be overkill here.

   gcc also has a __attribute__((__hot__)) to move hot functions into
   a special section, but I don't see any sense in this right now in
   the kernel context */
#define __cold      __attribute__((__cold__))
#define __hot      __attribute__((__hot__))

#define __linktime_error(message) __attribute__((__error__(message)))

#if __GNUC_MINOR__ >= 5
/*
 * Mark a position in code as unreachable.  This can be used to
 * suppress control flow warnings after asm blocks that transfer
 * control elsewhere.
 *
 * Early snapshots of gcc 4.5 don't support this and we can't detect
 * this in the preprocessor, but we can live with this because they're
 * unreleased.  Really, we need to have autoconf for the kernel.
 */
#define unreachable() __builtin_unreachable()

#endif
#endif

#if __GNUC_MINOR__ > 0
#define __compiletime_object_size(obj) __builtin_object_size(obj, 0)
#endif
#if __GNUC_MINOR__ >= 4 && !defined(__CHECKER__)
#define __compiletime_warning(message) __attribute__((warning(message)))
#define __compiletime_error(message) __attribute__((error(message)))
#endif

#endif

#endif /* __GNUC__ */

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/* Optimization barrier */
#ifndef barrier
#define barrier() __memory_barrier()
#endif

/* Unreachable code */
#ifndef unreachable
#define unreachable() do { } while (1)
#endif

#ifndef RELOC_HIDE
#define RELOC_HIDE(ptr, off) \
  ({ unsigned long __ptr; \
     __ptr = (unsigned long) (ptr); \
    (typeof(ptr)) (__ptr + (off)); })
#endif

/*
 * Allow us to mark functions as 'deprecated' and have gcc emit a nice
 * warning for each use, in hopes of speeding the functions removal.
 * Usage is:
 *    int __deprecated foo(void)
 */
#ifndef __deprecated
#define __deprecated   /* unimplemented */
#endif

#ifndef __must_check
#define __must_check
#endif

/*
 * Allow us to avoid 'defined but not used' warnings on functions and data,
 * as well as force them to be emitted to the assembly file.
 *
 * As of gcc 3.4, static functions that are not marked with attribute((used))
 * may be elided from the assembly file.  As of gcc 3.4, static data not so
 * marked will not be elided, but this may change in a future gcc version.
 *
 * NOTE: Because distributions shipped with a backported unit-at-a-time
 * compiler in gcc 3.3, we must define __used to be __attribute__((used))
 * for gcc >=3.3 instead of 3.4.
 *
 * In prior versions of gcc, such functions and data would be emitted, but
 * would be warned about except with attribute((unused)).
 *
 * Mark functions that are referenced only in inline assembly as __used so
 * the code is emitted even though it appears to be unreferenced.
 */
#ifndef __used
#define __used /* unimplemented */
#endif

#ifndef __maybe_unused
#define __maybe_unused /* unimplemented */
#endif

#ifndef __always_unused
#define __always_unused /* unimplemented */
#endif

#ifndef noinline
#define noinline
#endif

/*
 * Rather then using noinline to prevent stack consumption, use
 * noinline_for_stack instead.  For documentaiton reasons.
 */
#define noinline_for_stack noinline

#ifndef __always_inline
#define __always_inline inline
#endif

/*
 * From the GCC manual:
 *
 * Many functions do not examine any values except their arguments,
 * and have no effects except the return value.  Basically this is
 * just slightly more strict class than the `pure' attribute above,
 * since function is not allowed to read global memory.
 *
 * Note that a function that has pointer arguments and examines the
 * data pointed to must _not_ be declared `const'.  Likewise, a
 * function that calls a non-`const' function usually must not be
 * `const'.  It does not make sense for a `const' function to return
 * `void'.
 */
#ifndef __attribute_const__
# define __attribute_const__  /* unimplemented */
#endif

/*
 * Tell gcc if a function is cold. The compiler will assume any path
 * directly leading to the call is unlikely.
 */
#ifndef __cold
#define __cold
#endif

/* Simple shorthand for a section definition */
#ifndef __section
#define __section(S) __attribute__ ((__section__(#S)))
#endif

/* Are two types/vars the same type (ignoring qualifiers)? */
#ifndef __same_type
#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#endif

/* Compile time object size, -1 for unknown */
#ifndef __compiletime_object_size
#define __compiletime_object_size(obj) -1
#endif
#ifndef __compiletime_warning
#define __compiletime_warning(message)
#endif
#ifndef __compiletime_error
#define __compiletime_error(message)
#endif
#ifndef __linktime_error
#define __linktime_error(message)
#endif
/*
 * Prevent the compiler from merging or refetching accesses.  The compiler
 * is also forbidden from reordering successive instances of ACCESS_ONCE(),
 * but only when the compiler is aware of some particular ordering.  One way
 * to make the compiler aware of ordering is to put the two invocations of
 * ACCESS_ONCE() in different C statements.
 *
 * This macro does absolutely -nothing- to prevent the CPU from reordering,
 * merging, or refetching absolutely anything at any time.  Its main intended
 * use is to mediate communication between process-level code and irq/NMI
 * handlers, all running on the same CPU.
 */
#define ACCESS_ONCE(x) (*(volatile typeof(x) *)&(x))

/* Visibility type */
#ifndef __hidden
#define __hidden
#endif
#ifndef __internal
#define __internal
#endif
#ifndef __protected
#define __protected
#endif

#endif // COMPILER_H_
