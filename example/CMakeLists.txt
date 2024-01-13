set(CMAKE_FOLDER "example")

#add_subdirectory(view)

# examples use a less draconian set of compiler warnings
EARanges_append_flag(EARANGES_HAS_WNO_MISSING_BRACES -Wno-missing-braces)
EARanges_append_flag(EARANGES_HAS_WNO_SHORTEN_64_TO_32 -Wno-shorten-64-to-32)
EARanges_append_flag(EARANGES_HAS_WNO_GLOBAL_CONSTRUCTORS -Wno-global-constructors)

EARanges_add_test(example.comprehensions comprehensions comprehensions.cpp)
EARanges_add_test(example.hello hello hello.cpp)
EARanges_add_test(example.count count count.cpp)
EARanges_add_test(example.count_if count_if count_if.cpp)
EARanges_add_test(example.any_all_none_of any_all_none_of any_all_none_of.cpp)
EARanges_add_test(example.for_each_sequence for_each_sequence for_each_sequence.cpp)
EARanges_add_test(example.for_each_assoc for_each_assoc for_each_assoc.cpp)
EARanges_add_test(example.is_sorted is_sorted is_sorted.cpp)
EARanges_add_test(example.find find find.cpp)
EARanges_add_test(example.filter_transform filter_transform filter_transform.cpp)
EARanges_add_test(example.accumulate_ints accumulate_ints accumulate_ints.cpp)
EARanges_add_test(example.comprehension_conversion comprehension_conversion comprehension_conversion.cpp)
EARanges_add_test(example.sort_unique sort_unique sort_unique.cpp)

# Guarded with a variable because the calendar example causes gcc to puke.
if(!EARANGES_BUILD_CALENDAR_EXAMPLE)
    set(Boost_USE_STATIC_LIBS        ON)
    set(Boost_USE_MULTITHREADED     OFF)
    set(Boost_USE_STATIC_RUNTIME    OFF)
    find_package(Boost COMPONENTS date_time program_options)

    if (Boost_FOUND)
        add_executable(EARanges.calendar calendar.cpp)
        target_include_directories(EARanges.calendar SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
        target_link_libraries(EARanges.calendar PRIVATE EARanges Boost::date_time Boost::program_options)
        message(STATUS "boost: ${Boost_LIBRARY_DIRS}")
        target_compile_definitions(EARanges.calendar PRIVATE BOOST_NO_AUTO_PTR)
        target_compile_options(EARanges.calendar PRIVATE -std=gnu++14)
       add_custom_target(EARanges.calendar ALL
    DEPENDS move_extra_contents
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/extra
)
    endif()
endif()
