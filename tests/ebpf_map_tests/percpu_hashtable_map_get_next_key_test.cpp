#include <gtest/gtest.h>

extern "C" {
#include <stdint.h>
#include <errno.h>

#include <dev/ebpf/ebpf_map.h>
}

namespace {
class PercpuHashTableMapGetNextKeyTest : public ::testing::Test {
      protected:
	struct ebpf_map map;

	virtual void
	SetUp()
	{
		int error;

		struct ebpf_map_attr attr;
		attr.type = EBPF_MAP_TYPE_PERCPU_HASHTABLE;
		attr.key_size = sizeof(uint32_t);
		attr.value_size = sizeof(uint32_t);
		attr.max_entries = 100;
		attr.flags = 0;

		error = ebpf_map_init(&map, &attr);
		ASSERT_TRUE(!error);
	}

	virtual void
	TearDown()
	{
		ebpf_map_deinit(&map, NULL);
	}
};

TEST_F(PercpuHashTableMapGetNextKeyTest, GetFirstKey)
{
	int error;
	uint32_t key = 100, value = 200, next_key = 0;

	error = ebpf_map_update_elem_from_user(&map, &key, &value, 0);
	EXPECT_EQ(0, error);

	error = ebpf_map_get_next_key_from_user(&map, NULL, &next_key);
	EXPECT_EQ(0, error);
	EXPECT_EQ(100, next_key);
}

TEST_F(PercpuHashTableMapGetNextKeyTest, CorrectGetNextKey)
{
	int error;
	bool discovered[100];

	for (uint32_t i = 0; i < 100; i++) {
		discovered[i] = false;
		error = ebpf_map_update_elem_from_user(&map, &i, &i, 0);
		ASSERT_TRUE(!error);
	}

	uint32_t next_key, next_key_copy;
	error = ebpf_map_get_next_key_from_user(&map, NULL, &next_key);
	EXPECT_EQ(0, error);
	discovered[next_key] = true;

	while (!error) {
		next_key_copy = next_key;
		error = ebpf_map_get_next_key_from_user(&map, &next_key_copy, &next_key);
		discovered[next_key] = true;
	}

	for (uint32_t i = 0; i < 100; i++) {
		EXPECT_EQ(discovered[i], true);
	}
}
} // namespace
