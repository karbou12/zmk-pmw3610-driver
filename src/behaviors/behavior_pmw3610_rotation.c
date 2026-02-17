/*
 * Copyright (c) 2026 Tano Karbou (github: karbou12 / X: @karbou_12)
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_pmw3610_rotation

// Dependencies
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <drivers/behavior.h>

#include <zmk/behavior.h>
#include <dt-bindings/zmk/pmw3610_rotation.h>
#include <pmw3610.h>

// LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);
LOG_MODULE_DECLARE(pmw3610, CONFIG_INPUT_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

enum cw_mode {
    CW,
    CCW,
};

struct behavior_pmw3610_rotation_config {
    enum cw_mode cw_mode;
    uint8_t step_angle_degree;
};

static int on_pmw3610_rotation_binding_pressed(struct zmk_behavior_binding *binding,
                                               struct zmk_behavior_binding_event event) {
    LOG_WRN("%s\n", __FUNCTION__);
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct behavior_pmw3610_rotation_config *cfg = dev->config;

    switch (cfg->cw_mode) {
        case CW:
            rotate_device_with_step(cfg->step_angle_degree);
            return ZMK_BEHAVIOR_OPAQUE;

        case CCW:
            rotate_device_with_step(-cfg->step_angle_degree);
            return ZMK_BEHAVIOR_OPAQUE;

        default:
            return -ENOTSUP;
    };
}

static int on_pmw3610_rotation_binding_released(struct zmk_behavior_binding *binding,
                                                struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api pmw3610_rotation_driver_api = {
    .binding_pressed = on_pmw3610_rotation_binding_pressed,
    .binding_released = on_pmw3610_rotation_binding_released,
};

#define PMW3610_ROT_INST(n)                                                                                 \
    static const struct behavior_pmw3610_rotation_config behavior_pmw3610_rotation_config_##n = {              \
        .cw_mode = DT_ENUM_IDX(DT_DRV_INST(n), cw_mode),                        \
        .step_angle_degree = DT_INST_PROP(n, step_angle_degree),              \
    };                                                                          \
    BEHAVIOR_DT_INST_DEFINE(n,                                                  \
                            NULL,                              \
                            NULL,                                               \
                            NULL,                              \
                            &behavior_pmw3610_rotation_config_##n,                           \
                            POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,   \
                            &pmw3610_rotation_driver_api);

DT_INST_FOREACH_STATUS_OKAY(PMW3610_ROT_INST)

#endif
