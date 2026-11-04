#include <iostream>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>            //Как раз для http запросов
#include <format>
#include <thread>               // Для задержки
#include <chrono>
#include "lib/StorageStructures.hpp"     // Структура для descriptor

using json = nlohmann::json;

// URL = https://ranobelib.me/ru/book/203736--seoul-object-story

// Инициилизация парсера
void parserInit(cpr::Session* session, const cpr::Cookies& cookies);


// /// @breif Функция для сохранения Cooki
// void saveCookies(const cpr::Cookies& cookies, const std::string& filename) {
//     json j = json::object();
// 
//     for(const auto& [name, value] : cookies) {
//         j[name] = value;
//     }
// 
//     std::ofstream file(filename);
//     if(!file.is_open()){
//         std::cerr << "Не удалось открыть файл для записи: " << filename << std::endl;
//         return;
//     }
// 
//     file << j.dump(2);
//     std::cout << "Save Cookie in file: " << filename << std::endl;
// }

void loadCookies() {
    return;
}


/// @brief отправка запроса и получение json
json get_json_from_url(cpr::Session* session, const std::string& url){
    session->SetUrl(url);
    cpr::Response resp = session->Get();
   
    if(resp.status_code == 200){
        return json::parse(resp.text);

    } else {
        std::cerr << "Pars error: " << resp.status_code << std::endl;
        return 1;
    }

}

/// @brief Получить контент главы в виде json
json get_chapter_content(cpr::Session* session, const std::string& comix_url_title, const int& volume, const int& chapter, const std::string& branch_id){
    std::string url = std::format("https://api.imglib.info/api/manga/{}/chapter?volume={}&number={}&branch_id={}", comix_url_title, volume, chapter, branch_id);
    return get_json_from_url(session, url);
}

/// std::this_thread::sleep_for(std::chrono::seconds(2)); //Сделать задержку на 2 секунды

// ==========

// 

struct CapterData {
    int volume;
    int chapter;
    int branch_id;
    std::string comix_url_title = "";
};

int main(){
    CapterData chaptData{1, 224, 22612, "203736--seoul-object-story"};
    LibComix comixData;

    cpr::Url url = {"http://192.168.0.18:8000"};

    cpr::Session session;
    cpr::Cookies cookies = {
        {"mangalib_session", ""},
        {"XSRF-TOKEN", ""}
    };

    parserInit(&session, cookies);

    // ===== Окончательная сборка парсера

    // std::cout << "Status: " << resp1.status_code << std::endl;
    // std::cout << json::parse(resp1.text).dump(2) << std::endl;

    return 0;
}


void parserInit(cpr::Session* session, const cpr::Cookies& cookies) {

    cpr::Header headers = {
        {"User-Agent", "Mozilla/5.0 (X11; Linux x86_64; rv:154.0) Gecko/20100101 Firefox/154.0"},
        {"Origin", "https://ranobelib.me"},
        {"Content-Type", "application/json"},
        {"Host", "api.imglib.info"},
        {"Accept-Language", "en-US,en;q=0.9"},
        {"Accept-Encoding", "gzip, deflate, br, zstd"}
    };

    session->SetHeader(headers);
    session->SetCookies(cookies);
}
