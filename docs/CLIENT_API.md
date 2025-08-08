# Client API Documentation

## Overview

The client-side components provide user interface, input handling, and communication with the server. The client architecture consists of display utilities, input processing, and core client functionality.

## Display Functions

### Description
Display functions provide a consistent user interface for the chatroom application with colored terminal output.

### Header File
```cpp
#include "Client/Display.hpp"
```

### Available Functions

#### `void display_login()`
Displays the main login screen with options for login, register, and quit.
- **Output**: Colored terminal menu with options
- **Options Shown**:
  - `login`: User authentication
  - `register`: New user registration  
  - `quit`: Exit application
- **Usage**: Called at application startup
- **Example Output**:
  ```
  ***************聊天室***************
  *------------登录(login)-----------*
  *------------注册(register)--------*
  *------------退出(quit)------------*
  **************chatroom**************
  ```

#### `void display_menu1()`
Displays the main user menu with all available commands.
- **Categories**:
  - **General Functions**: Messages, system notifications, chat
  - **Friend Management**: Add, list, delete, shield friends
  - **Group Management**: Create, join, manage groups
- **Commands Shown**:
  - `message`: View unread messages
  - `system`: View system messages
  - `notice`: View system notifications
  - `chat-`: Start private chat
  - `list-f`: List friends
  - `list-g`: List groups
  - `add-`: Add friend/group
  - `create`: Create group
  - `about`: Group information
  - `menu`: Show menu
  - `help`: Show help
  - `shield-`: Block conversations
  - `restore-`: Restore conversations
  - `delete-`: Delete friends
  - `agree-`: Accept friend requests
  - `refuse-`: Reject friend requests

#### `void display_menu2()`
Displays group-specific management menu.
- **Group Management Commands**:
  - `members`: View group members
  - `request`: View join requests
  - `pass-`: Approve join requests
  - `deny-`: Reject join requests
  - `set-x-x`: Set member permissions
  - `remove-`: Remove group members
  - `dissolve`: Dissolve group
  - `menu`: Return to main menu
  - `exit`: Exit group management

#### `void display_help()`
Displays help information (placeholder implementation).
- **Current**: Shows "暂时还没想好帮助内容" (Help content not yet implemented)
- **Future**: Should contain comprehensive command help

---

## Input Functions

### Description
Input functions handle user command processing, validation, and parsing.

### Header File
```cpp
#include "Client/Input.hpp"
```

### Command Constants
The following constants define command types for client-server communication:

```cpp
#define QUIT 0                // Exit application
#define LOGHIN_CHECK 1        // Login verification
#define REGISTER_CHECK 2      // Registration
#define ADDFRIEND 3          // Add friend request
#define AGREEADDFRIEND 4     // Accept friend request
#define LISTFRIEND 5         // List friends
#define CHATFRIEND 6         // Start private chat
#define FRIENDMSG 7          // Send friend message
#define EXITCHAT 8           // Exit chat
#define SHIELDFRIEND 9       // Block friend
#define DELETEFRIEND 10      // Delete friend
#define RESTOREFRIEND 11     // Restore friend
#define NEWMESSAGE 12        // New message notification
#define LOOKSYSTEM 13        // View system messages
#define REFUSEADDFRIEND 14   // Reject friend request
#define CREATEGROUP 15       // Create group
#define LISTGROUP 16         // List groups
#define ADDGROUP 17          // Join group
#define ADD 18               // General add command
#define LOOKNOTICE 19        // View notifications
#define ABOUTGROUP 20        // Group information
#define REQUSTLIST 21        // Request list
#define PASSAPPLY 22         // Approve application
#define DENYAPPLY 23         // Deny application
#define SETMEMBER 24         // Set member permissions
#define EXITGROUP 25         // Exit group
#define DISPLAYMEMBER 26     // Display members
#define REMOVEMEMBER 27      // Remove member
#define INFOXXXX 28          // Information command
#define CHATGROUP 29         // Group chat
#define GROUPMSG 30          // Group message
#define EXITGROUPCHAT 31     // Exit group chat
#define SENDFILE 32          // Send file
#define RECVFILE 33          // Receive file
#define SENDFILE_G 34        // Send file to group
#define RECVFILE_G 35        // Receive file from group
#define DISSOLVE 36          // Dissolve group
```

### Input Functions

#### `string get_login()`
Gets and validates login command from user.
- **Returns**: "login", "register", or "quit"
- **Validation**: Only accepts valid commands
- **Error Handling**: Prompts user until valid input received
- **EOF Handling**: Returns "EOF" if input stream closed
- **Example**:
  ```cpp
  string command = get_login();
  if (command == "login") {
      // Process login
  }
  ```

#### `string get_uid()`
Gets and validates user ID input.
- **Returns**: 4-digit numeric string
- **Validation**: Must be exactly 4 digits
- **Error Handling**: Prompts until valid format entered
- **Format**: "1234" (4 numeric characters)
- **Example**:
  ```cpp
  string uid = get_uid();  // Returns something like "1001"
  ```

#### `string get_pwd()`
Gets password input from user.
- **Returns**: Password string
- **Security**: Basic input (not hidden - could be enhanced)
- **Validation**: No specific format requirements
- **Example**:
  ```cpp
  string password = get_pwd();
  ```

#### `Command get_command(string my_uid)`
Parses user command input into Command object.
- **Parameters**:
  - `my_uid`: Current user's ID
- **Returns**: Command object with parsed command details
- **Parsing**: Converts text commands to command flags and parameters
- **Support**: Handles all client commands with parameters
- **Example**:
  ```cpp
  Command cmd = get_command("1001");
  // User types "add-f 1002" -> creates ADDFRIEND command
  ```

---

## Client Core Functions

### Description
Core client functions handle server communication and business logic.

### Header File
```cpp
#include "Client/client.hpp"
```

### Utility Functions

#### `void my_error(const char *errorMsg)`
Displays error message and exits application.
- **Parameters**: Error message string
- **Behavior**: Prints error and terminates program
- **Usage**: For fatal errors

#### `void *recvfunc(void *arg)`
Thread function for receiving messages from server.
- **Parameters**: Thread argument (socket information)
- **Returns**: void pointer (thread return)
- **Threading**: Runs in separate thread
- **Purpose**: Continuous message reception

### Authentication Functions

#### `string Login(TcpSocket cfd_class)`
Handles user login process.
- **Parameters**: TCP socket for server communication
- **Returns**: User ID string on success
- **Process**:
  1. Gets user credentials
  2. Sends login command to server
  3. Processes server response
  4. Returns user ID if successful
- **Example**:
  ```cpp
  string uid = Login(socket);
  if (!uid.empty()) {
      cout << "Logged in as: " << uid << endl;
  }
  ```

#### `bool Register(TcpSocket cfd_class)`
Handles user registration process.
- **Parameters**: TCP socket for server communication
- **Returns**: true on successful registration
- **Process**:
  1. Gets new user credentials
  2. Sends registration command
  3. Processes server response
- **Example**:
  ```cpp
  if (Register(socket)) {
      cout << "Registration successful!" << endl;
  }
  ```

#### `void Quit(TcpSocket cfd_class)`
Handles graceful application exit.
- **Parameters**: TCP socket to close
- **Process**: Sends quit command and closes connections

### Friend Management Functions

#### `bool AddFriend(TcpSocket cfd_class, Command command)`
Sends friend request to another user.
- **Parameters**: Socket and command with friend's UID
- **Returns**: true on successful request sent
- **Usage**: For adding new friends

#### `bool AgreeAddFriend(TcpSocket cfd_class, Command command)`
Accepts incoming friend request.
- **Parameters**: Socket and command with requester's UID
- **Returns**: true on successful acceptance

#### `bool RefuseAddFriend(TcpSocket cfd_class, Command command)`
Rejects incoming friend request.
- **Parameters**: Socket and command with requester's UID
- **Returns**: true on successful rejection

#### `bool ListFirHistory(TcpSocket cfd_class, Command command)`
Lists friend list and message history.
- **Parameters**: Socket and command
- **Returns**: true on successful listing
- **Display**: Shows friends and recent conversations

#### `bool DeleteFriend(TcpSocket cfd_class, Command command)`
Removes friend from friend list.
- **Parameters**: Socket and command with friend's UID
- **Returns**: true on successful deletion

#### `bool ShieldFriend(TcpSocket cfd_class, Command command)`
Blocks messages from specific friend.
- **Parameters**: Socket and command with friend's UID
- **Returns**: true on successful blocking

#### `bool Restorefriend(TcpSocket cfd_class, Command command)`
Unblocks previously blocked friend.
- **Parameters**: Socket and command with friend's UID
- **Returns**: true on successful restoration

### Messaging Functions

#### `bool ChatFriend(TcpSocket cfd_class, Command command)`
Initiates private chat with friend.
- **Parameters**: Socket and command with friend's UID
- **Returns**: true on successful chat initiation
- **Mode**: Enters interactive chat mode

#### `bool ExitChatFriend(TcpSocket cfd_class, Command command)`
Exits private chat mode.
- **Parameters**: Socket and command
- **Returns**: true on successful exit
- **Mode**: Returns to main menu

#### `bool NewMessage(TcpSocket cfd_class, Command command)`
Checks for new unread messages.
- **Parameters**: Socket and command
- **Returns**: true on successful check
- **Display**: Shows unread message count and senders

### Group Management Functions

#### `bool CreateGroup(TcpSocket cfd_class, Command command)`
Creates a new chat group.
- **Parameters**: Socket and command with group details
- **Returns**: true on successful creation
- **Permissions**: Creator becomes group owner

#### `bool ListGroup(TcpSocket cfd_class, Command command)`
Lists user's groups.
- **Parameters**: Socket and command
- **Returns**: true on successful listing
- **Display**: Shows group names and member counts

#### `bool AddGroup(TcpSocket cfd_class, Command command)`
Requests to join existing group.
- **Parameters**: Socket and command with group ID
- **Returns**: true on successful request sent

#### `bool AboutGroup(TcpSocket cfd_class, Command command)`
Shows detailed group information.
- **Parameters**: Socket and command with group ID
- **Returns**: true on successful information retrieval
- **Display**: Group details, members, permissions

#### `bool ChatGroup(TcpSocket cfd_class, Command command)`
Initiates group chat.
- **Parameters**: Socket and command with group ID
- **Returns**: true on successful chat initiation
- **Mode**: Enters group chat mode

#### `bool ExitChatGroup(TcpSocket cfd_class, Command command)`
Exits group chat mode.
- **Parameters**: Socket and command
- **Returns**: true on successful exit

#### `bool ExitGroup(TcpSocket cfd_class, Command command)`
Leaves a group.
- **Parameters**: Socket and command with group ID
- **Returns**: true on successful exit from group

#### `bool Dissolve(TcpSocket cfd_class, Command command)`
Dissolves group (owner only).
- **Parameters**: Socket and command with group ID
- **Returns**: true on successful dissolution
- **Permission**: Requires group ownership

### Group Administration Functions

#### `bool RequestList(TcpSocket cfd_class, Command command)`
Views pending group join requests.
- **Parameters**: Socket and command with group ID
- **Returns**: true on successful listing
- **Permission**: Requires admin privileges

#### `bool PassApply(TcpSocket cfd_class, Command command)`
Approves group join request.
- **Parameters**: Socket and command with applicant details
- **Returns**: true on successful approval
- **Permission**: Requires admin privileges

#### `bool DenyApply(TcpSocket cfd_class, Command command)`
Rejects group join request.
- **Parameters**: Socket and command with applicant details
- **Returns**: true on successful rejection
- **Permission**: Requires admin privileges

#### `bool SetMember(TcpSocket cfd_class, Command command)`
Sets member permissions in group.
- **Parameters**: Socket and command with member and permission details
- **Returns**: true on successful permission change
- **Permission**: Requires owner privileges

#### `bool DisplyMember(TcpSocket cfd_class, Command command)`
Displays group member list.
- **Parameters**: Socket and command with group ID
- **Returns**: true on successful display
- **Display**: Member names and roles

#### `bool RemoveMember(TcpSocket cfd_class, Command command)`
Removes member from group.
- **Parameters**: Socket and command with member details
- **Returns**: true on successful removal
- **Permission**: Requires admin privileges

### System Functions

#### `bool LookSystem(TcpSocket cfd_class, Command command)`
Views system messages.
- **Parameters**: Socket and command
- **Returns**: true on successful retrieval
- **Content**: Server notifications, maintenance messages

#### `bool LookNotice(TcpSocket cfd_class, Command command)`
Views system notices.
- **Parameters**: Socket and command  
- **Returns**: true on successful retrieval
- **Content**: Friend requests, group invitations

#### `bool InfoXXXX(TcpSocket cfd_class, Command command)`
Generic information retrieval function.
- **Parameters**: Socket and command with info type
- **Returns**: true on successful information retrieval
- **Usage**: For various information queries

## Threading Model

### Main Thread
- Handles user input and command processing
- Manages UI display and menu navigation
- Sends commands to server

### Receive Thread
- Continuously listens for server messages
- Processes incoming notifications
- Updates UI with new messages
- Handles server responses

## Error Handling

### Network Errors
- Connection failures handled gracefully
- Automatic reconnection attempts for critical operations
- User notification of connection issues

### Input Validation
- All user inputs validated before processing
- Clear error messages for invalid inputs
- Retry mechanisms for correctable errors

### Server Response Handling
- Timeout handling for server operations
- Error code processing and user feedback
- Graceful degradation for partial failures

## Best Practices

### User Interface
1. Use consistent color coding throughout application
2. Provide clear feedback for all user actions
3. Implement proper input validation
4. Handle edge cases gracefully

### Network Communication
1. Always check return values from network operations
2. Implement proper timeout handling
3. Use thread-safe operations for concurrent access
4. Handle connection drops and reconnections

### Memory Management
1. Properly manage socket resources
2. Clean up threads on application exit
3. Free allocated memory for messages and commands
4. Handle resource cleanup in error conditions