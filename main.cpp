#include <iostream>
#include <string>
#include <vector>

class Bucket
{
public:
    Bucket() = default;
    Bucket(const std::string &key, const std::string &value)
    {
        m_key = key;
        m_value = value;
    }

private:
    std::string m_key;
    std::string m_value;
    friend class HashTable;
};

class HashTable
{
public:
    HashTable(uint32_t size = 10)
    {
        m_size = size;
        m_table.resize(size);
    }

    template <typename K, typename V>
    void insert(K &&key, V &&value)
    {
        // handle same key being added
        // Check if h1 > size (out of bounds)
        if (static_cast<float>(m_elements + 1) / m_size >= 0.75)
        {
            std::cout << "resizing..." << std::endl;
            resize(m_size * 2);
        }

        Bucket bucket(key, value);
        size_t index = hash(key) % m_size;

        if (isOccupied(index))
        {
            std::cout << "collision at index " << index << std::endl;
            index = probe(index);
        }

        m_table[index] = bucket;
        m_elements++;
    }

    void print() const
    {
        int count = 0;
        for (const Bucket &b : m_table)
        {
            count += 1;
            if (b.m_key.empty() && b.m_value.empty())
            {
                std::cout << count << " EMPTY" << std::endl;
            }
            else
            {
                std::cout << count << " KEY: " << b.m_key << " VALUE: " << b.m_value << std::endl;
            }
        }
    }

private:
    uint32_t m_size;
    uint32_t m_elements;
    std::vector<Bucket> m_table;
    size_t hash(const std::string &key)
    {
        return std::hash<std::string>{}(key);
    }

    size_t probe(size_t index)
    {
        size_t newIndex = index;
        size_t attempts = 0;

        while (isOccupied(newIndex))
        {
            newIndex += 1;
            if (newIndex >= m_size)
            {
                newIndex = 0;
            }

            attempts++;
            if (attempts >= m_size)
            {
                throw std::runtime_error("Hash table is full");
            }
        }
        return newIndex;
    }

    void resize(size_t newSize)
    {
        std::vector<Bucket> oldTable = std::move(m_table);
        m_size = newSize;
        m_elements = 0;
        m_table.resize(newSize);

        for (const auto &bucket : oldTable)
        {
            if (!bucket.m_key.empty())
            {
                insert(std::string(bucket.m_key),
                       std::string(bucket.m_value));
            }
        }
    }

    bool isOccupied(size_t index) const
    {
        return !m_table[index].m_key.empty();
    }
};

int main()
{
    HashTable ht = HashTable();
    ht.insert("1", "1");
    ht.insert("2", "2");
    ht.insert("3", "3");
    ht.insert("4", "4");
    ht.insert("5", "5");
    ht.insert("6", "6");
    ht.insert("7", "7");
    ht.insert("8", "8");
    ht.insert("9", "9");
    ht.print();
    return 0;
}