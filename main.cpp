#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

// Enumerations and Data Structures
enum class Status {
    TODO,
    ONGOING,
    DONE
};

struct Task {
    int priority;
    Status status;
    std::vector<std::string> assigned_employees;
};

struct Employee {
    std::vector<std::string> ongoing_tasks;
    int done_tasks = 0;
};

using TaskMap = std::map<std::string, Task>;
using EmployeeMap = std::map<std::string, Employee>;

struct ProjectData {
    TaskMap tasks;
    EmployeeMap employees;
};

namespace Project {
void add_task(ProjectData& data, const std::string& name, int priority) {
    data.tasks[name] = Task{priority, Status::TODO, {}};
}

void add_employee(ProjectData& data, const std::string& name) {
    data.employees[name] = Employee{};
}

void assign_employee(ProjectData& data, const std::string& task_name, const std::string& emp_name) {
    auto& task = data.tasks.at(task_name);
    if (task.status == Status::TODO)
        task.status = Status::ONGOING;
    task.assigned_employees.push_back(emp_name);
    data.employees[emp_name].ongoing_tasks.push_back(task_name);
}

void finish_task(ProjectData& data, const std::string& task_name) {
    auto& task = data.tasks.at(task_name);
    if (task.status != Status::ONGOING)
        return;
    task.status = Status::DONE;

    for (const auto& emp_name : task.assigned_employees) {
        auto& emp = data.employees.at(emp_name);
        auto& ongoing = emp.ongoing_tasks;
        auto new_end = std::remove(ongoing.begin(), ongoing.end(), task_name);
        int removed = std::distance(new_end, ongoing.end());
        emp.done_tasks += removed;
        ongoing.erase(new_end, ongoing.end());
    }
    task.assigned_employees.clear();
}

void report_all(ProjectData& data) {
    int todo = 0, ongoing = 0, done = 0;
    for (const auto& [name, task] : data.tasks) {
        switch (task.status) {
        case Status::TODO: ++todo; break;
        case Status::ONGOING: ++ongoing; break;
        case Status::DONE: ++done; break;
        }
    }
    std::cout << "TODO: " << todo << "\n"
              << "ONGOING: " << ongoing << "\n"
              << "DONE: " << done << "\n";
}

void report_ongoing(ProjectData& data) {
    std::vector<const TaskMap::value_type*> ongoing_tasks;
    for (const auto& pair : data.tasks) {
        if (pair.second.status == Status::ONGOING)
            ongoing_tasks.push_back(&pair);
    }
    std::sort(ongoing_tasks.begin(), ongoing_tasks.end(), [](auto a, auto b) {
        if (a->second.priority != b->second.priority)
            return a->second.priority < b->second.priority;
        return a->first < b->first;
    });
    for (const auto* task_ptr : ongoing_tasks) {
        const auto& task = task_ptr->second;
        std::set<std::string> sorted_employees(task.assigned_employees.begin(), task.assigned_employees.end());
        std::cout << task_ptr->first << " (" << task.priority << "): ";
        bool first = true;
        for (const auto& emp : sorted_employees) {
            if (!first)
                std::cout << ", ";
            first = false;
            std::cout << emp;
        }
        std::cout << "\n";
    }
}

void report_employee(ProjectData& data, const std::string& emp_name) {
    const auto& emp = data.employees.at(emp_name);
    std::cout << emp_name << " has done " << emp.done_tasks << " tasks.\n";
    if (emp.ongoing_tasks.empty()) {
        std::cout << emp_name << " is currently not working on any tasks.\n";
    }
    else {
        std::cout << emp_name << " is currently working on these tasks:\n";
        int index = 1;
        for (const auto& task : emp.ongoing_tasks) {
            std::cout << index++ << ". " << task << "\n";
        }
    }
}

} // namespace Project

namespace Command {
// Command string constants to reduce magic values.
inline const std::string ADD_TASK = "add_task";
inline const std::string ADD_EMPLOYEE = "add_employee";
inline const std::string ASSIGN_EMPLOYEE = "assign_employee";
inline const std::string FINISH_TASK = "finish_task";
inline const std::string REPORT = "report";
inline const std::string ALL = "all";
inline const std::string ONGOING = "ongoing";
inline const std::string EMPLOYEE = "employee";
} // namespace Command

// Utility to tokenize the input
std::vector<std::string> tokenize(const std::string& line) {
    std::istringstream iss(line);
    return {std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>()};
}

// Process a single command using our command constants and Project namespace functions.
void process_command(ProjectData& data, const std::vector<std::string>& tokens) {
    if (tokens.empty())
        return;

    // Mapping command strings to functions using our Command namespace constants.
    std::map<std::string, std::function<void(const std::vector<std::string>&)>> command_map = {
        {Command::ADD_TASK, [&](const std::vector<std::string>& args) {
             if (args.size() != 3) return;
             Project::add_task(data, args[1], std::stoi(args[2]));
         }},
        {Command::ADD_EMPLOYEE, [&](const std::vector<std::string>& args) {
             if (args.size() != 2) return;
             Project::add_employee(data, args[1]);
         }},
        {Command::ASSIGN_EMPLOYEE, [&](const std::vector<std::string>& args) {
             if (args.size() != 3) return;
             Project::assign_employee(data, args[1], args[2]);
         }},
        {Command::FINISH_TASK, [&](const std::vector<std::string>& args) {
             if (args.size() != 2) return;
             Project::finish_task(data, args[1]);
         }},
        {Command::REPORT, [&](const std::vector<std::string>& args) {
             if (args.size() < 2) return;
             if (args[1] == Command::ALL)
                 Project::report_all(data);
             else if (args[1] == Command::ONGOING)
                 Project::report_ongoing(data);
             else if (args[1] == Command::EMPLOYEE && args.size() == 3)
                 Project::report_employee(data, args[2]);
         }}};

    auto it = command_map.find(tokens[0]);
    if (it != command_map.end()) {
        it->second(tokens);
    }
}

int main() {
    ProjectData data;
    std::string line;
    while (std::getline(std::cin, line)) {
        auto tokens = tokenize(line);
        process_command(data, tokens);
    }
    return 0;
}
