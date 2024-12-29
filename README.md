# 🌐 Webserver

**Webserver** is a project from 42 school that involves creating a lightweight, custom HTTP web server in C++. It is designed to mimic the behavior of popular web servers like **NGINX**, handling configurations, routing, error pages, CGI execution, file uploads, and more.

---

## 🛠 Features

- **Custom HTTP Server**:
  - Handles multiple simultaneous client connections using **epoll** for scalability.
  - Provides support for HTTP methods like `GET`, `POST`, and `DELETE`.

- **Configuration Parsing**:
  - Reads and applies server settings from a user-defined configuration file.

- **CGI Handling**:
  - Supports running CGI scripts for dynamic content generation.

- **Autoindexing**:
  - Dynamically generates directory listings when `autoindex` is enabled.

- **File Uploads**:
  - Allows file uploads to a specified directory.

- **Error Handling**:
  - Custom error pages for HTTP status codes (403, 404, 405, etc.).

---

## 🚀 Getting Started
- Prerequisites
- C++98 or later.
- A Linux environment (tested with Ubuntu).
- Make and GCC/C++ installed.

## Build and Run
- Clone the repository:
    ```bash
    git clone https://github.com/yourusername/webserver.git
    cd webserver
- Build the server:
    ```bash
    make
- Run the server:
    ```bash
    ./webserver server.conf
## Configuration
The server is configured via a server.conf file. Here's an example:
    ```bash
    server {
    # Default server to handle unmatched requests
    server_name localhost:4000;
    host 127.0.0.1;
    port 4000;

    # Default root directory
    root /workspaces/webserver/www/default;

    # Error pages
    error_page 403 /workspaces/webserver/www/error_page/403.html;
    error_page 404 /workspaces/webserver/www/error_page/404.html;

    # Client request body size limit
    body_size 1024;

    # Default route
    location / {
        root /workspaces/webserver/www/default;
        methods GET;
        index index.html;
        autoindex on;
        }
    }

### Key Directives
- server_name: The domain or IP address for the server.
- port: The port the server listens on.
- root: Root directory for serving files.
- error_page: Path to custom error pages.
- body_size: Maximum size for client request bodies.
- location: Define routes and methods for specific paths.

## 🛠 Features Breakdown
- HTTP Methods
- GET: Retrieve files or resources.
- POST: Upload files.
- DELETE: Remove files or directories.
- Autoindex
- When enabled, generates a directory listing for accessible folders.
- CGI Execution
- Runs scripts located in the cgi-bin directory to generate dynamic responses.
- File Uploads
- Saves uploaded files to the upload directory.


## 📂 Project Structure

```plaintext
.
├── Makefile                       # Build and clean the project
├── README.md                      # Project documentation
├── include                        # Header files
│   ├── CgiHandler.hpp
│   ├── Config.hpp
│   ├── ConfigParser.hpp
│   ├── Connection.hpp
│   ├── Epoll.hpp
│   ├── FileSystem.hpp
│   ├── HttpRequest.hpp
│   ├── HttpResponse.hpp
│   ├── Logger.hpp
│   ├── Server.hpp
│   ├── SignalHandler.hpp
│   └── Utility.hpp
├── server.conf                    # Default server configuration
├── src                            # Source files
│   ├── Log
│   │   └── Logger.cpp             # Logging system implementation
│   ├── config
│   │   └── ConfigParser.cpp       # Parsing configuration files
│   ├── core
│   │   ├── Connection.cpp         # Client connection management
│   │   ├── Epoll.cpp              # Epoll-based event handling
│   │   └── Server.cpp             # Server logic
│   ├── http
│   │   ├── CgiHandler.cpp         # CGI execution handler
│   │   ├── HttpRequest.cpp        # HTTP request processing
│   │   └── HttpResponse.cpp       # HTTP response generation
│   ├── main.cpp                   # Entry point of the server
│   └── utils
│       ├── FileSystem.cpp         # File and directory utilities
│       ├── SignalHandler.cpp      # Graceful shutdown handling
│       └── Utility.cpp            # Helper functions
└── www                            # Server's public files and directories
    ├── default
    │   ├── autoindex              # Autoindex testing files
    │   ├── cgi-bin                # CGI scripts
    │   ├── index.html             # Default index page
    │   ├── noindex                # Example directory without autoindex
    │   ├── singlefile             # Single file directory
    │   └── upload                 # File upload directory
    └── error_page                 # Custom error pages (403, 404, etc.)
