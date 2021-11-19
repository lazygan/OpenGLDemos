//
// Created by gangan on 2021/11/13.
//

#ifndef LPV_DEMO_H
#define LPV_DEMO_H

class Demo {
public:
    virtual void init() = 0;
    virtual void uninit() = 0;
    virtual void update() = 0;
};


#endif //LPV_DEMO_H
