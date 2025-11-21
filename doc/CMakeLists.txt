#
# Build the documentation
#
# To build the documentation with a local doxygen-awesome-css tarball:
#
#   cmake -S . -B obj \
#         -DDOXYGEN_AWESOME_CSS_TARBALL=/path/to/doxygen-awesome-css.tar.gz
#   cmake --build obj --target docs
#
# The tarball can be downloaded from:
#   https://github.com/jothepro/doxygen-awesome-css/archive/refs/tags/v2.4.1.tar.gz
#
find_package(Doxygen)

if (DOXYGEN_FOUND)
    # Allow specifying a local tarball for doxygen-awesome-css (useful for packaging)
    if (NOT DEFINED DOXYGEN_AWESOME_CSS_TARBALL)
        set(DOXYGEN_AWESOME_CSS_TARBALL
            "https://github.com/jothepro/doxygen-awesome-css/archive/refs/tags/v2.4.1.tar.gz")
    endif()

    include(FetchContent)
    FetchContent_Declare(
        doxygen-awesome-css
        URL ${DOXYGEN_AWESOME_CSS_TARBALL}
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(doxygen-awesome-css)

    # Get the path to doxygen-awesome.css
    FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

    # Project title shown in documentation
    set(DOXYGEN_PROJECT_NAME ${PROJECT_NAME})
    # Project version number shown in documentation
    set(DOXYGEN_PROJECT_NUMBER ${PROJECT_VERSION})
    # Brief description shown below project name
    set(DOXYGEN_PROJECT_BRIEF "Unit testing library with mock support")
    # Project favicon (browser tab icon)
    set(DOXYGEN_PROJECT_ICON ${CMAKE_CURRENT_SOURCE_DIR}/favicon.png)

    # Number of spaces used for indentation in code blocks
    set(DOXYGEN_TAB_SIZE 4)
    # Generate output optimized for C (vs C++)
    set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)
    # Enable parsing of markdown in comments
    set(DOXYGEN_MARKDOWN_SUPPORT YES)
    # Warn about undocumented members to improve documentation quality
    set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
    # Disable built-in clipboard (using doxygen-awesome extension instead)
    set(DOXYGEN_HTML_COPY_CLIPBOARD NO)
    # Disable page outline panel (using interactive TOC extension instead)
    set(DOXYGEN_PAGE_OUTLINE_PANEL NO)

    # Required configuration for doxygen-awesome-css theme
    # Generate treeview sidebar for navigation
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    # Enable default index pages
    set(DOXYGEN_DISABLE_INDEX NO)
    # Use top navigation bar instead of full sidebar (required for theme compatibility)
    set(DOXYGEN_FULL_SIDEBAR NO)
    # Use light color style (required for Doxygen >= 1.9.5)
    set(DOXYGEN_HTML_COLORSTYLE LIGHT)

    # Disable diagram generation (not relevant for C projects)
    set(DOXYGEN_HAVE_DOT NO)
    set(DOXYGEN_CLASS_DIAGRAMS NO)
    set(DOXYGEN_CALL_GRAPH NO)
    set(DOXYGEN_CALLER_GRAPH NO)

    # Preprocessor defines to use when parsing code
    set(DOXYGEN_PREDEFINED DOXYGEN
                           CMOCKA_PRINTF_ATTRIBUTE\(x,y\))

    # Custom layout file to rename "Topics" to "API Reference" and simplify navigation
    set(DOXYGEN_LAYOUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/DoxygenLayout.xml)
    # Custom HTML header with doxygen-awesome extension initialization
    set(DOXYGEN_HTML_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/header.html)
    # Modern CSS theme for documentation with custom cmocka.org color scheme
    set(DOXYGEN_HTML_EXTRA_STYLESHEET ${AWESOME_CSS_DIR}/doxygen-awesome.css
                                      ${CMAKE_CURRENT_SOURCE_DIR}/doxygen-custom.css)
    # JavaScript extensions: dark mode toggle, copy button, paragraph links, interactive TOC
    set(DOXYGEN_HTML_EXTRA_FILES ${AWESOME_CSS_DIR}/doxygen-awesome-darkmode-toggle.js
                                 ${AWESOME_CSS_DIR}/doxygen-awesome-fragment-copy-button.js
                                 ${AWESOME_CSS_DIR}/doxygen-awesome-paragraph-link.js
                                 ${AWESOME_CSS_DIR}/doxygen-awesome-interactive-toc.js)

    set(_doxyfile_template "${CMAKE_BINARY_DIR}/CMakeDoxyfile.in")
    set(_target_doxyfile "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.docs")
    configure_file("${_doxyfile_template}" "${_target_doxyfile}")

    doxygen_add_docs(docs
                     ${cmocka-library_SOURCE_DIR}
                     ${cmocka-header_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR})
endif(DOXYGEN_FOUND)
