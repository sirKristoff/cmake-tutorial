cmake_minimum_required(VERSION 3.17)

list(APPEND CMAKE_MESSAGE_CONTEXT "subdir")

message("var: ${var}")

# change value of variable only in current scope
set(var "subdir")
message(NOTICE [[set(var "subdir")]])

message("var: ${var}")
