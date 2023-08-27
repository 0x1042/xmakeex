-- add_rules("mode.debug", "mode.release")
add_defines("VERSION=\"v0.1.0\"")

add_requires("openmp")

add_requires("vcpkg::folly", { alias = "folly" })
add_requires("vcpkg::glog", { alias = "glog" })
add_requires("vcpkg::gflags", { alias = "gflags" })
add_requires("vcpkg::double-conversion", { alias = "double-conversion" })
add_requires("vcpkg::fmt", { alias = "fmt" })
add_requires("vcpkg::asio[coroutine]", { alias = "asio" })
add_requires("vcpkg::rapidjson", { alias = "rapidjson" })
add_requires("vcpkg::abseil[cxx17]", { alias = "abseil" })
add_requires("vcpkg::cli11", { alias = "cli11" })
add_requires("vcpkg::wangle", { alias = "wangle" })
add_requires("vcpkg::spdlog", { alias = "spdlog" })

set_languages("c11", "cxx20")

target("xmakeex")
    set_kind("binary")
    add_files("src/*.cpp")
    add_files("src/infra/*.cpp")
    add_files("src/cli/*.cpp")
    add_files("src/socks/*.cpp")
    add_links("pthread")
    add_cxflags("-g")
    add_packages("openmp")
    add_packages("folly")
    add_packages("glog")
    add_packages("gflags")
    add_packages("double-conversion")
    add_packages("fmt")
    add_packages("asio")
    add_packages("rapidjson")
    add_packages("abseil")
    add_packages("cli11")
    add_packages("wangle")
    add_packages("spdlog")
