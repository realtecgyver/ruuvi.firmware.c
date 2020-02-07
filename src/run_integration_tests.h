#ifndef RUN_INTEGRATION_TEST_H
#define RUN_INTEGRATION_TEST_H

#include "app_config.h"
#include "ruuvi_interface_log.h"

/**
 * @addtogroup Test
 */
/*@{*/
/**
 * @defgroup integration_test Run integration tests
 * @brief Test the complete system on real hardware
 *
 * Test results are printed as JSON log via RTT.
 */
/*@}*/
/**
 * @addtogroup integration_test
 */
/*@{*/
/**
 * @file run_integration_test.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-02-03
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

/** @brief Print test open JSON to console */
void integration_test_start(void);

/** @brief Run flash integration test. Tests watchdog too. */
void integration_test_flash(void);

/** @brief Print test close JSON to console */
void integration_test_stop(void);

/*@}*/
#endif