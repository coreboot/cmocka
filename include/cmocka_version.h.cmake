/*
 * Copyright 2022      Andreas Schneider <asn@cryptomilk.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _CMOCKA_VERSION_H
#define _CMOCKA_VERSION_H

/**
 * @defgroup cmocka_version 📌 Version Information
 * @ingroup cmocka
 * @brief Query cmocka library version at compile time and runtime.
 *
 * These macros provide access to the cmocka library version information,
 * allowing you to check which version of cmocka is being used in your tests.
 *
 * The version is composed of three components:
 * - MAJOR: Incremented for incompatible API changes
 * - MINOR: Incremented for added functionality in a backward-compatible manner
 * - MICRO: Incremented for backward-compatible bug fixes
 *
 * ## Version Check Example
 *
 * You can use CMOCKA_VERSION_INT to check if the library meets a minimum
 * version requirement:
 *
 * @code
 * #include <cmocka_version.h>
 *
 * // Check if cmocka version is at least 1.2.0
 * #if CMOCKA_VERSION_INT >= CM_VERSION_INT(1, 2, 0)
 *     // Use features available in cmocka 1.2.0 or later
 * #endif
 * @endcode
 *
 * ## Version String Example
 *
 * The CMOCKA_VERSION macro can be used to get the version as a string:
 *
 * @code
 * #include <stdio.h>
 * #include <cmocka_version.h>
 *
 * int main(void) {
 *     printf("cmocka version: %s\n", CMOCKA_VERSION);
 *     return 0;
 * }
 * @endcode
 *
 * @{
 */

/**
 * @cond INTERNAL
 */

/**
 * @brief Helper macro to create an integer version value (internal use).
 */
#define CM_VERSION_INT(a, b, c) ((a) << 16 | (b) << 8 | (c))

/**
 * @brief Helper macro to create a version string (internal use).
 */
#define CM_VERSION_DOT(a, b, c) a ##.## b ##.## c

/**
 * @brief Helper macro to create a version string (internal use).
 */
#define CM_VERSION(a, b, c) CMOCKA_VERSION_DOT(a, b, c)

/** @endcond */

/**
 * @brief cmocka major version number.
 *
 * This is incremented when incompatible API changes are made.
 */
#define CMOCKA_VERSION_MAJOR  @cmocka_VERSION_MAJOR@

/**
 * @brief cmocka minor version number.
 *
 * This is incremented when functionality is added in a backward-compatible
 * manner.
 */
#define CMOCKA_VERSION_MINOR  @cmocka_VERSION_MINOR@

/**
 * @brief cmocka micro (patch) version number.
 *
 * This is incremented for backward-compatible bug fixes.
 */
#define CMOCKA_VERSION_MICRO  @cmocka_VERSION_PATCH@

/**
 * @brief cmocka version as an integer for easy comparison.
 *
 * This macro provides the complete version number as a single integer value,
 * which is useful for compile-time version checks.
 *
 * The version is encoded as: (major << 16) | (minor << 8) | micro
 *
 * @code
 * // Require at least cmocka 1.1.5
 * #if CMOCKA_VERSION_INT < CM_VERSION_INT(1, 1, 5)
 * #error "cmocka 1.1.5 or later is required"
 * #endif
 * @endcode
 *
 * @see CM_VERSION_INT()
 * @see CMOCKA_VERSION_MAJOR
 * @see CMOCKA_VERSION_MINOR
 * @see CMOCKA_VERSION_MICRO
 */
#define CMOCKA_VERSION_INT CM_VERSION_INT(CMOCKA_VERSION_MAJOR, \
                                          CMOCKA_VERSION_MINOR, \
                                          CMOCKA_VERSION_MICRO)

/**
 * @brief cmocka version as a string (e.g., "1.9.0").
 *
 * This macro provides the complete version number as a string constant.
 *
 * @code
 * #include <stdio.h>
 * #include <cmocka_version.h>
 *
 * printf("Using cmocka version: %s\n", CMOCKA_VERSION);
 * @endcode
 *
 * @see CMOCKA_VERSION_INT
 * @see CMOCKA_VERSION_MAJOR
 * @see CMOCKA_VERSION_MINOR
 * @see CMOCKA_VERSION_MICRO
 */
#define CMOCKA_VERSION     CM_VERSION(CMOCKA_VERSION_MAJOR, \
                                      CMOCKA_VERSION_MINOR, \
                                      CMOCKA_VERSION_MICRO)

/** @} */ /* cmocka_version */

#endif /* _CMOCKA_VERSION_H */
