# This is just an example of how to correctly write package configure files for modern cmake.
# However it is a bit complicated so I won't do it for other packages
find_package(ospray REQUIRED)

if (TARGET ospray::ospray)
  message(STATIS "found target ospray::ospray")
  if (TARGET ospray::ospray_common)
    message(STATIS "found target ospray::ospray_common")
  endif ()
  set_target_properties(ospray::ospray
    PROPERTIES INTERFACE_COMPILE_DEFINITIONS USE_OSP)
else ()
    # Because the OSPRAY_LIBRARIES includes keywords such as "optimized" and "debug", 
    # we have to remove them first in order to create a valid target. We also want to
    # seperate different libraries from the OSPRAY_LIBRARIES variable
    FOREACH (l ${OSPRAY_LIBRARIES})
        if ((NOT "${l}" STREQUAL "optimized") AND (NOT "${l}" STREQUAL "debug"))
            GET_FILENAME_COMPONENT(_name_ ${l} NAME_WE)
            if ("${_name_}" STREQUAL "libtbb_debug")
                set(OSPRAY_tbb_debug_LIBRARY ${l})
            elseif ("${_name_}" STREQUAL "libtbbmalloc_debug")
                set(OSPRAY_tbb_malloc_debug_LIBRARY ${l})
            elseif ("${_name_}" STREQUAL "libtbb")
                set(OSPRAY_tbb_optimized_LIBRARY ${l})
            elseif ("${_name_}" STREQUAL "libtbbmalloc")
                set(OSPRAY_tbb_malloc_optimized_LIBRARY ${l})
            elseif (("${_name_}" STREQUAL "libembree3") OR ("${_name_}" STREQUAL "libembree2"))
                set(OSPRAY_embree_LIBRARY ${l})
            elseif ("${_name_}" STREQUAL "libospray_module_ispc")
                set(OSPRAY_module_ispc_LIBRARY ${l})
            elseif ("${_name_}" STREQUAL "libospray_common")
                set(OSPRAY_common_LIBRARY ${l})
            elseif ("${_name_}" STREQUAL "libospray")
                set(OSPRAY_core_LIBRARY ${l})
            else ()
                message(FATAL_ERROR "Unrecongized library name \"${_name_}\" path \"${l}\"")
            endif ()
        endif ()
    endforeach ()

    # Create ospray targets
    # target -- ospray::ospray
    if (OSPRAY_core_LIBRARY AND NOT TARGET ospray::ospray)
        if (IS_ABSOLUTE "${OSPRAY_core_LIBRARY}")
            add_library(ospray::ospray UNKNOWN IMPORTED)
            set_target_properties(ospray::ospray PROPERTIES IMPORTED_LOCATION
                "${OSPRAY_core_LIBRARY}")
        else ()
            add_library(ospray::ospray INTERFACE IMPORTED)
            set_target_properties(ospray::ospray PROPERTIES IMPORTED_LIBNAME
                "${OSPRAY_core_LIBRARY}")
        endif ()
        set_target_properties(ospray::ospray PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${OSPRAY_INCLUDE_DIRS}"
            INTERFACE_COMPILE_DEFINITIONS USE_OSPRAY)
    endif ()
    # target -- ospray::common
    if (OSPRAY_common_LIBRARY AND NOT TARGET ospray::common)
        if (IS_ABSOLUTE "${OSPRAY_common_LIBRARY}")
            add_library(ospray::common UNKNOWN IMPORTED)
            set_target_properties(ospray::common PROPERTIES IMPORTED_LOCATION
                "${OSPRAY_common_LIBRARY}")
        else ()
            add_library(ospray::common INTERFACE IMPORTED)
            set_target_properties(ospray::common PROPERTIES IMPORTED_LIBNAME
                "${OSPRAY_common_LIBRARY}")
        endif ()
        set_target_properties(ospray::ospray PROPERTIES INTERFACE_LINK_LIBRARIES ospray::common)
    endif ()

    # If ospray is still using ISPC, we create a ospray::module_ispc target
    if (OSPRAY_module_ispc_LIBRARY AND NOT TARGET ospray::module_ispc)
        if (IS_ABSOLUTE "${OSPRAY_module_ispc_LIBRARY}")
            add_library(ospray::module_ispc UNKNOWN IMPORTED)
            set_target_properties(ospray::module_ispc PROPERTIES IMPORTED_LOCATION
                "${OSPRAY_module_ispc_LIBRARY}")
        else ()
            add_library(ospray::module_ispc INTERFACE IMPORTED)
            set_target_properties(ospray::module_ispc PROPERTIES IMPORTED_LIBNAME
                "${OSPRAY_module_ispc_LIBRARY}")
        endif ()
        set_target_properties(ospray::ospray PROPERTIES INTERFACE_LINK_LIBRARIES ospray::module_ispc)
    endif ()


    # If we have ospray mpi module built, lets link against it also
    if (EXISTS ${LIBRARY_PATH_PREFIX}ospray_module_mpi${LIBRARY_SUFFIX} AND
        NOT TARGET ospray::module_mpi)

        # Setup ospray::mpi target
        add_library(ospray::module_mpi UNKNOWN IMPORTED)
        set_target_properties(ospray::module_mpi PROPERTIES IMPORTED_LOCATION
            "${LIBRARY_PATH_PREFIX}ospray_module_mpi${LIBRARY_SUFFIX}")

        # However, lets setup ospray mpi common libraries also if possible
        if (${LIBRARY_PATH_PREFIX}ospray_mpi_common${LIBRARY_SUFFIX} AND
            NOT TARGET ospray::mpi_common)
            add_library(ospray::mpi_common UNKNOWN IMPORTED)
            set_target_properties(ospray::mpi_common PROPERTIES IMPORTED_LOCATION
                "${LIBRARY_PATH_PREFIX}ospray_mpi_common${LIBRARY_SUFFIX}")
            set_target_properties(ospray::module_mpi PROPERTIES INTERFACE_LINK_LIBRARIES ospray::mpi_common)
        endif ()
        if (EXISTS ${LIBRARY_PATH_PREFIX}ospray_mpi_maml${LIBRARY_SUFFIX} AND
            NOT TARGET ospray::mpi_maml)
            add_library(ospray::mpi_maml UNKNOWN IMPORTED)
            set_target_properties(ospray::mpi_maml PROPERTIES IMPORTED_LOCATION
                "${LIBRARY_PATH_PREFIX}ospray_mpi_maml${LIBRARY_SUFFIX}")
            set_target_properties(ospray::module_mpi PROPERTIES INTERFACE_LINK_LIBRARIES ospray::mpi_maml)
        endif ()

        # Well be careful with MPI ... make sure ENABLE_MPI is ON
        # set_target_properties(ospray::ospray PROPERTIES INTERFACE_LINK_LIBRARIES ospray::module_mpi)

    endif ()

    # If we have ospray mpi module built, lets link against it also
    # VisIt module is a customized module, there will be no pre-defined target
    if (EXISTS ${LIBRARY_PATH_PREFIX}ospray_module_visit${LIBRARY_SUFFIX})

        add_library(ospray::module_visit UNKNOWN IMPORTED)
        set_target_properties(ospray::module_visit PROPERTIES IMPORTED_LOCATION
            "${LIBRARY_PATH_PREFIX}ospray_module_visit${LIBRARY_SUFFIX}")

        add_library(ospray::module_visit_common UNKNOWN IMPORTED)
        set_target_properties(ospray::module_visit_common PROPERTIES IMPORTED_LOCATION
            "${LIBRARY_PATH_PREFIX}ospray_module_visit_common${LIBRARY_SUFFIX}")
        set_target_properties(ospray::module_visit PROPERTIES INTERFACE_LINK_LIBRARIES ospray::module_visit_common)

        set_target_properties(ospray::ospray PROPERTIES INTERFACE_LINK_LIBRARIES ospray::module_visit)

    endif ()

    # Create tbb target
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        # Link against `tbb*_debug` libraries
        if (OSPRAY_tbb_debug_LIBRARY AND NOT TARGET ospray::tbb) # target -- ospray::tbb
            if (IS_ABSOLUTE "${OSPRAY_tbb_debug_LIBRARY}")
                add_library(ospray::tbb UNKNOWN IMPORTED)
                set_target_properties(ospray::tbb PROPERTIES IMPORTED_LOCATION
                    "${OSPRAY_tbb_debug_LIBRARY}")
            else ()
                add_library(ospray::tbb UNKNOWN IMPORTED)
                set_target_properties(ospray::tbb PROPERTIES IMPORTED_LOCATION
                    "${OSPRAY_tbb_debug_LIBRARY}")
            endif ()
        endif ()
        if (OSPRAY_tbb_malloc_debug_LIBRARY AND NOT TARGET ospray::tbbmalloc) # target -- ospray::tbbmalloc
            if (IS_ABSOLUTE "${OSPRAY_tbb_malloc_debug_LIBRARY}")
                add_library(ospray::tbbmalloc UNKNOWN IMPORTED)
                set_target_properties(ospray::tbbmalloc PROPERTIES IMPORTED_LOCATION
                    "${OSPRAY_tbb_malloc_debug_LIBRARY}")
            else ()
                add_library(ospray::tbbmalloc UNKNOWN IMPORTED)
                set_target_properties(ospray::tbbmalloc PROPERTIES IMPORTED_LOCATION
                    "${OSPRAY_tbb_malloc_debug_LIBRARY}")
            endif ()
        endif ()
    else ()
        # Link against `tbb*` libraries
        if (OSPRAY_tbb_optimized_LIBRARY AND NOT TARGET ospray::tbb) # target -- ospray::tbb
            if (IS_ABSOLUTE "${OSPRAY_tbb_optimized_LIBRARY}")
                add_library(ospray::tbb UNKNOWN IMPORTED)
                set_target_properties(ospray::tbb PROPERTIES IMPORTED_LOCATION
                    "${OSPRAY_tbb_optimized_LIBRARY}")
            else ()
                add_library(ospray::tbb UNKNOWN IMPORTED)
                set_target_properties(ospray::tbb PROPERTIES IMPORTED_LOCATION
                    "${OSPRAY_tbb_optimized_LIBRARY}")
            endif ()
        endif ()
        if (OSPRAY_tbb_malloc_optimized_LIBRARY AND NOT TARGET ospray::tbbmalloc) # target -- ospray::tbbmalloc
            if (IS_ABSOLUTE "${OSPRAY_tbb_malloc_optimized_LIBRARY}")
                add_library(ospray::tbbmalloc UNKNOWN IMPORTED)
                set_target_properties(ospray::tbbmalloc PROPERTIES IMPORTED_LOCATION
                    "${OSPRAY_tbb_malloc_optimized_LIBRARY}")
            else ()
                add_library(ospray::tbbmalloc UNKNOWN IMPORTED)
                set_target_properties(ospray::tbbmalloc PROPERTIES IMPORTED_LOCATION
                    "${OSPRAY_tbb_malloc_optimized_LIBRARY}")
            endif ()
        endif ()
    endif ()
    if (TARGET ospray::tbb)
        if (TARGET ospray::tbbmalloc)
            set_target_properties(ospray::tbb PROPERTIES INTERFACE_LINK_LIBRARIES ospray::tbbmalloc)
        endif ()
        set_target_properties(ospray::ospray PROPERTIES INTERFACE_LINK_LIBRARIES ospray::tbb)
    endif ()

    # Create embree target
    if (OSPRAY_embree_LIBRARY AND NOT TARGET ospray::embree) # always guard with a check
        if (IS_ABSOLUTE "${OSPRAY_embree_LIBRARY}")
            add_library(ospray::embree UNKNOWN IMPORTED)
            set_target_properties(ospray::embree PROPERTIES IMPORTED_LOCATION
                "${OSPRAY_embree_LIBRARY}")
        else ()
            add_library(ospray::embree INTERFACE IMPORTED)
            set_target_properties(ospray::embree PROPERTIES IMPORTED_LIBNAME
                "${OSPRAY_embree_LIBRARY}")
        endif ()
        set_target_properties(ospray::ospray PROPERTIES INTERFACE_LINK_LIBRARIES ospray::embree)
    endif ()

endif ()