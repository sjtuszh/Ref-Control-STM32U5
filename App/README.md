# App 模块说明

## 1. 目录定位

`App` 目录用于放置应用层业务代码。

这一层的目标不是直接操作芯片启动流程，也不是直接管理 HAL/ThreadX 内核细节，而是专注回答两个问题：

1. 系统要做哪些业务
2. 每个业务在线程里具体做什么

当前工程已经按“业务职责”拆成多个模块，整体方向是对的，适合后续继续扩展为多线程应用架构。

---

## 2. 当前目录结构

```text
App/
├─ Inc/
│  ├─ app_comm.hpp
│  ├─ app_data.hpp
│  ├─ app_display.hpp
│  ├─ app_led.hpp
│  ├─ app_sensor.hpp
│  ├─ app_threads.hpp
│  └─ common_inc.h
└─ Src/
   ├─ app_comm.cpp
   ├─ app_data.cpp
   ├─ app_display.cpp
   ├─ app_led.cpp
   ├─ app_sensor.cpp
   ├─ app_threads.cpp
   └─ main.cpp
```

说明：

- `Inc/` 放各模块对外暴露的头文件
- `Src/` 放各模块具体实现
- `app_threads.*` 负责组织“线程里做什么”
- `app_display.*` 负责 OLED 显示业务
- `app_led.*` 负责 LED 指示灯业务
- `app_data.*` 预留为共享数据与线程通信相关模块
- `app_sensor.*` 预留为传感器采集与处理模块
- `app_comm.*` 预留为外部通信模块

---

## 3. 为什么线程创建不放在 App 里

当前工程基于 STM32CubeMX + ThreadX。

系统启动路径是：

1. `Core/Src/main.c`
2. 外设初始化完成
3. 调用 `MX_ThreadX_Init()`
4. 进入 `tx_kernel_enter()`
5. 在 `Core/Src/app_threadx.c` 的 `App_ThreadX_Init()` 中创建线程对象

因此建议的职责划分是：

- `Core/Src/main.c`
  负责芯片启动、时钟配置、HAL 外设初始化

- `Core/Src/app_threadx.c`
  负责 ThreadX 对象创建
  例如线程、队列、互斥锁、事件标志组、信号量

- `App/Src/*.cpp`
  负责每个线程具体执行业务逻辑

这样做的好处是：

1. 启动流程清晰
2. RTOS 对象集中管理
3. 业务代码和内核装配代码分层明确
4. 后续加线程时，不会把 `main.c` 和业务代码搅在一起

所以，`App` 层更适合写“线程函数内容”，不适合直接承担系统启动入口。

---

## 4. 当前建议的模块职责

### 4.1 `app_threads.*`

职责：

- 定义各业务线程入口函数
- 组织“某个线程循环里做什么”
- 不负责创建 ThreadX 线程对象

建议理解为：

“这里写线程的业务逻辑，不写线程的出生过程”

例如：

- `OledThreadMain()`
- `LedThreadMain()`
- 后续还可以继续加：
  - `SensorAcquireThreadMain()`
  - `SensorProcessThreadMain()`
  - `CommThreadMain()`

### 4.2 `app_display.*`

职责：

- OLED 初始化
- 页面绘制
- 字符串/状态/图形显示
- 后续菜单页、状态页、告警页等都建议放这里

当前已有内容：

- `AppDisplayInit()`
- `AppDisplayShowTestScreen()`

后续可扩展方向：

- `AppDisplayShowHomePage()`
- `AppDisplayShowSensorPage()`
- `AppDisplayShowAlarmPage()`

### 4.3 `app_led.*`

职责：

- LED 刷新逻辑
- 后续可扩展为状态灯状态机

当前已有内容：

- `AppLedUpdate()`

后续可扩展方向：

- 运行灯
- 故障灯
- 通信灯
- 充电/供电状态灯

### 4.4 `app_data.*`

职责建议：

- 定义全局共享业务数据结构
- 放线程间通信对象
- 提供统一的数据读写接口

这个模块后面会很重要，因为显示线程、通信线程、传感器处理线程都会依赖这里。

适合放的内容包括：

- 最新传感器处理结果结构体
- `TX_MUTEX`
- `TX_QUEUE`
- `TX_EVENT_FLAGS_GROUP`
- 统一的读写 API

### 4.5 `app_sensor.*`

职责建议：

- 传感器初始化
- 原始数据采集
- 数据滤波/标定/有效性检查
- 状态计算

建议后续拆成两个层次：

1. 采集线程
2. 处理线程

这样更清楚，也更利于后期增加滤波和异常判断。

### 4.6 `app_comm.*`

职责建议：

- 串口/USB/RS485/CAN 等外部通信
- 接收外部命令
- 上报系统状态和传感器数据

建议后续尽量避免在通信线程里直接访问太多底层业务模块，而是通过共享数据区或消息机制获取信息。

---

## 5. 推荐的线程结构

如果后续继续扩展，建议参考下面这个结构：

### 5.1 显示线程

`OledThreadMain()`

职责：

- 初始化 OLED
- 周期刷新显示页面
- 只负责显示，不直接采集传感器

### 5.2 LED 线程

`LedThreadMain()`

职责：

- 更新 LED 状态
- 按系统状态控制闪烁模式

### 5.3 传感器采集线程

`SensorAcquireThreadMain()`

职责：

- 周期读取传感器原始值
- 将原始数据发送到队列

### 5.4 传感器处理线程

`SensorProcessThreadMain()`

职责：

- 从原始数据队列取数据
- 做滤波、校准、异常判断
- 更新共享处理结果

### 5.5 通信线程

`CommThreadMain()`

职责：

- 处理外部通信协议
- 上报共享处理结果
- 接收控制命令

---

## 6. 推荐的数据流设计

建议把数据流设计成两段：

### 6.1 采集到处理：使用队列

```text
SensorAcquireThread
    -> sensor_raw_queue
    -> SensorProcessThread
```

原因：

- 原始采样天然是“一个一个产生”的消息流
- 用队列可以解耦采样速度和处理速度

### 6.2 处理到显示/通信：使用共享最新值

```text
SensorProcessThread
    -> latest_sensor_data + mutex
    -> DisplayThread
    -> CommThread
```

原因：

- 显示线程和通信线程更关心“最新状态”
- 不一定需要处理所有历史采样值
- 用共享结构体读取更直接

---

## 7. 推荐的线程通信方式

### 7.1 `TX_QUEUE`

适合：

- 线程之间传递固定大小消息
- 例如原始采样值、命令包、状态包

推荐用途：

- 传感器采集线程 -> 传感器处理线程
- 通信接收线程 -> 控制处理线程

### 7.2 `TX_MUTEX`

适合：

- 保护共享结构体
- 确保多个线程不会同时修改同一份数据

推荐用途：

- 保护最新传感器处理结果
- 保护共享配置参数

### 7.3 `TX_EVENT_FLAGS_GROUP`

适合：

- 事件通知
- 通知某个线程某个状态发生变化

推荐用途：

- 新数据到达
- 通信收到命令
- 告警状态发生变化

### 7.4 `TX_SEMAPHORE`

适合：

- 同步资源
- 等待某个中断/DMA/外设动作完成

推荐用途：

- DMA 发送完成同步
- ADC/I2C 异步操作完成同步

---

## 8. 推荐的共享数据结构示例

后续可以在 `app_data.*` 里维护类似这样的结构：

```cpp
typedef struct
{
    float level_mm;
    float temperature;
    uint32_t sample_tick;
    uint8_t sensor_ok;
    uint8_t alarm_high;
    uint8_t alarm_low;
} AppSensorData_t;
```

然后配套：

- 一个 `TX_MUTEX`
- 一组读接口
- 一组写接口

例如：

- `AppDataInit()`
- `AppDataUpdateSensorData(...)`
- `AppDataGetSensorData(...)`

这样显示线程和通信线程都不需要知道采样和处理细节，只拿最终结果即可。

---

## 9. 推荐的优先级思路

通常建议如下：

1. 传感器采集线程：较高
2. 通信线程：中高
3. 传感器处理线程：中
4. 显示线程：中低
5. LED 线程：低

原因：

- 采样和通信更偏实时
- 显示允许稍慢一点
- LED 最不敏感

具体优先级值需要根据后续实际负载再微调。

---

## 10. 后续建议

当前 `App` 目录已经有了一个不错的骨架，建议下一步按下面顺序推进：

1. 先完善 `app_data.*`
   先把共享数据区、mutex、queue 结构搭起来

2. 再完善 `app_sensor.*`
   先做采集，再做处理

3. 然后让 `app_display.*` 从共享数据区读数据
   不再只显示测试页面

4. 最后接入 `app_comm.*`
   做上报和命令接收

---

## 11. 一句话总结

当前这套 `App` 结构适合作为“业务层”长期维护：

- `Core` 负责系统启动和 RTOS 对象创建
- `App` 负责业务模块和线程逻辑
- `app_data` 作为线程通信与共享数据中心
- `app_sensor / app_display / app_led / app_comm` 按职责分层

如果后面继续按照这个方向扩展，代码会比把所有逻辑堆在 `main.c` 或单个 `Main.cpp` 里清晰很多。
