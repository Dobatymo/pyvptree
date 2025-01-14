#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <MathUtils.hpp>
#include <VPTree.hpp>

#include <Eigen/Core>
#include <chrono>
#include <exception>
#include <iostream>
#include <random>
#include <sstream>
#include <stdint.h>
#include <vector>

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <nmmintrin.h>
#endif

using namespace testing;

float distance(const Eigen::Vector3d &v1, const Eigen::Vector3d &v2) { return (v2 - v1).norm(); }

/* inline uint_fast8_t popcnt_u128(__uint128_t n) */
/* { */
/*     const uint64_t      n_hi    = n >> 64; */
/*     const uint64_t      n_lo    = n; */
/*     const uint_fast8_t  cnt_hi  = __builtin_popcountll(n_hi); */
/*     const uint_fast8_t  cnt_lo  = __builtin_popcountll(n_lo); */
/*     const uint_fast8_t  cnt     = cnt_hi + cnt_lo; */

/*     return  cnt; */
/* } */

int64_t dist_hamming(const std::vector<unsigned char> &p1, const std::vector<unsigned char> &p2) {

    // assume v1 and v2 sizes are multiple of 8
    // assume 32 bytes for now
    int64_t result = 0;
    const uint64_t *a = (reinterpret_cast<const uint64_t *>(&p1[0]));
    const uint64_t *b = (reinterpret_cast<const uint64_t *>(&p2[0]));
    for (int i = 0; i < p1.size() / sizeof(uint64_t); i++) {
        result += _mm_popcnt_u64(a[i] ^ b[i]);
    }
    return result;
}

namespace vptree::tests {
TEST(VPTests, TestHamming) {

    std::vector<unsigned char> b1 = {255, 255, 255, 255, 255, 255, 255, 255};
    std::vector<unsigned char> b2 = {255, 255, 255, 255, 255, 255, 255, 255};

    EXPECT_EQ(dist_hamming(b1, b2), 0);

    std::vector<unsigned char> b12 = {1,   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

    std::vector<unsigned char> b22 = {2,   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

    EXPECT_EQ(dist_hamming(b12, b22), 2);

    std::vector<unsigned char> b13 = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

    std::vector<unsigned char> b23 = {0,   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

    EXPECT_EQ(dist_hamming(b13, b23), 8);

    std::vector<unsigned char> b14 = {255, 255, 255, 255, 255, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

    std::vector<unsigned char> b24 = {0,   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
    EXPECT_EQ(dist_hamming(b14, b24), 9);

    std::vector<unsigned char> b15 = {255, 255, 255, 255, 255, 253, 255, 9,   255, 255, 255, 255, 255, 255, 255, 255,
                                      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
    std::vector<unsigned char> b25 = {0,   255, 255, 255, 255, 255, 255, 0,   255, 255, 255, 255, 255, 255, 255, 255,
                                      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
    EXPECT_EQ(dist_hamming(b15, b25), 11);

    std::vector<unsigned char> b16 = {255, 255, 255, 255, 255, 253, 255, 9,   255, 255, 255, 255, 255, 255, 255, 255,
                                      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
    std::vector<unsigned char> b26 = {0,   255, 255, 255, 255, 255, 255, 0,   255, 255, 255, 255, 255, 255, 255, 255,
                                      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
    EXPECT_EQ(dist_hamming(b16, b26), dist_hamming(b26, b16));

    std::vector<unsigned char> b17 = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

    std::vector<unsigned char> b27 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    EXPECT_EQ(dist_hamming(b17, b27), 256);
}

TEST(VPTests, TestEmpty) {
    VPTree<Eigen::Vector3d, float, distance> tree;
    tree.set(std::vector<Eigen::Vector3d>());

    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(-10, 10);

    std::vector<Eigen::Vector3d> queries;
    queries.resize(100);
    for (Eigen::Vector3d &point : queries) {
        point[0] = distribution(generator);
        point[1] = distribution(generator);
        point[2] = distribution(generator);
    }

    std::vector<int64_t> indices;
    std::vector<float> distances;
    EXPECT_THROW(tree.search1NN(queries, indices, distances), std::runtime_error);

    VPTree<Eigen::Vector3d, float, distance> treeEmpty2;
    EXPECT_THROW(treeEmpty2.search1NN(queries, indices, distances), std::runtime_error);

    VPTree<Eigen::Vector3d, float, distance> nonEmpty;
    nonEmpty.set(queries);
    EXPECT_NO_THROW(nonEmpty.search1NN(queries, indices, distances));
}

TEST(VPTests, TestToString) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(-10, 10);

    const unsigned int numPoints = 14001;
    std::vector<Eigen::Vector3d> points;
    points.reserve(numPoints);
    points.resize(numPoints);
    for (Eigen::Vector3d &point : points) {
        point[0] = distribution(generator);
        point[1] = distribution(generator);
        point[2] = distribution(generator);
    }

    VPTree<Eigen::Vector3d, float, distance> tree(points);

    std::stringstream ss;
    ss << tree;
}

TEST(VPTests, TestCopy) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(-10, 10);

    const unsigned int numPoints = 14001;
    std::vector<Eigen::Vector3d> points;
    points.reserve(numPoints);
    points.resize(numPoints);
    for (Eigen::Vector3d &point : points) {
        point[0] = distribution(generator);
        point[1] = distribution(generator);
        point[2] = distribution(generator);
    }

    VPTree<Eigen::Vector3d, float, distance> tree2;
    VPTree<Eigen::Vector3d, float, distance> tree(points);

    tree2 = tree;

    std::vector<Eigen::Vector3d> queries;
    queries.resize(100);
    for (Eigen::Vector3d &point : queries) {
        point[0] = distribution(generator);
        point[1] = distribution(generator);
        point[2] = distribution(generator);
    }

    std::vector<int64_t> indices;
    std::vector<float> distances;
    std::vector<int64_t> indices2;
    std::vector<float> distances2;
    tree.search1NN(queries, indices, distances);
    tree2.search1NN(queries, indices2, distances2);

    EXPECT_EQ(indices.size(), indices2.size());
    for (int i = 0; i < indices.size(); ++i) {
        EXPECT_EQ(indices[i], indices2[i]) << "Vectors x and y differ at index " << i;
        EXPECT_EQ(distances[i], distances2[i]) << "Vectors x and y differ at distance " << i;
    }
}

TEST(VPTests, TestSerialization) {

    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(-10, 10);

    const unsigned int numPoints = 14001;
    std::vector<Eigen::Vector3d> points;
    points.reserve(numPoints);
    points.resize(numPoints);
    for (Eigen::Vector3d &point : points) {
        point[0] = distribution(generator);
        point[1] = distribution(generator);
        point[2] = distribution(generator);
    }

    VPTree<Eigen::Vector3d, float, distance> tree2;
    VPTree<Eigen::Vector3d, float, distance> tree(points);
    auto state = tree.serialize();
    tree2.deserialize(state);

    std::vector<Eigen::Vector3d> queries;
    queries.resize(100);
    for (Eigen::Vector3d &point : queries) {
        point[0] = distribution(generator);
        point[1] = distribution(generator);
        point[2] = distribution(generator);
    }

    std::vector<int64_t> indices;
    std::vector<float> distances;
    std::vector<int64_t> indices2;
    std::vector<float> distances2;
    tree.search1NN(queries, indices, distances);
    tree2.search1NN(queries, indices2, distances2);

    EXPECT_EQ(indices.size(), indices2.size());
    for (int i = 0; i < indices.size(); ++i) {
        EXPECT_EQ(indices[i], indices2[i]) << "Vectors x and y differ at index " << i;
        EXPECT_EQ(distances[i], distances2[i]) << "Vectors x and y differ at distance " << i;
    }
}

TEST(VPTests, TestCreation) {

    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(-10, 10);

    const unsigned int numPoints = 10000;
    std::vector<Eigen::Vector3d> points;
    points.reserve(numPoints);
    points.resize(numPoints);
    for (Eigen::Vector3d &point : points) {
        point[0] = distribution(generator);
        point[1] = distribution(generator);
        point[2] = distribution(generator);
    }

    auto start = std::chrono::steady_clock::now();

    VPTree<Eigen::Vector3d, float, distance> tree(points);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<float> diff = end - start;
}

TEST(VPTests, TestSearch) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(-10, 10);

    const unsigned int numPoints = 4e4;
    std::vector<Eigen::Vector3d> points;
    points.resize(numPoints);
    for (Eigen::Vector3d &point : points) {
        point[0] = distribution(generator);
        point[1] = distribution(generator);
        point[2] = distribution(generator);
    }

    auto start = std::chrono::steady_clock::now();
    VPTree<Eigen::Vector3d, float, distance> tree(points);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<float> diff = end - start;

    std::vector<Eigen::Vector3d> queries;
    queries.resize(5000);
    for (Eigen::Vector3d &point : queries) {
        point[0] = distribution(generator);
        point[1] = distribution(generator);
        point[2] = distribution(generator);
    }
    /* std::vector<VPTree<Eigen::Vector3d, float, distance>::VPTreeSearchResultElement> results; */
    std::vector<int64_t> indices;
    std::vector<float> distances;
    start = std::chrono::steady_clock::now();
    tree.search1NN(queries, indices, distances);
    end = std::chrono::steady_clock::now();
    diff = end - start;
}
} // namespace vptree::tests
