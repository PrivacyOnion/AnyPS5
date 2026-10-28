include(${CMAKE_CURRENT_SOURCE_DIR}/../libSceSaveDataDialog.native/SaveDataDialogLibrary.cmake)
add_sce_savedata_dialog_library(libSceSaveDataDialog)
target_sources(libSceSaveDataDialog PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Export.cpp)
