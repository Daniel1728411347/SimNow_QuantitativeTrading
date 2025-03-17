cmake_minimum_required(VERSION 3.10)
project(SimNow_QuantitativeTrading)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(/utf-8)
# 输出调试信息
message(STATUS "项目源目录: ${CMAKE_SOURCE_DIR}")

# ===== 目录结构设置 =====
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/third_party")
#set(CTP_API_DIR "${THIRD_PARTY_DIR}/ctp_api")
set(CTP_API_DIR "${THIRD_PARTY_DIR}/CTP_APIPast")

# 添加在链接之前的验证步骤
if(EXISTS "${CTP_API_DIR}/thostmduserapi.lib")
    message(STATUS "找到行情库: ${CTP_API_DIR}/thostmduserapi.lib")
else()
    message(FATAL_ERROR "找不到行情库: ${CTP_API_DIR}/thostmduserapi.lib")
endif()

if(EXISTS "${CTP_API_DIR}/thosttraderapi.lib")
    message(STATUS "找到交易库: ${CTP_API_DIR}/thosttraderapi.lib")
else()
    message(FATAL_ERROR "找不到交易库: ${CTP_API_DIR}/thosttraderapi.lib")
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

# 明确指定源文件路径
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
        "${CTP_API_DIR}/thostmduserapi.lib"
        "${CTP_API_DIR}/thosttraderapi.lib"
)

# 生成最终可执行文件
add_executable(${PROJECT_NAME} ${SRC_DIR}/main.cpp)
# 让可执行文件在运行时找到 DLL
set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_ENVIRONMENT "PATH=${CTP_API_DIR}/;%PATH%"
)
# 链接库文件
target_link_libraries(${PROJECT_NAME} PRIVATE
        ctp_md_adapter
        ctp_trader_adapter
        bar_generator
        strategy_loader
        "${CTP_API_DIR}/thostmduserapi.lib"
        "${CTP_API_DIR}/thosttraderapi.lib"
)

# 输出目标配置信息
message(STATUS "CTP行情库: ${CTP_API_DIR}/thostmduserapi.lib")
message(STATUS "CTP交易库: ${CTP_API_DIR}/thosttraderapi.lib")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
