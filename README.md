quant_system/
├── CMakeLists.txt                  # 主构建文件
├── include/                        # 公共头文件
│   ├── common/
│   │   ├── macros.hpp             # 通用宏定义（如跨平台修饰符）
│   │   └── types.hpp              # 统一数据类型（Decimal、Timestamp等）
│   └── utils/
│   │ ├── lockfree_queue.hpp      # 无锁队列实现
│   │ └── memory_pool.hpp         # 自定义内存池

│   │

​      └── core/

├── src/
│   ├── core/                      # 核心交易系统模块
│   │   ├── ctp_gateway/           # CTP接入层
│   │   │   ├── ctp_md_adapter.cpp # 行情接口实现
│   │   │   └── ctp_trader_adapter.cpp # 交易接口实现
│   │   ├── market_engine/         # 行情引擎
│   │   │   ├── tick_processor.cpp # Tick数据处理
│   │   │   └── bar_generator.cpp  # K线生成
│   │   ├── strategy_engine/       # 策略引擎
│   │   │   ├── strategy_loader.cpp # 策略动态加载
│   │   │   └── signal_generator.cpp # 信号生成
│   │   ├── order_manager/         # 订单管理
│   │   │   ├── order_router.cpp   # 智能路由
│   │   │   └── order_book.cpp     # 本地订单簿
│   │   └── risk_engine/           # 风控引擎
│   │       ├── risk_calculator.cpp # 风险计算
│   │       └── circuit_breaker.cpp # 熔断机制
│   ├── infrastructure/            # 基础设施
│   │   ├── event_bus/             # 事件总线
│   │   │   ├── zmq_event_bus.cpp  # ZeroMQ实现
│   │   │   └── event.hpp          # 事件定义（行情、订单等）
│   │   ├── logger/                # 日志系统
│   │   │   └── async_logger.cpp   # 异步日志实现
│   │   └── cache/                 # 缓存层
│   │       └── redis_cache.cpp    # Redis客户端封装
│   └── main.cpp                   # 系统入口
├── third_party/                   # 第三方依赖
│   ├── ctp_api/                   # CTP接口头文件及库
│   ├── zeromq/                    # ZeroMQ库
│   └── protobuf/                  # Protobuf库
├── config/                        # 配置文件
│   ├── strategy_config.yaml       # 策略参数
│   └── risk_rules.json            # 风控规则
└── tests/                         # 测试代码
    ├── unit_tests/                # 单元测试
    │   ├── test_market_engine.cpp
    │   └── test_order_manager.cpp
    └── perf_tests/                # 性能测试
        └── latency_benchmark.cpp
