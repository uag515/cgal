message("Configuring libCGAL_ImageIO")

find_package( ZLIB )

if(ZLIB_FOUND)
  get_dependency_version(ZLIB)
  cache_set(CGAL_ImageIO_3RD_PARTY_INCLUDE_DIRS ${CGAL_ImageIO_3RD_PARTY_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIR} )
  cache_set(CGAL_ImageIO_3RD_PARTY_LIBRARIES    ${CGAL_ImageIO_3RD_PARTY_LIBRARIES} ${ZLIB_LIBRARY} )
  cache_set(CGAL_ImageIO_3RD_PARTY_DEFINITIONS  "-DCGAL_USE_ZLIB")
  cache_set(CGAL_ImageIO_USE_ZLIB "ON")
endif(ZLIB_FOUND)

set( CGAL_ImageIO_BASENAME CGAL_ImageIO)

if(COMMAND add_config_flag)
  set( CGAL_HAS_IMAGEIO TRUE ) 
  add_config_flag( CGAL_HAS_IMAGEIO )
endif()

use_essential_libs()

include_directories( SYSTEM ${CGAL_3RD_PARTY_INCLUDE_DIRS} ${CGAL_ImageIO_3RD_PARTY_INCLUDE_DIRS} .)

link_directories    ( ${CGAL_LIBRARIES_DIR} ${CGAL_3RD_PARTY_LIBRARIES_DIRS} )

collect_cgal_library( CGAL_ImageIO "")

add_dependencies( CGAL_ImageIO CGAL )

add_definitions( ${CGAL_3RD_PARTY_DEFINITIONS} ${CGAL_ImageIO_3RD_PARTY_DEFINITIONS} )

# CGAL_ImageIO only depends on CGAL in DEBUG, but we still link it
# in both build types.
target_link_libraries( CGAL_ImageIO CGAL ${CGAL_3RD_PARTY_LIBRARIES} ${CGAL_ImageIO_3RD_PARTY_LIBRARIES} )

message("libCGAL_ImageIO is configured")
    

  
if(NOT ZLIB_FOUND)
  message( STATUS "NOTICE: libCGAL_ImageIO needs ZLib to read compressed files. That feature will not be activated.")
endif(NOT ZLIB_FOUND) 

