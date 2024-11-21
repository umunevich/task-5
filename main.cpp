#include <iostream>
#include <thread>
#include <future>
#include <string>
#include <syncstream>
#include <chrono>

void slow(const std::string& name);
void quick(const std::string& name);
void work();
void thread1(std::promise<std::string>& promiseA2, std::shared_future<std::string> resultB, std::shared_future<std::string> resultD);
void thread2(std::promise<std::string>& promiseD);


int main()
{
    work();
}

void work() {

    std::promise<std::string> promiseA2;
    std::promise<std::string> promiseB;
    std::promise<std::string> promiseD;

    std::future<std::string> resultA2 = promiseA2.get_future();
    std::future<std::string> resultB = promiseB.get_future();
    std::future<std::string> resultD = promiseD.get_future();

    std::shared_future<std::string> shared_resultB = resultB.share();
    std::shared_future<std::string> shared_resultD = resultD.share();

    auto start = std::chrono::system_clock::now();

    std::thread t1(thread1, std::ref(promiseA2), shared_resultB, shared_resultD);
    std::thread t2(thread2, std::ref(promiseD));

    slow("A1");
    std::string A2 = resultA2.get();
    quick("B(A1," + A2 + ")");
    promiseB.set_value("B");
    quick("C1(B)");

    t1.join();
    t2.join();
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << "Time: " << duration.count() << " seconds" << std::endl;
    std::cout << "Work is done!" << std::endl;
}

void slow(const std::string& name) {
    std::osyncstream syncStream(std::cout);
    syncStream << name << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(7));
}

void quick(const std::string& name) {
    std::osyncstream syncStream(std::cout);
    syncStream << name << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void thread1(std::promise<std::string>& promiseA2, std::shared_future<std::string> resultB, std::shared_future<std::string> resultD) {
    slow("A2");
    promiseA2.set_value("A2");
    std::string B = resultB.get();
    std::string D = resultD.get();

    quick("C2(" + B + ", " + D + ")");
}

void thread2(std::promise<std::string>& promiseD) {
    slow("D");
    promiseD.set_value("D");
    quick("F");
}
