#include <iostream>
#include <loadbalancer.h>

using namespace std;

int main()
{
    LoadBalancer load_balancer;
    load_balancer.start();
    return 0;
}
