#include<iostream>
#include<map>
#include<vector>
#include<chrono>
#include<ctime>
#include<sstream>
#include<iomanip>
#include<limits>
#include<algorithm>
#include<fstream>

enum class Priority{
    LOW=0,
    MEDIUM=1,
    HIGH=2, 
    URGENT=3
};

enum class itemstatus{
    Completed,
    Not_Completed
};

std::chrono::system_clock::time_point parse_date(const std::string &date_str) {
    std::tm t = {};
    std::istringstream ss(date_str);
    ss >> std::get_time(&t, "%Y-%m-%d"); 
    if (ss.fail()) {
        std::cerr << "Error parsing date string. Using current time." << std::endl;
        return std::chrono::system_clock::now();
    }
    return std::chrono::system_clock::from_time_t(std::mktime(&t));
}

int validate(const std::string& prompt, int low, int high) {
    int input;
    while (true) {
        std::cout << prompt;
        std::cin >> input;
        if (std::cin.fail() || input < low || input > high) {
            std::cout << "Invalid input. Please enter a number from " << low << " to " << high << ".\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return input;
        }
    }
}

// Helper to get string input
std::string get_string_input(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, input);
    return input;
}


class to_do_item{
public:
    std::string name;
    Priority priority;
    itemstatus status;
    std::string category;
    std::chrono::system_clock::time_point due_date;

    to_do_item(std::string name, Priority priority, std::string category,
               std::chrono::system_clock::time_point due_date = std::chrono::system_clock::now())
        : name(name), priority(priority), status(itemstatus::Not_Completed),
          category(category), due_date(due_date) {}

    to_do_item() : priority(Priority::LOW), due_date(std::chrono::system_clock::now()){}
};


std::string convert(Priority p) {
    switch (p) {
        case Priority::LOW: return "LOW";
        case Priority::MEDIUM: return "MEDIUM";
        case Priority::HIGH: return "HIGH";
        case Priority::URGENT: return "URGENT";
        default: return "UNKNOWN";
    }
}


class to_do_list{
private:
    std::map<std::string,to_do_item> to_do_items;

public:
    void new_entry(const std::string& item_name, Priority p, std::string category, std::chrono::system_clock::time_point due_date) {
        if(to_do_items.count(item_name) > 0) {
            std::cout << "The item is already in the list.\n";
            return;
        }
        to_do_items[item_name] = to_do_item(item_name, p, category, due_date);
        std::cout << "'" << item_name << "' has been added.\n";
    }

    void check_entry(const std::string& keyword) const {
        std::cout << "Searching for items with keyword: '" << keyword << "'\n";
        std::cout << "--------------------------------\n";
        bool found = false;
        for(const auto& pair : to_do_items) {
            const auto& item_name = pair.first;
            const auto& list_items = pair.second;
            if (item_name.find(keyword) != std::string::npos ||
                list_items.category.find(keyword) != std::string::npos) {
                std::cout << "Item: " << item_name << std::endl;
                std::cout << "  Status: " << (list_items.status == itemstatus::Completed ? "Completed" : "Not Completed") << std::endl;
                std::cout << "  Category: " << list_items.category << std::endl;
                std::cout << "  Priority: " << convert(list_items.priority) << std::endl;
                std::cout << "--------------------------------\n";
                found = true;
            }
        }
        if (!found) {
            std::cout << "No items found matching the keyword.\n";
        }
    }
    
    void display() const {
        if (to_do_items.empty()) {
            std::cout << "There is no list to display." << std::endl;
            return;
        }
        
        std::cout << "How would you like to Display your list?" << std::endl;
        std::cout << "1. Unsorted (default)\n";
        std::cout << "2. Sorted by Due Date\n";
        std::cout << "3. Sorted by Priority\n";
        std::cout << "4. Sorted by Status\n";
        std::cout << "5. Sorted by Category\n";
        int choice = validate("Please click one option: ", 1, 5);
        
        std::vector<to_do_item> items;
        for (const auto& pair : to_do_items) {
            items.push_back(pair.second);
        }
        
        switch (choice) {
            case 2:
                std::sort(items.begin(), items.end(), [](const to_do_item& a, const to_do_item& b) {
                    return a.due_date < b.due_date;
                });
                break;
            case 3:
                std::sort(items.begin(), items.end(), [](const to_do_item& a, const to_do_item& b) {
                    return static_cast<int>(a.priority) > static_cast<int>(b.priority);
                });
                break;
            case 4:
                std::sort(items.begin(), items.end(), [](const to_do_item& a, const to_do_item& b) {
                    return a.status < b.status;
                });
                break;
            case 5:
                std::sort(items.begin(), items.end(), [](const to_do_item& a, const to_do_item& b) {
                    return a.category < b.category;
                });
                break;
            case 1:
            default:
                break;
        }

        std::cout << "To-do List:\n";
        std::cout << "--------------------------------\n";
        bool found_items = false;
        for (const auto& item : items) {
            std::cout << "Item: " << item.name << "\n";
            std::cout << "  Status: " << (item.status == itemstatus::Completed ? "Completed" : "Not Completed") << "\n";
            std::cout << "  Category: " << item.category << "\n";
            std::time_t due_time = std::chrono::system_clock::to_time_t(item.due_date);
            std::cout << "  Due: " << std::ctime(&due_time)<<std::endl;
            found_items = true;
        }
        if (!found_items) {
            std::cout << "No items to display in this list." << std::endl;
        }
        std::cout << "--------------------------------\n";
    }

    std::map<std::string, to_do_item>::iterator find_and_select_item(const std::string& keyword) {
        std::vector<std::map<std::string, to_do_item>::iterator> matches;
        for (auto it = to_do_items.begin(); it != to_do_items.end(); ++it) {
            if (it->first.find(keyword) != std::string::npos ||
                it->second.category.find(keyword) != std::string::npos) {
                matches.push_back(it);
            }
        }
        if (matches.empty()) {
            std::cout << "No items found matching the keyword.\n";
            return to_do_items.end();
        }
        if (matches.size() == 1) {
            std::cout << "One item found. Taking action on '" << matches[0]->first << "'.\n";
            return matches[0];
        }
        std::cout << "Multiple items found. Please choose one to take action on:\n";
        for (size_t i = 0; i < matches.size(); ++i) {
            std::cout << i + 1 << ". " << matches[i]->first << " (Category: " << matches[i]->second.category << ")\n";
        }
        int choice;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        if (std::cin.fail() || choice < 1 || choice > matches.size()) {
            std::cout << "Invalid choice. No action taken.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return to_do_items.end();
        }
        return matches[choice - 1];
    }

    void mark_complete(const std::string& keyword) {
        auto it = find_and_select_item(keyword);
        if (it != to_do_items.end()) {
            it->second.status = itemstatus::Completed;
            std::cout << it->first << " has been marked completed.\n";
        }
    }

    void remove(const std::string& keyword) {
        auto it = find_and_select_item(keyword);
        if (it != to_do_items.end()) {
            std::string item_name = it->first;
            to_do_items.erase(it);
            std::cout << item_name << " has been removed from the list.\n";
        }
    }
    
    void check_alerts() const {
        bool f = true;
        auto now = std::chrono::system_clock::now();
        auto tomorrow_alert_time = now + std::chrono::hours(24);
        for(const auto& pair : to_do_items) {
            const auto& item = pair.second;
            if(item.due_date > now && item.status == itemstatus::Not_Completed && item.due_date < tomorrow_alert_time) {
                std::cout << "Reminder: The task '" << item.name << "' is due soon!\n";
                f = false;
            }
        }
        if (f) {
            std::cout << "No upcoming deadlines in the next 24 hours.\n";
        }
        std::cout << "--------------------------\n";
    }

    void clear_list() {
        to_do_items.clear();
        std::cout << "Your list has been cleared.\n";
    }
    
    void summary() {
        int completed = 0;
        int not_completed = 0;
        int total = 0;
        std::map<Priority, int> priority_counts;

        for (const auto& pair : to_do_items) {
            const auto& item = pair.second;
            if (item.status == itemstatus::Completed) {
                completed++;
            } else {
                not_completed++;
            }
            priority_counts[item.priority]++;
        }
        total = completed + not_completed;
        if (total == 0) {
            std::cout << "The list is empty. No summary to display.\n";
            return;
        }
        std::cout << "--- List Statistics ---\n";
        std::cout << "Total items: " << total << std::endl;
        std::cout << "Completed: " << completed << " Percentage " << (static_cast<float>(completed) / total) * 100 << "%\n";
        std::cout << "Not Completed: " << not_completed << " Percentage " << (static_cast<float>(not_completed) / total) * 100 << "%\n";
        std::cout << "-----------------------\n";
        std::cout << "Priorities count\n";
        std::cout << "-----------------------\n";
        std::cout << "Low priority: " << priority_counts[Priority::LOW] << std::endl;
        std::cout << "Medium priority: " << priority_counts[Priority::MEDIUM] << std::endl;
        std::cout << "High priority: " << priority_counts[Priority::HIGH] << std::endl;
        std::cout << "Urgent priority: " << priority_counts[Priority::URGENT] << std::endl;
        std::cout << "-----------------------\n";
    }
    
    void add_new_item_from_user_input() {
        std::string name = get_string_input("Enter the new task name: ");
        int prio_int = validate("Enter the priority (0=LOW, 1=MEDIUM, 2=HIGH, 3=URGENT): ", 0, 3);
        std::string category = get_string_input("Enter the category: ");
        new_entry(name, static_cast<Priority>(prio_int), category, std::chrono::system_clock::now());
    }
    
    void edit_item(const std::string& item_name) {
        auto it = to_do_items.find(item_name);
        if (it == to_do_items.end()) {
            std::cout << "Seems like the item is not part of the list. Would you like to add it? (1 for Yes, 0 for No)" << std::endl;
            int choice = validate("Enter your choice: ", 0, 1);
            if (choice == 1) {
                add_new_item_from_user_input();
            }
            return;
        }
        std::cout << "Editing item: " << item_name << std::endl;
        to_do_item& item = it->second;
        std::cout << "What do you want to change?" << std::endl;
        std::cout << "1. Name" << std::endl;
        std::cout << "2. Priority" << std::endl;
        std::cout << "3. Category" << std::endl;
        std::cout << "4. Due Date" << std::endl;
        int edit_choice = validate("Enter your choice (1-4): ", 1, 4);

        switch (edit_choice) {
            case 1: {
                std::string new_name = get_string_input("Enter new name: ");
                if (to_do_items.count(new_name) > 0) {
                    std::cout << "This name already exists. Cannot change." << std::endl;
                    return;
                }
                to_do_item temp_item = item;
                to_do_items.erase(item_name);
                temp_item.name = new_name;
                to_do_items[new_name] = temp_item;
                std::cout << "Name updated successfully." << std::endl;
                break;
            }
            case 2: {
                int new_prio_int = validate("Enter new priority (0=LOW, 1=MEDIUM, 2=HIGH, 3=URGENT): ", 0, 3);
                item.priority = static_cast<Priority>(new_prio_int);
                std::cout << "Priority updated successfully." << std::endl;
                break;
            }
            case 3: {
                std::string new_cat = get_string_input("Enter new category: ");
                item.category = new_cat;
                std::cout << "Category updated successfully." << std::endl;
                break;
            }
            case 4: {
                std::string date_str = get_string_input("Enter new due date (YYYY-MM-DD): ");
                item.due_date = parse_date(date_str);
                std::cout << "Due date updated successfully." << std::endl;
                break;
            }
        }
    }

    void save_to_file(const std::string& filename = "todo_data.txt") const {
        std::ofstream outfile(filename);
        if (!outfile.is_open()) {
            std::cerr << "Error: Could not open file for saving!\n";
            return;
        }
        for (const auto& pair : to_do_items) {
            const auto& item = pair.second;
            std::time_t due_time = std::chrono::system_clock::to_time_t(item.due_date);
            outfile << item.name << "|" << static_cast<int>(item.priority) << "|" << static_cast<int>(item.status) << "|" << item.category << "|" << due_time << "\n";
        }
        outfile.close();
        std::cout << "Data successfully saved to " << filename << ".\n";
    }

    void load_from_file(const std::string& filename = "todo_data.txt") {
        std::ifstream infile(filename);
        if (!infile.is_open()) {
            std::cout << "No saved data found. Starting a new list.\n";
            return;
        }
        std::string line;
        while (std::getline(infile, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string name_str, priority_str, status_str, category_str, due_date_str;
            if (!std::getline(ss, name_str, '|') ||
                !std::getline(ss, priority_str, '|') ||
                !std::getline(ss, status_str, '|') ||
                !std::getline(ss, category_str, '|') ||
                !std::getline(ss, due_date_str)) {
                std::cerr << "Skipping malformed line: " << line << "\n";
                continue;
            }
            int priority_int = std::stoi(priority_str);
            int status_int = std::stoi(status_str);
            std::time_t due_time_t = std::stoll(due_date_str);
            to_do_item loaded_item(name_str, static_cast<Priority>(priority_int), category_str, std::chrono::system_clock::from_time_t(due_time_t));
            loaded_item.status = static_cast<itemstatus>(status_int);
            to_do_items[name_str] = loaded_item;
        }
        infile.close();
        std::cout << "Data loaded successfully.\n";
    }
};

int main() {
    to_do_list my_list;
    my_list.load_from_file();
    my_list.check_alerts();
    int choice;
    do {
        std::cout << "\n--- To-Do List Menu ---\n";
        std::cout << "1. Add a new task\n";
        std::cout << "2. Display tasks\n";
        std::cout << "3. Mark task as complete\n";
        std::cout << "4. Edit a task\n";
        std::cout << "5. Remove a task\n";
        std::cout << "6. Search for a task\n";
        std::cout << "7. View summary\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter your choice: ";
        choice = validate("Enter your choice: ", 0, 7);
        switch (choice) {
            case 1:
                my_list.add_new_item_from_user_input();
                break;
            case 2:
                my_list.display();
                break;
            case 3:
                my_list.mark_complete(get_string_input("Enter item name or keyword to mark complete: "));
                break;
            case 4:
                my_list.edit_item(get_string_input("Enter item name or keyword to edit: "));
                break;
            case 5:
                my_list.remove(get_string_input("Enter item name or keyword to remove: "));
                break;
            case 6:
                my_list.check_entry(get_string_input("Enter keyword to search for: "));
                break;
            case 7:
                my_list.summary();
                break;
            case 0:
                my_list.save_to_file();
                std::cout << "Exiting. Goodbye!\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    } while (choice != 0);
    return 0;
}