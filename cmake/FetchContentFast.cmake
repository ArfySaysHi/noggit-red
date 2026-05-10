# This allows faster configure time when working with cmake files
macro(FetchContent_PopulateFast dep)
  FetchContent_MakeAvailable(${dep})
endmacro()
