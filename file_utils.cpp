#include "file_utils.hpp"
#include <vector>
#include <string>
#include <algorithm>

bool is_source_file(const fs::path& path) {
    static const std::vector<std::string> extensions = {
        // C и C++
        ".c", ".cpp", ".cxx", ".cc", ".h", ".hpp", ".hxx", ".hh", ".inc",
        // Python
        ".py", ".pyw", ".pyx", ".pxd", ".pxi",
        // Java
        ".java", ".jav",
        // JavaScript
        ".js", ".jsx", ".mjs",
        // TypeScript
        ".ts", ".tsx",
        // Vim Script
        ".vim", ".vimrc",
        // Plain Text (обычно не включается, но добавим для полноты)
        ".txt",
        // Prolog
        ".pl", ".pro", ".p",
        // Makefile
        "Makefile", ".mk", ".mak",
        // Shell
        ".sh", ".bash", ".zsh", ".fish",
        // Autoconf
        ".ac", ".in",
        // Batch
        ".bat", ".cmd",
        // YAML
        ".yaml", ".yml",
        // Markdown
        ".md", ".markdown",
        // Perl
        ".pl", ".pm", ".t",
        // Module Definition
        ".def",
        // sed
        ".sed",
        // AWK
        ".awk",
        // HTML
        ".html", ".htm", ".xhtml",
        // Windows Resource
        ".rc",
        // Lua
        ".lua",
        // C Shell
        ".csh",
        // C#
        ".cs",
        // Emacs Lisp
        ".el", ".elc",
        // Forth
        ".fth", ".4th",
        // IDL
        ".idl",
        // JSON
        ".json",
        // Objective C
        ".m", ".mm",
        // SVG
        ".svg",
        // SystemVerilog
        ".sv", ".svh",
        // Wolfram
        ".wl", ".nb",
        // XML
        ".xml"
    };
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return std::find(extensions.begin(), extensions.end(), ext) != extensions.end();
}
