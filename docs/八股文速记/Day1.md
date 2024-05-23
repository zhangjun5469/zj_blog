# Reids 的五种数据结构+使用场景
Redis 是一个开源的内存数据结构存储系统，广泛用于缓存、消息队列、实时分析等领域。Redis 提供了五种主要的数据结构，每种数据结构适用于不同的使用场景。下面将介绍这五种数据结构及其典型的使用场景。

### 1. 字符串（String）

**描述**：
- 最简单、最基本的数据结构。
- 可以存储任何类型的数据，如**文本、数字、二进制数据**等。
- 最大长度为 512 MB。

**使用场景**：
- **缓存**：存储频繁访问的数据，如数据库查询结果。
- **计数器**：实现计数功能，如网站访问量、视频播放量。
- **会话数据**：存储用户会话信息，如登录状态、购物车内容。

**示例**：
```shell
SET key "value"
GET key
INCR counter
```

### 2. 哈希（Hash）

**描述**：
- 存储键值对集合，类似于 Python 的字典或 Java 的 HashMap。
- 适合存储对象信息，如用户信息、商品信息。
- 每个哈希最多可以包含 2^32 - 1 个键值对。

**使用场景**：
- **用户信息存储**：存储用户属性信息，如用户 ID、用户名、邮箱等。
- **配置存储**：存储配置信息，如应用设置、系统参数。

**示例**：
```shell
HSET user:1001 name "Alice"
HGET user:1001 name
HMSET user:1001 age 30 country "USA"
HGETALL user:1001
```

### 3. 列表（List）

**描述**：
- 有序的字符串列表，可以从两端进行操作（插入和弹出）。
- 适合存储有序数据，如消息队列、任务列表。
- 列表的长度仅受限于内存大小。

**使用场景**：
- **消息队列**：实现简单的消息队列系统。
- **任务调度**：存储任务列表，按顺序处理任务。
- **文章评论**：存储按时间顺序的评论列表。

**示例**：
```shell
LPUSH queue "task1"
RPUSH queue "task2"
LPOP queue
RPOP queue
```

### 4. 集合（Set）

**描述**：
- 无序的唯一字符串集合。
- 适合存储不重复的数据集合。
- 支持集合运算，如交集、并集、差集等。

**使用场景**：
- **标签系统**：存储文章的标签集合。
- **社交网络**：存储用户的好友列表、关注列表。
- **抽奖活动**：存储参与活动的用户 ID，确保唯一性。

**示例**：
```shell
SADD tags "Redis" "Database" "NoSQL"
SMEMBERS tags
SISMEMBER tags "Redis"
SREM tags "NoSQL"
```

### 5. 有序集合（Sorted Set）

**描述**：
- 有序的唯一字符串集合，每个元素关联一个分数，用于排序。
- 适合存储需要排序的数据，如排行榜、优先队列。
- 支持按分数范围、成员范围进行操作。

**使用场景**：
- **排行榜**：存储用户积分、游戏排名。
- **优先队列**：实现带优先级的任务调度。
- **带权重的数据存储**：存储需要排序的数据，如商品评分、电影排名。

**示例**：
```shell
ZADD leaderboard 100 "Alice"
ZADD leaderboard 200 "Bob"
ZRANGE leaderboard 0 -1 WITHSCORES
ZRANK leaderboard "Alice"
ZREM leaderboard "Alice"
```

### 其他高级数据结构

除了以上五种基础数据结构，Redis 还支持其他一些高级数据结构和功能，如：

- **位图（Bitmaps）**：用于操作位集合，适合实现布隆过滤器等功能。
- **HyperLogLog**：用于基数估计算法，适合计算大规模数据集合的基数。
- **地理空间索引（Geospatial Indexes）**：用于存储地理位置数据，支持基于半径的地理查询。

通过合理选择和使用 Redis 提供的数据结构，可以极大地提高应用程序的性能和灵活性，满足各种不同的业务需求。

# Redis 内存淘汰策略

Redis 内存淘汰策略（Redis Eviction Policy）是当 Redis 内存达到最大限制时，用来决定如何处理新写入数据的一系列规则。Redis 提供了多种内存淘汰策略，以适应不同的应用场景和需求。以下是 Redis 的主要内存淘汰策略及其详细描述：

1. **noeviction**（默认策略）
   - **描述**：当内存使用达到限制时，不再接受新的写入操作，会返回错误。
   - **适用场景**：适用于不希望任何数据被丢弃的场景，如缓存失效可能导致严重后果的系统。

2. **allkeys-lru**（所有键 - 最近最少使用）
   - **描述**：在所有键中，淘汰最近最少使用的键。
   - **适用场景**：适用于需要缓存经常使用的数据，但允许丢弃不常用数据的场景。

3. **volatile-lru**（有过期时间的键 - 最近最少使用）
   - **描述**：只在设置了过期时间的键中，淘汰最近最少使用的键。
   - **适用场景**：适用于缓存系统，尤其是那些部分数据有过期时间但希望尽量保留未设置过期时间的数据的场景。

4. **allkeys-random**（所有键 - 随机淘汰）
   - **描述**：在所有键中，随机淘汰某个键。
   - **适用场景**：适用于简单的缓存需求，没有明显的访问模式或访问频率的场景。

5. **volatile-random**（有过期时间的键 - 随机淘汰）
   - **描述**：只在设置了过期时间的键中，随机淘汰某个键。
   - **适用场景**：适用于缓存系统，部分数据有过期时间且不关心淘汰哪个具体数据的场景。

6. **volatile-ttl**（有过期时间的键 - 优先淘汰存活时间最短的键）
   - **描述**：只在设置了过期时间的键中，优先淘汰剩余存活时间（TTL）最短的键。
   - **适用场景**：适用于缓存系统，希望尽量保留有较长剩余时间的数据的场景。

### 配置方法

要配置 Redis 的内存淘汰策略，可以在 `redis.conf` 文件中设置 `maxmemory-policy` 参数。例如：

```shell
maxmemory-policy allkeys-lru
```

或者通过命令行配置：

```shell
CONFIG SET maxmemory-policy allkeys-lru
```

### 内存限制设置

除了配置淘汰策略，还需要设置 Redis 的最大内存限制，这可以在 `redis.conf` 文件中通过 `maxmemory` 参数进行配置，例如：

```shell
maxmemory 256mb
```

或者通过命令行配置：

```shell
CONFIG SET maxmemory 256mb
```

### 总结

选择合适的内存淘汰策略需要根据具体的应用需求和使用场景来决定。理解每种策略的工作原理和适用场景，可以帮助优化 Redis 的性能和可靠性。

# Redis如何处理带有过期时间的key的删除的呢？
Redis 处理带有过期时间的键（keys）的删除有两种主要方式：**惰性删除**（Lazy Deletion）和**定期删除**（Periodic Deletion）。

### 1. 惰性删除（Lazy Deletion）

**描述**：当客户端访问一个键时，Redis 会检查该键是否已过期。如果过期，Redis 会立即删除该键并返回一个空值或错误。这种方法仅在键被访问时才会检查其过期状态。

**优点**：惰性删除的性能开销较小，因为只在访问时才检查过期状态。

**缺点**：如果一个过期的键从未被访问，它将一直占用内存，直到某个时候被主动访问。

**示例**：

```shell
127.0.0.1:6379> SET mykey "hello" EX 10
OK
127.0.0.1:6379> GET mykey
"hello"
# 等待10秒后
127.0.0.1:6379> GET mykey
(nil)  # 键已过期并被删除
```

### 2. 定期删除（Periodic Deletion）

**描述**：Redis 会周期性地随机检查一部分设置了过期时间的键，并删除其中已过期的键。这是通过后台任务（通常每秒运行10次）来完成的，每次任务会随机挑选一部分键来检查。

**优点**：定期删除能减少内存中滞留的过期键的数量，从而降低内存占用。

**缺点**：这增加了一些额外的CPU开销，尤其是在有大量过期键的情况下。

### 内部机制

Redis 的定期删除策略涉及以下几个步骤：

1. **选择数据库**：Redis 依次检查各个数据库。
2. **挑选键**：从每个数据库中随机挑选一定数量的设置了过期时间的键。
3. **检查和删除**：检查这些键的过期时间，删除已经过期的键。

具体的操作频率和检查的键数量可以通过配置参数进行调整：

- **`hz`**：定义 Redis 每秒运行后台任务的频率（默认值为 10）。
- **`active-expire-effort`**：控制检查过期键的力度（取值范围 1 到 10，默认值为 1）。

### 内存淘汰和过期键

如果 Redis 配置了最大内存限制（`maxmemory`），并且内存达到限制时，过期键的删除还会影响到内存淘汰策略的执行：

- **优先删除过期键**：在执行内存淘汰策略之前，Redis 会优先删除过期的键。
- **与内存淘汰策略结合**：如果删除过期键后内存仍然不足，Redis 会根据配置的内存淘汰策略进一步淘汰数据。

### 过期策略配置示例

在 `redis.conf` 文件中，可以调整 Redis 的过期处理行为和内存管理策略：

```shell
# 设置最大内存
maxmemory 256mb

# 设置内存淘汰策略
maxmemory-policy allkeys-lru

# 设置后台任务的频率
hz 10

# 设置检查过期键的力度
active-expire-effort 1
```

总结起来，Redis 通过惰性删除和定期删除相结合的方式管理带有过期时间的键，确保内存资源的有效使用并减少过期键对系统性能的影响。

# 如何看待和处理 MySQL 和 Redis 数据一致性问题
MySQL 和 Redis 常常一起使用以发挥各自的优势：MySQL 提供可靠的持久化存储，Redis 提供快速的缓存和数据操作。然而，由于两者之间的特性差异，如何确保数据的一致性是一个关键问题。以下是一些常见的策略和处理方法：

### 数据一致性挑战

1. **异步更新**：数据更新先写入 MySQL，再异步更新 Redis 缓存，可能导致短时间内数据不一致。
2. **缓存过期**：缓存数据过期后重新加载，可能加载到旧数据。
3. **缓存失效**：手动或自动清除缓存时可能会出现数据一致性问题。

### 常见策略

#### 1. Cache Aside Pattern（旁路缓存）

这种模式是最常用的缓存模式。主要步骤如下：

- **读操作**：
  1. 先从 Redis 缓存读取数据。
  2. 如果缓存中没有数据（缓存未命中），则从 MySQL 读取数据。
  3. 将从 MySQL 获取的数据写入 Redis 缓存，以备下次读取。

- **写操作**：
  1. 更新 MySQL 数据库。
  2. 使 Redis 中相应的缓存失效或更新缓存。

**示例代码**：

```python
def get_data(key):
    data = redis.get(key)
    if data is None:
        data = mysql_query("SELECT value FROM table WHERE key = %s", (key,))
        redis.set(key, data)
    return data

def set_data(key, value):
    mysql_query("UPDATE table SET value = %s WHERE key = %s", (value, key))
    redis.delete(key)  # 使缓存失效
```

#### 2. Write Through Pattern（直写缓存）

在这种模式下，所有数据写操作先写入缓存，然后由缓存同步到数据库。

- **写操作**：
  1. 写入 Redis 缓存。
  2. Redis 同步写入 MySQL。

这种方法确保写入操作的一致性，但增加了写操作的复杂度和延迟。

#### 3. Write Behind Pattern（延迟写入）

在这种模式下，写操作首先更新缓存，然后异步地将数据写入数据库。由于写入数据库是异步的，这种方法可以提高写入性能，但可能导致数据一致性问题。

#### 4. 双写一致性（Redis + MySQL）

在写操作中，同时更新 Redis 和 MySQL：

1. 开始数据库事务。
2. 更新 MySQL。
3. 更新 Redis。
4. 提交事务。

如果任何步骤失败，则回滚事务，以确保数据一致性。

**示例代码**：

```python
def set_data(key, value):
    try:
        mysql.begin_transaction()
        mysql_query("UPDATE table SET value = %s WHERE key = %s", (value, key))
        redis.set(key, value)
        mysql.commit_transaction()
    except Exception as e:
        mysql.rollback_transaction()
        raise e
```

### 实践中的策略选择

1. **业务需求**：选择合适的策略应基于业务需求。例如，Cache Aside 适用于读操作频繁且对数据实时一致性要求不高的场景。
2. **数据更新频率**：如果数据更新频繁，使用 Write Through 或 Write Behind 可能更合适。
3. **一致性要求**：在对一致性要求高的场景下，双写一致性是更好的选择，但需要处理好事务和回滚。

### 避免缓存穿透、雪崩和击穿

1. **缓存穿透**：使用布隆过滤器或在缓存中存储空值，防止大量不存在的查询直接打到数据库。
2. **缓存雪崩**：缓存失效时，采用随机过期时间避免大量缓存同时失效。
3. **缓存击穿**：对热点数据设置互斥锁，防止大量请求同时查询数据库并重建缓存。

### 总结

处理 MySQL 和 Redis 之间的数据一致性问题没有万能的解决方案。选择合适的策略需要根据具体的业务需求和使用场景权衡性能和一致性。通过设计合理的缓存策略和事务机制，可以有效减少数据不一致的风险。

# 双写一致性里考虑redis的事务回滚
Redis 提供了事务机制，通过 `MULTI`、`EXEC`、`DISCARD` 和 `WATCH` 等命令来实现。这些命令可以帮助我们确保在处理双写一致性时，Redis 和 MySQL 数据的一致性。

### 使用 Redis 事务实现双写一致性

为了在 MySQL 和 Redis 之间实现双写一致性，可以使用 Redis 的 `WATCH` 命令来监控键的变化，并结合事务来确保一致性。以下是实现双写一致性的示例代码，包括 Redis 事务回滚的处理：

#### 示例代码（Python）

```python
import redis
import MySQLdb

# 连接 Redis
redis_client = redis.StrictRedis(host='localhost', port=6379, db=0)

# 连接 MySQL
mysql_conn = MySQLdb.connect(user='root', passwd='password', db='mydb', host='localhost')
mysql_cursor = mysql_conn.cursor()

def set_data(key, value):
    try:
        # 开始 MySQL 事务
        mysql_conn.begin()

        # 更新 MySQL
        mysql_cursor.execute("UPDATE mytable SET value=%s WHERE key=%s", (value, key))
        
        # 监控 Redis 键
        redis_client.watch(key)
        
        # 开始 Redis 事务
        pipeline = redis_client.pipeline()
        pipeline.multi()
        pipeline.set(key, value)

        # 执行 Redis 事务
        pipeline.execute()

        # 提交 MySQL 事务
        mysql_conn.commit()
    except Exception as e:
        # 回滚 MySQL 事务
        mysql_conn.rollback()
        
        # 放弃 Redis 事务
        redis_client.unwatch()
        
        raise e

def get_data(key):
    try:
        data = redis_client.get(key)
        if data is None:
            mysql_cursor.execute("SELECT value FROM mytable WHERE key=%s", (key,))
            data = mysql_cursor.fetchone()
            if data:
                redis_client.set(key, data[0])
        return data
    except Exception as e:
        raise e
```

### 详细步骤

1. **开始 MySQL 事务**：调用 `mysql_conn.begin()` 开始 MySQL 事务。
2. **更新 MySQL**：执行 SQL 语句更新 MySQL 数据库中的数据。
3. **监控 Redis 键**：使用 `redis_client.watch(key)` 监控需要更新的 Redis 键，确保在事务执行期间如果该键被其他客户端修改，事务将被中止。
4. **开始 Redis 事务**：使用 `pipeline.multi()` 开始 Redis 事务。
5. **更新 Redis**：在 Redis 事务中执行 `pipeline.set(key, value)` 命令更新键值。
6. **执行 Redis 事务**：调用 `pipeline.execute()` 执行 Redis 事务。如果在监控的键期间被其他客户端修改，事务将失败。
7. **提交 MySQL 事务**：如果 Redis 事务执行成功，调用 `mysql_conn.commit()` 提交 MySQL 事务。
8. **异常处理**：如果在上述步骤中任何一步发生异常，回滚 MySQL 事务，并使用 `redis_client.unwatch()` 放弃 Redis 事务。

### 注意事项

1. **事务隔离**：确保 MySQL 和 Redis 的事务隔离级别适当，以防止数据冲突。
2. **异常处理**：详细处理各种异常情况，确保在发生错误时系统能够正确回滚并保持一致性。
3. **性能考虑**：由于双写一致性涉及到跨系统的事务处理，可能会带来性能开销，需要根据业务场景权衡性能和一致性需求。
4. **幂等性**：保证数据操作的幂等性，以应对可能的重试操作。

通过上述方式，可以更好地处理 MySQL 和 Redis 之间的数据一致性问题，并在发生异常时正确回滚 Redis 事务，确保数据的一致性。