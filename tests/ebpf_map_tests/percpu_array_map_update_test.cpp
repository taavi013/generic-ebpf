#include <gtest/gtest.h>

extern "C" {
#include <stdint.h>
#include <errno.h>

#include <dev/ebpf/ebpf_map.h>
}

namespace {
class PercpuArrayMapUpdateTest : public ::testing::Test {
  protected:
    struct ebpf_map map;

    virtual void
    SetUp()
    {
        int error;

        error = ebpf_map_init(&map, EBPF_MAP_TYPE_PERCPU_ARRAY, sizeof(uint32_t),
                              sizeof(uint32_t), 100, 0);
        ASSERT_TRUE(!error);
    }

    virtual void
    TearDown()
    {
        ebpf_map_deinit(&map, NULL);
    }
};

TEST_F(PercpuArrayMapUpdateTest, UpdateWithMaxPlusOneKey)
{
    int error;
    uint32_t key = 100, value = 100;

    error = ebpf_map_update_elem(&map, &key, &value, 0);

    EXPECT_EQ(EINVAL, error);
}

TEST_F(PercpuArrayMapUpdateTest, CorrectUpdate)
{
    int error;
    uint32_t key = 50, value = 100;

    error = ebpf_map_update_elem(&map, &key, &value, 0);

    EXPECT_EQ(0, error);
}

TEST_F(PercpuArrayMapUpdateTest, CorrectUpdateOverwrite)
{
    int error;
    uint32_t key = 50, value = 100;

    error = ebpf_map_update_elem(&map, &key, &value, 0);
    ASSERT_TRUE(!error);

    value = 101;
    error = ebpf_map_update_elem(&map, &key, &value, 0);

    EXPECT_EQ(0, error);
}

TEST_F(PercpuArrayMapUpdateTest, CorrectUpdateMoreThanMaxEntries)
{
    int error;
    uint32_t i;

    for (i = 0; i < 100; i++) {
        error = ebpf_map_update_elem(&map, &i, &i, 0);
        ASSERT_TRUE(!error);
    }

    error = ebpf_map_update_elem(&map, &i, &i, 0);
    EXPECT_EQ(EBUSY, error);
}
}