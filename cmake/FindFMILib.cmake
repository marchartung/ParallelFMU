# Find the fmi library provided by JModelica.
#
# Sets the usual variables expected for find_package scripts:
#
# FMILIB_INCLUDE_DIR - header location
# FMILIB_LIBRARIES - library to link against
# FMILIB_FOUND - true if the fmi library was found

FIND_PATH (FMILIB_INCLUDE_DIR NAMES fmilib.h HINTS ${FMILIB_HOME}/include)
FIND_LIBRARY (FMILIB_LIBRARY NAMES fmilib_shared HINTS ${FMILIB_HOME}/lib)

# Support the REQUIRED and QUIET arguments, and set FMILIB_FOUND if found.
INCLUDE (FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS (FMILIB DEFAULT_MSG FMILIB_LIBRARY FMILIB_INCLUDE_DIR)

IF (FMILIB_FOUND)
  SET (FMILIB_LIBRARIES ${FMILIB_LIBRARY})
  MESSAGE (STATUS "FMI library include = ${FMILIB_INCLUDE_DIR}")
  MESSAGE (STATUS "FMI library         = ${FMILIB_LIBRARY}")
ELSE (FMILIB_FOUND)
  MESSAGE (STATUS "No FMI library found")
ENDIF (FMILIB_FOUND)

MARK_AS_ADVANCED (FMILIB_LIBRARY FMILIB_INCLUDE_DIR)
