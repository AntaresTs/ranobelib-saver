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
json get_chapter_content(cpr::Session* session, const std::string& id_url_title, const int& volume, const int& chapter, const std::string& branch_id){
    std::string url = std::format("https://api.imglib.info/api/manga/{}/chapter?volume={}&number={}&branch_id={}", id_url_title, volume, chapter, branch_id);
    return get_json_from_url(*session, url);
}

json get_title_info(cpr::Session* session, const std::string& id_url_title) {
    // 203736--seoul-object-story

    std::string url = std::format(
        "https://api.imglib.info/api/manga/{}?fields[0]=background&fields[1]=eng_name&fields[2]=otherNames&fields[3]=summary&fields[4]=releaseDate&fields[5]=country&fields[6]=type_id&fields[7]=caution&fields[8]=views&fields[9]=close_view&fields[10]=rate_avg&fields[11]=rate&fields[12]=genres&fields[13]=tags&fields[14]=teams&fields[15]=user&fields[16]=franchise&fields[17]=authors&fields[18]=publisher&fields[19]=userRating&fields[20]=moderated&fields[21]=metadata&fields[22]=metadata.count&fields[23]=metadata.close_comments&fields[24]=translation_quality_rating&fields[25]=manga_status_id&fields[26]=chap_count&fields[27]=status_id&fields[28]=extra_release_status&fields[29]=artists&fields[30]=format",
        id_url_title
    );
    return get_json_from_url(*session, url);
}

/// @brief распарсинг json
void processor(cpr::Session* session, const CapterData chapterData){
//    json json_chapter_content = get_chapter_content(&session, chapterData.id_url_title, chapterData.volume, chapterData.chapter, chapterData.branch_id);
    json json_title_info = get_title_info(session, chapterData.id_url_title);

    // ===========

    json j = json_title_info.at("data");

    std::string original_name = j.value("name", "").get<std::string>();

    std::string title_ru = j.value("rus_name", "").get<std::string>();

    //std::string otherNames = j
    //
    json cover = j.at("cover");

    std::string ageRestriction = j.at("ageRestriction").value("label", std::string{});
    
    // Страна происхождения
    std::string make country = j.at("type").value("label", std::string{});

    json summary = j.at("content");

    std::string year_of_release = j.value("releaseDate", std::string{});

    json translate_teams = j.at("teams");

    json genres = j.at("genres");

    json tags = j.at("tags");

    std::string publisher = j.contains("publisher") && j["publisher"].is_object() ? j["publisher"].value("name", std::string{}) : std::string{};

    //j.at("publisher").value("name", std::string{});

    std::string franchise = j.at("franchise").value("name", std::string{});

    json authors = j.at("authors");

    json artists = j.at("artists");
}


/// std::this_thread::sleep_for(std::chrono::seconds(2)); //Сделать задержку на 2 секунды

// ==========

// 

struct CapterData {
    int volume;
    int chapter;
    int branch_id;
    std::string id_url_title = "";
};

int main(){
    CapterData chapterData{1, 224, 22612, "203736--seoul-object-story"};
    LibComix comixData;

    cpr::Url url = {"http://192.168.0.18:8000"};

    cpr::Session session;
    cpr::Cookies cookies = {
        {"mangalib_session", ""},
        {"XSRF-TOKEN", ""}
    };

    parserInit(&session, cookies);

    //

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
