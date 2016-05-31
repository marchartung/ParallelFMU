#will find OMCompiler and sets than OMC_COMPILER and OMC_FOUND
find_program(OMC_COMPILER NAMES omc)
if (OMC_COMPILER)
  set( OMC_FOUND TRUE )
endif()

if( OMC_FOUND )
    message( STATUS "Found omc compiler : ${OMC_COMPILER}" )
endif( OMC_FOUND )
