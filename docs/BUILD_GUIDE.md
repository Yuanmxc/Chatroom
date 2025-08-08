# Build and Deployment Guide

## Overview

This guide provides step-by-step instructions for building, configuring, and deploying the chatroom application on various platforms.

## Prerequisites

### System Requirements

#### Minimum Requirements
- **OS**: Linux (Ubuntu 18.04+, CentOS 7+, Arch Linux)
- **CPU**: 1 core, 1 GHz
- **RAM**: 512 MB
- **Storage**: 100 MB free space
- **Network**: TCP port access (default: 8080)

#### Recommended Requirements
- **OS**: Linux (Ubuntu 20.04+, CentOS 8+)
- **CPU**: 2+ cores, 2+ GHz
- **RAM**: 2+ GB
- **Storage**: 1+ GB free space
- **Network**: Dedicated server with static IP

### Software Dependencies

#### Core Build Tools
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake git

# CentOS/RHEL/Fedora
sudo yum install gcc gcc-c++ cmake git
# or for newer versions
sudo dnf install gcc gcc-c++ cmake git

# Arch Linux
sudo pacman -S base-devel cmake git
```

#### C++ Compiler
- **GCC**: 7.0+ (supports C++11)
- **Clang**: 6.0+ (alternative to GCC)

```bash
# Check compiler version
gcc --version
g++ --version
```

#### CMake
- **Version**: 3.12 or higher

```bash
# Check CMake version
cmake --version

# If version is too old, install newer version
wget https://github.com/Kitware/CMake/releases/download/v3.25.1/cmake-3.25.1-linux-x86_64.sh
chmod +x cmake-3.25.1-linux-x86_64.sh
sudo ./cmake-3.25.1-linux-x86_64.sh --prefix=/usr/local --skip-license
```

### Library Dependencies

#### nlohmann-json Library
JSON library for C++ serialization.

```bash
# Ubuntu/Debian
sudo apt install nlohmann-json3-dev

# CentOS/RHEL (EPEL repository required)
sudo yum install epel-release
sudo yum install json-devel

# Fedora
sudo dnf install json-devel

# Arch Linux
sudo pacman -S nlohmann-json

# Manual installation (if package not available)
git clone https://github.com/nlohmann/json.git
cd json
mkdir build && cd build
cmake ..
make -j4
sudo make install
```

#### Redis and hiredis Library
Database backend and C client library.

```bash
# Ubuntu/Debian
sudo apt install redis-server libhiredis-dev

# CentOS/RHEL
sudo yum install redis hiredis-devel

# Fedora
sudo dnf install redis hiredis-devel

# Arch Linux
sudo pacman -S redis hiredis
```

#### POSIX Threads (pthread)
Usually included with system, but verify:

```bash
# Check if pthread is available
echo '#include <pthread.h>' | gcc -E - > /dev/null && echo "pthread available" || echo "pthread missing"
```

## Build Instructions

### 1. Clone Repository

```bash
git clone <repository-url>
cd chatroom
```

### 2. Verify Dependencies

```bash
# Check all required packages
./scripts/check_dependencies.sh  # If available
# or manually check each dependency
```

Create a simple dependency check script:

```bash
#!/bin/bash
# save as scripts/check_dependencies.sh

echo "=== Dependency Check ==="

# Check GCC
if command -v gcc &> /dev/null; then
    echo "✓ GCC: $(gcc --version | head -n1)"
else
    echo "✗ GCC not found"
fi

# Check CMake
if command -v cmake &> /dev/null; then
    CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
    echo "✓ CMake: $CMAKE_VERSION"
else
    echo "✗ CMake not found"
fi

# Check nlohmann-json
if pkg-config --exists nlohmann_json; then
    echo "✓ nlohmann-json: $(pkg-config --modversion nlohmann_json)"
else
    echo "✗ nlohmann-json not found"
fi

# Check hiredis
if pkg-config --exists hiredis; then
    echo "✓ hiredis: $(pkg-config --modversion hiredis)"
else
    echo "✗ hiredis not found"
fi

# Check Redis server
if command -v redis-server &> /dev/null; then
    echo "✓ Redis server: $(redis-server --version | head -n1)"
else
    echo "✗ Redis server not found"
fi

echo "=== End Check ==="
```

### 3. Configure Build

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# For debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# For release build
cmake -DCMAKE_BUILD_TYPE=Release ..

# With custom installation prefix
cmake -DCMAKE_INSTALL_PREFIX=/opt/chatroom ..
```

#### CMake Configuration Options

```bash
# Enable verbose output
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..

# Specify compiler
cmake -DCMAKE_CXX_COMPILER=g++ ..
cmake -DCMAKE_CXX_COMPILER=clang++ ..

# Custom library paths
cmake -DCMAKE_PREFIX_PATH=/usr/local ..
```

### 4. Build Project

```bash
# Build with make
make

# Parallel build (faster)
make -j$(nproc)

# Build specific targets
make server
make client

# Verbose build output
make VERBOSE=1
```

#### Alternative Build Systems

```bash
# Using Ninja (faster build)
cmake -G Ninja ..
ninja

# Using specific generator
cmake -G "Unix Makefiles" ..
```

### 5. Verify Build

```bash
# Check if executables were created
ls -la server client

# Test basic functionality
./server --help    # If help option implemented
./client --help    # If help option implemented

# Check library dependencies
ldd server
ldd client
```

## Configuration

### Redis Configuration

#### 1. Start Redis Service

```bash
# SystemD (Ubuntu 18.04+, CentOS 7+)
sudo systemctl start redis
sudo systemctl enable redis  # Start on boot

# Traditional init (older systems)
sudo service redis-server start

# Manual start
redis-server /etc/redis/redis.conf
```

#### 2. Configure Redis

Edit Redis configuration file:

```bash
# Find config file location
redis-cli config get dir
sudo find /etc -name "redis.conf" 2>/dev/null

# Edit configuration
sudo vim /etc/redis/redis.conf
```

Key configuration options:

```conf
# /etc/redis/redis.conf

# Bind to all interfaces (be careful in production)
bind 0.0.0.0

# Set password (recommended)
requirepass your_secure_password

# Enable persistence
save 900 1
save 300 10
save 60 10000

# Set memory limit
maxmemory 256mb
maxmemory-policy allkeys-lru

# Log level
loglevel notice
logfile /var/log/redis/redis-server.log
```

#### 3. Test Redis Connection

```bash
# Test local connection
redis-cli ping
# Expected: PONG

# Test with authentication
redis-cli -a your_password ping

# Test from application
redis-cli
127.0.0.1:6379> SET test "Hello Redis"
127.0.0.1:6379> GET test
127.0.0.1:6379> DEL test
127.0.0.1:6379> QUIT
```

### Server Configuration

#### 1. Configuration File

Create server configuration file:

```bash
# Create config directory
sudo mkdir -p /etc/chatroom

# Create configuration file
sudo vim /etc/chatroom/server.conf
```

Example configuration:

```ini
# /etc/chatroom/server.conf

[server]
port=8080
bind_address=0.0.0.0
max_connections=1000

[database]
redis_host=127.0.0.1
redis_port=6379
redis_password=your_password
redis_database=0

[threading]
min_threads=4
max_threads=32
thread_timeout=300

[logging]
log_level=info
log_file=/var/log/chatroom/server.log
enable_debug=false

[security]
max_login_attempts=3
session_timeout=3600
enable_rate_limiting=true
```

#### 2. Create Log Directory

```bash
sudo mkdir -p /var/log/chatroom
sudo chown $USER:$USER /var/log/chatroom
```

### Network Configuration

#### 1. Firewall Configuration

```bash
# UFW (Ubuntu)
sudo ufw allow 8080/tcp
sudo ufw reload

# iptables (general Linux)
sudo iptables -A INPUT -p tcp --dport 8080 -j ACCEPT
sudo iptables-save > /etc/iptables/rules.v4

# firewalld (CentOS/RHEL)
sudo firewall-cmd --permanent --add-port=8080/tcp
sudo firewall-cmd --reload
```

#### 2. SELinux Configuration (CentOS/RHEL)

```bash
# Check SELinux status
sestatus

# Allow network connections
sudo setsebool -P httpd_can_network_connect 1

# Create custom SELinux policy if needed
sudo semanage port -a -t http_port_t -p tcp 8080
```

## Deployment

### Development Deployment

#### 1. Local Testing

```bash
# Terminal 1: Start Redis
redis-server

# Terminal 2: Start server
cd build
./server

# Terminal 3: Start client
./client
```

#### 2. Development Scripts

Create development helper scripts:

```bash
# scripts/dev-setup.sh
#!/bin/bash
echo "Starting development environment..."

# Start Redis in background
redis-server --daemonize yes

# Wait for Redis to start
sleep 2

# Start server
echo "Starting chatroom server..."
cd build
./server &
SERVER_PID=$!

echo "Server started with PID: $SERVER_PID"
echo "Connect with: ./client"
echo "Stop with: kill $SERVER_PID"
```

### Production Deployment

#### 1. System Service Configuration

Create systemd service files:

```bash
# /etc/systemd/system/chatroom-server.service
[Unit]
Description=Chatroom Server
After=network.target redis.service
Requires=redis.service

[Service]
Type=simple
User=chatroom
Group=chatroom
WorkingDirectory=/opt/chatroom
ExecStart=/opt/chatroom/bin/server
ExecReload=/bin/kill -HUP $MAINPID
Restart=on-failure
RestartSec=10

# Environment
Environment=REDIS_HOST=127.0.0.1
Environment=REDIS_PORT=6379

# Security
NoNewPrivileges=true
PrivateTmp=true
ProtectSystem=strict
ProtectHome=true
ReadWritePaths=/var/log/chatroom

[Install]
WantedBy=multi-user.target
```

#### 2. User and Directory Setup

```bash
# Create system user
sudo useradd -r -s /bin/false chatroom

# Create installation directory
sudo mkdir -p /opt/chatroom/{bin,config,logs}

# Copy binaries
sudo cp build/server /opt/chatroom/bin/
sudo cp build/client /opt/chatroom/bin/

# Set permissions
sudo chown -R chatroom:chatroom /opt/chatroom
sudo chmod +x /opt/chatroom/bin/*

# Create log directory
sudo mkdir -p /var/log/chatroom
sudo chown chatroom:chatroom /var/log/chatroom
```

#### 3. Service Management

```bash
# Install service
sudo systemctl daemon-reload
sudo systemctl enable chatroom-server

# Start service
sudo systemctl start chatroom-server

# Check status
sudo systemctl status chatroom-server

# View logs
sudo journalctl -u chatroom-server -f

# Stop service
sudo systemctl stop chatroom-server
```

#### 4. Reverse Proxy Configuration (Optional)

Using nginx as reverse proxy:

```nginx
# /etc/nginx/sites-available/chatroom
server {
    listen 80;
    server_name chatroom.example.com;

    location / {
        proxy_pass http://127.0.0.1:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        
        # WebSocket support (if implemented)
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";
    }
}
```

Enable the configuration:

```bash
sudo ln -s /etc/nginx/sites-available/chatroom /etc/nginx/sites-enabled/
sudo nginx -t
sudo systemctl restart nginx
```

### Docker Deployment

#### 1. Dockerfile

```dockerfile
# Dockerfile
FROM ubuntu:20.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    nlohmann-json3-dev \
    libhiredis-dev \
    redis-server \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Copy source code
COPY . .

# Build application
RUN mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc)

# Create runtime user
RUN useradd -r -s /bin/false chatroom

# Set permissions
RUN chown -R chatroom:chatroom /app

# Expose port
EXPOSE 8080

# Switch to non-root user
USER chatroom

# Start command
CMD ["./build/server"]
```

#### 2. Docker Compose

```yaml
# docker-compose.yml
version: '3.8'

services:
  redis:
    image: redis:7-alpine
    restart: unless-stopped
    volumes:
      - redis_data:/data
    ports:
      - "6379:6379"
    command: redis-server --appendonly yes

  chatroom-server:
    build: .
    restart: unless-stopped
    ports:
      - "8080:8080"
    depends_on:
      - redis
    environment:
      - REDIS_HOST=redis
      - REDIS_PORT=6379
    volumes:
      - ./logs:/app/logs

volumes:
  redis_data:
```

#### 3. Docker Commands

```bash
# Build and start
docker-compose up -d

# View logs
docker-compose logs -f chatroom-server

# Stop services
docker-compose down

# Rebuild after changes
docker-compose build --no-cache
docker-compose up -d
```

## Monitoring and Maintenance

### Log Management

#### 1. Log Rotation

```bash
# /etc/logrotate.d/chatroom
/var/log/chatroom/*.log {
    daily
    missingok
    rotate 30
    compress
    delaycompress
    notifempty
    create 644 chatroom chatroom
    postrotate
        systemctl reload chatroom-server
    endscript
}
```

#### 2. Log Analysis

```bash
# Monitor real-time logs
tail -f /var/log/chatroom/server.log

# Search for errors
grep -i error /var/log/chatroom/server.log

# Count connections per day
grep "client connected" /var/log/chatroom/server.log | \
    awk '{print $1}' | sort | uniq -c
```

### Performance Monitoring

#### 1. System Resources

```bash
# Monitor CPU and memory usage
top -p $(pgrep server)
htop -p $(pgrep server)

# Monitor network connections
ss -tulpn | grep :8080
netstat -tlpn | grep :8080

# Monitor Redis performance
redis-cli --latency-history
redis-cli info stats
```

#### 2. Application Metrics

```bash
# Check active connections
redis-cli keys "user:*:status" | wc -l

# Monitor message throughput
redis-cli monitor | grep -E "(lpush|lrange)"

# Database size
redis-cli info memory
```

### Backup and Recovery

#### 1. Redis Backup

```bash
# Manual backup
redis-cli BGSAVE

# Automated backup script
#!/bin/bash
# scripts/backup-redis.sh
BACKUP_DIR="/opt/backups/redis"
DATE=$(date +%Y%m%d_%H%M%S)

mkdir -p $BACKUP_DIR
redis-cli BGSAVE
cp /var/lib/redis/dump.rdb $BACKUP_DIR/dump_$DATE.rdb
gzip $BACKUP_DIR/dump_$DATE.rdb

# Keep only last 7 days
find $BACKUP_DIR -name "dump_*.rdb.gz" -mtime +7 -delete
```

#### 2. Configuration Backup

```bash
# Backup configuration files
tar -czf /opt/backups/chatroom-config-$(date +%Y%m%d).tar.gz \
    /etc/chatroom/ \
    /etc/systemd/system/chatroom-server.service
```

## Troubleshooting

### Common Build Issues

#### 1. Missing Dependencies

```bash
# Error: nlohmann/json.hpp not found
# Solution: Install nlohmann-json development package
sudo apt install nlohmann-json3-dev

# Error: hiredis.h not found
# Solution: Install hiredis development package  
sudo apt install libhiredis-dev

# Error: pthread library not found
# Solution: Install build-essential
sudo apt install build-essential
```

#### 2. CMake Issues

```bash
# Error: CMake version too old
# Check version
cmake --version

# Update CMake
sudo apt remove cmake
wget https://github.com/Kitware/CMake/releases/download/v3.25.1/cmake-3.25.1-linux-x86_64.sh
chmod +x cmake-3.25.1-linux-x86_64.sh
sudo ./cmake-3.25.1-linux-x86_64.sh --prefix=/usr/local --skip-license
```

#### 3. Linking Errors

```bash
# Error: undefined reference to pthread functions
# Solution: Ensure CMakeLists.txt includes pthread linking
# Add to CMakeLists.txt: target_link_libraries(server pthread)

# Error: undefined reference to hiredis functions
# Solution: Ensure hiredis is linked
# Add to CMakeLists.txt: target_link_libraries(server hiredis)
```

### Runtime Issues

#### 1. Connection Problems

```bash
# Cannot connect to server
# Check if server is running
ps aux | grep server

# Check if port is open
netstat -tlpn | grep 8080
ss -tlpn | grep 8080

# Check firewall
sudo ufw status
sudo iptables -L | grep 8080
```

#### 2. Redis Issues

```bash
# Cannot connect to Redis
# Check if Redis is running
systemctl status redis
ps aux | grep redis

# Test Redis connection
redis-cli ping

# Check Redis configuration
redis-cli config get bind
redis-cli config get requirepass
```

#### 3. Permission Issues

```bash
# Permission denied errors
# Check file permissions
ls -la /opt/chatroom/bin/server

# Check user permissions
sudo -u chatroom /opt/chatroom/bin/server

# Fix permissions
sudo chown chatroom:chatroom /opt/chatroom/bin/server
sudo chmod +x /opt/chatroom/bin/server
```

### Performance Issues

#### 1. High CPU Usage

```bash
# Profile application
perf record -g ./server
perf report

# Check thread usage
top -H -p $(pgrep server)

# Monitor system calls
strace -p $(pgrep server)
```

#### 2. Memory Leaks

```bash
# Use valgrind for memory debugging
valgrind --leak-check=full --show-leak-kinds=all ./server

# Monitor memory usage
watch 'ps aux | grep server'
```

#### 3. Network Issues

```bash
# Monitor network traffic
iftop -i eth0 -P
nethogs

# Check connection limits
ulimit -n
cat /proc/sys/fs/file-max

# Monitor socket states
ss -s
netstat -s
```

### Debug Mode

#### 1. Enable Debug Build

```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Run with debugger
gdb ./server
(gdb) run
(gdb) bt  # Show backtrace on crash
```

#### 2. Enable Verbose Logging

```bash
# If application supports log levels
./server --log-level=debug

# Or set environment variable
export CHATROOM_DEBUG=1
./server
```

This comprehensive guide covers all aspects of building, configuring, and deploying the chatroom application in various environments from development to production.