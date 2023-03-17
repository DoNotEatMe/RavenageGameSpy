#include <iostream>
#include <string>
#include <curl/curl.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <ctime>
#include <Windows.h>
#include <chrono>
#include <ctime>
#include <rapidjson/document.h>
#include <filesystem>
#include <thread>


std::vector<std::string> split_string(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string line;
    while (std::getline(iss, line, delimiter)) {
        tokens.push_back(line);
    }
    return tokens;
}

static size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string* data)
{
    data->append(ptr, size * nmemb);
    return size * nmemb;
}


// unused
/*
void cURLingGS(std::string& readBuffer, std::string& spreadsheet_id) {

    // Set up the cURL library.
    CURL* curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize cURL" << std::endl;
        return;
    }

    // Set the cURL options.
    std::string url = "https://docs.google.com/spreadsheets/d/" + spreadsheet_id + "/export?format=csv&id=" + spreadsheet_id;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    // Perform the cURL request.
    CURLcode res = curl_easy_perform(curl);

    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);


    if (res != CURLE_OK)
    {
        std::cerr << "cURLingGS failed: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return;
    }
    else {

        if (response_code == 429) {



            int awake = 0;
            while (awake != 1) {

                int awake_in = 1;
                std::cout << "Error 429: Too many requests" << std::endl;
                std::cout << "Sleep for 30s" << std::endl;
                std::chrono::milliseconds duration(30000);
                std::this_thread::sleep_for(duration);
                awake++;
            }

            std::cout << "Oh sh*t, here we go again" << std::endl;

            cURLingGS(readBuffer, spreadsheet_id);

        }

    }

    // Clean up the cURL library.
    curl_easy_cleanup(curl);
}
*/

void cURLingAppid(std::string& readBuffer, std::string appid) {

    readBuffer.clear();

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize cURL" << std::endl;
        return;
    }

    std::string url = "https://store.steampowered.com/api/appdetails?appids=" + appid + "&cc=US";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    CURLcode res = curl_easy_perform(curl);

    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

    if (res != CURLE_OK)
    {
        std::cerr << "cURLingAppid failed: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return;
    }
    else {

        if (response_code == 429) {

            int awake = 0;
            while (awake != 1) {

                int awake_in = 1;
                std::cout << "\rError 429: Too many requests" << std::endl;
                std::cout << "Sleep for 1min" << std::endl;
                std::chrono::milliseconds duration(60000);
                std::this_thread::sleep_for(duration);
                
                awake++;
            }

            std::cout << "Oh sh*t, here we go again" << std::endl;

            cURLingAppid(readBuffer, appid);

        }

    }

    // Clean up the cURL library.
    curl_easy_cleanup(curl);
}

void cURLingReviews(std::string& readBuffer, std::string appid) {

    readBuffer.clear();

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize cURL" << std::endl;
        return;
    }

    std::string url = "https://store.steampowered.com/appreviews/" + appid + "?json=1&filter=recent&language=all&num_per_page=20";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cerr << "cURLingReviews failed: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return;
    }

    // Clean up the cURL library.
    curl_easy_cleanup(curl);
}

int main()
{
    auto start_time = std::chrono::steady_clock::now();

    std::cout << "Starting..." << std::endl;

    int count(0);
    
    std::string readBuffer;

    std::cout << "Getting appid..." << std::endl;
    std::ifstream input_file("appid.csv");
    if (!input_file.is_open()) {
        std::cerr << "Failed to open input file (appid.csv)" << std::endl;
        return 1;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(input_file, line)) {
        if (line.length() > 0) {
            lines.push_back(line);
        }
    }

    // Close the input file
    input_file.close();
    std::cout << "Succeed!" << std::endl;
    std::cout << "Got " << lines.size() << " appids" << std::endl;

    // Get the current time
    auto now = std::chrono::system_clock::now();

    // Convert the time to a time_t object
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    // Convert the time to a string in the format "YYYYMMDD"
    char buffer[80];
    struct tm timeinfo;
    localtime_s(&timeinfo, &time);
    std::strftime(buffer, 80, "%Y_%m_%d", &timeinfo);
    std::string date_str(buffer);
    
    std::filesystem::path subdir = "result";
    std::filesystem::create_directories(subdir);

    std::string filename = "data_" + date_str + ".csv";

    std::filesystem::path filepath = subdir / filename;

    std::ofstream out(filepath);
    std::cout << filename << " created" << std::endl;
    out << "title,appid,url,developers,publishers,priceUSD,platforms,metacritic score, metacritic link, total recommendations, positive recomendations, negative recommendations, review score, coming soon, release date";
    out << "\n";

    std::cout << "Ok, let's go..." << std::endl;

    // Process each line of data
    for (const auto& appid : lines) {

        std::string appid_clean = appid;
        appid_clean.erase(0, appid_clean.find_first_not_of(" \t\n\r\f\v"));
        appid_clean.erase(appid_clean.find_last_not_of(" \t\n\r\f\v") + 1);

        readBuffer.clear();
        
        cURLingAppid(readBuffer, appid_clean);
        
        rapidjson::Document document;
        document.Parse(readBuffer.c_str());

        if (!document.IsObject()) {
            std::cout << "\rdocument.IsObject() rapidjson Assert error " << appid_clean << std::endl;
            
            continue;
        }

        const rapidjson::Value& app = document[appid_clean.c_str()];
        const rapidjson::Value& success = app["success"];

        if (success.GetBool() == false) {
            std::cout << "\rsuccess.GetBool() == false " << appid_clean << std::endl;
            continue;
        }

        const rapidjson::Value& data = app["data"];

        std::string title = data["name"].GetString();
        std::string steam_appid = std::to_string(data["steam_appid"].GetInt());

        std::string developers;
        if (data.HasMember("developers")) {
            for (const auto& dev : data["developers"].GetArray()) {
                developers += dev.GetString();
                if (data["developers"].GetArray().Size() > 1) {
                    developers += "; ";
                }
            }
        }

        std::string publishers;
        if (data.HasMember("publishers")) {
            for (const auto& pub : data["publishers"].GetArray()) {
                publishers += pub.GetString();
                if (data["publishers"].GetArray().Size() > 1) {
                    publishers += "; ";
                }
            }
        }

        std::string price;
        if (data.HasMember("price_overview")) {
            if (data["price_overview"].HasMember("final_formatted")) {
                price = data["price_overview"]["final_formatted"].GetString();
            }
        }

        std::string platforms;
        if (data.HasMember("platforms")) {
            if (data["platforms"].HasMember("windows")) {
                if (data["platforms"]["windows"].GetBool() == true) {
                    platforms += "windows;";
                }
            }
            if (data["platforms"].HasMember("mac")) {
                if (data["platforms"]["mac"].GetBool() == true) {
                    platforms += "mac;";
                }
            }
            if (data["platforms"].HasMember("linux")) {
                if (data["platforms"]["linux"].GetBool() == true) {
                    platforms += "linux;";
                }
            }
        }

        std::string metacritic_score;
        std::string metacritic_url;
        if (data.HasMember("metacritic")) {
            if (data["metacritic"].HasMember("score")) {
                metacritic_score = data["metacritic"]["score"].GetInt();
            }
            if (data["metacritic"].HasMember("url")) {
                metacritic_url = data["metacritic"]["url"].GetString();
            }
        }

        std::string coming_soon;
        std::string release_date;
        // Format release date to human date
        if (data.HasMember("release_date")) {
            if (data["release_date"].HasMember("coming_soon")) {
                if (data["release_date"]["coming_soon"].GetBool() == true) {
                    coming_soon = "true";
                }
                if (data["release_date"]["coming_soon"].GetBool() == false) {
                    coming_soon = "false";
                }
            }
            /*
            if (data["release_date"].HasMember("date")) {
                if (data["release_date"]["date"].GetStringLength() != 0) {

                    std::string date_str = data["release_date"]["date"].GetString();

                    std::tm date = {};
                    std::istringstream ss(date_str);

                    // Parse the date string into a tm structure
                    if ((date_str.size() == 12 || date_str.size() == 11) && date_str != "Coming soon") {
                        ss >> std::get_time(&date, "%b %d, %Y");
                        // Check for errors
                        if (ss.fail()) {
                            std::cout << appid_clean << std::endl;
                            std::cout << "Error parsing date string: " << date_str << std::endl;
                            return 1;
                        }

                        // Format the date as a string in dd/mm/yyyy format
                        char date_formatted[11];
                        std::strftime(date_formatted, sizeof(date_formatted), "%d/%m/%Y", &date);

                        // Print the formatted date string
                        release_date = date_formatted;
                    }
                    else if (date_str.size() == 9) {
                        ss >> std::get_time(&date, "%b %Y");
                        // Check for errors
                        if (ss.fail()) {
                            std::cout << appid_clean << std::endl;
                            std::cout << "Error parsing date string: " << date_str << std::endl;
                            return 1;
                        }

                        // Format the date as a string in dd/mm/yyyy format
                        char date_formatted[11];
                        std::strftime(date_formatted, sizeof(date_formatted), "%d/%m/%Y", &date);

                        // Print the formatted date string
                        release_date = date_formatted;
                    }
                    else {
                        release_date = date_str;
                    }

                }
            }
            */

            if (data["release_date"].HasMember("date")) {
                if (data["release_date"]["date"].GetStringLength() != 0) {
                    std::string date_str = data["release_date"]["date"].GetString();
                    std::tm t = {};
                    std::istringstream ss(date_str);
                    char date_formatted[11]; // Declare the array here
                    if (ss >> std::get_time(&t, "%d %b, %Y")) {
                        std::strftime(date_formatted, sizeof(date_formatted), "%d/%m/%Y", &t);
                        release_date = date_formatted; // Set the formatted date string
                    }
                    else {
                        ss.clear(); // clear the fail bit
                        ss.seekg(0, ss.beg); // reset the input stream
                        if (ss >> std::get_time(&t, "%b %d, %Y")) {
                            std::strftime(date_formatted, sizeof(date_formatted), "%d/%m/%Y", &t);
                            release_date = date_formatted; // Set the formatted date string
                        }
                        else {
                            release_date = date_str;
                            
                        }
                    }
                }
            }
        }

        cURLingReviews(readBuffer, appid_clean);
        rapidjson::Document DocReview;
        DocReview.Parse(readBuffer.c_str());


        if (!DocReview.IsObject()) {
            std::cout << "\rdocument.IsObject() rapidjson Assert error " << appid_clean << std::endl;
            continue;
        }

        if (DocReview["success"].GetInt() == 0) {
            std::cout << "\rsuccess.GetBool() == false " << appid_clean << std::endl;
            continue;
        }

        rapidjson::Value& query = DocReview["query_summary"];
        std::string total_recs = "";
        std::string pos_recs = "";
        std::string neg_recs = "";
        std::string rec_score = "";

        if (DocReview.HasMember("query_summary")) {
            if (query.HasMember("total_reviews")) {
                total_recs = std::to_string(query["total_reviews"].GetInt());
            }
            if (query.HasMember("total_positive")) {
                pos_recs = std::to_string(query["total_positive"].GetInt());
            }
            if (query.HasMember("total_negative")) {
                neg_recs = std::to_string(query["total_negative"].GetInt());
            }
            if (query.HasMember("review_score_desc")) {
                rec_score = query["review_score_desc"].GetString();
            }
        }

        std::string steam_link = "https://store.steampowered.com/app/" + appid_clean;

        out << 
           '"' + title + '"' + "," +
            steam_appid + "," +
            steam_link + "," +
            '"'+ developers + '"' + "," +
            '"'+ publishers + '"'+ "," +
            price + "," +
            platforms + "," +
            metacritic_score + "," +
            metacritic_url + "," +
            total_recs + "," +
            pos_recs + "," +
            neg_recs + "," +
            rec_score + "," +
            coming_soon + "," +
            release_date + "\n";

        count++;
        
        std::cout << "\r";
        for (int i = 0; i < 50; i++) {
            std::cout << " ";  // Print spaces to overwrite previous output
        }
        std::cout << "\r";

        float progress = (static_cast<float>(count) / lines.size()) * 100.0;
        std::cout << "Progress: " << static_cast<int>(progress) << "%";
        std::cout.flush();
    }

    out.close();

    auto end_time = std::chrono::steady_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    double duration_s = static_cast<double>(duration_ms.count()) / 1000;
    
    std::cout << "\r";
    for (int i = 0; i < 50; i++) {
        std::cout << " ";  // Print spaces to overwrite previous output
    }
    std::cout << "\r";

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Program finished." << std::endl;
    std::cout << "Running time " << static_cast<int>(duration_s) << "s" << std::endl;
    std::cout << "Avg action time " << duration_s / count << "s" << std::endl;
    std::cout << "Inserted: " << count << "/" << lines.size() << " lines in " << filename << std::endl;
    std::cout << "Press any key to exit." << std::endl;
    std::cin.get(); 

    return 0;
}
