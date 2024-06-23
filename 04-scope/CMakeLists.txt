cmake_minimum_required(VERSION 3.17)
project(Scoping
        DESCRIPTION "")


function(fun_inner)
    # list(APPEND CMAKE_MESSAGE_CONTEXT "inner")
    set(CMAKE_MESSAGE_CONTEXT "${CMAKE_MESSAGE_CONTEXT};inner")
    message("var: ${var}")
    # change value only in parent scope!
    set(var "parent-inner" PARENT_SCOPE)
    message(NOTICE [[set(var "parent-inner" PARENT_SCOPE)]])
    message("var: ${var}")
endfunction()

function(fun_outer)
    list(APPEND CMAKE_MESSAGE_CONTEXT "outer")
    message("var: ${var}")
    # change value only in fun_outer() scope
    set(var "outer")
    message(NOTICE [[set(var "outer")]])
    # side effect: fun_inner() changes var in parrent scope
    fun_inner()
    message("var: ${var}")
endfunction()


list(APPEND CMAKE_MESSAGE_CONTEXT "global")

set(var "global")
message("var: ${var}")
fun_outer()
message("var: ${var}")

add_subdirectory(subdir)
# subdir list file changes var only in its scope
message("var: ${var}")
