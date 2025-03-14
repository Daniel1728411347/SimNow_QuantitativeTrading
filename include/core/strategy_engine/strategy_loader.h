//
// Created by adm on 2025/3/12.
//

#ifndef STRATEGY_LOADER_H
#define STRATEGY_LOADER_H
#pragma once
// ---- 简单策略交易的类 ---- //

#include <functional>
#include "ThostFtdcUserApiStruct.h"
#include "bar_generator.h"
#include "ctp_trader_adapter.h"

typedef void(*reqOrderInsertFun)(
    TThostFtdcInstrumentIDType instrumentID,
    TThostFtdcPriceType price,
    TThostFtdcVolumeType volume,
    TThostFtdcDirectionType direction);

using ReqOrderInsertFunctionType = std::function<
    void(TThostFtdcInstrumentIDType instrumentID,
    TThostFtdcPriceType price,
    TThostFtdcVolumeType volume,
    TThostFtdcDirectionType direction)>;

void StrategyCheckAndTrade(TThostFtdcInstrumentIDType instrumentID, CustomTradeSpi *customTradeSpi);

#endif //STRATEGY_LOADER_H
