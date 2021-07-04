file(GLOB_RECURSE SOURCES src/*)

set(CATCH vendor/catch2/catch_amalgamated.cpp vendor/catch2/catch_amalgamated.hpp)

add_executable(YART-Test ${SOURCES} ${CATCH})

source_group(" " REGULAR_EXPRESSION "(src|include)/[^/]+\.(h|cpp)")
source_group(accelerators REGULAR_EXPRESSION "(src|include)/accelerators/.*")
source_group(cameras REGULAR_EXPRESSION "(src|include)/cameras/.*")
source_group(core REGULAR_EXPRESSION "(src|include)/core/.*")
source_group(filters REGULAR_EXPRESSION "(src|include)/filters/.*")
source_group(geometry REGULAR_EXPRESSION "(src|include)/geometry/.*")
source_group(math REGULAR_EXPRESSION "(src|include)/math/.*")
source_group(samplers REGULAR_EXPRESSION "(src|include)/samplers/.*")
source_group(vendor/catch2 FILES ${CATCH})

target_include_directories(YART-Test PUBLIC src vendor/catch2 ../YART-Lib/include)
target_link_libraries(YART-Test PUBLIC YART-Lib zlibstatic Imath OpenEXR)