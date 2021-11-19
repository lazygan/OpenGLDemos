//
// Created by gangan on 2021/11/16.
//

#ifndef LPV_BDEMO_H
#define LPV_BDEMO_H


class BDemo: public Demo {
protected:
    CTextureManager texManager;
    glm::mat4 m = glm::mat4(1.0f);
    glm::mat4 v, mvp, mv, vp, p;
    glm::mat3 mn;
    shared_ptr<CLightObject>  light;
    glm::mat4 v_light;
    glm::mat4 p_light;
    glm::mat4 mvp_light;
    glm::mat4 inverse_vLight;
    glm::mat3 mn_light;
    glm::vec3 lightPosition;
    glm::mat4 biasMatrix = glm::mat4(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
    );
public:
    virtual void init() = 0;
    virtual void uninit() = 0;
    virtual void update() {
        Camera& c = renderer::get_camera();
        v =c.view;
        p =c.projection;
        mn = glm::transpose(glm::inverse(glm::mat3(v*m)));
        mvp = p * v * m;
        mv = v * m;
        vp = p * v;
        if(light!=nullptr) {
            light->computeMatrixes();
            v_light = light->getViewMatrix();
            p_light = light->getProjMatrix();
            mvp_light = p_light * v_light * m;
            inverse_vLight = glm::inverse(v_light);
            mn_light = glm::transpose(glm::inverse(glm::mat3(v_light * m)));
            lightPosition = light->getPosition();
        }
    };

};


#endif //LPV_BDEMO_H
