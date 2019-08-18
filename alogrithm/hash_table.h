#pragma once
#include <vector>
#include <list>
#include <iostream>
#include <algorithm>
#include <initializer_list>

/******** hash表大部分接口都未实现，这里我们使用的是链表法解决hash冲突 ******/
template<typename Key, typename T, typename hasher = std::hash<Key> >
class HashTable
{
public:
	using value_type = std::pair<Key, T> ;
	using Bucket = std::list<value_type>;
	using Vec = std::vector<Bucket>;
	using iterator_type = size_t;
	using bucket_size = size_t;

	/****/
	HashTable() :_bucket_count(8), _size(0), _load_factor(0.0)
	{
		_data.resize(_bucket_count);
	}

	HashTable(const HashTable &other)
	{
		_bucket_count = other._bucket_count;
		_size = other._size;
		_data = other._data;
	}

	HashTable(HashTable &&other)
	{
		_bucket_count = other._bucket_count;
		_size = other._size;
		_data = std::move(other._data);
	}

	HashTable &operator=(const HashTable &other)
	{
		if (&other != this)
		{
			_bucket_count = other._bucket_count;
			_size = other._size;
			_data = other._data;
		}
		return this;
	}

	HashTable &operator=(HashTable &&other)
	{
		if (&other != this)
		{
			_bucket_count = other._bucket_count;
			_size = other._size;
			_data = std::move(other._data);
		}
		return this;
	}

	/***/
	std::pair<iterator_type, bool> insert(const std::pair<Key, T> &kv)
	{
		/** 一定保证key是唯一的，如果已经存在那么插入失败 */
		size_t hashVal = _hash_handler(kv.first.c_str());
		size_t bucket_pos = hashVal % _bucket_count;
		bool inserted = false;
		if (check_key_uniqure(kv.first, bucket_pos))
		{
			_data[bucket_pos].push_back(kv);
			inserted = true;
			++_size;
		}
		return std::make_pair<typename HashTable<Key,T, hasher>::iterator_type, bool>(std::move(bucket_pos), std::move(inserted));
	}

	void insert(const std::initializer_list<value_type> &args)
	{
		auto ptr = args.begin();
		for (; ptr != args.end(); ++ptr)
		{
			insert(*ptr);
		}
	}

	size_t size()const { return _size; }

private:
	bool check_key_uniqure(const typename value_type::first_type &key, const bucket_size &pos)     //这里的检查key应该好需要再优化下
	{
		auto &bucket_data = _data[pos];
		auto par = std::find_if(bucket_data.begin(), bucket_data.end(), [&key](const value_type &kv) {
			if (kv.first == key)
				return true;
			else
				return false;
		});

		if (par != bucket_data.end())
			return false;
		else
			return true;
	}

private:
	Vec _data;
	size_t _bucket_count;
	size_t _size;
	float _load_factor;
	hasher _hash_handler;
};

//main.cpp
int main()
{
	/** Test 1 */
	HashTable<std::string, int> myhash;
	myhash.insert(std::make_pair<std::string, int>("test", 123));
	myhash.insert(std::make_pair<std::string, int>("hello", 456));
	myhash.insert(std::make_pair<std::string, int>("hello", 789));
	std::cout << "myhash size: " << myhash.size() << std::endl;
	getchar();
	return 1;
}
