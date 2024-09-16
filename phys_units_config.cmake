# Ensure the target is not already defined
if(NOT TARGET phys_units::phys_units)

    # Define an interface library target
    add_library(phys_units::phys_units INTERFACE IMPORTED)

    # Specify the interface include directories
    set_target_properties(phys::units PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/inc"
    )

endif()
