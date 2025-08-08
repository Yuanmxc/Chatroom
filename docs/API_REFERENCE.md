# API Reference

## Overview

This document provides a comprehensive reference for all public APIs, functions, and data structures in the chatroom application. Functions are organized by module for easy navigation.

## Table of Contents

- [Core Library APIs](#core-library-apis)
  - [TcpSocket Class](#tcpsocket-class)
  - [Message Structure](#message-structure)
  - [Command Structure](#command-structure)
  - [Color Constants](#color-constants)
- [Server APIs](#server-apis)
  - [ThreadPool Class](#threadpool-class)
  - [TaskQueue Class](#taskqueue-class)
  - [TcpServer Class](#tcpserver-class)
  - [Redis Class](#redis-class)
- [Client APIs](#client-apis)
  - [Display Functions](#display-functions)
  - [Input Functions](#input-functions)
  - [Client Core Functions](#client-core-functions)
- [Command Constants](#command-constants)

---

## Core Library APIs

### TcpSocket Class

#### Constructors

##### `TcpSocket()`
```cpp
TcpSocket();
```
**Description**: Creates a new TCP socket for communication.  
**Parameters**: None  
**Returns**: TcpSocket instance  
**Exceptions**: May throw system exceptions if socket creation fails  
**Example**:
```cpp
TcpSocket client;
```

##### `TcpSocket(int socket)`
```cpp
TcpSocket(int socket);
```
**Description**: Wraps an existing socket file descriptor.  
**Parameters**:
- `socket` (int): Existing socket file descriptor  
**Returns**: TcpSocket instance  
**Example**:
```cpp
TcpSocket client(existing_fd);
```

##### `TcpSocket(string recv)`
```cpp
TcpSocket(string recv);
```
**Description**: Creates socket with notification capability.  
**Parameters**:
- `recv` (string): Must be "recv" to enable notification socket  
**Returns**: TcpSocket instance with dual sockets  
**Example**:
```cpp
TcpSocket client("recv");
```

#### Public Methods

##### `getfd()`
```cpp
int getfd() const;
```
**Description**: Returns the main communication socket file descriptor.  
**Parameters**: None  
**Returns**: (int) Socket file descriptor or -1 if invalid  
**Thread-Safe**: Yes  

##### `getrecvfd()`
```cpp
int getrecvfd() const;
```
**Description**: Returns the notification socket file descriptor.  
**Parameters**: None  
**Returns**: (int) Notification socket file descriptor or -1 if not available  
**Thread-Safe**: Yes  

##### `connectToHost()`
```cpp
int connectToHost(string ip, unsigned short port);
```
**Description**: Connects to a remote server.  
**Parameters**:
- `ip` (string): Server IP address (e.g., "127.0.0.1")
- `port` (unsigned short): Server port number  
**Returns**: (int) 0 on success, -1 on failure  
**Blocking**: Yes  
**Example**:
```cpp
if (client.connectToHost("127.0.0.1", 8080) == 0) {
    // Connected successfully
}
```

##### `sendMsg()`
```cpp
int sendMsg(string msg);
```
**Description**: Sends a message with automatic length prefixing.  
**Parameters**:
- `msg` (string): Message content to send  
**Returns**: (int) Number of bytes sent, -1 on error  
**Protocol**: Automatically adds 4-byte length header  
**Thread-Safe**: No (caller must synchronize)  
**Example**:
```cpp
int result = client.sendMsg("Hello, server!");
if (result > 0) {
    // Message sent successfully
}
```

##### `recvMsg()`
```cpp
string recvMsg();
```
**Description**: Receives a complete message.  
**Parameters**: None  
**Returns**: (string) Received message content  
**Blocking**: Yes (blocks until complete message received)  
**Thread-Safe**: No (caller must synchronize)  
**Example**:
```cpp
string response = client.recvMsg();
if (!response.empty()) {
    // Process received message
}
```

---

### Message Structure

#### Fields

##### `SendUid`
```cpp
string SendUid;
```
**Description**: Unique identifier of the message sender.  
**Type**: string  
**Constraints**: Should be valid user ID  

##### `RecvUid`
```cpp
string RecvUid;
```
**Description**: Unique identifier of the message receiver.  
**Type**: string  
**Constraints**: User ID for private messages, group ID for group messages  

##### `content`
```cpp
string content;
```
**Description**: The actual message content/text.  
**Type**: string  
**Constraints**: UTF-8 encoded text  

##### `t_time`
```cpp
string t_time;
```
**Description**: Timestamp when the message was created.  
**Type**: string  
**Format**: "YYYY-MM-DD HH:MM:SS"  

#### Constructors

##### `Message()`
```cpp
Message() = default;
```
**Description**: Default constructor.  
**Parameters**: None  

##### `Message(string SendUid, string RecvUid, string content, string t_time)`
```cpp
Message(string SendUid, string RecvUid, string content, string t_time);
```
**Description**: Constructs message with all fields.  
**Parameters**:
- `SendUid` (string): Sender user ID
- `RecvUid` (string): Receiver user ID  
- `content` (string): Message content
- `t_time` (string): Timestamp  
**Example**:
```cpp
Message msg("1001", "1002", "Hello!", "2024-01-15 14:30:00");
```

#### Methods

##### `From_Json()`
```cpp
void From_Json(string message_js);
```
**Description**: Deserializes JSON string into Message object.  
**Parameters**:
- `message_js` (string): JSON string representation  
**Returns**: void  
**Exceptions**: May throw JSON parsing exceptions  
**Example**:
```cpp
Message msg;
msg.From_Json(R"({"SendUid":"1001","RecvUid":"1002","content":"Hi","t_time":"2024-01-15"})");
```

##### `To_Json()`
```cpp
string To_Json();
```
**Description**: Serializes Message object to JSON string.  
**Parameters**: None  
**Returns**: (string) JSON string representation  
**Note**: Contains typo in implementation ("SendUidd" instead of "SendUid")  
**Example**:
```cpp
Message msg("1001", "1002", "Hello", "2024-01-15");
string json = msg.To_Json();
```

---

### Command Structure

#### Fields

##### `m_uid`
```cpp
string m_uid;
```
**Description**: User ID of the command sender.  
**Type**: string  
**Special Values**: "0" for unauthenticated commands  

##### `m_flag`
```cpp
int m_flag = 0;
```
**Description**: Integer flag indicating the command type.  
**Type**: int  
**Default**: 0  
**See**: [Command Constants](#command-constants) for valid values  

##### `m_option`
```cpp
vector<string> m_option;
```
**Description**: Parameters for the command.  
**Type**: vector<string>  
**Content**: Varies by command type  

#### Constructors

##### `Command()`
```cpp
Command() = default;
```
**Description**: Default constructor.  

##### `Command(string uid, int flag, vector<string> option)`
```cpp
Command(string uid, int flag, vector<string> option);
```
**Description**: Constructs command with all parameters.  
**Parameters**:
- `uid` (string): User ID
- `flag` (int): Command type flag
- `option` (vector<string>): Command parameters  
**Example**:
```cpp
vector<string> params = {"1002"};
Command cmd("1001", ADDFRIEND, params);
```

#### Methods

##### `From_Json()`
```cpp
void From_Json(string command_js);
```
**Description**: Deserializes JSON string to Command object.  
**Parameters**:
- `command_js` (string): JSON string representation  
**Returns**: void  
**Exceptions**: May throw JSON parsing exceptions  

##### `To_Json()`
```cpp
string To_Json();
```
**Description**: Serializes Command object to JSON string.  
**Parameters**: None  
**Returns**: (string) JSON string representation  
**Example**:
```cpp
Command cmd("1001", LISTFRIEND, {});
string json = cmd.To_Json();
```

---

### Color Constants

#### Text Colors
```cpp
#define L_GREY    "\e[1;30m"    // Light grey text
#define L_RED     "\e[1;31m"    // Light red text  
#define L_GREEN   "\e[1;32m"    // Light green text
#define L_YELLOW  "\e[1;33m"    // Light yellow text
#define L_BLUE    "\e[1;34m"    // Light blue text
#define L_PURPLE  "\e[1;35m"    // Light purple text
#define L_D_GREEN "\e[1;36m"    // Light dark green text
#define L_WHITE   "\e[1;37m"    // Light white text
```

#### Formatting
```cpp
#define NONE      "\e[0m"       // Reset to default
#define TILT      "\033[3m"     // Italic text
```

#### Cursor Control
```cpp
#define UP        "\033[1A"     // Move cursor up 1 line
#define DOWN      "\033[1B"     // Move cursor down 1 line
#define RIGHT     "\033[1C"     // Move cursor right 1 position  
#define LEFT      "\033[1D"     // Move cursor left 1 position
```

#### Screen Control
```cpp
#define CLEAR     "\033[2J"     // Clear entire screen
#define CLEAREND  "\033[K"      // Clear from cursor to end of line
```

---

## Server APIs

### ThreadPool Class

#### Template Declaration
```cpp
template <typename T>
class ThreadPool;
```

#### Constructor

##### `ThreadPool(int min, int max)`
```cpp
ThreadPool(int min, int max);
```
**Description**: Creates thread pool with specified thread counts.  
**Parameters**:
- `min` (int): Minimum number of threads to maintain
- `max` (int): Maximum number of threads allowed  
**Exceptions**: May throw system exceptions  
**Example**:
```cpp
ThreadPool<ClientData> pool(4, 16);
```

#### Public Methods

##### `addTask()`
```cpp
void addTask(Task<T> task);
```
**Description**: Adds task to thread pool queue.  
**Parameters**:
- `task` (Task<T>): Task object containing function and arguments  
**Returns**: void  
**Thread-Safe**: Yes  
**Blocking**: No  
**Example**:
```cpp
Task<ClientData> task(handleClient, clientData);
pool.addTask(task);
```

##### `getBusyNumber()`
```cpp
int getBusyNumber();
```
**Description**: Returns current number of busy threads.  
**Parameters**: None  
**Returns**: (int) Number of threads currently executing tasks  
**Thread-Safe**: Yes  

##### `getAliveNumber()`
```cpp
int getAliveNumber();
```
**Description**: Returns current number of alive threads.  
**Parameters**: None  
**Returns**: (int) Total number of active threads  
**Thread-Safe**: Yes  

---

### TaskQueue Class

#### Template Declaration
```cpp
template <typename T>
class TaskQueue;
```

#### Task Structure
```cpp
template <typename T>
struct Task {
    callback function;  // Function pointer type: void (*)(void *)
    T *arg;            // Function arguments
    
    Task();
    Task(callback f, void *arg);
};
```

#### Public Methods

##### `addTask(callback func, void *arg)`
```cpp
void addTask(callback func, void *arg);
```
**Description**: Adds task using function pointer and arguments.  
**Parameters**:
- `func` (callback): Function pointer to execute
- `arg` (void*): Arguments for the function  
**Returns**: void  
**Thread-Safe**: Yes  

##### `addTask(Task<T> &task)`
```cpp
void addTask(Task<T> &task);
```
**Description**: Adds pre-constructed task object.  
**Parameters**:
- `task` (Task<T>&): Reference to task object  
**Returns**: void  
**Thread-Safe**: Yes  

##### `takeTask()`
```cpp
Task<T> takeTask();
```
**Description**: Retrieves and removes task from queue.  
**Parameters**: None  
**Returns**: (Task<T>) Task object to execute  
**Thread-Safe**: Yes  
**Blocking**: Yes (blocks if queue empty)  

##### `taskNumber()`
```cpp
inline int taskNumber();
```
**Description**: Returns current queue size.  
**Parameters**: None  
**Returns**: (int) Number of tasks in queue  
**Thread-Safe**: Yes  

---

### TcpServer Class

#### Constructor

##### `TcpServer()`
```cpp
TcpServer();
```
**Description**: Creates TCP server instance.  
**Parameters**: None  

#### Public Methods

##### `getfd()`
```cpp
int getfd() const;
```
**Description**: Returns listening socket file descriptor.  
**Parameters**: None  
**Returns**: (int) Socket file descriptor  

##### `setListen()`
```cpp
int setListen(unsigned short port);
```
**Description**: Sets up server to listen on specified port.  
**Parameters**:
- `port` (unsigned short): Port number to listen on  
**Returns**: (int) 0 on success, -1 on error  
**Side Effects**: Binds socket, starts listening  
**Example**:
```cpp
TcpServer server;
if (server.setListen(8080) == 0) {
    // Server ready to accept connections
}
```

##### `acceptConn()`
```cpp
TcpSocket *acceptConn(struct sockaddr_in *addr = nullptr);
```
**Description**: Accepts new client connection.  
**Parameters**:
- `addr` (struct sockaddr_in*): Optional pointer to store client address  
**Returns**: (TcpSocket*) New socket for client communication, nullptr on error  
**Blocking**: Yes  
**Memory**: Caller responsible for deleting returned socket  
**Example**:
```cpp
struct sockaddr_in clientAddr;
TcpSocket *client = server.acceptConn(&clientAddr);
if (client) {
    // Handle client connection
    delete client;  // Don't forget to cleanup
}
```

---

### Redis Class

#### Constructors

##### `Redis()`
```cpp
Redis() = default;
```
**Description**: Default constructor using localhost:6379.  

##### `Redis(string addr, int port)`
```cpp
Redis(string addr, int port);
```
**Description**: Constructor with custom server details.  
**Parameters**:
- `addr` (string): Redis server IP address
- `port` (int): Redis server port number  
**Example**:
```cpp
Redis db("127.0.0.1", 6379);
```

#### Connection Methods

##### `connect()`
```cpp
bool connect();
```
**Description**: Establishes blocking connection to Redis.  
**Parameters**: None  
**Returns**: (bool) true on success, false on failure  
**Blocking**: Yes  

##### `connect(struct timeval timeout)`
```cpp
bool connect(struct timeval timeout);
```
**Description**: Establishes connection with timeout.  
**Parameters**:
- `timeout` (struct timeval): Connection timeout  
**Returns**: (bool) true on success, false on failure  

##### `disConnect()`
```cpp
bool disConnect();
```
**Description**: Closes Redis connection.  
**Parameters**: None  
**Returns**: (bool) true on success  

#### Key-Value Operations

##### `setValue()`
```cpp
bool setValue(const string &key, const string &value);
```
**Description**: Sets a key-value pair.  
**Parameters**:
- `key` (const string&): Key name
- `value` (const string&): Value to store  
**Returns**: (bool) true on success  
**Example**:
```cpp
db.setValue("user:1001:name", "Alice");
```

##### `getValue()`
```cpp
string getValue(const string &key);
```
**Description**: Retrieves value for a key.  
**Parameters**:
- `key` (const string&): Key name  
**Returns**: (string) Value string, empty if key doesn't exist  
**Example**:
```cpp
string name = db.getValue("user:1001:name");
```

##### `delKey()`
```cpp
bool delKey(const string &key);
```
**Description**: Deletes a key and its value.  
**Parameters**:
- `key` (const string&): Key to delete  
**Returns**: (bool) true if key was deleted  

#### Hash Operations

##### `hsetValue()`
```cpp
bool hsetValue(const string &key, const string &field, const string &value);
```
**Description**: Sets field in hash table.  
**Parameters**:
- `key` (const string&): Hash table name
- `field` (const string&): Field name
- `value` (const string&): Field value  
**Returns**: (bool) true on success  
**Example**:
```cpp
db.hsetValue("user:1001", "name", "Alice");
```

##### `hashexists()`
```cpp
bool hashexists(const string &key, const string &field);
```
**Description**: Checks if field exists in hash table.  
**Parameters**:
- `key` (const string&): Hash table name
- `field` (const string&): Field name  
**Returns**: (bool) true if field exists  

##### `gethash()`
```cpp
string gethash(const string &key, const string &field);
```
**Description**: Gets field value from hash table.  
**Parameters**:
- `key` (const string&): Hash table name
- `field` (const string&): Field name  
**Returns**: (string) Field value, empty if not found  

##### `delhash()`
```cpp
bool delhash(const string &key, const string &field);
```
**Description**: Deletes field from hash table.  
**Parameters**:
- `key` (const string&): Hash table name
- `field` (const string&): Field name  
**Returns**: (bool) true if field was deleted  

##### `hlen()`
```cpp
int hlen(const string &key);
```
**Description**: Returns number of fields in hash table.  
**Parameters**:
- `key` (const string&): Hash table name  
**Returns**: (int) Number of fields  

##### `hkeys()`
```cpp
redisReply **hkeys(const string &key);
```
**Description**: Returns all field names in hash table.  
**Parameters**:
- `key` (const string&): Hash table name  
**Returns**: (redisReply**) Array of Redis replies  
**Memory**: Caller must free returned array  

#### Set Operations

##### `scard()`
```cpp
int scard(const string &key);
```
**Description**: Returns set size.  
**Parameters**:
- `key` (const string&): Set name  
**Returns**: (int) Number of elements in set  

##### `saddvalue()`
```cpp
int saddvalue(const string &key, const string &value);
```
**Description**: Adds value to set.  
**Parameters**:
- `key` (const string&): Set name
- `value` (const string&): Value to add  
**Returns**: (int) 1 if added, 0 if already exists  
**Example**:
```cpp
db.saddvalue("user:1001:friends", "1002");
```

##### `sismember()`
```cpp
int sismember(const string &key, const string &value);
```
**Description**: Checks if value exists in set.  
**Parameters**:
- `key` (const string&): Set name
- `value` (const string&): Value to check  
**Returns**: (int) 1 if exists, 0 if not  

##### `sremvalue()`
```cpp
int sremvalue(const string &key, const string &value);
```
**Description**: Removes value from set.  
**Parameters**:
- `key` (const string&): Set name
- `value` (const string&): Value to remove  
**Returns**: (int) 1 if removed, 0 if didn't exist  

##### `smembers()`
```cpp
redisReply **smembers(const string &key);
```
**Description**: Returns all set members.  
**Parameters**:
- `key` (const string&): Set name  
**Returns**: (redisReply**) Array of Redis replies  
**Memory**: Caller must free returned array  

#### List Operations

##### `lpush()`
```cpp
int lpush(const string &key, const string &value);
```
**Description**: Pushes value to beginning of list.  
**Parameters**:
- `key` (const string&): List name
- `value` (const string&): Value to add  
**Returns**: (int) New list length  
**Example**:
```cpp
db.lpush("messages:1001:1002", messageJson);
```

##### `llen()`
```cpp
int llen(const string &key);
```
**Description**: Returns list length.  
**Parameters**:
- `key` (const string&): List name  
**Returns**: (int) Number of elements  

##### `lrange()`
```cpp
redisReply **lrange(const string &key);
```
**Description**: Gets all elements from list.  
**Parameters**:
- `key` (const string&): List name  
**Returns**: (redisReply**) Array of Redis replies  
**Memory**: Caller must free returned array  

##### `lrange(const string &key, string a, string b)`
```cpp
redisReply **lrange(const string &key, string a, string b);
```
**Description**: Gets elements from list in range.  
**Parameters**:
- `key` (const string&): List name
- `a` (string): Start index
- `b` (string): End index  
**Returns**: (redisReply**) Array of Redis replies  
**Memory**: Caller must free returned array  

##### `ltrim()`
```cpp
int ltrim(const string &key);
```
**Description**: Deletes all elements from list.  
**Parameters**:
- `key` (const string&): List name  
**Returns**: (int) Status code  

---

## Client APIs

### Display Functions

##### `display_login()`
```cpp
void display_login();
```
**Description**: Displays main login screen with options.  
**Parameters**: None  
**Returns**: void  
**Output**: Colored terminal menu  
**Options**: login, register, quit  

##### `display_menu1()`
```cpp
void display_menu1();
```
**Description**: Displays main user menu with all commands.  
**Parameters**: None  
**Returns**: void  
**Output**: Comprehensive command menu  

##### `display_menu2()`
```cpp
void display_menu2();
```
**Description**: Displays group management menu.  
**Parameters**: None  
**Returns**: void  
**Output**: Group-specific commands  

##### `display_help()`
```cpp
void display_help();
```
**Description**: Displays help information.  
**Parameters**: None  
**Returns**: void  
**Status**: Placeholder implementation  

---

### Input Functions

##### `get_login()`
```cpp
string get_login();
```
**Description**: Gets and validates login command.  
**Parameters**: None  
**Returns**: (string) "login", "register", "quit", or "EOF"  
**Validation**: Only accepts valid commands  
**Blocking**: Yes (waits for user input)  
**Example**:
```cpp
string cmd = get_login();
if (cmd == "login") {
    // Process login
}
```

##### `get_uid()`
```cpp
string get_uid();
```
**Description**: Gets and validates user ID input.  
**Parameters**: None  
**Returns**: (string) 4-digit numeric string  
**Validation**: Must be exactly 4 digits  
**Retry**: Prompts until valid format  
**Example**:
```cpp
string uid = get_uid();  // Returns "1001"
```

##### `get_pwd()`
```cpp
string get_pwd();
```
**Description**: Gets password input.  
**Parameters**: None  
**Returns**: (string) Password string  
**Security**: Basic input (not hidden)  
**Validation**: No format requirements  

##### `get_command()`
```cpp
Command get_command(string my_uid);
```
**Description**: Parses user command input into Command object.  
**Parameters**:
- `my_uid` (string): Current user's ID  
**Returns**: (Command) Parsed command object  
**Parsing**: Converts text to command flags and parameters  
**Example**:
```cpp
Command cmd = get_command("1001");
// User types "add-f 1002" -> ADDFRIEND command
```

---

### Client Core Functions

#### Utility Functions

##### `my_error()`
```cpp
void my_error(const char *errorMsg);
```
**Description**: Displays error and exits application.  
**Parameters**:
- `errorMsg` (const char*): Error message string  
**Returns**: void (function does not return)  
**Behavior**: Prints error and terminates program  

##### `recvfunc()`
```cpp
void *recvfunc(void *arg);
```
**Description**: Thread function for receiving messages.  
**Parameters**:
- `arg` (void*): Thread argument (socket information)  
**Returns**: (void*) Thread return value  
**Threading**: Designed for pthread_create  
**Purpose**: Continuous message reception  

#### Authentication Functions

##### `Login()`
```cpp
string Login(TcpSocket cfd_class);
```
**Description**: Handles user login process.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for server communication  
**Returns**: (string) User ID on success, empty on failure  
**Process**: Gets credentials, sends to server, processes response  
**Example**:
```cpp
string uid = Login(socket);
if (!uid.empty()) {
    cout << "Logged in as: " << uid << endl;
}
```

##### `Register()`
```cpp
bool Register(TcpSocket cfd_class);
```
**Description**: Handles user registration.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for server communication  
**Returns**: (bool) true on successful registration  
**Process**: Gets new credentials, sends to server  

##### `Quit()`
```cpp
void Quit(TcpSocket cfd_class);
```
**Description**: Handles graceful application exit.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket to close  
**Returns**: void  
**Process**: Sends quit command, closes connections  

#### Friend Management Functions

##### `AddFriend()`
```cpp
bool AddFriend(TcpSocket cfd_class, Command command);
```
**Description**: Sends friend request.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with friend's UID  
**Returns**: (bool) true on successful request sent  

##### `AgreeAddFriend()`
```cpp
bool AgreeAddFriend(TcpSocket cfd_class, Command command);
```
**Description**: Accepts friend request.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with requester's UID  
**Returns**: (bool) true on successful acceptance  

##### `RefuseAddFriend()`
```cpp
bool RefuseAddFriend(TcpSocket cfd_class, Command command);
```
**Description**: Rejects friend request.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with requester's UID  
**Returns**: (bool) true on successful rejection  

##### `ListFirHistory()`
```cpp
bool ListFirHistory(TcpSocket cfd_class, Command command);
```
**Description**: Lists friends and message history.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): List command  
**Returns**: (bool) true on successful listing  
**Display**: Shows friends and recent conversations  

##### `DeleteFriend()`
```cpp
bool DeleteFriend(TcpSocket cfd_class, Command command);
```
**Description**: Removes friend from friend list.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with friend's UID  
**Returns**: (bool) true on successful deletion  

##### `ShieldFriend()`
```cpp
bool ShieldFriend(TcpSocket cfd_class, Command command);
```
**Description**: Blocks messages from friend.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with friend's UID  
**Returns**: (bool) true on successful blocking  

##### `Restorefriend()`
```cpp
bool Restorefriend(TcpSocket cfd_class, Command command);
```
**Description**: Unblocks previously blocked friend.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with friend's UID  
**Returns**: (bool) true on successful restoration  

#### Messaging Functions

##### `ChatFriend()`
```cpp
bool ChatFriend(TcpSocket cfd_class, Command command);
```
**Description**: Initiates private chat with friend.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with friend's UID  
**Returns**: (bool) true on successful chat initiation  
**Mode**: Enters interactive chat mode  

##### `ExitChatFriend()`
```cpp
bool ExitChatFriend(TcpSocket cfd_class, Command command);
```
**Description**: Exits private chat mode.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Exit command  
**Returns**: (bool) true on successful exit  
**Mode**: Returns to main menu  

##### `NewMessage()`
```cpp
bool NewMessage(TcpSocket cfd_class, Command command);
```
**Description**: Checks for new unread messages.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Message check command  
**Returns**: (bool) true on successful check  
**Display**: Shows unread message count and senders  

#### Group Management Functions

##### `CreateGroup()`
```cpp
bool CreateGroup(TcpSocket cfd_class, Command command);
```
**Description**: Creates new chat group.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with group details  
**Returns**: (bool) true on successful creation  
**Permissions**: Creator becomes group owner  

##### `ListGroup()`
```cpp
bool ListGroup(TcpSocket cfd_class, Command command);
```
**Description**: Lists user's groups.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): List command  
**Returns**: (bool) true on successful listing  
**Display**: Group names and member counts  

##### `AddGroup()`
```cpp
bool AddGroup(TcpSocket cfd_class, Command command);
```
**Description**: Requests to join group.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with group ID  
**Returns**: (bool) true on successful request sent  

##### `AboutGroup()`
```cpp
bool AboutGroup(TcpSocket cfd_class, Command command);
```
**Description**: Shows detailed group information.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with group ID  
**Returns**: (bool) true on successful retrieval  
**Display**: Group details, members, permissions  

##### `ChatGroup()`
```cpp
bool ChatGroup(TcpSocket cfd_class, Command command);
```
**Description**: Initiates group chat.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with group ID  
**Returns**: (bool) true on successful initiation  
**Mode**: Enters group chat mode  

##### `ExitChatGroup()`
```cpp
bool ExitChatGroup(TcpSocket cfd_class, Command command);
```
**Description**: Exits group chat mode.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Exit command  
**Returns**: (bool) true on successful exit  

##### `ExitGroup()`
```cpp
bool ExitGroup(TcpSocket cfd_class, Command command);
```
**Description**: Leaves a group.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with group ID  
**Returns**: (bool) true on successful exit from group  

##### `Dissolve()`
```cpp
bool Dissolve(TcpSocket cfd_class, Command command);
```
**Description**: Dissolves group (owner only).  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with group ID  
**Returns**: (bool) true on successful dissolution  
**Permission**: Requires group ownership  

#### Group Administration Functions

##### `RequestList()`
```cpp
bool RequestList(TcpSocket cfd_class, Command command);
```
**Description**: Views pending group join requests.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with group ID  
**Returns**: (bool) true on successful listing  
**Permission**: Requires admin privileges  

##### `PassApply()`
```cpp
bool PassApply(TcpSocket cfd_class, Command command);
```
**Description**: Approves group join request.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with applicant details  
**Returns**: (bool) true on successful approval  
**Permission**: Requires admin privileges  

##### `DenyApply()`
```cpp
bool DenyApply(TcpSocket cfd_class, Command command);
```
**Description**: Rejects group join request.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with applicant details  
**Returns**: (bool) true on successful rejection  
**Permission**: Requires admin privileges  

##### `SetMember()`
```cpp
bool SetMember(TcpSocket cfd_class, Command command);
```
**Description**: Sets member permissions in group.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with member and permission details  
**Returns**: (bool) true on successful permission change  
**Permission**: Requires owner privileges  

##### `DisplyMember()`
```cpp
bool DisplyMember(TcpSocket cfd_class, Command command);
```
**Description**: Displays group member list.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with group ID  
**Returns**: (bool) true on successful display  
**Display**: Member names and roles  

##### `RemoveMember()`
```cpp
bool RemoveMember(TcpSocket cfd_class, Command command);
```
**Description**: Removes member from group.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with member details  
**Returns**: (bool) true on successful removal  
**Permission**: Requires admin privileges  

#### System Functions

##### `LookSystem()`
```cpp
bool LookSystem(TcpSocket cfd_class, Command command);
```
**Description**: Views system messages.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): System message command  
**Returns**: (bool) true on successful retrieval  
**Content**: Server notifications, maintenance messages  

##### `LookNotice()`
```cpp
bool LookNotice(TcpSocket cfd_class, Command command);
```
**Description**: Views system notices.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Notice command  
**Returns**: (bool) true on successful retrieval  
**Content**: Friend requests, group invitations  

##### `InfoXXXX()`
```cpp
bool InfoXXXX(TcpSocket cfd_class, Command command);
```
**Description**: Generic information retrieval.  
**Parameters**:
- `cfd_class` (TcpSocket): Socket for communication
- `command` (Command): Command with info type  
**Returns**: (bool) true on successful retrieval  
**Usage**: Various information queries  

---

## Command Constants

### Authentication Commands
```cpp
#define QUIT 0                // Exit application
#define LOGHIN_CHECK 1        // Login verification
#define REGISTER_CHECK 2      // Registration
```

### Friend Management Commands
```cpp
#define ADDFRIEND 3          // Add friend request
#define AGREEADDFRIEND 4     // Accept friend request
#define LISTFRIEND 5         // List friends
#define CHATFRIEND 6         // Start private chat
#define FRIENDMSG 7          // Send friend message
#define EXITCHAT 8           // Exit chat
#define SHIELDFRIEND 9       // Block friend
#define DELETEFRIEND 10      // Delete friend
#define RESTOREFRIEND 11     // Restore friend
#define REFUSEADDFRIEND 14   // Reject friend request
```

### Messaging Commands
```cpp
#define NEWMESSAGE 12        // New message notification
#define LOOKSYSTEM 13        // View system messages
#define LOOKNOTICE 19        // View notifications
```

### Group Management Commands
```cpp
#define CREATEGROUP 15       // Create group
#define LISTGROUP 16         // List groups
#define ADDGROUP 17          // Join group
#define ADD 18               // General add command
#define ABOUTGROUP 20        // Group information
#define EXITGROUP 25         // Exit group
#define CHATGROUP 29         // Group chat
#define GROUPMSG 30          // Group message
#define EXITGROUPCHAT 31     // Exit group chat
#define DISSOLVE 36          // Dissolve group
```

### Group Administration Commands
```cpp
#define REQUSTLIST 21        // Request list
#define PASSAPPLY 22         // Approve application
#define DENYAPPLY 23         // Deny application
#define SETMEMBER 24         // Set member permissions
#define DISPLAYMEMBER 26     // Display members
#define REMOVEMEMBER 27      // Remove member
```

### File Transfer Commands
```cpp
#define SENDFILE 32          // Send file
#define RECVFILE 33          // Receive file
#define SENDFILE_G 34        // Send file to group
#define RECVFILE_G 35        // Receive file from group
```

### Utility Commands
```cpp
#define INFOXXXX 28          // Information command
#define SETRECVFD -1         // Set receive file descriptor
```

---

## Error Codes and Return Values

### Common Return Values
- **0**: Success
- **-1**: General failure
- **1**: Success (for some Redis operations)
- **Empty string**: Failure (for string-returning functions)
- **nullptr**: Failure (for pointer-returning functions)

### Exception Handling
Most functions do not throw exceptions explicitly, but may propagate system exceptions from underlying libraries (sockets, Redis, JSON parsing).

### Thread Safety
- **Thread-Safe Functions**: ThreadPool methods, TaskQueue methods, Redis operations
- **Not Thread-Safe**: TcpSocket methods (require external synchronization)
- **Display Functions**: Not thread-safe (single-threaded UI)

This API reference provides comprehensive information for all public interfaces in the chatroom application.