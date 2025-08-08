# Chatroom Documentation

## Overview

Welcome to the comprehensive documentation for the Chatroom application - a multi-threaded C++ chat server and client system with Redis backend. This documentation suite provides everything you need to understand, build, deploy, and extend the chatroom application.

## Documentation Structure

### 📋 [Project Overview](PROJECT_OVERVIEW.md)
**Start here for a high-level understanding of the system**
- System architecture and components
- Key features and capabilities
- Technical specifications
- Communication protocols
- Threading model and data storage

### 🔧 [Build and Deployment Guide](BUILD_GUIDE.md)
**Complete instructions for setting up and deploying the application**
- Prerequisites and dependencies
- Step-by-step build instructions
- Configuration options
- Development and production deployment
- Docker deployment
- Monitoring and maintenance
- Comprehensive troubleshooting guide

### 📚 API Documentation

#### [Core Library API](LIBRARY_API.md)
**Essential components for network communication and data handling**
- **TcpSocket Class**: Network communication abstraction
- **Message Structure**: Chat message format and JSON serialization
- **Command Structure**: Client-server command protocol
- **Color Utilities**: Terminal formatting and display

#### [Server API](SERVER_API.md)
**Multi-threaded server components and database integration**
- **ThreadPool Class**: Concurrent task processing
- **TaskQueue Class**: Thread-safe task management
- **TcpServer Class**: TCP connection handling
- **Redis Class**: Database operations and data persistence

#### [Client API](CLIENT_API.md)
**User interface and client-side functionality**
- **Display Functions**: User interface and menus
- **Input Functions**: Command processing and validation
- **Client Core Functions**: Authentication, messaging, and group management

### 💡 [Usage Examples](USAGE_EXAMPLES.md)
**Practical code samples and implementation patterns**
- Basic socket communication examples
- Message and command handling
- Database operations with Redis
- Complete server and client implementations
- File transfer examples
- Error handling patterns
- Performance monitoring

### 📖 [API Reference](API_REFERENCE.md)
**Comprehensive function reference for quick lookup**
- Complete function signatures and parameters
- Return values and error codes
- Thread safety information
- Usage examples for each API
- Command constants reference

## Quick Start Guide

### For Developers New to the Project

1. **Start with [Project Overview](PROJECT_OVERVIEW.md)** to understand the architecture
2. **Follow [Build Guide](BUILD_GUIDE.md)** to set up your development environment
3. **Review [Usage Examples](USAGE_EXAMPLES.md)** to see practical implementations
4. **Use [API Reference](API_REFERENCE.md)** for detailed function documentation

### For System Administrators

1. **Read [Project Overview](PROJECT_OVERVIEW.md)** for system requirements
2. **Follow [Build Guide - Production Deployment](BUILD_GUIDE.md#production-deployment)** section
3. **Configure monitoring using [Build Guide - Monitoring](BUILD_GUIDE.md#monitoring-and-maintenance)**

### For API Users

1. **Check [API Reference](API_REFERENCE.md)** for function signatures
2. **See [Usage Examples](USAGE_EXAMPLES.md)** for implementation patterns
3. **Refer to specific API docs**: [Library](LIBRARY_API.md), [Server](SERVER_API.md), or [Client](CLIENT_API.md)

## Key Features Documented

### Core Functionality
- ✅ **Multi-threaded Server**: Concurrent client handling with thread pool
- ✅ **TCP Socket Communication**: Reliable network communication
- ✅ **Redis Integration**: Persistent data storage and retrieval
- ✅ **JSON Protocol**: Structured message and command format
- ✅ **User Authentication**: Secure login and registration system

### Messaging System
- ✅ **Private Messaging**: One-to-one communication
- ✅ **Group Chat**: Multi-user group messaging
- ✅ **Message History**: Persistent message storage
- ✅ **Real-time Notifications**: Instant message delivery
- ✅ **File Sharing**: Binary file transfer capabilities

### User Management
- ✅ **Friend System**: Add, remove, and manage friends
- ✅ **User Status**: Online/offline status tracking
- ✅ **Message Blocking**: Shield/restore conversations
- ✅ **Group Administration**: Create and manage chat groups

### Advanced Features
- ✅ **Colorized Terminal UI**: Enhanced user experience
- ✅ **Command System**: Comprehensive command processing
- ✅ **Error Handling**: Robust error management
- ✅ **Performance Monitoring**: Server statistics and monitoring
- ✅ **Docker Support**: Containerized deployment

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    Chatroom System                         │
├─────────────────────────────────────────────────────────────┤
│  Client Layer                                              │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐          │
│  │  Display    │ │   Input     │ │   Client    │          │
│  │  Functions  │ │  Processing │ │    Core     │          │
│  └─────────────┘ └─────────────┘ └─────────────┘          │
├─────────────────────────────────────────────────────────────┤
│  Network Layer                                             │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐          │
│  │ TcpSocket   │ │   Message   │ │   Command   │          │
│  │    Class    │ │  Structure  │ │  Structure  │          │
│  └─────────────┘ └─────────────┘ └─────────────┘          │
├─────────────────────────────────────────────────────────────┤
│  Server Layer                                              │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐          │
│  │ ThreadPool  │ │ TaskQueue   │ │ TcpServer   │          │
│  │    Class    │ │    Class    │ │    Class    │          │
│  └─────────────┘ └─────────────┘ └─────────────┘          │
├─────────────────────────────────────────────────────────────┤
│  Data Layer                                                │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐          │
│  │    Redis    │ │    User     │ │   Message   │          │
│  │   Database  │ │    Data     │ │   Storage   │          │
│  └─────────────┘ └─────────────┘ └─────────────┘          │
└─────────────────────────────────────────────────────────────┘
```

## Technology Stack

- **Language**: C++11
- **Build System**: CMake 3.12+
- **Networking**: POSIX Sockets
- **Threading**: POSIX Threads (pthread)
- **Database**: Redis with hiredis client
- **Serialization**: nlohmann/json
- **Platform**: Linux (Ubuntu, CentOS, Arch)

## Documentation Standards

### Code Examples
All code examples in this documentation are:
- ✅ **Tested and Verified**: Examples work with the actual codebase
- ✅ **Complete**: Include necessary headers and error handling
- ✅ **Commented**: Clear explanations of key concepts
- ✅ **Best Practices**: Follow recommended patterns and conventions

### API Documentation
API documentation includes:
- ✅ **Function Signatures**: Complete parameter and return types
- ✅ **Parameter Descriptions**: Purpose and constraints for each parameter
- ✅ **Return Values**: Possible return values and their meanings
- ✅ **Thread Safety**: Concurrency considerations
- ✅ **Usage Examples**: Practical implementation examples
- ✅ **Error Conditions**: Common error scenarios and handling

### Build Instructions
Build documentation provides:
- ✅ **Multi-Platform Support**: Instructions for major Linux distributions
- ✅ **Dependency Management**: Complete dependency installation guides
- ✅ **Configuration Options**: Customization and optimization settings
- ✅ **Troubleshooting**: Common issues and solutions
- ✅ **Production Ready**: Enterprise deployment considerations

## Contributing to Documentation

### Documentation Guidelines
- Keep examples practical and tested
- Include error handling in code samples
- Use consistent formatting and style
- Provide context for complex concepts
- Update documentation with code changes

### File Organization
```
docs/
├── README.md              # This file - documentation index
├── PROJECT_OVERVIEW.md    # System architecture and features
├── BUILD_GUIDE.md         # Build, deploy, and maintenance guide
├── LIBRARY_API.md         # Core library documentation
├── SERVER_API.md          # Server-side API documentation
├── CLIENT_API.md          # Client-side API documentation
├── USAGE_EXAMPLES.md      # Code samples and examples
└── API_REFERENCE.md       # Complete function reference
```

## Support and Resources

### Getting Help
1. **Check the documentation** - Most questions are answered here
2. **Review examples** - See practical implementations
3. **Check troubleshooting** - Common issues and solutions
4. **Examine the code** - Source code is well-commented

### Development Resources
- **Build System**: CMake for cross-platform builds
- **Dependencies**: All dependencies documented with installation instructions
- **Testing**: Examples include test patterns and debugging techniques
- **Deployment**: Production-ready deployment configurations

### Performance Considerations
- **Threading**: Optimal thread pool configuration guidance
- **Memory**: Memory management best practices
- **Network**: Connection pooling and optimization
- **Database**: Redis optimization and monitoring

## Version Information

This documentation is current as of the latest version of the chatroom application. Key features documented:

- **Server**: Multi-threaded with thread pool management
- **Client**: Full-featured with colorized terminal interface
- **Database**: Redis integration with comprehensive operations
- **Protocol**: JSON-based message and command system
- **Deployment**: Docker and systemd service support

---

**Note**: This documentation suite is designed to be comprehensive and self-contained. Each document can be read independently, but they work together to provide complete coverage of the chatroom system.

For the most up-to-date information, always refer to the source code and this documentation together.