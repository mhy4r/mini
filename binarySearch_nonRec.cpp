#include <iostream>
#include <vector>

int bs (std::vector<int>& list, int l, int r, int key)
{
    while (r - l > 1) {
        int mid = l + (r - l) >> 1;
        if (list[mid] > key)
            r = mid;
        else
            l = mid;
    }
    return list[l] == key ? l : -1;
}

int main()
{
    int key, num;
    std::vector<int> nums;
    std::cin >> key;
    while (std::cin >> num)
        nums.push_back(num);
        
    num = bs(nums, 0, nums.size() - 1, key);
    if (num == -1) std::cout << "not found" << std::endl;
    else
        std::cout << "index is " << num << std::endl;
}