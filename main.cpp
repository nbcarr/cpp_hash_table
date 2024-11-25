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
        m_max_load_factor = .75;
        m_table.resize(size);
    }

    template <typename K, typename V>
    void insert(K &&key, V &&value)
    {
        if (needsResize())
        {
            std::cout << "resizing..." << std::endl;
            resize(m_size * 2);
        }

        Bucket bucket(key, value);
        size_t index = hash(key) % m_size;

        // Overwrite existing keys
        if (auto existing = contains(key)) {
            m_table[*existing] = Bucket(key, value);
            return;
        }

        if (isOccupied(index))
        {
            std::cout << "collision at index " << index << std::endl;
            index = probe(index);
        }

        m_table[index] = bucket;
        m_elements++;
    }

    template <typename K>
    std::optional<std::string> get(K &&key)
    {
        std::optional<size_t> index = contains(key);
        if (!index)
        {
            return std::nullopt;
        }
        return m_table[*index].m_value;
    }

    template <typename K>
    void remove(K &&key)
    {
        std::optional<size_t> index = contains(key);
        if (!index)
        {
            std::cout << "Key does not exist" << std::endl;
            return;
        }
        size_t curr = *index;
        m_table[curr] = Bucket();
        while (true)
        {
            size_t next = (curr + 1) % m_size;
            if (!isOccupied(next))
            {
                break;
            }
            size_t ideal = hash(m_table[next].m_key) % m_size;
            // If next element can slide backwards (needs to wrap around for comparison)
            if ((ideal <= curr) || (ideal > next))
            {
                // Move it back
                m_table[curr] = std::move(m_table[next]);
                m_table[next] = Bucket();
                curr = next;
            }
            else
            {
                // Element belongs where it is
                break;
            }
        }
        m_elements--;
    }

    void clear()
    {
        m_elements = 0;
        m_table = std::vector<Bucket>(m_size);
    }

    uint32_t size() const
    {
        return m_size;
    }

    bool empty() const
    {
        return m_elements == 0;
    }

    float load_factor() const
    {
        return static_cast<float>(m_elements) / m_size;
    }

    uint32_t capacity() const
    {
        return m_size;
    }

    float max_load_factor()
    {
        return m_max_load_factor;
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
    float m_max_load_factor;
    std::vector<Bucket> m_table;
    size_t hash(const std::string &key)
    {
        return std::hash<std::string>{}(key);
    }

    bool needsResize()
    {
        return load_factor() >= max_load_factor();
    }

    std::optional<size_t> contains(const std::string &key)
    {
        size_t index = hash(key) % m_size;
        size_t newIndex = index;
        size_t attempts = 0;

        while (attempts < m_size)
        {
            if (!isOccupied(newIndex))
            {
                return std::nullopt;
            }

            if (m_table[newIndex].m_key == key)
            {
                return newIndex;
            }

            attempts++;
            newIndex = (newIndex + 1) % m_size;
        }

        return std::nullopt;
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
    HashTable ht(3);

    ht.insert("cat", "meow");
    ht.insert("dog", "woof");
    if (auto val = ht.get("cat"))
    {
        std::cout << "cat says: " << *val << std::endl;
    }

    ht.insert("act", "theater");
    if (auto val = ht.get("act"))
    {
        std::cout << "act: " << *val << std::endl;
    }

    ht.insert("bird", "chirp");
    ht.insert("fish", "blub");

    std::vector<std::string> keys = {"cat", "dog", "act", "bird", "fish", "bear"};
    for (const auto &key : keys)
    {
        if (auto val = ht.get(key))
        {
            std::cout << key << " -> " << *val << std::endl;
        }
        else
        {
            std::cout << "ERROR: Couldn't find " << key << std::endl;
        }
    }

    std::vector<std::string> removeKeys = {"fish", "bear"};
    for (const auto &key : removeKeys)
    {
        ht.remove(key);
        if (auto val = ht.get(key))
        {
            std::cout << key << " -> " << *val << std::endl;
        }
        else
        {
            std::cout << "ERROR: Couldn't find " << key << std::endl;
        }
    }

    ht.print();
}