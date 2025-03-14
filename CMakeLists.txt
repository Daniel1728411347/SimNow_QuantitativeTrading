cmake_minimum_required(VERSION 3.10)
project(SimNow_QuantitativeTrading)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 输出调试信息
message(STATUS "项目源目录: ${CMAKE_SOURCE_DIR}")

# ===== 目录结构设置 =====
# 定义主要目录路径变量
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/third_party")
#set(CTP_API_DIR "${THIRD_PARTY_DIR}/ctp_api")
set(CTP_API_DIR "${THIRD_PARTY_DIR}/CTP_APIPast")



if(WIN32)
    set(CTP_MD_LIB "${CTP_API_DIR}/thostmduserapi.lib")
    set(CTP_TRADER_LIB "${CTP_API_DIR}/thosttraderapi.lib")

    # 添加复制DLL的命令（这部分会在创建可执行文件后执行）
    set(DLL_COPY_COMMAND
            COMMAND ${CMAKE_COMMAND} -E copy "${CTP_API_DIR}/bin/thostmduserapi.dll" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy "${CTP_API_DIR}/bin/thosttraderapi.dll" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
else()
    # Linux 配置
    set(CTP_MD_LIB thostmduserapi_se)
    set(CTP_TRADER_LIB thosttraderapi_se)
    set(DLL_COPY_COMMAND "")
    # Linux 系统需要的额外库
    set(SYSTEM_LIBS pthread dl)
endif()

link_directories(${CTP_API_DIR})

include_directories(
        ${INCLUDE_DIR}
        ${INCLUDE_DIR}/common
        ${INCLUDE_DIR}/core
        ${INCLUDE_DIR}/core/CTP_Gateway
        ${INCLUDE_DIR}/core/market_engine
        ${INCLUDE_DIR}/core/strategy_engine
        ${INCLUDE_DIR}/utils
        ${CTP_API_DIR}
)


# 明确指定源文件路径 - 根据您之前的描述
set(BAR_GENERATOR_SOURCES "${SRC_DIR}/core/market_engine/bar_generator.cpp")
set(CTP_MD_ADAPTER_SOURCES "${SRC_DIR}/core/CTP_Gateway/ctp_md_adapter.cpp")
set(CTP_TRADER_ADAPTER_SOURCES "${SRC_DIR}/core/CTP_Gateway/ctp_trader_adapter.cpp")
set(STRATEGY_LOADER_SOURCES "${SRC_DIR}/core/strategy_engine/strategy_loader.cpp")

# 定义模块化组件
# 1. BarGenerator模块
add_library(bar_generator STATIC
        ${BAR_GENERATOR_SOURCES}
        ${INCLUDE_DIR}/core/market_engine/bar_generator.h
)
target_link_directories(bar_generator PUBLIC ${INCLUDE_DIR})

# 2. CTP行情适配器模块
add_library(ctp_md_adapter STATIC
        ${CTP_MD_ADAPTER_SOURCES}
        ${INCLUDE_DIR}/core/CTP_Gateway/ctp_md_adapter.h
)
target_include_directories(ctp_md_adapter PUBLIC ${INCLUDE_DIR} ${CTP_API_DIR})
target_link_libraries(ctp_md_adapter PUBLIC
        bar_generator
        "${CTP_API_DIR}/thostmduserapi.lib"
)

# 3. CTP交易适配器模块
add_library(ctp_trader_adapter STATIC
        ${CTP_TRADER_ADAPTER_SOURCES}
        ${INCLUDE_DIR}/core/CTP_Gateway/ctp_trader_adapter.h
)
target_include_directories(ctp_trader_adapter PUBLIC ${INCLUDE_DIR} ${CTP_API_DIR})
target_link_libraries(ctp_trader_adapter PRIVATE
        ctp_md_adapter
        bar_generator
        "${CTP_API_DIR}/thosttraderapi.lib"
)

# 4. 创建策略引擎库
add_library(strategy_loader STATIC
        ${STRATEGY_LOADER_SOURCES}
        ${INCLUDE_DIR}/core/strategy_engine/strategy_loader.h
)
target_include_directories(strategy_loader PUBLIC ${INCLUDE_DIR} ${CTP_API_DIR})
target_link_libraries(strategy_loader PRIVATE
        bar_generator
        ctp_trader_adapter
        ${CTP_MD_LIB}
        ${CTP_TRADER_LIB}
)


add_executable(${PROJECT_NAME} ${SRC_DIR}/main.cpp)


if(MSVC)
    # MSVC使用相对路径链接
    target_link_libraries(${PROJECT_NAME} PRIVATE
            thostmduserapi
            thosttraderapi
    )
elseif(MINGW)
    # MinGW需要完整路径
    target_link_libraries(${PROJECT_NAME} PRIVATE
            "${CTP_API_DIR}/thostmduserapi.lib"
            "${CTP_API_DIR}/thosttraderapi.lib"
    )
else()
    # 其他平台（如Linux）
    target_link_libraries(${PROJECT_NAME} PRIVATE
            thostmduserapi_se
            thosttraderapi_se
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE
        ctp_md_adapter
        ctp_trader_adapter
        bar_generator
        strategy_loader
        "${CTP_API_DIR}/thostmduserapi.lib"  # 直接链接
        "${CTP_API_DIR}/thosttraderapi.lib"
)

# 添加DLL复制命令（仅Windows）
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD ${DLL_COPY_COMMAND})
endif()
# 输出目标配置信息
message(STATUS "CTP行情库: ${CTP_MD_LIB}")
message(STATUS "CTP交易库: ${CTP_TRADER_LIB}")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")