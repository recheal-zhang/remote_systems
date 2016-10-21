
# Remote monitoring and control software

标签： UDP/TCP, broadcast, epoll

---

[TOC]

---
## server

### 1. server_main_thread
the main thread to control the server application

### 2. broadcast_udp_heart_packet_thread
broadcast the server's heart packet to clients 

### 3. recv_tcp_heart_packet_from_client_via_epoll_thread
this thread use epoll in background 

### 4. wait_user_cmd_and_exec_thread

### 5. receive_back_content_and_show_module
 
### 6. file_get_module

### 7. exe_file_send_module


## client

### 1. client_main_thread

### 2. recv_udp_heart_packet_from_server_thread

### 3. send_tcp_heart_packet_to_server_thread

### 4. recv_tcp_cmd_and_exec_thread

### 5. cmd_content_send_back_module

### 6. gnome-screenshot_module

### 7. file_send_module

### 8. exe_file_get_and_chmod_module

