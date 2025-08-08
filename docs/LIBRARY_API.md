# Core Library API Documentation

## Overview

The core library provides essential components for network communication, message handling, command processing, and terminal formatting in the chatroom application.

## TCPSocket Class

### Description
The `TcpSocket` class provides a high-level abstraction for TCP socket communication, handling both client and server-side networking operations.

### Header File
```cpp
#include "lib/TCPSocket.hpp"
```

### Class Definition
```cpp
class TcpSocket {
public:
    TcpSocket();
    TcpSocket(int socket);
    TcpSocket(string recv);
    ~TcpSocket();
    
    // Getters
    int getfd() const;
    int getrecvfd() const;
    
    // Network operations
    int connectToHost(string ip, unsigned short port);
    int sendMsg(string msg);
    string recvMsg();
    
private:
    int readn(char *buf, int size);
    int writen(const char *msg, int size);
    
    int m_fd = -1;      // Communication socket
    int recv_fd = -1;   // Receive notification socket
};
```

### Constructor Details

#### `TcpSocket()`
Creates a new TCP socket for communication.
- **Returns**: New TcpSocket instance
- **Usage**: For creating client sockets

#### `TcpSocket(int socket)`
Wraps an existing socket file descriptor.
- **Parameters**: 
  - `socket`: Existing socket file descriptor
- **Usage**: For server-side accepted connections

#### `TcpSocket(string recv)`
Creates a socket with notification capability.
- **Parameters**:
  - `recv`: Must be "recv" to enable notification socket
- **Usage**: For clients needing separate notification channel

### Public Methods

#### `int getfd() const`
Returns the main communication socket file descriptor.
- **Returns**: Socket file descriptor or -1 if invalid

#### `int getrecvfd() const` 
Returns the notification socket file descriptor.
- **Returns**: Notification socket file descriptor or -1 if not available

#### `int connectToHost(string ip, unsigned short port)`
Connects to a remote server.
- **Parameters**:
  - `ip`: Server IP address (e.g., "127.0.0.1")
  - `port`: Server port number
- **Returns**: 0 on success, -1 on failure
- **Example**:
  ```cpp
  TcpSocket client;
  if (client.connectToHost("127.0.0.1", 8080) == 0) {
      cout << "Connected successfully!" << endl;
  }
  ```

#### `int sendMsg(string msg)`
Sends a message with automatic length prefixing.
- **Parameters**:
  - `msg`: Message content to send
- **Returns**: Number of bytes sent, -1 on error
- **Protocol**: Automatically adds 4-byte length header
- **Example**:
  ```cpp
  string message = "Hello, server!";
  int result = client.sendMsg(message);
  ```

#### `string recvMsg()`
Receives a complete message.
- **Returns**: Received message content
- **Protocol**: Automatically handles length-prefixed messages
- **Blocking**: This method blocks until a complete message is received
- **Example**:
  ```cpp
  string response = client.recvMsg();
  cout << "Received: " << response << endl;
  ```

---

## Message Structure

### Description
The `Message` struct represents a chat message with JSON serialization capabilities.

### Header File
```cpp
#include "lib/Message.hpp"
```

### Structure Definition
```cpp
struct Message {
    string SendUid;    // Sender user ID
    string RecvUid;    // Receiver user ID  
    string content;    // Message content
    string t_time;     // Timestamp
    
    // Constructors
    Message() = default;
    Message(string SendUid, string RecvUid, string content, string t_time);
    
    // JSON operations
    void From_Json(string message_js);
    string To_Json();
};
```

### Fields

#### `string SendUid`
Unique identifier of the message sender.

#### `string RecvUid`
Unique identifier of the message receiver. For group messages, this represents the group ID.

#### `string content`
The actual message content/text.

#### `string t_time`
Timestamp when the message was created or sent.

### Methods

#### `Message(string SendUid, string RecvUid, string content, string t_time)`
Constructs a message with all fields.
- **Parameters**: All message fields
- **Example**:
  ```cpp
  Message msg("user123", "user456", "Hello!", "2024-01-01 12:00:00");
  ```

#### `void From_Json(string message_js)`
Deserializes a JSON string into the Message object.
- **Parameters**:
  - `message_js`: JSON string representation
- **Example**:
  ```cpp
  Message msg;
  string json_str = R"({"SendUid":"user1","RecvUid":"user2","content":"Hi","t_time":"2024-01-01"})";
  msg.From_Json(json_str);
  ```

#### `string To_Json()`
Serializes the Message object to JSON string.
- **Returns**: JSON string representation
- **Note**: There's a typo in the implementation - "SendUidd" instead of "SendUid"
- **Example**:
  ```cpp
  Message msg("user1", "user2", "Hello", "2024-01-01");
  string json_output = msg.To_Json();
  ```

---

## Command Structure

### Description
The `Command` struct represents client-server commands with JSON serialization.

### Header File
```cpp
#include "lib/Command.hpp"
```

### Structure Definition
```cpp
struct Command {
    string m_uid;              // Sender user ID
    int m_flag = 0;           // Command type flag
    vector<string> m_option;   // Command parameters
    
    // Constructors
    Command() = default;
    Command(string uid, int flag, vector<string> option);
    
    // JSON operations
    void From_Json(string command_js);
    string To_Json();
};
```

### Fields

#### `string m_uid`
User ID of the command sender. Set to "0" if no authentication required.

#### `int m_flag`
Integer flag indicating the command type. See command constants in `Input.hpp`:
- `LOGHIN_CHECK = 1`: Login verification
- `REGISTER_CHECK = 2`: Registration
- `ADDFRIEND = 3`: Add friend request
- `LISTFRIEND = 5`: List friends
- `CHATFRIEND = 6`: Start private chat
- And many more...

#### `vector<string> m_option`
Parameters for the command, varies by command type.

### Methods

#### `Command(string uid, int flag, vector<string> option)`
Constructs a command with all parameters.
- **Example**:
  ```cpp
  vector<string> params = {"friend_uid"};
  Command cmd("user123", ADDFRIEND, params);
  ```

#### `void From_Json(string command_js)`
Deserializes JSON string to Command object.
- **Parameters**:
  - `command_js`: JSON string representation

#### `string To_Json()`
Serializes Command object to JSON string.
- **Returns**: JSON string representation
- **Example**:
  ```cpp
  Command cmd("user1", LISTFRIEND, {});
  string json_cmd = cmd.To_Json();
  ```

---

## Color Utilities

### Description
Terminal color and formatting constants for enhanced user interface.

### Header File
```cpp
#include "lib/Color.hpp"
```

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

### Usage Examples

#### Basic Color Usage
```cpp
#include "lib/Color.hpp"
#include <iostream>

cout << L_RED << "Error: " << NONE << "Something went wrong!" << endl;
cout << L_GREEN << "Success: " << NONE << "Operation completed." << endl;
cout << L_YELLOW << "Warning: " << NONE << "Please check input." << endl;
```

#### Menu Display
```cpp
cout << L_YELLOW << "***************聊天室***************" << NONE << endl
     << L_YELLOW "*" << NONE << L_BLUE << "------------登录(login)-----------"
     << NONE << L_YELLOW "*" << NONE << endl;
```

#### Cursor Manipulation
```cpp
cout << "Line 1" << endl;
cout << UP << "Overwriting line 1" << endl;  // Move up and overwrite
cout << CLEAR;  // Clear screen
```

## Error Handling

### Socket Errors
- Check return values: -1 typically indicates error
- Use `perror()` for system error messages
- Handle connection failures gracefully

### JSON Parsing Errors
- Ensure JSON strings are valid before parsing
- Handle malformed JSON gracefully
- Validate required fields exist

### Best Practices
1. Always check return values from socket operations
2. Use RAII principles for resource management
3. Validate input parameters before processing
4. Handle network timeouts and disconnections
5. Use color constants consistently for UI elements