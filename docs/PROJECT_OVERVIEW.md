# Chatroom Project Overview

## Introduction

This is a multi-threaded C++ chatroom application that provides real-time messaging capabilities between multiple clients through a centralized server. The project implements a client-server architecture with support for both private messaging and group chat functionality.

## Architecture

### System Architecture

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Client 1      │    │   TCP Server    │    │   Redis DB      │
│                 │◄──►│                 │◄──►│                 │
│  - Display UI   │    │  - ThreadPool   │    │  - User Data    │
│  - Input Handler│    │  - TaskQueue    │    │  - Messages     │
│  - TCP Socket   │    │  - Connection   │    │  - Groups       │
└─────────────────┘    │    Manager      │    └─────────────────┘
                       └─────────────────┘
┌─────────────────┐              │
│   Client 2      │              │
│                 │◄─────────────┘
└─────────────────┘
┌─────────────────┐
│   Client N      │
│                 │
└─────────────────┘
```

### Core Components

#### 1. **Library Components (`lib/`)**
- **TCPSocket**: Network communication abstraction
- **Message**: Message structure and JSON serialization
- **Command**: Command structure for client-server communication  
- **Color**: Terminal color and formatting utilities

#### 2. **Server Components (`Server/`)**
- **TCPServer**: TCP server implementation
- **ThreadPool**: Thread pool for handling concurrent connections
- **TaskQueue**: Task queue for thread pool management
- **Redis Integration**: Data persistence and user management
- **Option Handler**: Command processing and business logic

#### 3. **Client Components (`Client/`)**
- **Display**: User interface and menu systems
- **Input**: Command input processing and validation
- **Client**: Main client application logic

## Key Features

### User Management
- **User Registration**: Create new user accounts with unique UIDs
- **User Login**: Authenticate users with credentials
- **Friend Management**: Add, remove, and manage friend connections
- **User Status**: Online/offline status tracking

### Messaging System
- **Private Messages**: One-to-one messaging between users
- **Group Chat**: Multi-user group messaging
- **Message History**: Persistent message storage via Redis
- **Unread Messages**: Track and display unread message counts
- **System Notifications**: Server-generated system messages

### Group Management
- **Group Creation**: Create new chat groups
- **Member Management**: Add/remove group members
- **Group Administration**: Group owner privileges and member roles
- **Group Dissolution**: Delete groups and cleanup

### Advanced Features
- **File Sharing**: Send and receive files between users
- **Message Shielding**: Block/unblock conversations
- **Real-time Notifications**: Instant message delivery
- **Multi-threaded Server**: Concurrent client handling

## Technical Specifications

### Dependencies
- **C++ Standard**: C++11 or higher
- **JSON Library**: nlohmann/json for data serialization
- **Redis**: hiredis library for database operations
- **Threading**: POSIX threads (pthread)
- **Networking**: POSIX sockets

### Communication Protocol

#### Message Format
Messages are exchanged in JSON format with the following structure:
```json
{
  "SendUid": "sender_user_id",
  "RecvUid": "receiver_user_id", 
  "content": "message_content",
  "t_time": "timestamp"
}
```

#### Command Format
Commands use a structured format for client-server communication:
```json
{
  "uid": "user_id",
  "flag": command_type_integer,
  "option": ["parameter1", "parameter2", "..."]
}
```

### Threading Model
- **Main Thread**: Accepts new client connections
- **Worker Threads**: Handle client requests and message processing
- **Manager Thread**: Monitors and manages thread pool size
- **Client Threads**: Separate threads for sending and receiving

## Data Storage

### Redis Schema
- **Users**: User account information and credentials
- **Friends**: Friend relationship mappings
- **Groups**: Group information and member lists
- **Messages**: Message history and metadata
- **Notifications**: System notifications and friend requests

## Security Considerations
- User authentication with password verification
- Input validation for all user commands
- SQL injection prevention through parameterized queries
- Buffer overflow protection in network communication

## Performance Features
- **Connection Pooling**: Reuse database connections
- **Thread Pool**: Efficient resource utilization
- **Asynchronous I/O**: Non-blocking network operations
- **Message Queuing**: Efficient message delivery

## Build Requirements
- CMake 3.12 or higher
- GCC with C++11 support
- Redis server installation
- nlohmann-json library
- hiredis development libraries

This chatroom application provides a robust foundation for real-time communication with scalable architecture and comprehensive feature set.