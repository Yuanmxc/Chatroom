# Usage Examples and Code Samples

## Overview

This document provides comprehensive examples demonstrating how to use the chatroom APIs, from basic socket communication to complex multi-threaded server implementation.

## Basic Socket Communication

### Client-Side Socket Usage

```cpp
#include "lib/TCPSocket.hpp"
#include "lib/Message.hpp"
#include "lib/Command.hpp"
#include <iostream>

int main() {
    // Create and connect client socket
    TcpSocket client;
    if (client.connectToHost("127.0.0.1", 8080) != 0) {
        cerr << "Failed to connect to server" << endl;
        return -1;
    }
    
    // Send a login command
    Command loginCmd("1001", LOGHIN_CHECK, {"1001", "password123"});
    string cmdJson = loginCmd.To_Json();
    
    if (client.sendMsg(cmdJson) > 0) {
        cout << "Login command sent successfully" << endl;
        
        // Receive server response
        string response = client.recvMsg();
        cout << "Server response: " << response << endl;
    }
    
    return 0;
}
```

### Server-Side Socket Usage

```cpp
#include "Server/TCPServer.hpp"
#include "Server/ThreadPool.hpp"
#include <iostream>

// Client handler function
void handleClient(void* arg) {
    TcpSocket* client = static_cast<TcpSocket*>(arg);
    
    while (true) {
        string message = client->recvMsg();
        if (message.empty()) break;
        
        cout << "Received: " << message << endl;
        
        // Echo back to client
        client->sendMsg("Echo: " + message);
    }
    
    delete client;
}

int main() {
    // Create and setup server
    TcpServer server;
    if (server.setListen(8080) != 0) {
        cerr << "Failed to setup server" << endl;
        return -1;
    }
    
    // Create thread pool
    ThreadPool<TcpSocket> pool(4, 16);
    
    cout << "Server listening on port 8080..." << endl;
    
    while (true) {
        // Accept new client
        TcpSocket* client = server.acceptConn();
        if (client) {
            cout << "New client connected" << endl;
            
            // Add to thread pool
            Task<TcpSocket> task(handleClient, client);
            pool.addTask(task);
        }
    }
    
    return 0;
}
```

## Message and Command Handling

### Creating and Parsing Messages

```cpp
#include "lib/Message.hpp"
#include "lib/Command.hpp"
#include <iostream>

void messageExample() {
    // Create a message
    Message msg("1001", "1002", "Hello, friend!", "2024-01-15 14:30:00");
    
    // Convert to JSON
    string jsonMsg = msg.To_Json();
    cout << "JSON Message: " << jsonMsg << endl;
    
    // Parse JSON back to message
    Message receivedMsg;
    receivedMsg.From_Json(jsonMsg);
    
    cout << "Sender: " << receivedMsg.SendUid << endl;
    cout << "Receiver: " << receivedMsg.RecvUid << endl;
    cout << "Content: " << receivedMsg.content << endl;
    cout << "Time: " << receivedMsg.t_time << endl;
}

void commandExample() {
    // Create add friend command
    vector<string> params = {"1002"};  // Friend's UID
    Command addFriendCmd("1001", ADDFRIEND, params);
    
    // Convert to JSON for transmission
    string cmdJson = addFriendCmd.To_Json();
    cout << "Command JSON: " << cmdJson << endl;
    
    // Parse received command
    Command receivedCmd;
    receivedCmd.From_Json(cmdJson);
    
    cout << "User: " << receivedCmd.m_uid << endl;
    cout << "Command: " << receivedCmd.m_flag << endl;
    cout << "Parameters: ";
    for (const auto& param : receivedCmd.m_option) {
        cout << param << " ";
    }
    cout << endl;
}
```

### Command Processing Pattern

```cpp
#include "lib/Command.hpp"
#include "Client/Input.hpp"

bool processCommand(TcpSocket& socket, const Command& cmd) {
    switch (cmd.m_flag) {
        case ADDFRIEND:
            return AddFriend(socket, cmd);
            
        case LISTFRIEND:
            return ListFirHistory(socket, cmd);
            
        case CHATFRIEND:
            return ChatFriend(socket, cmd);
            
        case CREATEGROUP:
            return CreateGroup(socket, cmd);
            
        case LISTGROUP:
            return ListGroup(socket, cmd);
            
        default:
            cout << "Unknown command: " << cmd.m_flag << endl;
            return false;
    }
}

// Example usage
void clientMainLoop(TcpSocket& socket, const string& userUid) {
    while (true) {
        Command cmd = get_command(userUid);
        
        if (cmd.m_flag == QUIT) {
            Quit(socket);
            break;
        }
        
        if (!processCommand(socket, cmd)) {
            cout << "Command failed" << endl;
        }
    }
}
```

## Redis Database Operations

### User Management with Redis

```cpp
#include "Server/redis.hpp"
#include <iostream>

class UserManager {
private:
    Redis db;
    
public:
    UserManager() : db("127.0.0.1", 6379) {
        if (!db.connect()) {
            throw runtime_error("Failed to connect to Redis");
        }
    }
    
    // Register new user
    bool registerUser(const string& uid, const string& password, const string& name) {
        // Check if user already exists
        if (db.hashexists("user:" + uid, "password")) {
            cout << "User already exists: " << uid << endl;
            return false;
        }
        
        // Store user information
        db.hsetValue("user:" + uid, "password", password);
        db.hsetValue("user:" + uid, "name", name);
        db.hsetValue("user:" + uid, "status", "offline");
        db.hsetValue("user:" + uid, "created", getCurrentTime());
        
        cout << "User registered successfully: " << uid << endl;
        return true;
    }
    
    // Authenticate user
    bool authenticateUser(const string& uid, const string& password) {
        string storedPassword = db.gethash("user:" + uid, "password");
        if (storedPassword.empty()) {
            cout << "User not found: " << uid << endl;
            return false;
        }
        
        if (storedPassword != password) {
            cout << "Invalid password for user: " << uid << endl;
            return false;
        }
        
        // Update status to online
        db.hsetValue("user:" + uid, "status", "online");
        cout << "User authenticated: " << uid << endl;
        return true;
    }
    
    // Add friend relationship
    bool addFriend(const string& uid1, const string& uid2) {
        // Add to both users' friend lists
        int result1 = db.saddvalue("user:" + uid1 + ":friends", uid2);
        int result2 = db.saddvalue("user:" + uid2 + ":friends", uid1);
        
        if (result1 == 1 && result2 == 1) {
            cout << "Friend relationship created: " << uid1 << " <-> " << uid2 << endl;
            return true;
        }
        
        return false;
    }
    
    // Get user's friends
    vector<string> getFriends(const string& uid) {
        vector<string> friends;
        redisReply** replies = db.smembers("user:" + uid + ":friends");
        
        if (replies) {
            // Process replies and extract friend UIDs
            // Note: Actual implementation would need to handle Redis reply parsing
        }
        
        return friends;
    }
    
private:
    string getCurrentTime() {
        // Implementation to get current timestamp
        return "2024-01-15 14:30:00";
    }
};

// Usage example
void userManagementExample() {
    UserManager userMgr;
    
    // Register users
    userMgr.registerUser("1001", "password123", "Alice");
    userMgr.registerUser("1002", "password456", "Bob");
    
    // Authenticate
    if (userMgr.authenticateUser("1001", "password123")) {
        cout << "Alice logged in successfully" << endl;
    }
    
    // Add friend relationship
    userMgr.addFriend("1001", "1002");
}
```

### Message Storage with Redis

```cpp
class MessageManager {
private:
    Redis db;
    
public:
    MessageManager() : db("127.0.0.1", 6379) {
        db.connect();
    }
    
    // Store private message
    void storePrivateMessage(const Message& msg) {
        string key = "messages:" + msg.SendUid + ":" + msg.RecvUid;
        string reverseKey = "messages:" + msg.RecvUid + ":" + msg.SendUid;
        
        string msgJson = msg.To_Json();
        
        // Store in both directions for easy retrieval
        db.lpush(key, msgJson);
        db.lpush(reverseKey, msgJson);
        
        // Limit message history (keep last 1000 messages)
        // Implementation would need to trim lists periodically
    }
    
    // Store group message
    void storeGroupMessage(const string& groupId, const Message& msg) {
        string key = "group:" + groupId + ":messages";
        string msgJson = msg.To_Json();
        
        db.lpush(key, msgJson);
    }
    
    // Retrieve message history
    vector<Message> getMessageHistory(const string& uid1, const string& uid2, int limit = 50) {
        vector<Message> messages;
        string key = "messages:" + uid1 + ":" + uid2;
        
        redisReply** replies = db.lrange(key, "0", to_string(limit-1));
        
        if (replies) {
            // Parse replies and convert back to Message objects
            // Implementation would handle Redis reply parsing
        }
        
        return messages;
    }
};
```

## Thread Pool Implementation

### Complete Server with Thread Pool

```cpp
#include "Server/TCPServer.hpp"
#include "Server/ThreadPool.hpp"
#include "Server/redis.hpp"
#include "lib/Command.hpp"
#include <iostream>
#include <signal.h>

// Global variables for cleanup
ThreadPool<TcpSocket>* g_threadPool = nullptr;
TcpServer* g_server = nullptr;

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    cout << "Shutting down server..." << endl;
    if (g_threadPool) {
        delete g_threadPool;
    }
    if (g_server) {
        delete g_server;
    }
    exit(0);
}

// Client data structure
struct ClientData {
    TcpSocket* socket;
    Redis* db;
    string clientId;
    
    ClientData(TcpSocket* s, Redis* d) : socket(s), db(d) {}
    ~ClientData() { delete socket; }
};

// Client handler function
void handleClientConnection(void* arg) {
    ClientData* clientData = static_cast<ClientData*>(arg);
    TcpSocket* client = clientData->socket;
    Redis* db = clientData->db;
    
    cout << "Handling client connection" << endl;
    
    try {
        while (true) {
            // Receive command from client
            string cmdStr = client->recvMsg();
            if (cmdStr.empty()) break;
            
            Command cmd;
            cmd.From_Json(cmdStr);
            
            // Process command based on type
            string response = processClientCommand(cmd, *db);
            
            // Send response back to client
            if (!response.empty()) {
                client->sendMsg(response);
            }
        }
    } catch (const exception& e) {
        cerr << "Error handling client: " << e.what() << endl;
    }
    
    cout << "Client disconnected" << endl;
    delete clientData;
}

string processClientCommand(const Command& cmd, Redis& db) {
    switch (cmd.m_flag) {
        case LOGHIN_CHECK:
            return handleLogin(cmd, db);
            
        case REGISTER_CHECK:
            return handleRegister(cmd, db);
            
        case ADDFRIEND:
            return handleAddFriend(cmd, db);
            
        case LISTFRIEND:
            return handleListFriends(cmd, db);
            
        default:
            return "Unknown command";
    }
}

string handleLogin(const Command& cmd, Redis& db) {
    if (cmd.m_option.size() < 2) {
        return "Invalid login parameters";
    }
    
    string uid = cmd.m_option[0];
    string password = cmd.m_option[1];
    
    string storedPassword = db.gethash("user:" + uid, "password");
    if (storedPassword == password) {
        db.hsetValue("user:" + uid, "status", "online");
        return "Login successful";
    }
    
    return "Login failed";
}

int main() {
    // Setup signal handler
    signal(SIGINT, signalHandler);
    
    // Create server
    g_server = new TcpServer();
    if (g_server->setListen(8080) != 0) {
        cerr << "Failed to setup server" << endl;
        return -1;
    }
    
    // Create thread pool
    g_threadPool = new ThreadPool<TcpSocket>(4, 16);
    
    // Create Redis connection
    Redis db("127.0.0.1", 6379);
    if (!db.connect()) {
        cerr << "Failed to connect to Redis" << endl;
        return -1;
    }
    
    cout << "Chatroom server started on port 8080" << endl;
    cout << "Thread pool: 4-16 threads" << endl;
    cout << "Press Ctrl+C to shutdown" << endl;
    
    // Main server loop
    while (true) {
        struct sockaddr_in clientAddr;
        TcpSocket* client = g_server->acceptConn(&clientAddr);
        
        if (client) {
            cout << "New client connected from " 
                 << inet_ntoa(clientAddr.sin_addr) << endl;
            
            // Create client data
            ClientData* clientData = new ClientData(client, &db);
            
            // Add to thread pool
            Task<TcpSocket> task(handleClientConnection, clientData);
            g_threadPool->addTask(task);
            
            cout << "Active threads: " << g_threadPool->getAliveNumber() 
                 << ", Busy threads: " << g_threadPool->getBusyNumber() << endl;
        }
    }
    
    return 0;
}
```

## Client Application Example

### Complete Client Implementation

```cpp
#include "lib/TCPSocket.hpp"
#include "Client/Display.hpp"
#include "Client/Input.hpp"
#include <pthread.h>
#include <iostream>

// Global variables
TcpSocket* g_socket = nullptr;
string g_currentUser;
bool g_running = true;

// Receive thread function
void* receiveMessages(void* arg) {
    TcpSocket* socket = static_cast<TcpSocket*>(arg);
    
    while (g_running) {
        try {
            string message = socket->recvMsg();
            if (!message.empty()) {
                cout << "\n[New Message] " << message << endl;
                cout << ">> ";  // Prompt
                fflush(stdout);
            }
        } catch (const exception& e) {
            if (g_running) {
                cerr << "Receive error: " << e.what() << endl;
            }
            break;
        }
    }
    
    return nullptr;
}

int main() {
    cout << "=== Chatroom Client ===" << endl;
    
    // Create socket and connect to server
    g_socket = new TcpSocket();
    if (g_socket->connectToHost("127.0.0.1", 8080) != 0) {
        cerr << "Failed to connect to server" << endl;
        delete g_socket;
        return -1;
    }
    
    cout << "Connected to chatroom server" << endl;
    
    // Authentication loop
    while (true) {
        display_login();
        string loginChoice = get_login();
        
        if (loginChoice == "quit") {
            break;
        } else if (loginChoice == "login") {
            string uid = Login(*g_socket);
            if (!uid.empty()) {
                g_currentUser = uid;
                cout << "Welcome back, " << uid << "!" << endl;
                break;
            }
        } else if (loginChoice == "register") {
            if (Register(*g_socket)) {
                cout << "Registration successful! Please login." << endl;
            }
        }
    }
    
    if (g_currentUser.empty()) {
        cout << "Goodbye!" << endl;
        delete g_socket;
        return 0;
    }
    
    // Start receive thread
    pthread_t receiveThread;
    pthread_create(&receiveThread, nullptr, receiveMessages, g_socket);
    pthread_detach(receiveThread);
    
    // Main command loop
    cout << "Type 'menu' to see available commands" << endl;
    
    while (g_running) {
        cout << ">> ";
        Command cmd = get_command(g_currentUser);
        
        if (cmd.m_flag == QUIT) {
            g_running = false;
            Quit(*g_socket);
            break;
        }
        
        // Send command to server
        string cmdJson = cmd.To_Json();
        if (g_socket->sendMsg(cmdJson) <= 0) {
            cerr << "Failed to send command" << endl;
            break;
        }
        
        // Handle special commands that need immediate processing
        if (cmd.m_flag == CHATFRIEND || cmd.m_flag == CHATGROUP) {
            // Enter chat mode - handle differently
            cout << "Entering chat mode. Type 'exit' to leave chat." << endl;
        }
    }
    
    cout << "Disconnecting..." << endl;
    delete g_socket;
    return 0;
}
```

## File Transfer Example

### Client-Side File Sending

```cpp
#include <fstream>
#include <sys/stat.h>

bool sendFile(TcpSocket& socket, const string& filename, const string& recipient) {
    // Check if file exists
    struct stat fileStat;
    if (stat(filename.c_str(), &fileStat) != 0) {
        cout << "File not found: " << filename << endl;
        return false;
    }
    
    // Create file transfer command
    vector<string> params = {recipient, filename, to_string(fileStat.st_size)};
    Command fileCmd(g_currentUser, SENDFILE, params);
    
    // Send file transfer initiation
    if (socket.sendMsg(fileCmd.To_Json()) <= 0) {
        cout << "Failed to initiate file transfer" << endl;
        return false;
    }
    
    // Wait for server approval
    string response = socket.recvMsg();
    if (response != "FILE_TRANSFER_APPROVED") {
        cout << "File transfer rejected: " << response << endl;
        return false;
    }
    
    // Send file content
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cout << "Failed to open file: " << filename << endl;
        return false;
    }
    
    char buffer[4096];
    size_t totalSent = 0;
    
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        size_t bytesToSend = file.gcount();
        
        // Send chunk
        if (socket.sendMsg(string(buffer, bytesToSend)) <= 0) {
            cout << "Failed to send file chunk" << endl;
            return false;
        }
        
        totalSent += bytesToSend;
        cout << "Sent: " << totalSent << "/" << fileStat.st_size << " bytes\r";
        cout.flush();
    }
    
    cout << "\nFile sent successfully: " << filename << endl;
    return true;
}
```

## Error Handling Patterns

### Robust Socket Operations

```cpp
class SafeSocket {
private:
    TcpSocket socket;
    bool connected;
    int retryCount;
    
public:
    SafeSocket() : connected(false), retryCount(0) {}
    
    bool connectWithRetry(const string& ip, int port, int maxRetries = 3) {
        for (int i = 0; i < maxRetries; i++) {
            if (socket.connectToHost(ip, port) == 0) {
                connected = true;
                retryCount = 0;
                return true;
            }
            
            cout << "Connection attempt " << (i+1) << " failed. ";
            if (i < maxRetries - 1) {
                cout << "Retrying in 2 seconds..." << endl;
                sleep(2);
            }
        }
        
        cout << "Failed to connect after " << maxRetries << " attempts" << endl;
        return false;
    }
    
    bool sendMessage(const string& message) {
        if (!connected) {
            cerr << "Socket not connected" << endl;
            return false;
        }
        
        try {
            int result = socket.sendMsg(message);
            if (result <= 0) {
                cerr << "Send failed, attempting reconnection..." << endl;
                connected = false;
                return false;
            }
            return true;
        } catch (const exception& e) {
            cerr << "Send exception: " << e.what() << endl;
            connected = false;
            return false;
        }
    }
    
    string receiveMessage() {
        if (!connected) {
            return "";
        }
        
        try {
            return socket.recvMsg();
        } catch (const exception& e) {
            cerr << "Receive exception: " << e.what() << endl;
            connected = false;
            return "";
        }
    }
    
    bool isConnected() const { return connected; }
};
```

## Performance Monitoring

### Thread Pool Statistics

```cpp
class ServerMonitor {
private:
    ThreadPool<TcpSocket>* threadPool;
    Redis* database;
    
public:
    ServerMonitor(ThreadPool<TcpSocket>* pool, Redis* db) 
        : threadPool(pool), database(db) {}
    
    void printStatistics() {
        cout << "\n=== Server Statistics ===" << endl;
        cout << "Active Threads: " << threadPool->getAliveNumber() << endl;
        cout << "Busy Threads: " << threadPool->getBusyNumber() << endl;
        cout << "Idle Threads: " << (threadPool->getAliveNumber() - threadPool->getBusyNumber()) << endl;
        
        // Database statistics (if Redis supports it)
        cout << "Database Status: " << (database ? "Connected" : "Disconnected") << endl;
        
        cout << "=========================" << endl;
    }
    
    void startMonitoring() {
        while (true) {
            printStatistics();
            sleep(10);  // Print stats every 10 seconds
        }
    }
};

// Usage in main server
void startServerWithMonitoring() {
    ThreadPool<TcpSocket> pool(4, 16);
    Redis db("127.0.0.1", 6379);
    db.connect();
    
    ServerMonitor monitor(&pool, &db);
    
    // Start monitoring in separate thread
    thread monitorThread([&monitor]() {
        monitor.startMonitoring();
    });
    
    // Continue with server main loop...
}
```

This comprehensive set of examples demonstrates:

1. **Basic socket communication** - Client and server setup
2. **Message handling** - Creating, sending, and parsing messages
3. **Database operations** - User management and message storage with Redis
4. **Thread pool usage** - Multi-threaded server implementation
5. **Complete applications** - Full client and server examples
6. **File transfers** - Binary data transmission
7. **Error handling** - Robust network communication
8. **Performance monitoring** - Server statistics and monitoring

These examples provide a solid foundation for understanding and extending the chatroom application functionality.