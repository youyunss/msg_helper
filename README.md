安装
  ```bash
  cd build
  cmake ..
  make
  cd ..
  ```

运行
  ```bash
  cd bin
  ./msg_helper_client_example
  ./msg_helper_server_example
  ./test
  ```

目录
```bash
.
├── bin                                   #可执行文件
│   ├── msg_helper_client_example
│   ├── msg_helper_server_example
│   ├── test
│   └── tmp1
├── build                                 #编译
│   └── CMakeLists.txt
├── lib
│   ├── CMakeLists.txt
│   ├── msg_helper.cc
│   └── msg_helper.h
├── README.md
└── src                                   #源码
    ├── client
    │   └── msg_helper_client_example.cc
    ├── server
    │   └── msg_helper_server_example.cc
    └── test
        └── test.cc
```