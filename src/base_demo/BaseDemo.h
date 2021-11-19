//
// Created by gangan on 2021/11/13.
//

#ifndef LPV_BASEDEMO_H
#define LPV_BASEDEMO_H

#include "shadow_map.h"
#include "shadow_map.cpp"
#include "lights.h"
#include "lights.cpp"

class BaseDemo :public Demo{
public:
    void init();
    void uninit();
    void update();
};
#endif //LPV_BASEDEMO_H
