#include <catch_amalgamated.hpp>
#include <yart.h>

using namespace yart;
using namespace Catch;

template<int size>
struct Foo
{
	Foo()
	{
		for (int i = 0; i < size; i++)
		{
			data[i] = i * i;
		}
	}

	bool ConstructorRan() const 
	{
		for (int i = 0; i < size; i++)
			if (data[i] != i * i)
				return false;

		return true;
	}

	int data[size];
};

TEST_CASE("MemoryArena", "[memory]")
{
	MemoryArena arena(512);
	
	auto TestReadWrite = [](int* data, size_t size) 
	{ 
		for (size_t i = 1; i < size; i++)
		{
			data[i] = 1 + data[i - 1];
		}
	};

	int* ptr = arena.Alloc<int>(8, false);
	TestReadWrite(ptr, 8);

	// Check that we can service allocations larger than the blocksize
	ptr = arena.Alloc<int>(1024, false);
	TestReadWrite(ptr, 1024);

	// Make sure default constructor is run
	Foo<10>* foo = arena.Alloc<Foo<10>>();
	CHECK(foo->ConstructorRan());

	arena.Reset();

	Foo<1800>* bar = arena.Alloc<Foo<1800>>();
	CHECK(bar->ConstructorRan());
}

TEST_CASE("BlockedArray", "[memory]")
{
	SECTION("Block size divides array size")
	{
		std::vector<int> arr(512 * 512);
		for (int i = 0; i < 512 * 512; i++)
		{
			arr[i] = i;
		}
		BlockedArray<int, 3> barr(512, 512, arr.data());

		bool isEqual = true;
		for (int x = 0; x < 512; x++)
			for (int y = 0; y < 512; y++)
				if (barr(x, y) != y * 512 + x)
				{
					isEqual = false;
					break;
				}

		CHECK(isEqual);

		CHECK(barr.RoundUp(0) == 0);
		CHECK(barr.RoundUp(9) == 16);
		CHECK(barr.RoundUp(32) == 32);
	}

	SECTION("Block size does not divide array size")
	{
		std::vector<int> arr(200 * 100);
		for (int i = 0; i < 200 * 100; i++)
		{
			arr[i] = i;
		}
		BlockedArray<int, 3> barr(200, 100, arr.data());

		bool isEqual = true;
		for (int x = 0; x < 200; x++)
			for (int y = 0; y < 100; y++)
				if (barr(x, y) != y * 200 + x)
				{
					isEqual = false;
					break;
				}

		CHECK(isEqual);
	}
}