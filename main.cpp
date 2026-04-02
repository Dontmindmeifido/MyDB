#include "src/Interface/Interface.h"

int main() {
    Crud* crud = Crud::getInstance();

    crud->Create::run("GUESTS", 
        std::vector<std::string> {"FIRST_NAME", "LAST_NAME", "PHONE", "TIER"}, 
        std::vector<std::string> {"VARCHAR", "VARCHAR", "NUMBER", "VARCHAR"});
        
    crud->Update::run("GUESTS", std::vector<std::string> {"James", "Holden", "5550192", "GOLD"});
    crud->Update::run("GUESTS", std::vector<std::string> {"Amos", "Burton", "5559981", "PLATINUM"});
    crud->Update::run("GUESTS", std::vector<std::string> {"Naomi", "Nagata", "5557762", "SILVER"});
    crud->Update::run("GUESTS", std::vector<std::string> {"Alex", "Kamal", "5553344", "BRONZE"});
    crud->Update::run("GUESTS", std::vector<std::string> {"Chrisjen", "Avasarala", "5550001", "PLATINUM"});
    crud->Update::run("GUESTS", std::vector<std::string> {"Bobbie", "Draper", "5558877", "GOLD"});
    crud->Update::run("GUESTS", std::vector<std::string> {"Josephus", "Miller", "5554433", "NONE"});
    crud->Update::run("GUESTS", std::vector<std::string> {"Julie", "Mao", "5552211", "SILVER"});

    crud->Create::run("ROOMS", 
        std::vector<std::string> {"ROOM_NUM", "TYPE", "RATE", "STATUS"}, 
        std::vector<std::string> {"NUMBER", "VARCHAR", "NUMBER", "VARCHAR"});
        
    crud->Update::run("ROOMS", std::vector<std::string> {"101", "STANDARD", "150", "CLEAN"});
    crud->Update::run("ROOMS", std::vector<std::string> {"102", "STANDARD", "150", "OCCUPIED"});
    crud->Update::run("ROOMS", std::vector<std::string> {"103", "DOUBLE", "200", "CLEAN"});
    crud->Update::run("ROOMS", std::vector<std::string> {"201", "DELUXE", "350", "MAINTENANCE"});
    crud->Update::run("ROOMS", std::vector<std::string> {"202", "DELUXE", "350", "OCCUPIED"});
    crud->Update::run("ROOMS", std::vector<std::string> {"301", "SUITE", "800", "CLEAN"});
    crud->Update::run("ROOMS", std::vector<std::string> {"302", "SUITE", "800", "CLEAN"});
    crud->Update::run("ROOMS", std::vector<std::string> {"999", "PENTHOUSE", "2500", "OCCUPIED"});

    crud->Create::run("RESERVATIONS", 
        std::vector<std::string> {"RES_ID", "ROOM_NUM", "LAST_NAME", "CHECK_IN", "NIGHTS"}, 
        std::vector<std::string> {"NUMBER", "NUMBER", "VARCHAR", "DATETIME", "NUMBER"});
        
    crud->Update::run("RESERVATIONS", std::vector<std::string> {"8001", "102", "Holden", "12-04-2026", "3"});
    crud->Update::run("RESERVATIONS", std::vector<std::string> {"8002", "202", "Avasarala", "14-04-2026", "5"});
    crud->Update::run("RESERVATIONS", std::vector<std::string> {"8003", "999", "Mao", "20-04-2026", "14"});
    crud->Update::run("RESERVATIONS", std::vector<std::string> {"8004", "103", "Kamal", "01-05-2026", "2"});
    crud->Update::run("RESERVATIONS", std::vector<std::string> {"8005", "301", "Nagata", "05-05-2026", "7"});
    crud->Update::run("RESERVATIONS", std::vector<std::string> {"8006", "101", "Burton", "10-05-2026", "1"});

    crud->Create::run("SERVICES", 
        std::vector<std::string> {"SERVICE_ID", "NAME", "PRICE", "DEPT"}, 
        std::vector<std::string> {"NUMBER", "VARCHAR", "NUMBER", "VARCHAR"});
        
    crud->Update::run("SERVICES", std::vector<std::string> {"1", "ROOM_SERVICE", "45", "FOOD"});
    crud->Update::run("SERVICES", std::vector<std::string> {"2", "MINIBAR", "25", "FOOD"});
    crud->Update::run("SERVICES", std::vector<std::string> {"3", "LAUNDRY", "15", "MAINTENANCE"});
    crud->Update::run("SERVICES", std::vector<std::string> {"4", "SPA_MASSAGE", "120", "WELLNESS"});
    crud->Update::run("SERVICES", std::vector<std::string> {"5", "VALET_PARKING", "30", "FRONT_DESK"});
    crud->Update::run("SERVICES", std::vector<std::string> {"6", "AIRPORT_TAXI", "60", "CONCIERGE"});

    crud->Create::run("BILLS", 
        std::vector<std::string> {"BILL_ID", "RES_ID", "AMOUNT", "PAID"}, 
        std::vector<std::string> {"NUMBER", "NUMBER", "NUMBER", "VARCHAR"});
        
    crud->Update::run("BILLS", std::vector<std::string> {"9901", "8001", "450", "YES"});
    crud->Update::run("BILLS", std::vector<std::string> {"9902", "8002", "1750", "NO"});
    crud->Update::run("BILLS", std::vector<std::string> {"9903", "8003", "35000", "NO"});
    crud->Update::run("BILLS", std::vector<std::string> {"9904", "8004", "400", "YES"});
    crud->Update::run("BILLS", std::vector<std::string> {"9905", "8005", "5600", "NO"});

    Interface* interface = Interface::getInstance();
    GLFWwindow* window = interface->initWindow();
    interface->Render(window);

    return 0;
}