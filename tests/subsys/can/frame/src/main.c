/* main.c - Application main entry point */

/*
 * Copyright (c) 2019 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <logging/log.h>
LOG_MODULE_REGISTER(can_test, LOG_LEVEL_ERR);

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <can.h>

#include <ztest.h>

static void test_can_frame_to_msg(void)
{
	struct can_frame frame = { 0 };
	struct can_msg expected = { 0 };
	struct can_msg msg;
	const u8_t data[CAN_MAX_DLEN] = { 0x01, 0x02, 0x03, 0x04,
					  0x05, 0x06, 0x07, 0x08 };

	frame.can_id = BIT(31) | BIT(30) | 1234;
	frame.can_dlc = sizeof(data);
	memcpy(frame.data, data, sizeof(frame.data));

	expected.rtr = 1;
	expected.id_type = 1;
	expected.std_id = 1234;
	expected.dlc = sizeof(data);

	can_copy_frame_to_msg(&frame, &msg);

	LOG_HEXDUMP_DBG((const u8_t *)&frame, sizeof(frame), "frame");
	LOG_HEXDUMP_DBG((const u8_t *)&msg, sizeof(msg), "msg");
	LOG_HEXDUMP_DBG((const u8_t *)&expected, sizeof(expected), "expected");

	zassert_equal(msg.rtr, expected.rtr, "RTR bit not set");
	zassert_equal(msg.id_type, expected.id_type, "Id-type bit not set");
	zassert_equal(msg.std_id, expected.std_id, "Std CAN id invalid");
	zassert_equal(msg.dlc, expected.dlc, "Msg length invalid");
}

static void test_can_msg_to_frame(void)
{
	struct can_frame frame = { 0 };
	struct can_frame expected = { 0 };
	struct can_msg msg = { 0 };
	const u8_t data[CAN_MAX_DLEN] = { 0x01, 0x02, 0x03, 0x04,
					  0x05, 0x06, 0x07, 0x08 };

	expected.can_id = BIT(31) | BIT(30) | 1234;
	expected.can_dlc = sizeof(data);
	memcpy(expected.data, data, sizeof(expected.data));

	msg.rtr = 1;
	msg.id_type = 1;
	msg.std_id = 1234;
	msg.dlc = sizeof(data);
	memcpy(msg.data, data, sizeof(data));

	can_copy_msg_to_frame(&msg, &frame);

	LOG_HEXDUMP_DBG((const u8_t *)&frame, sizeof(frame), "frame");
	LOG_HEXDUMP_DBG((const u8_t *)&msg, sizeof(msg), "msg");
	LOG_HEXDUMP_DBG((const u8_t *)&expected, sizeof(expected), "expected");

	zassert_mem_equal(&frame.can_id, &expected.can_id, sizeof(frame.can_id),
			  "CAN ID not same");
	zassert_mem_equal(&frame.data, &expected.data, sizeof(frame.data),
			  "CAN data not same");
	zassert_equal(frame.can_dlc, expected.can_dlc,
		      "CAN msg length not same");
}

static void test_can_filter_to_msg_filter(void)
{
	struct can_filter filter = { 0 };
	struct can_msg_filter expected = { 0 };
	struct can_msg_filter msg_filter = { 0 };

	filter.can_id = BIT(31) | BIT(30) | 1234;
	filter.can_mask = BIT(31) | BIT(30) | 1234;

	expected.rtr = 1;
	expected.id_type = 1;
	expected.std_id = 1234;
	expected.rtr_mask = 1;
	expected.std_id_mask = 1234;

	can_copy_filter_to_msg_filter(&filter, &msg_filter);

	LOG_HEXDUMP_DBG((const u8_t *)&msg_filter, sizeof(msg_filter),
			"msg_filter");
	LOG_HEXDUMP_DBG((const u8_t *)&filter, sizeof(filter), "filter");
	LOG_HEXDUMP_DBG((const u8_t *)&expected, sizeof(expected), "expected");

	zassert_equal(msg_filter.rtr, expected.rtr, "RTR bit not set");
	zassert_equal(msg_filter.id_type, expected.id_type,
		      "Id-type bit not set");
	zassert_equal(msg_filter.std_id, expected.std_id,
		      "Std CAN id invalid");
	zassert_equal(msg_filter.rtr_mask, expected.rtr_mask,
		      "RTR mask bit not set");
	zassert_equal(msg_filter.std_id_mask, expected.std_id_mask,
		      "Std id mask not set");
}

static void test_can_msg_filter_to_filter(void)
{
	struct can_filter filter = { 0 };
	struct can_filter expected = { 0 };
	struct can_msg_filter msg_filter = { 0 };

	expected.can_id = BIT(31) | BIT(30) | 1234;
	expected.can_mask = BIT(31) | BIT(30) | 1234;

	msg_filter.rtr = 1;
	msg_filter.id_type = 1;
	msg_filter.std_id = 1234;
	msg_filter.rtr_mask = 1;
	msg_filter.std_id_mask = 1234;

	can_copy_msg_filter_to_filter(&msg_filter, &filter);

	LOG_HEXDUMP_DBG((const u8_t *)&msg_filter, sizeof(msg_filter),
			"msg_filter");
	LOG_HEXDUMP_DBG((const u8_t *)&filter, sizeof(filter), "filter");
	LOG_HEXDUMP_DBG((const u8_t *)&expected, sizeof(expected), "expected");

	zassert_mem_equal(&filter.can_id, &expected.can_id,
			  sizeof(filter.can_id), "CAN ID not same");
	zassert_mem_equal(&filter.can_mask, &expected.can_mask,
			  sizeof(filter.can_mask), "CAN mask not same");
}

void test_main(void)
{
	ztest_test_suite(test_can_frame,
			 ztest_unit_test(test_can_frame_to_msg),
			 ztest_unit_test(test_can_msg_to_frame),
			 ztest_unit_test(test_can_filter_to_msg_filter),
			 ztest_unit_test(test_can_msg_filter_to_filter));

	ztest_run_test_suite(test_can_frame);
}