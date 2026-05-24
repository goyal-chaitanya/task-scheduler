#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Task {
    string id;
    int duration;
    int priority;
    vector<string> dependencies;
};

class TaskScheduler {
private:
    unordered_map<string, Task> tasks;
    unordered_map<string, vector<string>> graph;
    unordered_map<string, int> indegree;

    void dfsVisit(const string& taskId,
                  unordered_map<string, int>& state,
                  vector<string>& order) const {
        state[taskId] = 1;

        auto it = graph.find(taskId);
        if (it != graph.end()) {
            for (const string& next : it->second) {
                if (state[next] == 1) {
                    throw runtime_error("Cycle detected while running DFS topological sort.");
                }
                if (state[next] == 0) {
                    dfsVisit(next, state, order);
                }
            }
        }

        state[taskId] = 2;
        order.push_back(taskId);
    }

public:
    void addTask(const string& id, int duration, int priority, const vector<string>& dependencies) {
        if (tasks.count(id)) {
            throw runtime_error("Duplicate task id: " + id);
        }

        tasks[id] = Task{id, duration, priority, dependencies};
        graph[id];
        indegree[id];
    }

    void buildGraph() {
        for (auto& entry : graph) {
            entry.second.clear();
        }
        for (auto& entry : indegree) {
            entry.second = 0;
        }

        for (const auto& entry : tasks) {
            const Task& task = entry.second;
            for (const string& dependency : task.dependencies) {
                if (!tasks.count(dependency)) {
                    throw runtime_error("Task '" + task.id + "' depends on unknown task '" + dependency + "'.");
                }

                graph[dependency].push_back(task.id);
                indegree[task.id]++;
            }
        }

        for (auto& entry : graph) {
            sort(entry.second.begin(), entry.second.end());
        }
    }

    vector<string> topologicalSortKahn() const {
        unordered_map<string, int> currentIndegree = indegree;
        queue<string> ready;

        vector<string> taskIds;
        for (const auto& entry : tasks) {
            taskIds.push_back(entry.first);
        }
        sort(taskIds.begin(), taskIds.end());

        for (const string& id : taskIds) {
            if (currentIndegree[id] == 0) {
                ready.push(id);
            }
        }

        vector<string> order;
        while (!ready.empty()) {
            string current = ready.front();
            ready.pop();
            order.push_back(current);

            auto it = graph.find(current);
            if (it == graph.end()) {
                continue;
            }

            for (const string& next : it->second) {
                currentIndegree[next]--;
                if (currentIndegree[next] == 0) {
                    ready.push(next);
                }
            }
        }

        if (order.size() != tasks.size()) {
            throw runtime_error("Cycle detected: no valid task execution order exists.");
        }

        return order;
    }

    vector<string> topologicalSortDFS() const {
        unordered_map<string, int> state;
        vector<string> order;

        vector<string> taskIds;
        for (const auto& entry : tasks) {
            taskIds.push_back(entry.first);
            state[entry.first] = 0;
        }
        sort(taskIds.begin(), taskIds.end());

        for (const string& id : taskIds) {
            if (state[id] == 0) {
                dfsVisit(id, state, order);
            }
        }

        reverse(order.begin(), order.end());
        return order;
    }

    vector<string> prioritySchedule() const {
        struct Compare {
            const unordered_map<string, Task>* tasks;

            bool operator()(const string& left, const string& right) const {
                const Task& a = tasks->at(left);
                const Task& b = tasks->at(right);

                if (a.priority != b.priority) {
                    return a.priority < b.priority;
                }
                if (a.duration != b.duration) {
                    return a.duration > b.duration;
                }
                return a.id > b.id;
            }
        };

        unordered_map<string, int> currentIndegree = indegree;
        priority_queue<string, vector<string>, Compare> ready((Compare{&tasks}));

        for (const auto& entry : tasks) {
            if (currentIndegree[entry.first] == 0) {
                ready.push(entry.first);
            }
        }

        vector<string> order;
        while (!ready.empty()) {
            string current = ready.top();
            ready.pop();
            order.push_back(current);

            auto it = graph.find(current);
            if (it == graph.end()) {
                continue;
            }

            for (const string& next : it->second) {
                currentIndegree[next]--;
                if (currentIndegree[next] == 0) {
                    ready.push(next);
                }
            }
        }

        if (order.size() != tasks.size()) {
            throw runtime_error("Cycle detected: priority schedule cannot be computed.");
        }

        return order;
    }

    int totalDuration(const vector<string>& order) const {
        int total = 0;
        for (const string& id : order) {
            total += tasks.at(id).duration;
        }
        return total;
    }

    void printOrder(const string& title, const vector<string>& order) const {
        cout << title << '\n';

        for (size_t i = 0; i < order.size(); ++i) {
            const Task& task = tasks.at(order[i]);
            cout << i + 1 << ". " << task.id
                 << " | duration: " << task.duration
                 << " | priority: " << task.priority << '\n';
        }
        cout << "Total sequential duration: " << totalDuration(order) << " time units\n\n";
    }
};

static vector<string> splitDependencies(const string& text) {
    vector<string> result;
    if (text == "-" || text.empty()) {
        return result;
    }

    stringstream ss(text);
    string item;
    while (getline(ss, item, ',')) {
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    return result;
}

static void loadTasksFromFile(const string& fileName, TaskScheduler& scheduler) {
    ifstream input(fileName);
    if (!input) {
        throw runtime_error("Could not open input file: " + fileName);
    }

    string line;
    while (getline(input, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        stringstream ss(line);
        string id;
        int duration;
        int priority;
        string dependencies;

        if (!(ss >> id >> duration >> priority >> dependencies)) {
            throw runtime_error("Invalid input line: " + line);
        }

        scheduler.addTask(id, duration, priority, splitDependencies(dependencies));
    }
}

int main(int argc, char* argv[]) {
    string fileName = "tasks.txt";
    if (argc > 1) {
        fileName = argv[1];
    }

    try {
        TaskScheduler scheduler;
        loadTasksFromFile(fileName, scheduler);
        scheduler.buildGraph();

        vector<string> kahnOrder = scheduler.topologicalSortKahn();
        vector<string> dfsOrder = scheduler.topologicalSortDFS();
        vector<string> priorityOrder = scheduler.prioritySchedule();

        scheduler.printOrder("Valid execution order using Kahn's Algorithm", kahnOrder);
        scheduler.printOrder("Valid execution order using DFS Topological Sort", dfsOrder);
        scheduler.printOrder("Priority-based execution order using Max Heap", priorityOrder);
    } catch (const exception& error) {
        cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
