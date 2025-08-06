set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)
set(APP_VERSION_PATCH 0)
set(APP_VERSION_BUILD 0)
set(APP_VERSION "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}.${APP_VERSION_BUILD}")

set(APP_NAME "8mbapp")
set(APP_DISPLAY_NAME "8mbapp")
set(APP_DESCRIPTION "Unofficial 8mb.video desktop app")
set(APP_COPYRIGHT "(c) 2025 Caprine Logic. All rights reserved.")
set(APP_ORG "Caprine Logic")
set(APP_USER_MODEL_ID "CaprineLogic.8mbapp")
set(APP_REPO_NAME "8mbapp")
set(APP_REPO_OWNER "depthbomb")

string(TIMESTAMP APP_BUILD_DATE "%Y-%m-%dT%H:%M:%S" UTC)

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE APP_GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
