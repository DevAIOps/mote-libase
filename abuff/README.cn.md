这是一个 `flat` 方式的缓存，全称是 `a dynamic buffer`，可以动态伸缩，这里并没有限制是字符串缓存 (以 `\0` 结尾)，可以是任意二进制内容，不过也提供了方便的字符串格式化接口。

# 1. 结构体

代码实现对应了结构体 `struct abuff` ，其中指针的使用方式如下。

```
struct abuff {
    size_t min;
    size_t max;
    size_t real;

    char *head;
    char *data;
    char *tail;
    char *end;
};

  head          data                 tail          end
   |             |                    |             |
   V             V                    V             V
  |-+-+-~~~-+-+-|-+-+-~~~~~~~~~~-+-+-|-+-+-~~~-+-+-|-+-~~~
  |  head room  |  some useful info  |  tail room  |
  |-+-+-~~~-+-+-|-+-+-~~~~~~~~~~-+-+-|-+-+-~~~-+-+-|-+-~~~
```

# 2. API

## 2.1 创建、初始化、清理

使用时提供了两类使用常见，一类是申请 `struct abuff` 内存，一类是使用现有的 `struct abuff` 结构体，两者都会检查入参是否合法，然后申请 `min` 大小的内存，并设置好相应的指针。

在释放资源时也略有区别，两者都会释放缓存，但是前者会释放申请的对象，后者则会重置成员对象，如下会有详细的介绍。

**注意** 最小值必须要大于 0 ，也就是说必须要申请内存。

### 2.1.1 申请内存

通过 `abuff_create()` 函数会申请 `sturct abuff` 内存，然后初始化；清理时会将申请的内存时放掉。

```
struct abuff *abuff_create(size_t smin, size_t smax);
void abuff_destory(struct abuff *buf);
```

### 2.1.2 直接初始化

使用 `abuff_init` 设置并初始化 `struct abuff` 中的成员对象；清理时会释放缓冲区 `head` 内存，同时重置成员对象。

```
int abuff_init(struct abuff *buf, size_t smin, size_t smax);
void abuff_cleanup(struct abuff *buf);
```

### 2.1.3 重新初始化

主要用于重新调整内存的大小，不过需要注意的是 **必须保证内存数据不可用**。在调整时，内存内容可能会被覆盖，如果大小一致，不会调整内存大小，数据也就不会被修改；而如果大小不一致，会通过 `realloc` 调整内存，并复位相关的成员变量。

```
int abuff_reinit(struct abuff *buf, size_t smin, size_t smax);
```

注意，这里只判断大小是否调整，实际上可以再针对其它场景进行优化，以减少对 `realloc` 函数的调用。

## 2.2 大小调整

目前仅提供了指数方式，也就是按照 `2` 的倍数，所以，建议最大和最小值按照 `2` 的倍数设置，例如 `16` `128` `1024` 等等。

调整方式有两种：A) 当前空间不足但是还没有到最大值，那么就需要调整；B) 因为 `data` 指针向前移动，实际前部还有可用空间，只需要简单移动当前数据即可。

### 2.2.1 调整位置

也就是第一种，当开头有空闲空间时才会移动，如果此时没有数据，那么会直接执行 `abuff_restart()` 也就是简单复位。

```
void abuff_shift(struct abuff *buf);
```

### 2.2.2 动态伸缩

其中 `abuff_exponent_expand()` 只是简单的指数伸缩，另外一个是按照最小指数伸缩，例如当前大小为 `5` 期望是 `7`，那么实际上会扩展到 `8` ，也就是最近的一个 `2` 指数。

```
size_t abuff_exponent_expand(struct abuff *buf);
size_t abuff_expand(struct abuff *buf, size_t expect);
```

### 2.2.3 综合

也就是先尝试移动，如果空间仍不满足，那么就扩展。

``` c
// 适用于获取当前还有多少剩余空间
size_t abuff_remain(struct abuff *buf);
// 用于明确需要多少空间
size_t abuff_ensure_size(struct abuff *buf, size_t len);
```

其中 `abuff_remain()` 判断是否还有足够剩余空间，包括了开始位置的内存，如果有那么会调整后返回大小，否则扩展。

## 2.2 字符串操作

格式化使用的是 `glibc` 中的 `vsnprintf()` 作格式化，该函数会返回字符串长度，当长度不够的时候，会尽量填充，同时保证字符串以 `\0` 终止。

注意，上述长度并未包含 `\0` 结束符，这里的 `abuff_length()` 函数返回的长度也是不包含 `\0` 终止符。

```
int abuff_printf(struct abuff *buf, const char *fmt, ...);
int abuff_vprintf(struct abuff *buf, const char *fmt, va_list ap);
```
