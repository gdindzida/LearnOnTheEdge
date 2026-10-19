# Library templates

`static/` and `shared/` are independent projects. Copy the desired directory as
a starting point and follow its README. Both expose a public header and link a
small consumer executable using a namespaced alias target.

A static library is linked into its consumer at build time. A shared library is
loaded at runtime and must be deployed alongside the executable or made available
through the platform's library search path. These examples demonstrate building
and linking; installation and CMake package exports can be added when needed.
