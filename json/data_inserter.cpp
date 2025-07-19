#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <libpq-fe.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

class DatabaseInserter {
private:
    PGconn* conn;
    map<string, int> ministry_map;
    map<string, int> office_map;
    
public:
    DatabaseInserter(const string& connection_string) {
        conn = PQconnectdb(connection_string.c_str());
        if (PQstatus(conn) != CONNECTION_OK) {
            cerr << "Connection to database failed: " << PQerrorMessage(conn) << endl;
            PQfinish(conn);
            exit(1);
        }
        cout << "Connected to database successfully!" << endl;
    }
    
    ~DatabaseInserter() {
        PQfinish(conn);
    }
    
    // Execute a query and return result
    PGresult* executeQuery(const string& query) {
        PGresult* res = PQexec(conn, query.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK && PQresultStatus(res) != PGRES_COMMAND_OK) {
            cerr << "Query execution failed: " << PQerrorMessage(conn) << endl;
            cerr << "Query: " << query << endl;
            PQclear(res);
            return nullptr;
        }
        return res;
    }
    
    // Escape string for SQL
    string escapeString(const string& str) {
        if (str.empty()) return "NULL";
        
        char* escaped = PQescapeLiteral(conn, str.c_str(), str.length());
        if (!escaped) {
            cerr << "Error escaping string: " << str << endl;
            return "NULL";
        }
        
        string result(escaped);
        PQfreemem(escaped);
        return result;
    }
    
    // Insert or get ministry ID
    int insertMinistry(const string& ministry_name) {
        if (ministry_map.find(ministry_name) != ministry_map.end()) {
            return ministry_map[ministry_name];
        }
        
        string query = "INSERT INTO ministries (ministry_name) VALUES (" + 
                      escapeString(ministry_name) + ") RETURNING ministry_id;";
        
        PGresult* res = executeQuery(query);
        if (!res) return -1;
        
        int ministry_id = atoi(PQgetvalue(res, 0, 0));
        ministry_map[ministry_name] = ministry_id;
        
        PQclear(res);
        cout << "Inserted ministry: " << ministry_name << " (ID: " << ministry_id << ")" << endl;
        return ministry_id;
    }
    
    // Insert or get office ID
    int insertOffice(const string& office_name, int ministry_id) {
        string office_key = office_name + "_" + to_string(ministry_id);
        if (office_map.find(office_key) != office_map.end()) {
            return office_map[office_key];
        }
        
        string query = "INSERT INTO offices (office_name, ministry_id) VALUES (" + 
                      escapeString(office_name) + ", " + to_string(ministry_id) + 
                      ") RETURNING office_id;";
        
        PGresult* res = executeQuery(query);
        if (!res) return -1;
        
        int office_id = atoi(PQgetvalue(res, 0, 0));
        office_map[office_key] = office_id;
        
        PQclear(res);
        cout << "Inserted office: " << office_name << " (ID: " << office_id << ")" << endl;
        return office_id;
    }
    
    // Insert service and return service ID
    int insertService(const json& service_data, int office_id) {
        string service_no = service_data.contains("ServiceNo") ? 
                           service_data["ServiceNo"].get<string>() : "";
        string service_name = service_data.contains("Service") ? 
                             service_data["Service"].get<string>() : "";
        string time_taken = service_data.contains("Time Taken") ? 
                           service_data["Time Taken"].get<string>() : "";
        
        // Handle charge field - can be string or array
        string charge = "";
        if (service_data.contains("Charge")) {
            if (service_data["Charge"].is_array()) {
                for (const auto& c : service_data["Charge"]) {
                    if (!charge.empty()) charge += "; ";
                    charge += c.get<string>();
                }
            } else {
                charge = service_data["Charge"].get<string>();
            }
        }
        
        string contact_section = service_data.contains("Contact and Service Delivery Section / Officer") ? 
                                service_data["Contact and Service Delivery Section / Officer"].get<string>() : 
                                (service_data.contains("Contact and Service Delivery Section / Officer ") ? 
                                 service_data["Contact and Service Delivery Section / Officer "].get<string>() : "");
        
        string grievance_section = service_data.contains("Grievance Handling Officer / Section") ? 
                                  service_data["Grievance Handling Officer / Section"].get<string>() : "";
        
        string monitoring_officer = service_data.contains("Monitoring Officer") ? 
                                   service_data["Monitoring Officer"].get<string>() : "";
        
        string responsible_officer = service_data.contains("Responsible Officer/Section") ? 
                                    service_data["Responsible Officer/Section"].get<string>() : "";
        
        string remarks = service_data.contains("Remarks") ? 
                        service_data["Remarks"].get<string>() : "";
        
        string query = "INSERT INTO services (office_id, service_no, service_name, time_taken, charge, "
                      "contact_section, grievance_section, monitoring_officer, responsible_officer, remarks) "
                      "VALUES (" + to_string(office_id) + ", " + 
                      (service_no.empty() ? "NULL" : service_no) + ", " +
                      escapeString(service_name) + ", " +
                      escapeString(time_taken) + ", " +
                      escapeString(charge) + ", " +
                      escapeString(contact_section) + ", " +
                      escapeString(grievance_section) + ", " +
                      escapeString(monitoring_officer) + ", " +
                      escapeString(responsible_officer) + ", " +
                      escapeString(remarks) + ") RETURNING service_id;";
        
        PGresult* res = executeQuery(query);
        if (!res) return -1;
        
        int service_id = atoi(PQgetvalue(res, 0, 0));
        PQclear(res);
        
        cout << "Inserted service: " << service_name << " (ID: " << service_id << ")" << endl;
        return service_id;
    }
    
    // Insert required documents
    void insertRequiredDocuments(const json& service_data, int service_id) {
        if (!service_data.contains("Required Documents")) return;
        
        const auto& docs = service_data["Required Documents"];
        if (!docs.is_array()) return;
        
        for (const auto& doc : docs) {
            string doc_text = doc.get<string>();
            if (doc_text == "—" || doc_text == "-" || doc_text.empty()) continue;
            
            string query = "INSERT INTO required_documents (service_id, document_text) VALUES (" + 
                          to_string(service_id) + ", " + escapeString(doc_text) + ");";
            
            PGresult* res = executeQuery(query);
            if (res) {
                PQclear(res);
                cout << "  - Inserted document: " << doc_text.substr(0, 50) << "..." << endl;
            }
        }
    }
    
    // Process a single JSON file
    void processJsonFile(const string& filename, const string& ministry_name, const string& office_name) {
        cout << "\n=== Processing " << filename << " ===" << endl;
        
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Could not open file: " << filename << endl;
            return;
        }
        
        json j;
        try {
            file >> j;
        } catch (const exception& e) {
            cerr << "Error parsing JSON: " << e.what() << endl;
            return;
        }
        
        // Insert ministry and office
        int ministry_id = insertMinistry(ministry_name);
        if (ministry_id == -1) return;
        
        int office_id = insertOffice(office_name, ministry_id);
        if (office_id == -1) return;
        
        // Process services
        if (j.contains("services") && j["services"].is_array()) {
            for (const auto& service : j["services"]) {
                int service_id = insertService(service, office_id);
                if (service_id != -1) {
                    insertRequiredDocuments(service, service_id);
                }
            }
        }
        
        cout << "Finished processing " << filename << endl;
    }
    
    // Main processing function
    void processAllFiles() {
        // Process each JSON file with appropriate ministry and office names
        processJsonFile("Ministry_Culture_Tourism_Civil Aviation_Citizen_Charter_Eng.json", 
                       "Ministry of Culture, Tourism and Civil Aviation", 
                       "National Museum");
        
        processJsonFile("Ministry_Foreign_Affair_Citizen_Charter_Eng.json", 
                       "Ministry of Foreign Affairs", 
                       "Passport Department");
        
        processJsonFile("Ministry_Home_Affairs_Immigration_Citizen_Charter_Eng.json", 
                       "Ministry of Home Affairs", 
                       "Department of Immigration");
    }
    
    // Display statistics
    void displayStats() {
        cout << "\n=== Database Statistics ===" << endl;
        
        PGresult* res = executeQuery("SELECT COUNT(*) FROM ministries;");
        if (res) {
            cout << "Total Ministries: " << PQgetvalue(res, 0, 0) << endl;
            PQclear(res);
        }
        
        res = executeQuery("SELECT COUNT(*) FROM offices;");
        if (res) {
            cout << "Total Offices: " << PQgetvalue(res, 0, 0) << endl;
            PQclear(res);
        }
        
        res = executeQuery("SELECT COUNT(*) FROM services;");
        if (res) {
            cout << "Total Services: " << PQgetvalue(res, 0, 0) << endl;
            PQclear(res);
        }
        
        res = executeQuery("SELECT COUNT(*) FROM required_documents;");
        if (res) {
            cout << "Total Required Documents: " << PQgetvalue(res, 0, 0) << endl;
            PQclear(res);
        }
    }
};

int main() {
    // Database connection string - modify as needed
    string connection_string = "host=localhost dbname=LGSF user=postgre password=";
    
    try {
        DatabaseInserter inserter(connection_string);
        
        cout << "Starting data insertion process..." << endl;
        inserter.processAllFiles();
        
        cout << "\nData insertion completed!" << endl;
        inserter.displayStats();
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}