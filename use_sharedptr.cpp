#include <iostream>
#include <memory>

void function1(std::shared_ptr<int[]> arr) {
    // 在函数1中使用共享指针管理的数组
    for (int i = 0; i < 5; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

void function2(std::shared_ptr<int[]> arr) {
    // 在函数2中使用共享指针管理的数组
    for (int i = 0; i < 5; i++) {
        std::cout << arr[i] * 2 << " ";
    }
    std::cout << std::endl;
}

int main() {
    // 创建一个共享指针来管理动态分配的整型数组
    std::shared_ptr<int[]> arr(new int[5]);

    // 初始化数组
    for (int i = 0; i < 5; i++) {
        arr[i] = i + 1;
    }

    // 在函数1和函数2中共享使用数组
    function1(arr);
    function2(arr);

    // 当共享指针超出作用域时，会自动释放数组资源

    return 0;
}
