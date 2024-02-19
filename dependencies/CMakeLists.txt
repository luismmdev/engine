include(FetchContent)

# sfml
FetchContent_Declare(
  sfml
  GIT_REPOSITORY  "https://github.com/SFML/SFML"
  GIT_TAG         69ea0cd863aed1d4092b970b676924a716ff718b # 2.6.1
)
add_subdirectory(sfml)

# imgui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY  "https://github.com/ocornut/imgui"
  GIT_TAG         8490a262c45a27dc5acbc0ef68f81ed4ba784cf1
)
add_subdirectory(imgui)

# imgui-sfml
FetchContent_Declare(
  imgui-sfml
  GIT_REPOSITORY  "https://github.com/SFML/imgui-sfml"
  GIT_TAG         2a4b9dd9b34df61d91e33667dbaf5902c4923867
)
add_subdirectory(imgui-sfml)
