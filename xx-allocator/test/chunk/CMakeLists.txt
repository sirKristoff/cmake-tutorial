set(target chunk-tests)

set(source
    TestChunk.cpp
)

add_executable(${target} ${source})

target_link_libraries(${target}
	PRIVATE
	gmock-dev
	GTest::gtest_main
	loki # TODO: źródła biblioteki
)
gtest_add_tests(TARGET ${target})
