// This file performs positive and negative tests on a vector of integers.

// Uncomment the next line to use precompiled headers
#include "pch.h"
// uncomment the next line if you do not use precompiled headers
//#include "gtest/gtest.h"
//
// the global test environment setup and tear down
// you should not need to change anything here
class Environment : public ::testing::Environment
{
public:
    ~Environment() override {}

    // Override this to define how to set up the environment.
    void SetUp() override
    {
        //  initialize random seed
        srand(time(nullptr));
    }

    // Override this to define how to tear down the environment.
    void TearDown() override {}
};

// create our test class to house shared data between tests
// you should not need to change anything here
class CollectionTest : public ::testing::Test
{
protected:
    // create a smart point to hold our collection
    std::unique_ptr<std::vector<int>> collection;

    void SetUp() override
    { // create a new collection to be used in the test
        collection.reset(new std::vector<int>);
    }

    void TearDown() override
    { //  erase all elements in the collection, if any remain
        collection->clear();
        // free the pointer
        collection.reset(nullptr);
    }

    // helper function to add random values from 0 to 99 count times to the collection
    void add_entries(int count)
    {
        assert(count > 0);
        for (auto i = 0; i < count; ++i)
            collection->push_back(rand() % 100);
    }
};

// When should you use the EXPECT_xxx or ASSERT_xxx macros?
// Use ASSERT when failure should terminate processing, such as the reason for the test case.
// Use EXPECT when failure should notify, but processing should continue

// Test that a collection is empty when created.
// Prior to calling this (and all other TEST_F defined methods),
//  CollectionTest::StartUp is called.
// Following this method (and all other TEST_F defined methods),
//  CollectionTest::TearDown is called
TEST_F(CollectionTest, CollectionSmartPointerIsNotNull)
{
    // is the collection created
    ASSERT_TRUE(collection);

    // if empty, the size must be 0
    ASSERT_NE(collection.get(), nullptr);
}

// Test that a collection is empty when created.
TEST_F(CollectionTest, IsEmptyOnCreate)
{
    // is the collection empty?
    ASSERT_TRUE(collection->empty());

    // if empty, the size must be 0
    ASSERT_EQ(collection->size(), 0);
}

/* Comment this test out to prevent the test from running
 * Uncomment this test to see a failure in the test explorer */
//TEST_F(CollectionTest, AlwaysFail)
//{
//    FAIL();
//}

// Test to verify adding a single value to an empty collection
TEST_F(CollectionTest, CanAddToEmptyVector)
{
    // is the collection empty?
    ASSERT_TRUE(collection->empty());
    // if empty, the size must be 0
    ASSERT_EQ(collection->size(), 0);

    add_entries(1);

    // is the collection still empty?
    ASSERT_FALSE(collection->empty());
    // if not empty, what must the size be?
    ASSERT_EQ(collection->size(), 1);
}

// Test to verify adding five values to collection
TEST_F(CollectionTest, CanAddFiveValuesToVector)
{
    // Check initial size of the collection
    size_t initial_size = collection->size();

    add_entries(5);

    // Check if five entries were added successfully
    ASSERT_EQ(collection->size(), initial_size + 5);
}

// Test to verify that max size is greater than or equal to size for 0, 1, 5, 10 entries
TEST_F(CollectionTest, MaxSizeGreaterOrEqualToSize)
{
    // Test for 0 entries
    collection->clear();
    ASSERT_TRUE(collection->max_size() >= collection->size());

    // Test for 1 entry
    add_entries(1);
    ASSERT_TRUE(collection->max_size() >= collection->size());

    // Test for 5 entries
    collection->clear();
    add_entries(5);
    ASSERT_TRUE(collection->max_size() >= collection->size());

    // Test for 10 entries
    collection->clear();
    add_entries(10);
    ASSERT_TRUE(collection->max_size() >= collection->size());
}

// Test to verify that capacity is greater than or equal to size for 0, 1, 5, 10 entries
TEST_F(CollectionTest, CapacityGreaterOrEqualToSize)
{
    // Test for 0 entries
    collection->clear();
    ASSERT_TRUE(collection->capacity() >= collection->size());

    // Test for 1 entry
    add_entries(1);
    ASSERT_TRUE(collection->capacity() >= collection->size());

    // Test for 5 entries
    collection->clear();
    add_entries(5);
    ASSERT_TRUE(collection->capacity() >= collection->size());

    // Test for 10 entries
    collection->clear();
    add_entries(10);
    ASSERT_TRUE(collection->capacity() >= collection->size());
}

// Test to verify resizing increases the collection
TEST_F(CollectionTest, ResizingIncreasesCollection)
{
    // Add 5 entries to the collection
    add_entries(5);

    // Store the current size of the collection
    size_t old_size = collection->size();

    // Resize the collection to a larger size
    collection->resize(old_size + 5);

    // Verify that the size of the collection has increased
    ASSERT_EQ(collection->size(), old_size + 5);

    // Add an EXPECT_* to verify that the new size of the collection is greater than the old size
    EXPECT_GT(collection->size(), old_size);
}

// Test to verify resizing decreases the collection
TEST_F(CollectionTest, ResizingDecreasesCollection)
{
    // Add 10 entries to the collection
    add_entries(10);

    // Store the current size of the collection
    size_t old_size = collection->size();

    // Resize the collection to a smaller size
    collection->resize(old_size - 5);

    // Verify that the size of the collection has decreased
    ASSERT_EQ(collection->size(), old_size - 5);

    // Add an EXPECT_* to verify that the new size of the collection is less than the old size
    EXPECT_LT(collection->size(), old_size);
}

// Test to verify resizing decreases the collection to zero
TEST_F(CollectionTest, ResizingDecreasesCollectionToZero)
{
    // Add 10 entries to the collection
    add_entries(10);

    // Resize the collection to zero
    collection->resize(0);

    // Verify that the size of the collection is now zero
    ASSERT_EQ(collection->size(), 0);
}

// Test to verify clear erases the collection
TEST_F(CollectionTest, ClearErasesCollection)
{
    // Add 10 entries to the collection
    add_entries(10);

    // Clear the collection
    collection->clear();

    // Verify that the size of the collection is now zero
    ASSERT_EQ(collection->size(), 0);
}

// Test to verify erase(begin,end) erases the collection
TEST_F(CollectionTest, EraseRangeErasesCollection)
{
    // Add 10 entries to the collection
    add_entries(10);

    // Verify that the size of the collection is now 10
    ASSERT_EQ(collection->size(), 10);

    // Erase the range from begin to end
    collection->erase(collection->begin(), collection->end());

    // Verify that the size of the collection is now zero
    ASSERT_EQ(collection->size(), 0);
}

// Test to verify reserve increases the capacity but not the size of the collection
TEST_F(CollectionTest, ReserveIncreasesCapacityNotSize)
{
    // Get the initial capacity
    size_t initial_capacity = collection->capacity();

    // Reserve more space
    collection->reserve(initial_capacity + 10);

    // Verify that the capacity of the collection has increased
    ASSERT_GT(collection->capacity(), initial_capacity);

    // Verify that the size of the collection has not changed
    ASSERT_EQ(collection->size(), 0);
}

// Test to verify the std::out_of_range exception is thrown when calling at() with an index out of bounds
TEST_F(CollectionTest, AtThrowsExceptionOnOutOfBounds)
{
    // Add 5 entries to the collection
    add_entries(5);

    // Verify that calling at() with an out of bounds index throws std::out_of_range
    ASSERT_THROW(collection->at(10), std::out_of_range);
}

TEST_F(CollectionTest, CollectionIsSortedAfterSort)
{
    // Add 10 random entries to the collection
    add_entries(10);

    // Sort the collection
    std::sort(collection->begin(), collection->end());

    // Verify that the collection is sorted in ascending order
    ASSERT_TRUE(std::is_sorted(collection->begin(), collection->end()));
}

// Test to verify the std::length_error exception is thrown when calling resize() with a negative number
TEST_F(CollectionTest, NegativeResizeThrowTest) 
{
    // Add 10 random entries to the collection
    add_entries(10);

    // Verify that calling resize with a negative number throws std::length_error
    ASSERT_THROW(collection->resize(-1), std::length_error);
}