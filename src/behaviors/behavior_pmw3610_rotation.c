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
#include <pmw3610_api.h>

LOG_MODULE_DECLARE(pmw3610, CONFIG_INPUT_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
static const struct behavior_parameter_value_metadata no_arg_values[] = {
    {
        .display_name = "Clockwise",
        .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE,
        .value = ROT_CW,
    },
    {
        .display_name = "Counter-Clockwise",
        .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE,
        .value = ROT_CCW,
    },
};

static const struct behavior_parameter_metadata_set no_args_set = {
    .param1_values = no_arg_values,
    .param1_values_len = ARRAY_SIZE(no_arg_values),
};

static const struct behavior_parameter_metadata_set metadata_sets[] = {no_args_set};

static const struct behavior_parameter_metadata metadata = {
    .sets_len = ARRAY_SIZE(metadata_sets),
    .sets = metadata_sets,
};
#endif // IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)

struct behavior_pmw3610_rotation_config {
    uint8_t step_angle_degree;
};

static uint8_t step_angle_degree = CONFIG_PMW3610_DIRECTION_ANGLE;

static int behavior_pmw3610_rotation_init(const struct device *dev) {
    const struct behavior_pmw3610_rotation_config *cfg = dev->config;
    step_angle_degree = cfg->step_angle_degree;
    if (step_angle_degree <= 0 || CONFIG_PMW3610_DIRECTION_ANGLE < step_angle_degree) {
        LOG_WRN("step_angle_dgree %u out of range -> %u",
                step_angle_degree, CONFIG_PMW3610_DIRECTION_ANGLE);
        step_angle_degree = CONFIG_PMW3610_DIRECTION_ANGLE;
        return 0;
    }

    while (360 % step_angle_degree != 0) {
        LOG_WRN("step_angle_dgree %u cannot be divisible by %u",
                step_angle_degree, CONFIG_PMW3610_DIRECTION_ANGLE);
        step_angle_degree++;
    }

    return 0;
};

static int on_pmw3610_rotation_binding_pressed(struct zmk_behavior_binding *binding,
                                               struct zmk_behavior_binding_event event) {
    switch (binding->param1) {
        case ROT_CW:
            rotate_device_with_step(step_angle_degree);
            return ZMK_BEHAVIOR_OPAQUE;

        case ROT_CCW:
            rotate_device_with_step(-step_angle_degree);
            return ZMK_BEHAVIOR_OPAQUE;

        default:
            LOG_ERR("Unknown TB_ROT command: %d", binding->param1);
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
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .parameter_metadata = &metadata,
#endif // IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
};

#define PMW3610_ROT_INST(n)                                                                                 \
    static const struct behavior_pmw3610_rotation_config behavior_pmw3610_rotation_config_##n = {              \
        .step_angle_degree = DT_INST_PROP(n, step_angle_degree),              \
    };                                                                          \
    BEHAVIOR_DT_INST_DEFINE(n,                                                  \
                            &behavior_pmw3610_rotation_init,                              \
                            NULL,                                               \
                            NULL,                              \
                            &behavior_pmw3610_rotation_config_##n,                           \
                            POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,   \
                            &pmw3610_rotation_driver_api);

DT_INST_FOREACH_STATUS_OKAY(PMW3610_ROT_INST)

#endif
