# Server API Documentation

## Overview

The server-side components provide multi-threaded request handling, task management, network services, and Redis database integration for the chatroom application.

## ThreadPool Class

### Description
A template-based thread pool implementation that manages worker threads for concurrent task processing.

### Header File
```cpp
#include "Server/ThreadPool.hpp"
```

### Class Definition
```cpp
template <typename T>
class ThreadPool {
public:
    ThreadPool(int min, int max);
    ~ThreadPool();
    
    // Task management
    void addTask(Task<T> task);
    
    // Statistics
    int getBusyNumber();
    int getAliveNumber();
    
private:
    static void *worker(void *arg);
    static void *manager(void *arg);
    void threadExit();
    
    // Thread synchronization
    pthread_mutex_t m_lock;
    pthread_cond_t m_notEmpty;
    
    // Thread management
    pthread_t *m_threadIDs;
    pthread_t m_managerID;
    TaskQueue<T> *m_taskQ;
    
    // Thread counts
    int m_minNum;    // Minimum threads
    int m_maxNum;    // Maximum threads
    int m_busyNum;   // Busy threads
    int m_aliveNum;  // Alive threads
    int m_exitNum;   // Threads to exit
    bool m_shutdown; // Shutdown flag
};
```

### Constructor

#### `ThreadPool(int min, int max)`
Creates a thread pool with specified minimum and maximum thread counts.
- **Parameters**:
  - `min`: Minimum number of threads to maintain
  - `max`: Maximum number of threads allowed
- **Features**:
  - Automatically creates minimum number of worker threads
  - Creates one manager thread for dynamic scaling
  - Initializes synchronization primitives
- **Example**:
  ```cpp
  ThreadPool<int> pool(4, 16);  // 4-16 threads
  ```

### Public Methods

#### `void addTask(Task<T> task)`
Adds a task to the thread pool queue for execution.
- **Parameters**:
  - `task`: Task object containing function pointer and arguments
- **Thread-Safe**: Yes
- **Blocking**: No (adds to queue)
- **Example**:
  ```cpp
  Task<ClientData> clientTask(handleClient, clientData);
  pool.addTask(clientTask);
  ```

#### `int getBusyNumber()`
Returns the current number of busy (working) threads.
- **Returns**: Number of threads currently executing tasks
- **Thread-Safe**: Yes
- **Usage**: For monitoring and load balancing

#### `int getAliveNumber()`
Returns the current number of alive threads in the pool.
- **Returns**: Total number of active threads
- **Thread-Safe**: Yes
- **Usage**: For pool status monitoring

### Thread Management

#### Worker Threads
- Execute tasks from the task queue
- Block when no tasks available
- Automatically created/destroyed based on load

#### Manager Thread
- Monitors thread pool performance
- Creates new threads when needed
- Destroys idle threads to save resources
- Maintains thread count between min and max limits

---

## TaskQueue Class

### Description
Thread-safe task queue for storing and retrieving tasks in the thread pool.

### Header File
```cpp
#include "Server/TaskQueue.hpp"
```

### Task Structure
```cpp
template <typename T>
struct Task {
    callback function;  // Function pointer
    T *arg;            // Function arguments
    
    Task();
    Task(callback f, void *arg);
};
```

### Class Definition
```cpp
template <typename T>
class TaskQueue {
public:
    TaskQueue();
    ~TaskQueue();
    
    // Task operations
    void addTask(callback func, void *arg);
    void addTask(Task<T> &task);
    Task<T> takeTask();
    
    // Queue status
    inline int taskNumber();
    
private:
    pthread_mutex_t m_mutex;      // Mutex for thread safety
    std::queue<Task<T>> m_queue;  // Task storage queue
};
```

### Public Methods

#### `void addTask(callback func, void *arg)`
Adds a task using function pointer and arguments.
- **Parameters**:
  - `func`: Function pointer to execute
  - `arg`: Arguments for the function
- **Thread-Safe**: Yes
- **Example**:
  ```cpp
  taskQueue.addTask(processRequest, requestData);
  ```

#### `void addTask(Task<T> &task)`
Adds a pre-constructed task object.
- **Parameters**:
  - `task`: Reference to task object
- **Thread-Safe**: Yes

#### `Task<T> takeTask()`
Retrieves and removes a task from the queue.
- **Returns**: Task object to execute
- **Thread-Safe**: Yes
- **Blocking**: Blocks if queue is empty (used with condition variables)

#### `int taskNumber()`
Returns the current number of tasks in the queue.
- **Returns**: Queue size
- **Thread-Safe**: Yes (inline with mutex protection)

---

## TCPServer Class

### Description
TCP server implementation for accepting client connections.

### Header File
```cpp
#include "Server/TCPServer.hpp"
```

### Class Definition
```cpp
class TcpServer {
public:
    TcpServer();
    ~TcpServer();
    
    int getfd() const;
    int setListen(unsigned short port);
    TcpSocket *acceptConn(struct sockaddr_in *addr = nullptr);
    
private:
    int m_fd;  // Listening socket file descriptor
};
```

### Public Methods

#### `int getfd() const`
Returns the listening socket file descriptor.
- **Returns**: Socket file descriptor

#### `int setListen(unsigned short port)`
Sets up the server to listen on specified port.
- **Parameters**:
  - `port`: Port number to listen on
- **Returns**: 0 on success, -1 on error
- **Features**:
  - Binds to all available interfaces
  - Sets socket to reuse address
  - Starts listening for connections
- **Example**:
  ```cpp
  TcpServer server;
  if (server.setListen(8080) == 0) {
      cout << "Server listening on port 8080" << endl;
  }
  ```

#### `TcpSocket *acceptConn(struct sockaddr_in *addr = nullptr)`
Accepts a new client connection.
- **Parameters**:
  - `addr`: Optional pointer to store client address information
- **Returns**: New TcpSocket pointer for client communication, nullptr on error
- **Blocking**: Yes (blocks until client connects)
- **Example**:
  ```cpp
  struct sockaddr_in clientAddr;
  TcpSocket *client = server.acceptConn(&clientAddr);
  if (client) {
      // Handle client connection
  }
  ```

---

## Redis Class

### Description
Redis database integration providing key-value storage, hash tables, sets, and lists operations.

### Header File
```cpp
#include "Server/redis.hpp"
```

### Class Definition
```cpp
class Redis {
public:
    Redis() = default;
    Redis(string addr, int port);
    ~Redis();
    
    // Connection management
    bool connect();
    bool connect(struct timeval timeout);
    bool disConnect();
    
    // Key-Value operations
    bool setValue(const string &key, const string &value);
    string getValue(const string &key);
    bool delKey(const string &key);
    
    // Hash operations
    bool hsetValue(const string &key, const string &field, const string &value);
    bool hashexists(const string &key, const string &field);
    string gethash(const string &key, const string &field);
    bool delhash(const string &key, const string &field);
    int hlen(const string &key);
    redisReply **hkeys(const string &key);
    
    // Set operations
    int scard(const string &key);
    int saddvalue(const string &key, const string &value);
    int sismember(const string &key, const string &value);
    int sremvalue(const string &key, const string &value);
    redisReply **smembers(const string &key);
    
    // List operations
    int lpush(const string &key, const string &value);
    int llen(const string &key);
    redisReply **lrange(const string &key);
    redisReply **lrange(const string &key, string a, string b);
    int ltrim(const string &key);
    
private:
    string redis_addr = "127.0.0.1";
    int redis_port = 6379;
    redisContext *redis_client = nullptr;
};
```

### Constructor

#### `Redis(string addr, int port)`
Creates Redis connection with specified server details.
- **Parameters**:
  - `addr`: Redis server IP address
  - `port`: Redis server port number
- **Default**: localhost:6379

### Connection Methods

#### `bool connect()`
Establishes blocking connection to Redis server.
- **Returns**: true on success, false on failure
- **Example**:
  ```cpp
  Redis db("127.0.0.1", 6379);
  if (db.connect()) {
      cout << "Connected to Redis" << endl;
  }
  ```

#### `bool connect(struct timeval timeout)`
Establishes connection with timeout.
- **Parameters**:
  - `timeout`: Connection timeout specification
- **Returns**: true on success, false on failure

#### `bool disConnect()`
Closes Redis connection.
- **Returns**: true on success

### Key-Value Operations

#### `bool setValue(const string &key, const string &value)`
Sets a key-value pair.
- **Parameters**:
  - `key`: Key name
  - `value`: Value to store
- **Returns**: true on success
- **Example**:
  ```cpp
  db.setValue("user:1001:name", "Alice");
  ```

#### `string getValue(const string &key)`
Retrieves value for a key.
- **Parameters**:
  - `key`: Key name
- **Returns**: Value string, empty if key doesn't exist
- **Example**:
  ```cpp
  string name = db.getValue("user:1001:name");
  ```

#### `bool delKey(const string &key)`
Deletes a key and its value.
- **Parameters**:
  - `key`: Key to delete
- **Returns**: true if key was deleted

### Hash Operations

#### `bool hsetValue(const string &key, const string &field, const string &value)`
Sets a field in a hash table.
- **Parameters**:
  - `key`: Hash table name
  - `field`: Field name
  - `value`: Field value
- **Usage**: For storing structured user data
- **Example**:
  ```cpp
  db.hsetValue("user:1001", "name", "Alice");
  db.hsetValue("user:1001", "email", "alice@example.com");
  ```

#### `string gethash(const string &key, const string &field)`
Gets a field value from hash table.
- **Parameters**:
  - `key`: Hash table name
  - `field`: Field name
- **Returns**: Field value

#### `bool hashexists(const string &key, const string &field)`
Checks if field exists in hash table.
- **Returns**: true if field exists

#### `int hlen(const string &key)`
Returns number of fields in hash table.
- **Returns**: Field count

### Set Operations

#### `int saddvalue(const string &key, const string &value)`
Adds value to a set.
- **Parameters**:
  - `key`: Set name
  - `value`: Value to add
- **Returns**: 1 if added, 0 if already exists
- **Usage**: For friend lists, group members
- **Example**:
  ```cpp
  db.saddvalue("user:1001:friends", "1002");
  ```

#### `int sismember(const string &key, const string &value)`
Checks if value exists in set.
- **Returns**: 1 if exists, 0 if not

#### `int scard(const string &key)`
Returns set size.
- **Returns**: Number of elements in set

### List Operations

#### `int lpush(const string &key, const string &value)`
Pushes value to beginning of list.
- **Parameters**:
  - `key`: List name
  - `value`: Value to add
- **Returns**: New list length
- **Usage**: For message history
- **Example**:
  ```cpp
  db.lpush("messages:1001:1002", messageJson);
  ```

#### `int llen(const string &key)`
Returns list length.
- **Returns**: Number of elements

#### `redisReply **lrange(const string &key)`
Gets all elements from list.
- **Returns**: Array of Redis replies
- **Memory**: Caller must free returned array

## Data Schema Examples

### User Storage
```cpp
// User information
db.hsetValue("user:1001", "name", "Alice");
db.hsetValue("user:1001", "password", "hashed_password");
db.hsetValue("user:1001", "status", "online");

// Friend relationships
db.saddvalue("user:1001:friends", "1002");
db.saddvalue("user:1001:friends", "1003");
```

### Message Storage
```cpp
// Private messages
db.lpush("messages:1001:1002", messageJson);

// Group messages  
db.lpush("group:5001:messages", messageJson);
```

### Group Management
```cpp
// Group information
db.hsetValue("group:5001", "name", "Dev Team");
db.hsetValue("group:5001", "owner", "1001");

// Group members
db.saddvalue("group:5001:members", "1001");
db.saddvalue("group:5001:members", "1002");
```

## Error Handling

### Thread Pool Errors
- Check return values from pthread functions
- Handle thread creation failures gracefully
- Implement proper cleanup in destructors

### Redis Errors
- Always check return values from Redis operations
- Handle connection failures and reconnection
- Free Redis reply objects to prevent memory leaks
- Validate Redis responses before processing

### Best Practices
1. Use RAII for resource management
2. Implement proper error logging
3. Handle network disconnections gracefully
4. Use connection pooling for Redis operations
5. Monitor thread pool performance metrics
6. Implement graceful shutdown procedures