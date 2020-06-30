# --- BOILERPLATE: install / packaging

include(CMakePackageConfigHelpers)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/config.cmake.in
${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake
INSTALL_DESTINATION cmake
)

write_basic_package_version_file(
${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}ConfigVersion.cmake
COMPATIBILITY SameMinorVersion
)

install(EXPORT ${PROJECT_NAME}-targets
NAMESPACE CURSES::
DESTINATION cmake
)

install(FILES
${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake
${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}ConfigVersion.cmake
DESTINATION cmake
)

# --- CPack

set(CPACK_GENERATOR "TBZ2")
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_PACKAGE_VENDOR "William McBrine")
set(CPACK_PACKAGE_CONTACT "William McBrine")
# set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

install(FILES ${CPACK_RESOURCE_FILE_README} ${CPACK_RESOURCE_FILE_LICENSE}
DESTINATION share/docs/${PROJECT_NAME}
)

include(CPack)
